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
