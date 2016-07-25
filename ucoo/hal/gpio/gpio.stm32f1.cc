// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2015 Nicolas Schodet
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
#include "ucoo/hal/gpio/gpio.stm32f1.hh"
#include "ucoo/common.hh"

namespace ucoo {

GpioPort GPIOA (reg::GPIOA, Rcc::GPIOA);
GpioPort GPIOB (reg::GPIOB, Rcc::GPIOB);
GpioPort GPIOC (reg::GPIOC, Rcc::GPIOC);
GpioPort GPIOD (reg::GPIOD, Rcc::GPIOD);
GpioPort GPIOE (reg::GPIOE, Rcc::GPIOE);

void
Gpio::set ()
{
    port_->BSRR = mask_;
}

void
Gpio::reset ()
{
    port_->BRR = mask_;
}

/// Helper to avoid virtual dance.
static inline void
Gpio_set (GPIO_TypeDef *port, uint16_t mask, bool state)
{
    if (state)
        port->BSRR = mask;
    else
        port->BRR = mask;
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

/// Set four bits in a register for the corresponding one-bit mask.
static uint32_t
qmask_set (uint32_t mask, uint32_t reg, uint32_t bits)
{
    uint32_t dmask = mask * mask;
    uint32_t qmask = dmask * dmask;
    uint32_t qmask2 = qmask | qmask << 1;
    reg &= ~(qmask2 | qmask2 << 2);
    reg |= bits * qmask;
    return reg;
}

void
Gpio::input ()
{
    uint32_t cnf_mode = static_cast<uint8_t> (input_cnf_) | GPIO_MODE_Input;
    if (mask_ & 0xff)
        port_->CRL = qmask_set (mask_, port_->CRL, cnf_mode);
    else
        port_->CRH = qmask_set (mask_ >> 8, port_->CRH, cnf_mode);
    output_ = false;
}

void
Gpio::output ()
{
    uint32_t cnf_mode = static_cast<uint8_t> (output_cnf_)
        | static_cast<uint8_t> (speed_);
    if (mask_ & 0xff)
        port_->CRL = qmask_set (mask_, port_->CRL, cnf_mode);
    else
        port_->CRH = qmask_set (mask_ >> 8, port_->CRH, cnf_mode);
    output_ = true;
}

void
Gpio::input_cnf (InputCnf cnf)
{
    input_cnf_ = cnf;
    if (!output_)
        input ();
}

void
Gpio::output_cnf (OutputCnf cnf)
{
    output_cnf_ = cnf;
    if (output_)
        output ();
}

void
Gpio::speed (Speed s)
{
    speed_ = s;
    if (output_)
        output ();
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
    default:
        assert_unreachable ();
    }
    return port;
}

} // namespace ucoo
