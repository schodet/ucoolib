// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// }}}
#include "usb.stm32.hh"
#include <algorithm>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/otg_fs.h>
#include <libopencm3/stm32/otg_hs.h>

#include "usb_desc.stm32.h"

#if CONFIG_UCOO_HAL_USB_CDC_ACM
# include <libopencm3/usb/cdc.h>
#endif

#if defined (TARGET_stm32f4)
# if CONFIG_UCOO_HAL_USB_DRIVER_HS
#  define usb_isr otg_hs_isr
#  define usb_driver otghs_usb_driver
# else
#  define usb_isr otg_fs_isr
#  define usb_driver otgfs_usb_driver
# endif
#elif defined (TARGET_stm32f1)
#  define usb_isr otg_fs_isr
#  define usb_driver otgfs_usb_driver
#else
# error "not implemented for this target"
#endif

static usbd_device *usbdev;

// Buffer for control requests.
static uint8_t usb_control_buffer[128];

extern "C" {

void
usb_isr ()
{
    usbd_poll (usbdev);
}

}

namespace ucoo {

UsbStreamControl *UsbStreamControl::instance_;

const char *strings[] = {
    NULL,
    NULL
};

#if CONFIG_UCOO_HAL_USB_CDC_ACM

static void
usb_cdc_acm_send_serial_state (usbd_device *usbdev, bool active)
{
    struct serial_state_notification
    {
        struct usb_cdc_notification notification;
        uint16_t state;
    } n;
    n.notification.bmRequestType = 0xa1;
    n.notification.bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
    n.notification.wValue = 0;
    n.notification.wIndex = 0;
    n.notification.wLength = 2;
    n.state = active ? 3 : 0;
    usbd_ep_write_packet (usbdev, 0x82, reinterpret_cast<void *> (&n),
                          sizeof (n));
}

static int
usb_cdc_acm_control_request (
    usbd_device *usbdev,
    struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
    void (**complete) (usbd_device *usbdev, struct usb_setup_data *req))
{
    switch (req->bRequest)
    {
    case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
        usb_cdc_acm_send_serial_state (usbdev, true);
        return USBD_REQ_HANDLED;
    case USB_CDC_REQ_SET_LINE_CODING:
        if (*len < sizeof (struct usb_cdc_line_coding))
            return USBD_REQ_NOTSUPP;
        return USBD_REQ_HANDLED;
    }
    return 0;
}

#endif /* CONFIG_UCOO_HAL_USB_CDC_ACM */

UsbStreamControl::RxBuffer::RxBuffer (void)
    : size (0), offset (0)
{
}

UsbStreamControl::UsbStreamControl (const char *vendor, const char *product)
    : enabled_ (false), configured_ (false)
{
    assert (!instance_);
    instance_ = this;
    strings[0] = vendor;
    strings[1] = product;
}

UsbStreamControl::~UsbStreamControl ()
{
    disable ();
}

void
UsbStreamControl::enable ()
{
    if (!enabled_)
    {
#if defined (TARGET_stm32f4)
# if CONFIG_UCOO_HAL_USB_DRIVER_HS
        rcc_periph_clock_enable (RCC_OTGHS);
        rcc_periph_clock_enable (RCC_GPIOB);
        gpio_mode_setup (GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO14 | GPIO15);
        gpio_set_af (GPIOB, GPIO_AF12, GPIO14 | GPIO15);
# else
        rcc_periph_clock_enable (RCC_OTGFS);
        rcc_periph_clock_enable (RCC_GPIOA);
        gpio_mode_setup (GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11 | GPIO12);
        gpio_set_af (GPIOA, GPIO_AF10, GPIO11 | GPIO12);
# endif
#elif defined (TARGET_stm32f1)
        rcc_periph_clock_enable (RCC_OTGFS);
        rcc_periph_clock_enable (RCC_GPIOA);
#endif
        usbdev = usbd_init (&usb_driver, &usb_desc_dev, &usb_desc_config,
                            strings, lengthof (strings),
                            usb_control_buffer, sizeof (usb_control_buffer));
#if CONFIG_UCOO_HAL_USB_DRIVER_HS
        if (OTG_HS_CID == 0x2000)
        {
            // Different registers in F479.
            OTG_HS_GCCFG = (1 << 21) | (1 << 16);
            OTG_HS_DCTL = 0;
        }
#else
        if (OTG_FS_CID == 0x2000)
        {
            // Different registers in F479.
            OTG_FS_GCCFG = (1 << 21) | (1 << 16);
            OTG_FS_DCTL = 0;
        }
#endif
        usbd_register_set_config_callback (usbdev, set_config);
#if CONFIG_UCOO_HAL_USB_DRIVER_HS
        nvic_enable_irq (NVIC_OTG_HS_IRQ);
#else
        nvic_enable_irq (NVIC_OTG_FS_IRQ);
#endif
        enabled_ = true;
    }
}

void
UsbStreamControl::disable ()
{
    if (enabled_)
    {
        enabled_ = false;
        configured_ = false;
#if CONFIG_UCOO_HAL_USB_DRIVER_HS
        nvic_disable_irq (NVIC_OTG_HS_IRQ);
#else
        nvic_disable_irq (NVIC_OTG_FS_IRQ);
#endif
        usbd_disconnect (usbdev, true);
#if defined (TARGET_stm32f4)
# if CONFIG_UCOO_HAL_USB_DRIVER_HS
        rcc_periph_clock_disable (RCC_OTGHS);
        gpio_mode_setup (GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO14 | GPIO15);
# else
        rcc_periph_clock_disable (RCC_OTGFS);
        gpio_mode_setup (GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO11 | GPIO12);
# endif
#elif defined (TARGET_stm32f1)
        rcc_periph_clock_disable (RCC_OTGFS);
#endif
    }
}

void
UsbStreamControl::set_config (usbd_device *usbdev, uint16_t configured)
{
    instance_->configured_ = configured;
    if (configured)
    {
        for (int i = 0; i < stream_nb_; i++)
        {
            usbd_ep_setup (usbdev, 0x01 + i, USB_ENDPOINT_ATTR_BULK, ep_size_,
                           rx_callback);
            usbd_ep_setup (usbdev, 0x81 + i, USB_ENDPOINT_ATTR_BULK, ep_size_,
                           NULL);
        }
#if CONFIG_UCOO_HAL_USB_CDC_ACM
        usbd_ep_setup (usbdev, 0x82, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);
	usbd_register_control_callback (
            usbdev,
            USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
            USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
            usb_cdc_acm_control_request);
        usb_cdc_acm_send_serial_state (usbdev, true);
#endif
    }
}

void
UsbStreamControl::rx_callback (usbd_device *usbdev, uint8_t ep)
{
    assert (ep > 0 && ep <= stream_nb_);
    int num = ep - 1;
    RxBuffer &rb = instance_->rx_buffer_[num];
    assert (rb.size == 0 && rb.offset == 0);
    usbd_ep_nak_set (usbdev, ep, 1);
    rb.size = usbd_ep_read_packet (usbdev, ep, rb.buf, ep_size_);
}

UsbStream::UsbStream (UsbStreamControl &control, int num)
    : control_ (control), num_ (num)
{
    assert (num < UsbStreamControl::stream_nb_);
}

int
UsbStream::read (char *buf, int count)
{
    UsbStreamControl::RxBuffer &rb = control_.rx_buffer_[num_];
    /* Wait for reception. */
    if (!rb.size && !block_)
        return 0;
    while (!rb.size)
        barrier ();
    /* Copy to provided buffer. */
    int len = std::min (count, rb.size - rb.offset);
    buf = std::copy (rb.buf + rb.offset, rb.buf + rb.offset + len, buf);
    rb.offset += len;
    /* Reload buffer? */
    if (rb.offset == rb.size)
    {
        rb.offset = rb.size = 0;
        barrier ();
        usbd_ep_nak_set (usbdev, num_ + 1, 0);
    }
    /* Done. */
    return len;
}

int
UsbStream::write (const char *buf, int count)
{
    int left = count;
    while (left)
    {
        if (control_.configured_)
        {
            // Do the FIFO write with IRQ locked, as the USB IP does not
            // tolerate any interruption while the FIFO is being filled.
            int len = std::min (left, UsbStreamControl::ep_size_);
            irq_flags_t f = irq_lock ();
            len = usbd_ep_write_packet (usbdev, num_ + 0x81, buf, len);
            irq_restore (f);
            buf += len;
            left -= len;
        }
        if (!block_)
            break;
        barrier ();
    }
    return count - left;
}

int
UsbStream::poll ()
{
    UsbStreamControl::RxBuffer &rb = control_.rx_buffer_[num_];
    return rb.size - rb.offset;
}

} // namespace ucoo
