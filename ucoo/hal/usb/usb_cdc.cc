// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2016 Nicolas Schodet
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
#include "ucoo/hal/usb/usb_cdc.hh"
#include "ucoo/hal/usb/usb_cdc_desc.hh"
#include "ucoo/hal/usb/usb_driver.hh"

#include "ucoo/utils/irq_locked.hh"

namespace ucoo {

enum
{
    END_POINT_RX = 0x01,
    END_POINT_TX = 0x81,
    END_POINT_NOTIF = 0x82,
};

static const auto device_desc = usb_device_desc (
    0x0200, USB_CLASS_CDC, 0, 0, USB_CONTROL_END_POINT_SIZE,
    CONFIG_UCOO_HAL_USB_VENDOR_ID,
    CONFIG_UCOO_HAL_USB_PRODUCT_ID,
    0x0100, 1, 2, 0, 1);

static const auto configuration_desc = usb_configuration_desc (
    2, 1, 0,
    0x80 | (CONFIG_UCOO_HAL_USB_SELF_POWERED ? 0x40 : 0),
    CONFIG_UCOO_HAL_USB_MAX_POWER / 2,
    usb_interface_desc (
        0, 0, 1,
        USB_CLASS_CDC, USB_CDC_SUBCLASS_ACM, USB_CDC_PROTOCOL_AT, 0,
        usb_endpoint_desc (END_POINT_NOTIF, 0x03, 16, 128),
        usb_cdc_header_desc (0x0110),
        usb_cdc_call_management_desc (0, 1),
        usb_cdc_abstract_control_management_desc (2),
        usb_cdc_union_desc (0, 1)),
    usb_interface_desc (
        1, 0, 2,
        USB_CLASS_DATA, 0, 0, 0,
        usb_endpoint_desc_bulk (END_POINT_RX, CONFIG_UCOO_HAL_USB_EP_SIZE),
        usb_endpoint_desc_bulk (END_POINT_TX, CONFIG_UCOO_HAL_USB_EP_SIZE)));

details::UsbDesc
usb_cdc_default_device_desc ()
{
    return device_desc;
}

details::UsbDesc
usb_cdc_default_configuration_desc ()
{
    return configuration_desc;
}

UsbApplicationCdcAcm::SetupResult
UsbApplicationCdcAcm::handle_setup (uint16_t request_n_type, uint16_t value,
                                    uint16_t index, uint16_t length)
{
    SetupResult res = SetupResult::NotHandled;
    switch (request_n_type)
    {
    case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
        usb_trace ("setup set control line state");
        send_status ();
        send_serial_state_ = true;
        driver_.ep_write_ready (END_POINT_NOTIF);
        res = SetupResult::Handled;
        break;
    case USB_CDC_REQ_SET_LINE_CODING:
        usb_trace ("setup set line coding");
        recv (reinterpret_cast<char *> (&line_coding_),
              sizeof (line_coding_));
        res = SetupResult::Handled;
        break;
    }
    return res;
}

void
UsbApplicationCdcAcm::handle_configuration_set (uint8_t configuration)
{
    driver_.ep_setup (END_POINT_RX, UsbDriver::EpType::Bulk,
                      CONFIG_UCOO_HAL_USB_EP_SIZE, *this);
    driver_.ep_setup (END_POINT_TX, UsbDriver::EpType::Bulk,
                      CONFIG_UCOO_HAL_USB_EP_SIZE, *this);
    driver_.ep_setup (END_POINT_NOTIF, UsbDriver::EpType::Interrupt,
                      16, *this);
    if (!rx_buffer_.full ())
        driver_.ep_read_ready (END_POINT_RX, rx_buffer_.room ());
    if (!tx_buffer_.empty ())
        driver_.ep_write_ready (END_POINT_TX);
    send_serial_state_ = true;
    driver_.ep_write_ready (END_POINT_NOTIF);
    configured_ = true;
}

void
UsbApplicationCdcAcm::ep_handle_out (uint8_t ep_address, int bcnt)
{
    if (ep_address == 0)
        UsbApplicationWithSetup::ep_handle_out (0, bcnt);
    else if (ep_address == END_POINT_RX)
    {
        int r = driver_.ep_read (END_POINT_RX, rx_buffer_.write (),
                                 rx_buffer_.room ());
        rx_buffer_.written (r);
        driver_.ep_read_ready (END_POINT_RX, rx_buffer_.room ());
    }
    else
        assert_unreachable ();
}

void
UsbApplicationCdcAcm::ep_handle_in (uint8_t ep_address)
{
    if (ep_address == 0)
        UsbApplicationWithSetup::ep_handle_in (0);
    else if (ep_address == END_POINT_TX)
    {
        if (!tx_buffer_.empty ())
        {
            int r = driver_.ep_write (END_POINT_TX, tx_buffer_.read (),
                                      tx_buffer_.size ());
            tx_buffer_.drop (r);
            tx_buffer_.rewind ();
        }
    }
    else if (ep_address == END_POINT_NOTIF)
    {
        if (send_serial_state_)
        {
            struct
            {
                UsbCdcNotification notification;
                uint16_t state;
            } n;
            n.notification.bmRequestType = USB_REQ (INTERFACE, CLASS, IN, 0);
            n.notification.bNotification = USB_CDC_NOTIFICATION_SERIAL_STATE;
            n.notification.wValue = 0;
            n.notification.wIndex = 0;
            n.notification.wLength = 2;
            n.state = active_ ? 3 : 0;
            driver_.ep_write (END_POINT_NOTIF,
                              reinterpret_cast<const char *> (&n), sizeof (n));
            send_serial_state_ = false;
        }
    }
    else
        assert_unreachable ();
}

int
UsbApplicationCdcAcm::read (char *buf, int count)
{
    while (1)
    {
        {
            IrqLocked flags;
            if (!rx_buffer_.empty ())
            {
                int r = std::min (rx_buffer_.size (), count);
                const char *f = rx_buffer_.read ();
                std::copy (f, f + r, buf);
                rx_buffer_.drop (r);
                rx_buffer_.rewind ();
                if (configured_)
                    driver_.ep_read_ready (END_POINT_RX, rx_buffer_.room ());
                return r;
            }
            else if (!block_)
                return 0;
        }
        yield ();
    }
}

int
UsbApplicationCdcAcm::write (const char *buf, int count)
{
    int left = count;
    while (left)
    {
        {
            IrqLocked flags;
            if (!tx_buffer_.full ())
            {
                int r = std::min (tx_buffer_.room (), left);
                std::copy (buf, buf + r, tx_buffer_.write (r));
                if (configured_)
                    driver_.ep_write_ready (END_POINT_TX);
                buf += r;
                left -= r;
            }
        }
        if (!block_)
            break;
        yield ();
    }
    return count - left;
}

int
UsbApplicationCdcAcm::poll ()
{
    return rx_buffer_.size ();
}

void
UsbApplicationCdcAcm::recv_done ()
{
    // Do not actually use the line coding information.
    send_status ();
}

} // namespace ucoo
