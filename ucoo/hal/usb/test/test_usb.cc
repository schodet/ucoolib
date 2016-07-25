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
#include "ucoo/hal/usb/usb.hh"
#include "ucoo/hal/usb/usb_cdc.hh"
#include "ucoo/hal/gpio/gpio.hh"
#include "ucoo/arch/arch.hh"

static const auto string_descs_pack = ucoo::usb_descs_pack (
    ucoo::usb_string_desc (ucoo::USB_LANGUAGE_EN_US),
    ucoo::usb_string_desc (u"APBTeam"),
    ucoo::usb_string_desc (u"test"));

static const auto string_descs = ucoo::usb_descs (string_descs_pack);

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::UsbDriverDwcOtg driver (ucoo::UsbDriverDwcOtg::Instance::OTG_FS,
                                  ucoo::usb_cdc_default_device_desc (),
                                  ucoo::usb_cdc_default_configuration_desc (),
                                  string_descs);
    ucoo::UsbApplicationCdcAcm cdc (driver);
    driver.enable ();
    cdc.write ("hello", 5);
    while (1)
    {
        char buf[64];
        int r = cdc.read (buf, sizeof (buf));
        if (r)
            cdc.write (buf, r);
    }
}

