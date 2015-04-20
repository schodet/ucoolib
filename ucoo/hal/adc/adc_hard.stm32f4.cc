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
#include "adc_hard.stm32f4.hh"

#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>

namespace ucoo {

AdcHard::AdcHard (int n)
    : n_ (n)
{
    static const uint32_t bases[] = { ADC1, ADC2, ADC3 };
    assert (n < (int) lengthof (bases));
    base_ = bases[n];
}

AdcHard::~AdcHard ()
{
    disable ();
}

void
AdcHard::enable ()
{
    rcc_peripheral_enable_clock (&RCC_APB2ENR, RCC_APB2ENR_ADC1EN << n_);
    ADC_CR2 (base_) = ADC_CR2_ADON;
}

void
AdcHard::disable ()
{
    ADC_CR2 (base_) = 0;
    rcc_peripheral_disable_clock (&RCC_APB2ENR, RCC_APB2ENR_ADC1EN << n_);
}

int
AdcHard::read (int channel)
{
    ADC_SQR3 (base_) = channel;
    ADC_CR2 (base_) |= ADC_CR2_SWSTART;
    while (!(ADC_SR (base_) & ADC_SR_EOC))
        yield ();
    ADC_SR (base_) = ~ADC_SR_EOC;
    return ADC_DR (base_);
}

} // namespace ucoo
