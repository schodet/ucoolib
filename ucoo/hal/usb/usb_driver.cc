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
#include "ucoo/hal/usb/usb_driver.hh"
#include "ucoo/hal/usb/usb_def.hh"

namespace ucoo {

Trace<CONFIG_UCOO_HAL_USB_TRACE> usb_trace ("usb");

void
UsbDriver::register_application (UsbApplication &app)
{
    for (auto &ap : applications_)
    {
        if (!ap)
        {
            ap = &app;
            return;
        }
    }
    assert_unreachable ();
}

void
UsbDriver::set_configuration (int configuration)
{
    if (configuration == current_configuration_)
        return;
    assert (current_configuration_ == -1); // TODO: handle change.
    for (auto app : applications_)
    {
        if (!app)
            break;
        app->handle_configuration_set (configuration);
    }
    current_configuration_ = configuration;
}

void
UsbDriver::ep_setup (uint8_t address, EpType type, int size,
                     UsbApplication &app)
{
    bool in = address & 0x80;
    int ep = address & 0x7f;
    assert (ep < END_POINT_MAX);
    if (in)
        in_handlers_[ep] = &app;
    else
        out_handlers_[ep] = &app;
    ep_setup (address, type, size);
}

void
UsbDriver::handle_reset ()
{
    ep_setup (0, EpType::Control, USB_CONTROL_END_POINT_SIZE);
}

void
UsbDriver::handle_setup (uint32_t *msg)
{
    uint16_t bmRequestType_n_bRequest = (msg[0] >> 0) & 0xffff;
    uint16_t wValue = (msg[0] >> 16) & 0xffff;
    uint16_t wIndex = (msg[1] >> 0) & 0xffff;
    uint16_t wLength = (msg[1] >> 16) & 0xffff;
    bool ok = false;
    out_handlers_[0] = nullptr;
    in_handlers_[0] = nullptr;
    for (auto app : applications_)
    {
        if (!app)
            break;
        UsbApplication::SetupResult r = app->handle_setup (
            bmRequestType_n_bRequest, wValue, wIndex, wLength);
        if (r == UsbApplication::SetupResult::Handled)
        {
            out_handlers_[0] = app;
            in_handlers_[0] = app;
            ok = true;
            break;
        }
        else if (r == UsbApplication::SetupResult::Error)
            break;
    }
    if (!ok)
        ep_stall (0);
}

void
UsbDriver::ep_handle_out (uint8_t address, int bcnt)
{
    int ep_num = address;
    if (out_handlers_[ep_num])
        out_handlers_[ep_num]->ep_handle_out (address, bcnt);
    else
        ep_stall (address);
}

void
UsbDriver::ep_handle_in (uint8_t address)
{
    int ep_num = address & 0x7f;
    if (in_handlers_[ep_num])
        in_handlers_[ep_num]->ep_handle_in (address);
    else
        ep_stall (address);
}

} // namespace ucoo
