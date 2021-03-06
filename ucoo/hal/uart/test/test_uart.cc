// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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

#if defined (TARGET_stm32)
# include "ucoo/hal/gpio/gpio.hh"
#endif

#include "ucoo/common.hh"

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
#if defined (TARGET_host)
    ucoo::Uart u0, u1 ("uart1");
#elif defined (TARGET_stm32)
    // D8, D9: UART3
    // C12, D2: UART5
    ucoo::GPIOC.enable ();
    ucoo::GPIOD.enable ();
    ucoo::GPIOC[12].af (8);
    ucoo::GPIOD[2].af (8);
    ucoo::GPIOD[8].af (7);
    ucoo::GPIOD[9].af (7);
    ucoo::Uart u0 (ucoo::Uart::Instance::USART3);
    ucoo::Uart u1 (ucoo::Uart::Instance::UART5);
    u0.enable (38400, ucoo::Uart::Parity::EVEN, 1);
    u1.enable (38400, ucoo::Uart::Parity::EVEN, 1);
#endif
    ucoo::Uart *u[] = { &u0, &u1 };
    char buf[64];
    while (1)
    {
        for (int i = 0; i < (int) ucoo::lengthof (u); i++)
        {
            if (u[i]->poll ())
            {
                int len = u[i]->read (buf, sizeof (buf));
                u[i]->write (buf, len);
            }
        }
        ucoo::yield ();
    }
}

