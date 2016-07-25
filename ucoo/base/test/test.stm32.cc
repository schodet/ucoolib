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
#include "test.hh"

#include "config/ucoo/base/test.hh"
#if CONFIG_UCOO_BASE_TEST_TEST_STREAM_USE_USB
# include "ucoo/hal/usb/usb.hh"
# include "ucoo/hal/usb/usb_cdc.hh"
#else
# include "ucoo/hal/uart/uart.hh"
#endif

#include "ucoo/arch/syscalls.newlib.hh"

namespace ucoo {

Stream &
test_stream (void)
{
    static bool enabled = false;
#if CONFIG_UCOO_BASE_TEST_TEST_STREAM_USE_USB
    static const auto string_descs_pack = usb_descs_pack (
        usb_string_desc (USB_LANGUAGE_EN_US),
        usb_string_desc (CONFIG_UCOO_BASE_TEST_TEST_STREAM_USB_VENDOR),
        usb_string_desc (CONFIG_UCOO_BASE_TEST_TEST_STREAM_USB_PRODUCT));
    static const auto string_descs = ucoo::usb_descs (string_descs_pack);
    static UsbDriverDwcOtg driver (
        CONFIG_UCOO_BASE_TEST_TEST_STREAM_USB_INSTANCE,
        usb_cdc_default_device_desc (),
        usb_cdc_default_configuration_desc (),
        string_descs);
    static UsbApplicationCdcAcm cdc (driver);
    if (!enabled)
    {
        enabled = true;
        driver.enable ();
    }
    return cdc;
#else
    static Uart u (CONFIG_UCOO_BASE_TEST_TEST_STREAM_UART_INSTANCE);
    if (!enabled)
    {
        enabled = true;
        u.enable (CONFIG_UCOO_BASE_TEST_TEST_STREAM_UART_SPEED);
    }
    return u;
#endif
}

void
test_stream_setup (void)
{
    Stream *s = &test_stream ();
    syscalls_streams[0] = s;
    syscalls_streams[1] = s;
    syscalls_streams[2] = s;
}

} // namespace ucoo
