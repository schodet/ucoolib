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
#include "ucoo/hal/usb/usb_application.hh"
#include "ucoo/hal/usb/usb_driver.hh"
#include "ucoo/hal/usb/usb_def.hh"

namespace ucoo {

UsbApplication::UsbApplication (UsbDriver &driver)
    : driver_ (driver)
{
    driver.register_application (*this);
}

void
UsbApplicationWithSetup::ep_handle_out (uint8_t ep_address, int bcnt)
{
    assert (ep_address == 0);
    if (setup_stage_ == SetupStage::HostToDeviceOUT)
    {
        if (bcnt <= data_size_)
        {
            if (bcnt)
            {
                driver_.ep_read (0, data_, bcnt);
                data_ += bcnt;
                data_size_ -= bcnt;
            }
            if (bcnt < USB_CONTROL_END_POINT_SIZE)
                recv_done ();
        }
        else
        {
            driver_.ep_stall (0);
            setup_stage_ = SetupStage::Idle;
        }
    }
    else if (setup_stage_ == SetupStage::DeviceToHostStatusOUT && bcnt == 0)
    {
        // Status received, done.
        setup_stage_ = SetupStage::Idle;
    }
    else
    {
        driver_.ep_stall (0);
        setup_stage_ = SetupStage::Idle;
    }
}

void
UsbApplicationWithSetup::ep_handle_in (uint8_t ep_address)
{
    assert (ep_address == 0);
    if (setup_stage_ == SetupStage::DeviceToHostIN)
    {
        if (data_)
        {
            int r = driver_.ep_write (0, data_, data_size_);
            if (data_size_ < USB_CONTROL_END_POINT_SIZE)
                data_ = nullptr;
            else
            {
                data_ += r;
                data_size_ -= r;
            }
        }
        else
        {
            driver_.ep_read_ready (0, USB_CONTROL_END_POINT_SIZE);
            setup_stage_ = SetupStage::DeviceToHostStatusOUT;
        }
    }
    else if (setup_stage_ == SetupStage::HostToDeviceStatusIN)
    {
        // Status sent, done.
        setup_stage_ = SetupStage::Idle;
    }
    else
    {
        driver_.ep_stall (0);
        setup_stage_ = SetupStage::Idle;
    }
}

void
UsbApplicationWithSetup::send_status ()
{
    driver_.ep_write (0, nullptr, 0);
    setup_stage_ = SetupStage::HostToDeviceStatusIN;
}

void
UsbApplicationWithSetup::send (const char *data, int data_size)
{
    int r = driver_.ep_write (0, data, data_size);
    if (data_size < USB_CONTROL_END_POINT_SIZE)
        data_ = nullptr;
    else
    {
        data_ = const_cast<char *> (data) + r;
        data_size_ = data_size - r;
    }
    setup_stage_ = SetupStage::DeviceToHostIN;
}

void
UsbApplicationWithSetup::recv (char *data, int data_size)
{
    data_ = data;
    data_size_ = data_size;
    driver_.ep_read_ready (0, USB_CONTROL_END_POINT_SIZE);
    setup_stage_ = SetupStage::HostToDeviceOUT;
}

void
UsbApplicationWithSetup::recv_done ()
{
    // Must be overrided if used.
    assert_unreachable ();
}

UsbApplicationBasic::SetupResult
UsbApplicationBasic::handle_setup (uint16_t request_n_type,
                                   uint16_t value, uint16_t index,
                                   uint16_t length)
{
    SetupResult res = SetupResult::NotHandled;
    switch (request_n_type)
    {
    case USB_REQ_SET_ADDRESS:
        if (index == 0 && length == 0)
        {
            usb_trace ("setup set address %d", value);
            driver_.set_address (value);
            send_status ();
            res = SetupResult::Handled;
        }
        break;
    case USB_REQ_GET_DESCRIPTOR:
        usb_trace ("setup get descriptor 0x%04x", value);
        res = send_descriptor (value >> 8, value & 0xff, index, length);
        break;
    case USB_REQ_SET_CONFIGURATION:
        if (index == 0 && length == 0)
        {
            usb_trace ("setup set configuration %d", value);
            driver_.set_configuration (value);
            send_status ();
            res = SetupResult::Handled;
        }
        break;
    case USB_REQ_SET_INTERFACE:
        if (length == 0)
        {
            usb_trace ("setup set interface %d", value);
            // TODO
            send_status ();
            res = SetupResult::Handled;
        }
        break;
    }
    return res;
}

void
UsbApplicationBasic::handle_configuration_set (uint8_t configuration)
{
}

UsbApplicationBasic::SetupResult
UsbApplicationBasic::send_descriptor (uint8_t desc_type, uint8_t desc_index,
                                      uint16_t language, uint16_t length)
{
    const details::UsbDesc *desc = nullptr;
    switch (desc_type)
    {
    case 1: // Device.
        if (desc_index == 0 || language == 0)
            desc = &device_desc_;
        break;
    case 2: // Configuration.
        if (desc_index == 0 || language == 0)
            desc = &configuration_desc_;
        break;
    case 3: // String.
        if ((desc_index == 0 && language == 0)
            || (desc_index != 0 && desc_index < string_descs_nb_
                && language == ucoo::USB_LANGUAGE_EN_US))
            desc = &string_descs_[desc_index];
    }
    if (desc)
    {
        send (desc->desc,
              desc->desc_size < length ? desc->desc_size : length);
        return SetupResult::Handled;
    }
    else
        return SetupResult::NotHandled;
}

} // namespace ucoo
