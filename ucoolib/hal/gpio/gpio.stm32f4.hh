#ifndef ucoolib_hal_gpio_gpio_stm32f4_hh
#define ucoolib_hal_gpio_gpio_stm32f4_hh
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
#include "ucoolib/intf/io.hh"

#include <libopencm3/stm32/f4/gpio.h>

namespace ucoo {

/// General purpose input/output on STM32F4.
class Gpio : public Io
{
  public:
    enum Pull
    {
        PULL_NONE = GPIO_PUPD_NONE,
        PULL_UP = GPIO_PUPD_PULLUP,
        PULL_DOWN = GPIO_PUPD_PULLDOWN,
    };
    enum Speed
    {
        SPEED_2MHZ = GPIO_OSPEED_2MHZ,
        SPEED_25MHZ = GPIO_OSPEED_25MHZ,
        SPEED_50MHZ = GPIO_OSPEED_50MHZ,
        SPEED_100MHZ = GPIO_OSPEED_100MHZ,
    };
  public:
    /// Constructor, take the PORT base address, and pin BIT number.
    Gpio (uint32_t port, int bit);
    /// See Io::set.
    void set ();
    /// See Io::reset.
    void reset ();
    /// See Io::set.
    void set (bool state);
    /// See Io::toggle.
    void toggle ();
    /// See Io::get.
    bool get () const;
    /// See Io::input.
    void input ();
    /// See Io::output.
    void output ();
    /// Set pull-up or pull-down.
    void pull (Pull dir);
    /// Set output speed.
    void speed (Speed s);
  private:
    /// Port register base address.
    const uint32_t port_;
    /// IO bitmask.
    const uint16_t mask_;
};

inline
Gpio::Gpio (uint32_t port, int bit)
    : port_ (port), mask_ (1u << bit)
{
}

} // namespace ucoo

#endif // ucoolib_hal_gpio_gpio_stm32f4_hh
