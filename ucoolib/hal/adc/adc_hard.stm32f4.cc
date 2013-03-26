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
#include "adc_hard.stm32f4.hh"

#include <libopencm3/stm32/f4/adc.h>
#include <libopencm3/stm32/f4/rcc.h>

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
