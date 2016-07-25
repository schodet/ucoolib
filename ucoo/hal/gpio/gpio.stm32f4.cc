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
#include "ucoo/hal/gpio/gpio.stm32f4.hh"
#include "ucoo/common.hh"

namespace ucoo {

GpioPort GPIOA (reg::GPIOA, Rcc::GPIOA);
GpioPort GPIOB (reg::GPIOB, Rcc::GPIOB);
GpioPort GPIOC (reg::GPIOC, Rcc::GPIOC);
GpioPort GPIOD (reg::GPIOD, Rcc::GPIOD);
GpioPort GPIOE (reg::GPIOE, Rcc::GPIOE);
GpioPort GPIOF (reg::GPIOF, Rcc::GPIOF);
GpioPort GPIOG (reg::GPIOG, Rcc::GPIOG);
GpioPort GPIOH (reg::GPIOH, Rcc::GPIOH);
GpioPort GPIOI (reg::GPIOI, Rcc::GPIOI);
GpioPort GPIOJ (reg::GPIOJ, Rcc::GPIOJ);
GpioPort GPIOK (reg::GPIOK, Rcc::GPIOK);

void
Gpio::set ()
{
    port_->BSRR = mask_;
}

void
Gpio::reset ()
{
    port_->BSRR = mask_ << 16;
}

/// Helper to avoid virtual dance.
static inline void
Gpio_set (GPIO_TypeDef *port, uint16_t mask, bool state)
{
    if (state)
        port->BSRR = mask;
    else
        port->BSRR = mask << 16;
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
    Gpio_set (port_, mask_, !(port_->ODR & mask_));
}

bool
Gpio::get () const
{
    return port_->IDR & mask_;
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

/// Set four bits in a register for the corresponding one-bit mask.
static uint32_t
qmask_set (uint32_t mask, uint32_t reg, uint32_t bits)
{
    uint32_t qshift = mask * mask;
    qshift *= qshift;
    uint32_t qmask = qshift | qshift << 1;
    qmask = qmask | qmask << 2;
    reg &= ~qmask;
    reg |= bits * qshift;
    return reg;
}

void
Gpio::input ()
{
    port_->MODER = dmask_set (mask_, port_->MODER, GPIO_MODER_Input);
}

void
Gpio::output ()
{
    port_->MODER = dmask_set (mask_, port_->MODER, GPIO_MODER_Output);
}

void
Gpio::pull (Pull dir)
{
    port_->PUPDR = dmask_set (mask_, port_->PUPDR,
                              static_cast<uint32_t> (dir));
}

void
Gpio::speed (Speed s)
{
    port_->OSPEEDR = dmask_set (mask_, port_->OSPEEDR,
                                static_cast<uint32_t> (s));
}

void
Gpio::type (Type t)
{
    port_->OTYPER = (port_->OTYPER & ~mask_)
        | (mask_ * static_cast<uint32_t> (t));
}

void
Gpio::af (int num)
{
    port_->MODER = dmask_set (mask_, port_->MODER, GPIO_MODER_AF);
    if (mask_ & 0xff)
        port_->AFR[0] = qmask_set (mask_, port_->AFR[0], num);
    else
        port_->AFR[1] = qmask_set (mask_ >> 8, port_->AFR[1], num);
}

void
Gpio::analog ()
{
    port_->MODER = dmask_set (mask_, port_->MODER, GPIO_MODER_Analog);
}

int
GpioPort::get_port_index () const
{
    int port;
    switch (rcc_)
    {
    case Rcc::GPIOA:
        port = 0;
        break;
    case Rcc::GPIOB:
        port = 1;
        break;
    case Rcc::GPIOC:
        port = 2;
        break;
    case Rcc::GPIOD:
        port = 3;
        break;
    case Rcc::GPIOE:
        port = 4;
        break;
    case Rcc::GPIOF:
        port = 5;
        break;
    case Rcc::GPIOG:
        port = 6;
        break;
    case Rcc::GPIOH:
        port = 7;
        break;
    case Rcc::GPIOI:
        port = 8;
        break;
    case Rcc::GPIOJ:
        port = 9;
        break;
    case Rcc::GPIOK:
        port = 10;
        break;
    default:
        assert_unreachable ();
    }
    return port;
}

} // namespace ucoo
