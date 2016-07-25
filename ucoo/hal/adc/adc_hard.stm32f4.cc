// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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
#include "ucoo/hal/adc/adc_hard.stm32f4.hh"

namespace ucoo {

static ADC_TypeDef * const bases[] = { reg::ADC1, reg::ADC2, reg::ADC3 };
static const Rcc rccs[] = { Rcc::ADC1, Rcc::ADC2, Rcc::ADC3 };

AdcHard::AdcHard (AdcHard::Instance inst)
    : base_ (bases[static_cast<int> (inst)]),
      rcc_ (rccs[static_cast<int> (inst)])
{
}

AdcHard::~AdcHard ()
{
    disable ();
}

void
AdcHard::enable ()
{
    rcc_peripheral_clock_enable (rcc_);
    base_->CR2 = ADC_CR2_ADON;
}

void
AdcHard::disable ()
{
    base_->CR2 = 0;
    rcc_peripheral_clock_disable (rcc_);
}

int
AdcHard::read (int channel)
{
    base_->SQR3 = channel;
    base_->CR2 |= ADC_CR2_SWSTART;
    while (!(base_->SR & ADC_SR_EOC))
        yield ();
    base_->SR = ~ADC_SR_EOC;
    return base_->DR;
}

} // namespace ucoo
