#ifndef ucoo_hal_gpio_gpio_stm32f4_hh
#define ucoo_hal_gpio_gpio_stm32f4_hh
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
#include "ucoo/intf/io.hh"

#include <libopencm3/stm32/gpio.h>

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

#endif // ucoo_hal_gpio_gpio_stm32f4_hh
