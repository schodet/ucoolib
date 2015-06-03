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
#include "ucoo/utils/delay.hh"
#include "ucoo/arch/arch.hh"

#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/gpio.h>

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPDEN);
    gpio_mode_setup (GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                     GPIO12 | GPIO13 | GPIO14 | GPIO15);
    gpio_clear (GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);
    int i, j;
    while (1)
    {
        for (i = 0; i < 4; i++)
        {
            gpio_toggle (GPIOD, GPIO12 << (i % 4));
            ucoo::delay (1);
        }
        for (i = 0; i < 16; i++)
        {
            gpio_toggle (GPIOD, GPIO12 << (i % 4));
            ucoo::delay_ms (250);
        }
        for (i = 0; i < 16000; i++)
        {
            gpio_toggle (GPIOD, GPIO12 << (i % 4));
            ucoo::delay_us (250);
        }
        for (i = 0; i < 16; i++)
        {
            gpio_toggle (GPIOD, GPIO12 << (i % 4));
            for (j = 0; j < 1000; j++)
                ucoo::delay_us (250);
        }
    }
    return 0;
}
