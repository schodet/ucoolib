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
#include "ucoolib/hal/usb/usb.hh"
#include "ucoolib/arch/arch.hh"

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::UsbStreamControl usc ("APBTeam", "USB test");
    ucoo::UsbStream us[] = {
        ucoo::UsbStream (usc, 0),
#if UCOO_CONFIG_HAL_USB_STREAM_NB >= 2
        ucoo::UsbStream (usc, 1),
#endif
#if UCOO_CONFIG_HAL_USB_STREAM_NB >= 3
        ucoo::UsbStream (usc, 2),
#endif
    };
    if (UCOO_CONFIG_HAL_USB_STREAM_NB > 1)
    {
        for (int i = 0; i < UCOO_CONFIG_HAL_USB_STREAM_NB; i++)
            us[i].block (false);
    }
    char buf[6];
    while (1)
    {
        for (int i = 0; i < UCOO_CONFIG_HAL_USB_STREAM_NB; i++)
        {
            int len = us[i].read (buf + 2, sizeof (buf) - 2);
            if (len)
            {
                buf[0] = i + '0';
                buf[1] = '>';
                len += 2;
                if (UCOO_CONFIG_HAL_USB_STREAM_NB == 1)
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

