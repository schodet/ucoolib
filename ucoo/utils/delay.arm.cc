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
#include "ucoo/arch/reg.hh"
#include "ucoo/arch/rcc.stm32.hh"

#include <algorithm>

namespace ucoo {

void
delay_us (int us)
{
    // Suppose that frequency is a multiple of 8 MHz (to avoid 64 bit
    // division).
    int systick_mhz = rcc_ahb_freq_hz / (8 * 1000000);
    int cycles = systick_mhz * us;
    reg::SysTick->CTRL = 0;
    // Loop several times if cycles is too big for the systick timer. Some
    // nanoseconds are lost every second, I can live with that, it simplifies
    // code.
    while (cycles)
    {
        int loop_cycles = std::min (1 << 24, cycles);
        reg::SysTick->LOAD = loop_cycles - 1;
        reg::SysTick->VAL = 0;
        reg::SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
        while (!(reg::SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
            ;
        reg::SysTick->CTRL = 0;
        cycles -= loop_cycles;
    }
}

void
delay_ns (int ns)
{
    // Suppose that frequency is a multiple of 1 MHz (to avoid 64 bit
    // division).
    int hclock_mhz = rcc_ahb_freq_hz / 1000000;
    int cycles = (hclock_mhz * ns + 999) / 1000;
    reg::SysTick->CTRL = 0;
    // Loop once, ns is supposed to be small.
    reg::SysTick->LOAD = cycles - 1;
    reg::SysTick->VAL = 0;
    reg::SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while (!(reg::SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
        ;
    reg::SysTick->CTRL = 0;
}

} // namespace ucoo
