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
#include "ucoo/hal/gpio/gpio.hh"
#include "ucoo/common.hh"

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::GPIOD.enable ();
    ucoo::Gpio leds[] =
    {
        ucoo::GPIOD[12],
        ucoo::GPIOD[13],
        ucoo::GPIOD[14],
        ucoo::GPIOD[15],
    };
    for (int i = 0; i < ucoo::lengthof (leds); i++)
    {
        leds[i].output ();
        leds[i].reset ();
    }
    int i, j;
    while (1)
    {
        for (i = 0; i < 4; i++)
        {
            leds[i % 4].toggle ();
            ucoo::delay (1);
        }
        for (i = 0; i < 16; i++)
        {
            leds[i % 4].toggle ();
            ucoo::delay_ms (250);
        }
        for (i = 0; i < 16000; i++)
        {
            leds[i % 4].toggle ();
            ucoo::delay_us (250);
        }
        for (i = 0; i < 16; i++)
        {
            leds[i % 4].toggle ();
            for (j = 0; j < 1000; j++)
                ucoo::delay_us (250);
        }
    }
    return 0;
}
