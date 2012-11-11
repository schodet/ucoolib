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
#include "ucoolib/arch/arch.hh"
#include "ucoolib/hal/gpio/gpio.hh"
#include "ucoolib/utils/delay.hh"

#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/gpio.h>

void
test (ucoo::Io &loop_out, ucoo::Io &loop_in, ucoo::Io &led3, ucoo::Io &led4,
      ucoo::Io &led5, ucoo::Io &led6)
{
    loop_in.input ();
    loop_out.output ();
    led3.output ();
    led4.output ();
    led5.output ();
    led6.output ();
    led3.set ();
    led6.reset ();
    bool state = false;
    while (1)
    {
        led3.toggle ();
        led4.set (state);
        led5.set (loop_in.get ());
        led6.toggle ();
        state = !state;
        loop_out.set (state);
        ucoo::delay (1);
    }
}

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPBEN
                                 | RCC_AHB1ENR_IOPDEN);
    // For this test, shorten B6 & B7 to have loopback.
    gpio_mode_setup (GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO7);
    ucoo::Gpio loop_out (GPIOB, 6);
    ucoo::Gpio loop_in (GPIOB, 7);
    ucoo::Gpio led3 (GPIOD, 13);
    ucoo::Gpio led4 (GPIOD, 12);
    ucoo::Gpio led5 (GPIOD, 14);
    ucoo::Gpio led6 (GPIOD, 15);
    test (loop_out, loop_in, led3, led4, led5, led6);
    return 0;
}
