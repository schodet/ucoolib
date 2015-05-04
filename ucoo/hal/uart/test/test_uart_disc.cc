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
#include "ucoo/hal/uart/uart.hh"

#include "ucoo/arch/arch.hh"
#include "ucoo/hal/gpio/gpio.hh"
#include "ucoo/base/test/test.hh"

#include <libopencm3/stm32/rcc.h>

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
    u1.enable (38400, ucoo::Uart::Parity::EVEN, 1);
    u3.enable (38400, ucoo::Uart::Parity::EVEN, 1);
    u4.enable (38400, ucoo::Uart::Parity::EVEN, 1);
    // For this test, shorten B6 & B7 to have a loopback on UART1, shorten C10
    // & C11 to connect UART3 to UART4.
    rcc_periph_clock_enable (RCC_GPIOB);
    rcc_periph_clock_enable (RCC_GPIOC);
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
                u->enable (38400, ucoo::Uart::Parity::ODD, 1);
                break;
            case 'E':
                u->enable (38400, ucoo::Uart::Parity::EVEN, 1);
                break;
            case 'N':
                u->enable (38400, ucoo::Uart::Parity::NONE, 1);
                break;
            default:
                if (buf_i < static_cast<int> (sizeof (buf)))
                    buf[buf_i++] = c;
                break;
            }
        }
    }
}
