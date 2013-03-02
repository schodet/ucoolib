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
#include "ucoolib/hal/uart/uart.hh"

#include "ucoolib/arch/arch.hh"
#include "ucoolib/hal/gpio/gpio.hh"
#include "ucoolib/base/test/test.hh"

#include <libopencm3/stm32/f4/rcc.h>

static void
check_act (ucoo::Stream &ts, ucoo::Stream &u, char n)
{
    char buf[3 + 16 + 1];
    if (!u.poll ())
        return;
    int r = u.read (buf + 3, 16);
    if (r <= 0)
    {
        buf[3] = '#';
        r = 1;
    }
    buf[0] = '<';
    buf[1] = n;
    buf[2] = ':';
    buf[3 + r] = '>';
    ts.write (buf, 3 + r + 1);
}

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::Stream &ts = ucoo::test_stream ();
    ucoo::Uart u1 (0);
    ucoo::Uart u3 (2);
    ucoo::Uart u4 (3);
    u1.enable (38400, ucoo::Uart::EVEN, 1);
    u3.enable (38400, ucoo::Uart::EVEN, 1);
    u4.enable (38400, ucoo::Uart::EVEN, 1);
    // For this test, shorten B6 & B7 to have a loopback on UART1, shorten C10
    // & C11 to connect UART3 to UART4.
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPBEN
                                 | RCC_AHB1ENR_IOPCEN);
    gpio_mode_setup (GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,
                     GPIO6 | GPIO7);
    gpio_set_af (GPIOB, GPIO_AF7, GPIO6 | GPIO7);
    gpio_mode_setup (GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE,
                     GPIO10 | GPIO11);
    gpio_set_af (GPIOC, GPIO_AF7, GPIO10);
    gpio_set_af (GPIOC, GPIO_AF8, GPIO11);
    // Loop to report any activity on ports and provide a simple UI.
    char buf[64];
    int buf_i = 0;
    ucoo::Uart *u = &u1;
    while (1)
    {
        check_act (ts, u1, '1');
        check_act (ts, u3, '3');
        check_act (ts, u4, '4');
        while (ts.poll ())
        {
            char c = ts.getc ();
            switch (c)
            {
            case '?':
                static const char help[] =
                    "? - help\n"
                    "1, 3, 4 - set output uart\n"
                    ": - reset output buffer index\n"
                    "! - send output buffer\n"
                    "O, E, N - change parity to Odd, Even or None\n"
                    "any - fill output buffer\n";
                ts.write (help, sizeof (help));
                break;
            case '1':
                u = &u1;
                break;
            case '3':
                u = &u3;
                break;
            case '4':
                u = &u4;
                break;
            case ':':
                buf_i = 0;
                break;
            case '!':
                u->write (buf, buf_i);
                break;
            case 'O':
                u->enable (38400, ucoo::Uart::ODD, 1);
                break;
            case 'E':
                u->enable (38400, ucoo::Uart::EVEN, 1);
                break;
            case 'N':
                u->enable (38400, ucoo::Uart::NONE, 1);
                break;
            default:
                if (buf_i < static_cast<int> (sizeof (buf)))
                    buf[buf_i++] = c;
                break;
            }
        }
    }
}
