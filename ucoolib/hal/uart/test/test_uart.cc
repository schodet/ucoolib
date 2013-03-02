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
#include "ucoolib/hal/uart/uart.hh"

#include "ucoolib/arch/arch.hh"

#if defined (TARGET_stm32)
# include <libopencm3/stm32/f4/rcc.h>
# include "ucoolib/hal/gpio/gpio.hh"
#endif

#include "ucoolib/common.hh"

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
#if defined (TARGET_host)
    ucoo::Uart u0, u1 ("uart1");
#elif defined (TARGET_stm32)
    // D8, D9: UART3
    // C12, D2: UART5
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPCEN);
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPDEN);
    gpio_mode_setup (GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
    gpio_mode_setup (GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO8 | GPIO9);
    gpio_set_af (GPIOC, GPIO_AF8, GPIO12);
    gpio_set_af (GPIOD, GPIO_AF8, GPIO2);
    gpio_set_af (GPIOD, GPIO_AF7, GPIO8 | GPIO9);
    ucoo::Uart u0 (2), u1 (4);
    u0.enable (38400, ucoo::Uart::EVEN, 1);
    u1.enable (38400, ucoo::Uart::EVEN, 1);
#endif
    ucoo::Uart *u[] = { &u0, &u1 };
    char buf[64];
    while (1)
    {
        for (int i = 0; i < (int) lengthof (u); i++)
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

