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

// TODO add more timers.
// TODO timer uses a double frequency only if APB prescaler is not 1.

template<>
struct TimerHardware<TIM1>
{
    static const enum rcc_periph_clken clken = RCC_TIM1;
    static int freq () { return 2 * rcc_apb2_frequency; }
    static const unsigned int max = 0xffff;
    static const bool advanced = true;
    static const bool slave = true;
    static const bool one_pulse = true;
    static const int channels = 4;
};

template<>
struct TimerHardware<TIM2>
{
    static const enum rcc_periph_clken clken = RCC_TIM2;
    static int freq () { return 2 * rcc_apb1_frequency; }
#if defined TARGET_stm32f4
    static const unsigned int max = 0xffffffff;
#else
    static const unsigned int max = 0xffff;
#endif
    static const bool advanced = false;
    static const bool slave = true;
    static const bool one_pulse = true;
    static const int channels = 4;
};

template<>
struct TimerHardware<TIM3>
{
    static const enum rcc_periph_clken clken = RCC_TIM3;
    static int freq () { return 2 * rcc_apb1_frequency; }
    static const unsigned int max = 0xffff;
    static const bool advanced = false;
    static const bool slave = true;
    static const bool one_pulse = true;
    static const int channels = 4;
};

template<>
struct TimerHardware<TIM4>
{
    static const enum rcc_periph_clken clken = RCC_TIM4;
    static int freq () { return 2 * rcc_apb1_frequency; }
    static const unsigned int max = 0xffff;
    static const bool advanced = false;
    static const bool slave = true;
    static const bool one_pulse = true;
    static const int channels = 4;
};

template<>
struct TimerHardware<TIM5>
{
    static const enum rcc_periph_clken clken = RCC_TIM5;
    static int freq () { return 2 * rcc_apb1_frequency; }
#if defined TARGET_stm32f4
    static const unsigned int max = 0xffffffff;
#else
    static const unsigned int max = 0xffff;
#endif
    static const bool advanced = false;
    static const bool slave = true;
    static const bool one_pulse = true;
    static const int channels = 4;
};

template<>
struct TimerHardware<TIM10>
{
    static const enum rcc_periph_clken clken = RCC_TIM10;
    static int freq () { return 2 * rcc_apb2_frequency; }
    static const unsigned int max = 0xffff;
    static const bool advanced = false;
    static const bool slave = false;
    static const bool one_pulse = false;
    static const int channels = 1;
};

template<>
struct TimerHardware<TIM11>
{
    static const enum rcc_periph_clken clken = RCC_TIM10;
    static int freq () { return 2 * rcc_apb2_frequency; }
    static const unsigned int max = 0xffff;
    static const bool advanced = false;
    static const bool slave = false;
    static const bool one_pulse = false;
    static const int channels = 1;
};

template<uint32_t Base>
const unsigned int TimerHard<Base>::max = TimerHardware<Base>::max;

template<uint32_t Base>
TimerHard<Base>::~TimerHard ()
{
    disable ();
}

template<uint32_t Base>
template<typename... Options>
void
TimerHard<Base>::enable (int freq, bool start)
{
    using Hard = TimerHardware<Base>;
    rcc_periph_clock_enable (Hard::clken);
    int in_freq = Hard::freq ();
    int div = in_freq / freq;
    assert (div <= 0x10000 && in_freq / div == freq);
    freq_ = freq;
    using OptionsCombined = CombinedOptions<Options...>;
    TIM_CR1 (Base) = TIM_CR1_ARPE | OptionsCombined::cr1;
    TIM_CR2 (Base) = 0;
    if (Hard::slave)
        TIM_SMCR (Base) = OptionsCombined::smcr;
    if (Hard::channels > 0)
    {
        TIM_CCER (Base) = 0;
        TIM_CCMR1 (Base) = OptionsCombined::ccmr1;
        if (Hard::channels > 2)
            TIM_CCMR2 (Base) = OptionsCombined::ccmr2;
        TIM_CCER (Base) = OptionsCombined::ccer;
        TIM_CCR1 (Base) = OptionsCombined::ccr1;
        if (Hard::channels >= 2)
            TIM_CCR2 (Base) = OptionsCombined::ccr2;
        if (Hard::channels >= 3)
            TIM_CCR3 (Base) = OptionsCombined::ccr3;
        if (Hard::channels >= 4)
            TIM_CCR4 (Base) = OptionsCombined::ccr4;
    }
    TIM_PSC (Base) = div - 1;
    TIM_ARR (Base) = OptionsCombined::arr ? OptionsCombined::arr : Hard::max;
    if (Hard::advanced)
        TIM_BDTR (Base) = TIM_BDTR_MOE;
    TIM_EGR (Base) = TIM_EGR_UG;
    if (start)
        TIM_CR1 (Base) = TIM_CR1_ARPE | OptionsCombined::cr1 | TIM_CR1_CEN;
}

