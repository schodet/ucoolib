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
#include "ucoolib/hal/adc/adc.hh"

#include "ucoolib/arch/arch.hh"
#include "ucoolib/base/test/test.hh"
#include "ucoolib/utils/delay.hh"

#include <libopencm3/stm32/f4/adc.h>
#include "ucoolib/hal/gpio/gpio.hh"

#include <cstdio>

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::test_stream_setup ();
    // Have fun with temperature sensor.
    ucoo::AdcHard adc (0);
    adc.enable ();
    ADC_CCR = ADC_CCR_TSVREFE;
    ucoo::AdcHardChannel c (adc, 16);
    while (1)
    {
        int r = c.read ();
        std::printf ("ADC = %d\n", r);
        ucoo::delay (0.5);
    }
}
