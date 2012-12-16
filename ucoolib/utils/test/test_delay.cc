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
#include "ucoolib/utils/delay.hh"
#include "ucoolib/arch/arch.hh"

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
