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
#include "gpio.stm32f4.hh"

namespace ucoo {

void
Gpio::set ()
{
    GPIO_BSRR (port_) = mask_;
}

void
Gpio::reset ()
{
    GPIO_BSRR (port_) = mask_ << 16;
}

/// Helper to avoid virtual dance.
static inline void
Gpio_set (uint32_t port, uint16_t mask, bool state)
{
    if (state)
        GPIO_BSRR (port) = mask;
    else
        GPIO_BSRR (port) = mask << 16;
}

void
Gpio::set (bool state)
{
    Gpio_set (port_, mask_, state);
}

void
Gpio::toggle ()
{
    // Avoid read/modify/write ODR, to achieve atomic operation.
    Gpio_set (port_, mask_, !(GPIO_ODR (port_) & mask_));
}

bool
Gpio::get () const
{
    return GPIO_IDR (port_) & mask_;
}

void
Gpio::input ()
{
    uint32_t mask = mask_;
    uint32_t dmask = mask * mask;
    GPIO_MODER (port_) &= ~(dmask | dmask << 1);
}

void
Gpio::output ()
{
    uint32_t mask = mask_;
    uint32_t dmask = mask * mask;
    GPIO_MODER (port_) = (GPIO_MODER (port_) & ~(dmask << 1)) | dmask;
}

} // namespace ucoo
