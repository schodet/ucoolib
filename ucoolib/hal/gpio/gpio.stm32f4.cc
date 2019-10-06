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

/// Set two bits in a register for the corresponding one-bit mask.
static uint32_t
dmask_set (uint32_t mask, uint32_t reg, uint32_t bits)
{
    uint32_t dmask = mask * mask;
    reg &= ~(dmask | dmask << 1);
    reg |= bits * dmask;
    return reg;
}

void
Gpio::input ()
{
    GPIO_MODER (port_) = dmask_set (mask_, GPIO_MODER (port_),
                                    GPIO_MODE_INPUT);
}

void
Gpio::output ()
{
    GPIO_MODER (port_) = dmask_set (mask_, GPIO_MODER (port_),
                                    GPIO_MODE_OUTPUT);
}

void
Gpio::pull (Pull dir)
{
    GPIO_PUPDR (port_) = dmask_set (mask_, GPIO_PUPDR (port_), dir);
}

void
Gpio::speed (Speed s)
{
    GPIO_OSPEEDR (port_) = dmask_set (mask_, GPIO_OSPEEDR (port_), s);
}

} // namespace ucoo
