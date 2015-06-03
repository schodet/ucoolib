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
#include "delay.arm.hh"

#include <algorithm>

#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/f4/rcc.h>

namespace ucoo {

void
delay_us (int us)
{
    // Horrible hack: there is no record of the current systick speed, make
    // guesses based on APB2 clock. Also, suppose that frequency is a multiple
    // of 1 MHz (to avoid 64 bit division).
    int systick_mhz = rcc_ppre2_frequency / (4 * 1000000);
    int cycles = systick_mhz * us;
    STK_CTRL = 0;
    // Loop several times if cycles is too big for the systick timer. Some
    // nanoseconds are lost every second, I can live with that, it simplifies
    // code.
    while (cycles)
    {
        int loop_cycles = std::min (1 << 24, cycles);
        STK_LOAD = loop_cycles - 1;
        STK_VAL = 0;
        STK_CTRL = STK_CTRL_ENABLE;
        while (!(STK_CTRL & STK_CTRL_COUNTFLAG))
            ;
        STK_CTRL = 0;
        cycles -= loop_cycles;
    }
}

void
delay_ns (int ns)
{
    // Horrible hack: there is no record of the current hclock speed, make
    // guesses based on APB2 clock. Also, suppose that frequency is a multiple
    // of 1 MHz (to avoid 64 bit division).
    int hclock_mhz = rcc_ppre2_frequency / (1000000 / 2);
    int cycles = (hclock_mhz * ns + 999) / 1000;
    STK_CTRL = 0;
    // Loop once, ns is supposed to be small.
    STK_LOAD = cycles - 1;
    STK_VAL = 0;
    STK_CTRL = STK_CTRL_CLKSOURCE_AHB | STK_CTRL_ENABLE;
    while (!(STK_CTRL & STK_CTRL_COUNTFLAG))
        ;
    STK_CTRL = 0;
}

} // namespace ucoo
