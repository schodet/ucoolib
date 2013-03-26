// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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
