#ifndef ucoo_hal_timer_timer_stm32_tcc
#define ucoo_hal_timer_timer_stm32_tcc
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
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>

namespace ucoo {

template<uint32_t Base>
struct TimerHardware { };

template<>
struct TimerHardware<TIM2>
{
    static const enum rcc_periph_clken clken = RCC_TIM2;
    // TODO timer uses a double frequency only if APB prescaler is not 1.
    static int freq () { return 2 * rcc_apb1_frequency; }
};

template<uint32_t Base>
TimerHard<Base>::~TimerHard ()
{
    disable ();
}

template<uint32_t Base>
void
TimerHard<Base>::enable (int freq)
{
    rcc_periph_clock_enable (TimerHardware<Base>::clken);
    int in_freq = TimerHardware<Base>::freq ();
    int div = in_freq / freq;
    assert (div <= 0x10000 && in_freq / div == freq);
    TIM_CR1 (Base) = 0;
    TIM_PSC (Base) = div - 1;
    TIM_ARR (Base) = 0xffffffff;
    TIM_EGR (Base) = TIM_EGR_UG;
    TIM_CR1 (Base) = TIM_CR1_CEN;
}

template<uint32_t Base>
void
TimerHard<Base>::disable ()
{
    TIM_CR1 (Base) = 0;
    rcc_periph_clock_disable (TimerHardware<Base>::clken);
}

template<uint32_t Base>
uint32_t
TimerHard<Base>::get_value ()
{
    return TIM_CNT (Base);
}

} // namespace ucoo

#endif // ucoo_hal_timer_timer_stm32_tcc
