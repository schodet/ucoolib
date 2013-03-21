// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
//
// APBTeam:
//        Web: http://apbteam.org/
//      Email: team AT apbteam DOT org
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// }}}
#include "usb.stm32.hh"
#include <algorithm>

#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/gpio.h>
#include <libopencm3/cm3/nvic.h>

#include "usb_desc.stm32.h"

static usbd_device *usbdev;

extern "C" {

void
otg_fs_isr ()
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

UsbStreamControl::RxBuffer::RxBuffer (void)
    : size (0), offset (0)
{
}

UsbStreamControl::UsbStreamControl (const char *vendor, const char *product)
    : configured_ (false)
{
    assert (!instance_);
    instance_ = this;
    strings[0] = vendor;
    strings[1] = product;
    rcc_peripheral_enable_clock (&RCC_AHB2ENR, RCC_AHB2ENR_OTGFSEN);
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPAEN);
    gpio_mode_setup (GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
                     GPIO9 | GPIO11 | GPIO12);
    gpio_set_af (GPIOA, GPIO_AF10, GPIO9 | GPIO11 | GPIO12);
    usbdev = usbd_init (&otgfs_usb_driver, &usb_desc_dev, &usb_desc_config,
                        strings, lengthof (strings));
    usbd_register_set_config_callback (usbdev, set_config);
    nvic_enable_irq (NVIC_OTG_FS_IRQ);
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
            int len = std::min (left, UsbStreamControl::ep_size_);
            len = usbd_ep_write_packet (usbdev, num_ + 0x81, buf, len);
            buf += len;
            left -= len;
        }
        if (!block_)
            break;
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
