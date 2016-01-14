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
#include "ucoo/arch/arch.hh"

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::UsbStreamControl usc ("APBTeam", "USB test");
    usc.enable ();
    ucoo::UsbStream us[] = {
        ucoo::UsbStream (usc, 0),
#if CONFIG_UCOO_HAL_USB_STREAM_NB >= 2
        ucoo::UsbStream (usc, 1),
#endif
#if CONFIG_UCOO_HAL_USB_STREAM_NB >= 3
        ucoo::UsbStream (usc, 2),
#endif
    };
    if (CONFIG_UCOO_HAL_USB_STREAM_NB > 1)
    {
        for (int i = 0; i < CONFIG_UCOO_HAL_USB_STREAM_NB; i++)
            us[i].block (false);
    }
    char buf[6];
    while (1)
    {
        for (int i = 0; i < CONFIG_UCOO_HAL_USB_STREAM_NB; i++)
        {
            int len = us[i].read (buf + 2, sizeof (buf) - 2);
            if (len)
            {
                buf[0] = i + '0';
                buf[1] = '>';
                len += 2;
                if (CONFIG_UCOO_HAL_USB_STREAM_NB == 1)
                    us[i].write (buf, len);
                else
                {
                    const char *p = buf;
                    while (len)
                    {
                        int r = us[i].write (p, len);
                        p += r;
                        len -= r;
                    }
                }
            }
        }
    }
}