template<uint32_t Base>
void
TimerHard<Base>::disable ()
{
    TIM_CR1 (Base) = 0;
    rcc_periph_clock_disable (TimerHardware<Base>::clken);
    freq_ = 0;
}

template<uint32_t Base>
void
TimerHard<Base>::start ()
{
    TIM_CR1 (Base) |= TIM_CR1_CEN;
}

template<uint32_t Base>
void
TimerHard<Base>::set_reload (unsigned int value)
{
    assert (value <= TimerHardware<Base>::max);
    TIM_ARR (Base) = value;
}

template<uint32_t Base>
void
TimerHard<Base>::set_output_compare (int ch, unsigned int value)
{
    assert (ch > 0 && ch <= TimerHardware<Base>::channels);
    assert (value <= TimerHardware<Base>::max);
    *(&TIM_CCR1 (Base) + ch - 1) = value;
}

template<uint32_t Base>
unsigned int
TimerHard<Base>::get_value ()
{
    return TIM_CNT (Base);
}

template<uint32_t Base>
unsigned int
TimerHard<Base>::get_input_capture (int ch) const
{
    assert (ch > 0 && ch <= TimerHardware<Base>::channels);
    return *(&TIM_CCR1 (Base) + ch - 1);
}

template<uint32_t Base>
void
TimerHard<Base>::wait_update () const
{
    unsigned int mask = TIM_SR_UIF;
    while (!(TIM_SR (Base) & mask))
        ;
    TIM_SR (Base) = ~mask;
}

template<uint32_t Base>
void
TimerHard<Base>::wait_input_capture (int ch) const
{
    unsigned int mask = TIM_SR_CC1IF >> 1 << ch;
    while (!(TIM_SR (Base) & mask))
        ;
    TIM_SR (Base) = ~mask;
}

template<uint32_t Base>
void
TimerHard<Base>::enable_interrupt ()
{
    TIM_DIER (Base) |= TIM_DIER_UIE;
}

template<uint32_t Base>
void
TimerHard<Base>::disable_interrupt ()
{
    TIM_DIER (Base) &= ~TIM_DIER_UIE;
}

template<uint32_t Base>
void
TimerHard<Base>::clear_interrupt ()
{
    TIM_SR (Base) = ~TIM_SR_UIF;
}

template<uint32_t Base>
void
TimerHard<Base>::enable_updates ()
{
    TIM_CR1 (Base) &= ~TIM_CR1_UDIS;
}

template<uint32_t Base>
void
TimerHard<Base>::disable_updates ()
{
    TIM_CR1 (Base) |= TIM_CR1_UDIS;
}

template<uint32_t Base>
TimerHard<Base>::UpdateDisabled::UpdateDisabled (TimerHard<Base> &timer)
    : timer_ (timer)
{
    timer_.disable_updates ();
}

template<uint32_t Base>
TimerHard<Base>::UpdateDisabled::~UpdateDisabled ()
{
    timer_.enable_updates ();
}

template<uint32_t Base>
struct TimerHard<Base>::Option
{
    static const uint32_t base = Base;
    static const unsigned cr1 = 0;
    static const unsigned smcr = 0;
    static const unsigned ccmr1 = 0;
    static const unsigned ccmr2 = 0;
    static const unsigned ccer = 0;
    static const unsigned arr = 0;
    static const unsigned ccr1 = 0;
    static const unsigned ccr2 = 0;
    static const unsigned ccr3 = 0;
    static const unsigned ccr4 = 0;
};

template<uint32_t Base>
template<unsigned int value>
struct TimerHard<Base>::OptionReloadValue : public TimerHard<Base>::Option
{
    static_assert (value <= TimerHardware<Base>::max, "value too large");
    static const unsigned arr = value;
};

template<uint32_t Base>
struct TimerHard<Base>::OptionOnePulse : public TimerHard<Base>::Option
{
    static_assert (TimerHardware<Base>::one_pulse, "no one pulse mode");
    static const unsigned cr1 = TIM_CR1_OPM;
};

template<uint32_t Base>
template<int timer_input>
struct TimerHard<Base>::OptionTrigger : public TimerHard<Base>::Option
{
    static_assert (TimerHardware<Base>::slave, "no external trigger");
    static_assert (timer_input >= 1 && timer_input <= 2, "no such input");
    static const unsigned smcr =
        (timer_input == 1 ? TIM_SMCR_TS_IT1FP1 : TIM_SMCR_TS_IT1FP2)
        | TIM_SMCR_SMS_TM;
};

template<uint32_t Base>
template<int channel, typename TimerHard<Base>::Filter filter,
    typename TimerHard<Base>::Map map,
    typename TimerHard<Base>::Polarity polarity>
struct TimerHard<Base>::OptionInputCapture
{
    static_assert (channel == 1, "no such channel");
    // Always activate input capture (through Polarity values), even when only
    // used as a trigger, no harm is done.
};

template<uint32_t Base>
template<typename TimerHard<Base>::Filter filter,
    typename TimerHard<Base>::Map map,
    typename TimerHard<Base>::Polarity polarity>
struct TimerHard<Base>::OptionInputCapture<1, filter, map, polarity>
    : public TimerHard<Base>::Option
{
    static_assert (1 <= TimerHardware<Base>::channels, "no such channel");
    static const unsigned ccmr1 =
        (static_cast<int> (filter)
         * (TIM_CCMR1_IC1F_MASK & ~(TIM_CCMR1_IC1F_MASK - 1)))
        | (static_cast<int> (map)
           * (TIM_CCMR1_CC1S_MASK & ~(TIM_CCMR1_CC1S_MASK - 1)));
    static const unsigned ccer = static_cast<int> (polarity) * TIM_CCER_CC1E;
};

template<uint32_t Base>
template<typename TimerHard<Base>::Filter filter,
    typename TimerHard<Base>::Map map,
    typename TimerHard<Base>::Polarity polarity>
struct TimerHard<Base>::OptionInputCapture<2, filter, map, polarity>
    : public TimerHard<Base>::Option
{
    static_assert (2 <= TimerHardware<Base>::channels, "no such channel");
    static const unsigned ccmr1 =
        (static_cast<int> (filter)
         * (TIM_CCMR1_IC2F_MASK & ~(TIM_CCMR1_IC2F_MASK - 1)))
        | (static_cast<int> (map)
           * (TIM_CCMR1_CC2S_MASK & ~(TIM_CCMR1_CC2S_MASK - 1)));
    static const unsigned ccer = static_cast<int> (polarity) * TIM_CCER_CC2E;
};

template<uint32_t Base>
template<typename TimerHard<Base>::Filter filter,
    typename TimerHard<Base>::Map map,
    typename TimerHard<Base>::Polarity polarity>
struct TimerHard<Base>::OptionInputCapture<3, filter, map, polarity>
    : public TimerHard<Base>::Option
{
    static_assert (3 <= TimerHardware<Base>::channels, "no such channel");
    static const unsigned ccmr2 =
        (static_cast<int> (filter)
         * (TIM_CCMR2_IC3F_MASK & ~(TIM_CCMR2_IC3F_MASK - 1)))
        | (static_cast<int> (map)
           * (TIM_CCMR2_CC3S_MASK & ~(TIM_CCMR2_CC3S_MASK - 1)));
    static const unsigned ccer = static_cast<int> (polarity) * TIM_CCER_CC3E;
};

template<uint32_t Base>
template<typename TimerHard<Base>::Filter filter,
    typename TimerHard<Base>::Map map,
    typename TimerHard<Base>::Polarity polarity>
struct TimerHard<Base>::OptionInputCapture<4, filter, map, polarity>
    : public TimerHard<Base>::Option
{
    static_assert (4 <= TimerHardware<Base>::channels, "no such channel");
    static const unsigned ccmr2 =
        (static_cast<int> (filter)
         * (TIM_CCMR2_IC4F_MASK & ~(TIM_CCMR2_IC4F_MASK - 1)))
        | (static_cast<int> (map)
           * (TIM_CCMR2_CC4S_MASK & ~(TIM_CCMR2_CC4S_MASK - 1)));
    static const unsigned ccer = static_cast<int> (polarity) * TIM_CCER_CC4E;
};

template<uint32_t Base>
template<int channel, typename TimerHard<Base>::Polarity polarity>
struct TimerHard<Base>::OptionOutputCompare
{
    static_assert (channel == 1, "no such channel");
};

template<uint32_t Base>
template<typename TimerHard<Base>::Polarity polarity>
struct TimerHard<Base>::OptionOutputCompare<1, polarity>
    : public TimerHard<Base>::Option
{
    static_assert (1 <= TimerHardware<Base>::channels, "no such channel");
    static const unsigned ccmr1 = TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC1PE;
    static const unsigned ccer = static_cast<int> (polarity) * TIM_CCER_CC1E;
};

template<uint32_t Base>
template<typename TimerHard<Base>::Polarity polarity>
struct TimerHard<Base>::OptionOutputCompare<2, polarity>
    : public TimerHard<Base>::Option
{
    static_assert (2 <= TimerHardware<Base>::channels, "no such channel");
    static const unsigned ccmr1 = TIM_CCMR1_OC2M_PWM1 | TIM_CCMR1_OC2PE;
    static const unsigned ccer = static_cast<int> (polarity) * TIM_CCER_CC2E;
};

template<uint32_t Base>
template<typename TimerHard<Base>::Polarity polarity>
struct TimerHard<Base>::OptionOutputCompare<3, polarity>
    : public TimerHard<Base>::Option
{
    static_assert (3 <= TimerHardware<Base>::channels, "no such channel");
    static const unsigned ccmr2 = TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE;
    static const unsigned ccer = static_cast<int> (polarity) * TIM_CCER_CC3E;
};

template<uint32_t Base>
template<typename TimerHard<Base>::Polarity polarity>
struct TimerHard<Base>::OptionOutputCompare<4, polarity>
    : public TimerHard<Base>::Option
{
    static_assert (4 <= TimerHardware<Base>::channels, "no such channel");
    static const unsigned ccmr2 = TIM_CCMR2_OC4M_PWM1 | TIM_CCMR2_OC4PE;
    static const unsigned ccer = static_cast<int> (polarity) * TIM_CCER_CC4E;
};

template<uint32_t Base>
template<int channel, unsigned int value>
struct TimerHard<Base>::OptionOutputCompareValue
    : public TimerHard<Base>::Option
{
    static_assert (channel == 1, "no such channel");
};

template<uint32_t Base>
template<unsigned int value>
struct TimerHard<Base>::OptionOutputCompareValue<1, value>
    : public TimerHard<Base>::Option
{
    static_assert (1 <= TimerHardware<Base>::channels, "no such channel");
    static_assert (value <= TimerHardware<Base>::max, "value too large");
    static const unsigned ccr1 = value;
};

template<uint32_t Base>
template<unsigned int value>
struct TimerHard<Base>::OptionOutputCompareValue<2, value>
    : public TimerHard<Base>::Option
{
    static_assert (2 <= TimerHardware<Base>::channels, "no such channel");
    static_assert (value <= TimerHardware<Base>::max, "value too large");
    static const unsigned ccr2 = value;
};

template<uint32_t Base>
template<unsigned int value>
struct TimerHard<Base>::OptionOutputCompareValue<3, value>
    : public TimerHard<Base>::Option
{
    static_assert (3 <= TimerHardware<Base>::channels, "no such channel");
    static_assert (value <= TimerHardware<Base>::max, "value too large");
    static const unsigned ccr3 = value;
};

template<uint32_t Base>
template<unsigned int value>
struct TimerHard<Base>::OptionOutputCompareValue<4, value>
    : public TimerHard<Base>::Option
{
    static_assert (4 <= TimerHardware<Base>::channels, "no such channel");
    static_assert (value <= TimerHardware<Base>::max, "value too large");
    static const unsigned ccr4 = value;
};

template<uint32_t Base>
template<typename... Options>
struct TimerHard<Base>::CombinedOptions : public TimerHard<Base>::Option
{
};

template<uint32_t Base>
template<typename Option, typename... Options>
struct TimerHard<Base>::CombinedOptions<Option, Options...>
{
    static_assert (Base == Option::base, "option for another timer");
    static const uint32_t base = Base;
    static const unsigned cr1 = Option::cr1
        | CombinedOptions<Options...>::cr1;
    static const unsigned smcr = Option::smcr
        | CombinedOptions<Options...>::smcr;
    static const unsigned ccmr1 = Option::ccmr1
        | CombinedOptions<Options...>::ccmr1;
    static const unsigned ccmr2 = Option::ccmr2
        | CombinedOptions<Options...>::ccmr2;
    static const unsigned ccer = Option::ccer
        | CombinedOptions<Options...>::ccer;
    static const unsigned arr = Option::arr
        | CombinedOptions<Options...>::arr;
    static_assert (Option::arr == 0 || Option::arr == arr,
                   "conflicting reload values");
    static const unsigned ccr1 = Option::ccr1
        | CombinedOptions<Options...>::ccr1;
    static_assert (Option::ccr1 == 0 || Option::ccr1 == ccr1,
                   "conflicting ccr1 values");
    static const unsigned ccr2 = Option::ccr2
        | CombinedOptions<Options...>::ccr2;
    static_assert (Option::ccr2 == 0 || Option::ccr2 == ccr2,
                   "conflicting ccr2 values");
    static const unsigned ccr3 = Option::ccr3
        | CombinedOptions<Options...>::ccr3;
    static_assert (Option::ccr3 == 0 || Option::ccr3 == ccr3,
                   "conflicting ccr3 values");
    static const unsigned ccr4 = Option::ccr4
        | CombinedOptions<Options...>::ccr4;
    static_assert (Option::ccr4 == 0 || Option::ccr4 == ccr4,
                   "conflicting ccr4 values");
};

} // namespace ucoo

#endif // ucoo_hal_timer_timer_stm32_tcc
