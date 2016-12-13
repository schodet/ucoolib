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
#include "ucoo/arch/rcc.stm32.hh"
#include "ucoo/arch/reg.hh"

namespace ucoo {

// TODO add more timers.

template<>
struct TimerHardware<TimerInstance::TIM1>
{
    static constexpr TIM_TypeDef *base = reg::TIM1;
    static const Rcc rcc = Rcc::TIM1;
    static int freq_hz () { return rcc_apb2_timer_freq_hz; }
    static const unsigned int max = 0xffff;
    static const bool advanced = true;
    static const bool slave = true;
    static const bool one_pulse = true;
    static const int channels = 4;
};

template<>
struct TimerHardware<TimerInstance::TIM2>
{
    static constexpr TIM_TypeDef *base = reg::TIM2;
    static const Rcc rcc = Rcc::TIM2;
    static int freq_hz () { return rcc_apb1_timer_freq_hz; }
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
struct TimerHardware<TimerInstance::TIM3>
{
    static constexpr TIM_TypeDef *base = reg::TIM3;
    static const Rcc rcc = Rcc::TIM3;
    static int freq_hz () { return rcc_apb1_timer_freq_hz; }
    static const unsigned int max = 0xffff;
    static const bool advanced = false;
    static const bool slave = true;
    static const bool one_pulse = true;
    static const int channels = 4;
};

template<>
struct TimerHardware<TimerInstance::TIM4>
{
    static constexpr TIM_TypeDef *base = reg::TIM4;
    static const Rcc rcc = Rcc::TIM4;
    static int freq_hz () { return rcc_apb1_timer_freq_hz; }
    static const unsigned int max = 0xffff;
    static const bool advanced = false;
    static const bool slave = true;
    static const bool one_pulse = true;
    static const int channels = 4;
};

template<>
struct TimerHardware<TimerInstance::TIM5>
{
    static constexpr TIM_TypeDef *base = reg::TIM5;
    static const Rcc rcc = Rcc::TIM5;
    static int freq_hz () { return rcc_apb1_timer_freq_hz; }
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
struct TimerHardware<TimerInstance::TIM10>
{
    static constexpr TIM_TypeDef *base = reg::TIM10;
    static const Rcc rcc = Rcc::TIM10;
    static int freq_hz () { return rcc_apb2_timer_freq_hz; }
    static const unsigned int max = 0xffff;
    static const bool advanced = false;
    static const bool slave = false;
    static const bool one_pulse = false;
    static const int channels = 1;
};

template<>
struct TimerHardware<TimerInstance::TIM11>
{
    static constexpr TIM_TypeDef *base = reg::TIM11;
    static const Rcc rcc = Rcc::TIM10;
    static int freq_hz () { return rcc_apb2_timer_freq_hz; }
    static const unsigned int max = 0xffff;
    static const bool advanced = false;
    static const bool slave = false;
    static const bool one_pulse = false;
    static const int channels = 1;
};

template<TimerInstance inst>
const unsigned int TimerHard<inst>::max = TimerHardware<inst>::max;

template<TimerInstance inst>
TimerHard<inst>::~TimerHard ()
{
    disable ();
}

template<TimerInstance inst>
template<typename... Options>
void
TimerHard<inst>::enable (int freq_hz, bool start)
{
    rcc_peripheral_clock_enable (Hard::rcc);
    int in_freq_hz = Hard::freq_hz ();
    int div = in_freq_hz / freq_hz;
    assert (div <= 0x10000 && in_freq_hz / div == freq_hz);
    freq_hz_ = freq_hz;
    using OptionsCombined = CombinedOptions<Options...>;
    Hard::base->CR1 = TIM_CR1_ARPE | OptionsCombined::cr1;
    Hard::base->CR2 = 0;
    if (Hard::slave)
        Hard::base->SMCR = OptionsCombined::smcr;
    if (Hard::channels > 0)
    {
        Hard::base->CCER = 0;
        Hard::base->CCMR1 = OptionsCombined::ccmr1;
        if (Hard::channels > 2)
            Hard::base->CCMR2 = OptionsCombined::ccmr2;
        Hard::base->CCER = OptionsCombined::ccer;
        Hard::base->CCR1 = OptionsCombined::ccr1;
        if (Hard::channels >= 2)
            Hard::base->CCR2 = OptionsCombined::ccr2;
        if (Hard::channels >= 3)
            Hard::base->CCR3 = OptionsCombined::ccr3;
        if (Hard::channels >= 4)
            Hard::base->CCR4 = OptionsCombined::ccr4;
    }
    Hard::base->PSC = div - 1;
    Hard::base->ARR = OptionsCombined::arr ? OptionsCombined::arr : Hard::max;
    if (Hard::advanced)
        Hard::base->BDTR = TIM_BDTR_MOE;
    Hard::base->EGR = TIM_EGR_UG;
    if (start)
        Hard::base->CR1 = TIM_CR1_ARPE | OptionsCombined::cr1 | TIM_CR1_CEN;
}

template<TimerInstance inst>
void
TimerHard<inst>::disable ()
{
    Hard::base->CR1 = 0;
    rcc_peripheral_clock_disable (TimerHardware<inst>::rcc);
    freq_hz_ = 0;
}

template<TimerInstance inst>
void
TimerHard<inst>::start ()
{
    Hard::base->CR1 |= TIM_CR1_CEN;
}

template<TimerInstance inst>
void
TimerHard<inst>::set_reload (unsigned int value)
{
    assert (value <= Hard::max);
    Hard::base->ARR = value;
}

template<TimerInstance inst>
void
TimerHard<inst>::set_output_compare (int ch, unsigned int value)
{
    assert (ch > 0 && ch <= Hard::channels);
    assert (value <= Hard::max);
    *(&Hard::base->CCR1 + ch - 1) = value;
}

template<TimerInstance inst>
unsigned int
TimerHard<inst>::get_value ()
{
    return Hard::base->CNT;
}

template<TimerInstance inst>
unsigned int
TimerHard<inst>::get_input_capture (int ch) const
{
    assert (ch > 0 && ch <= Hard::channels);
    return *(&Hard::base->CCR1 + ch - 1);
}

template<TimerInstance inst>
void
TimerHard<inst>::wait_update () const
{
    unsigned int mask = TIM_SR_UIF;
    while (!(Hard::base->SR & mask))
        ;
    Hard::base->SR = ~mask;
}

template<TimerInstance inst>
void
TimerHard<inst>::wait_input_capture (int ch) const
{
    unsigned int mask = TIM_SR_CC1IF >> 1 << ch;
    while (!(Hard::base->SR & mask))
        ;
    Hard::base->SR = ~mask;
}

template<TimerInstance inst>
void
TimerHard<inst>::enable_interrupt ()
{
    Hard::base->DIER |= TIM_DIER_UIE;
}

template<TimerInstance inst>
void
TimerHard<inst>::disable_interrupt ()
{
    Hard::base->DIER &= ~TIM_DIER_UIE;
}

template<TimerInstance inst>
void
TimerHard<inst>::clear_interrupt ()
{
    Hard::base->SR = ~TIM_SR_UIF;
}

template<TimerInstance inst>
void
TimerHard<inst>::enable_updates ()
{
    Hard::base->CR1 &= ~TIM_CR1_UDIS;
}

template<TimerInstance inst>
void
TimerHard<inst>::disable_updates ()
{
    Hard::base->CR1 |= TIM_CR1_UDIS;
}

template<TimerInstance inst>
TimerHard<inst>::UpdateDisabled::UpdateDisabled (TimerHard<inst> &timer)
    : timer_ (timer)
{
    timer_.disable_updates ();
}

template<TimerInstance inst>
TimerHard<inst>::UpdateDisabled::~UpdateDisabled ()
{
    timer_.enable_updates ();
}

template<TimerInstance inst>
struct TimerHard<inst>::Option
{
    static const TimerInstance opinst = inst;
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

template<TimerInstance inst>
template<unsigned int value>
struct TimerHard<inst>::OptionReloadValue : public TimerHard<inst>::Option
{
    static_assert (value <= Hard::max, "value too large");
    static const unsigned arr = value;
};

template<TimerInstance inst>
struct TimerHard<inst>::OptionOnePulse : public TimerHard<inst>::Option
{
    static_assert (Hard::one_pulse, "no one pulse mode");
    static const unsigned cr1 = TIM_CR1_OPM;
};

template<TimerInstance inst>
template<int timer_input>
struct TimerHard<inst>::OptionTrigger : public TimerHard<inst>::Option
{
    static_assert (Hard::slave, "no external trigger");
    static_assert (timer_input >= 1 && timer_input <= 2, "no such input");
    static const unsigned smcr =
        (timer_input == 1 ? TIM_SMCR_TS_TI1FP1 : TIM_SMCR_TS_TI1FP2)
        | TIM_SMCR_SMS_TM;
};

template<TimerInstance inst>
template<int channel, typename TimerHard<inst>::Filter filter,
    typename TimerHard<inst>::Map map,
    typename TimerHard<inst>::Polarity polarity>
struct TimerHard<inst>::OptionInputCapture
{
    static_assert (channel == 1, "no such channel");
    // Always activate input capture (through Polarity values), even when only
    // used as a trigger, no harm is done.
};

template<TimerInstance inst>
template<typename TimerHard<inst>::Filter filter,
    typename TimerHard<inst>::Map map,
    typename TimerHard<inst>::Polarity polarity>
struct TimerHard<inst>::OptionInputCapture<1, filter, map, polarity>
    : public TimerHard<inst>::Option
{
    static_assert (1 <= TimerHard<inst>::Hard::channels, "no such channel");
    static const unsigned ccmr1 =
        (static_cast<int> (filter)
         * (TIM_CCMR1_IC1F & ~(TIM_CCMR1_IC1F - 1)))
        | (static_cast<int> (map)
           * (TIM_CCMR1_CC1S & ~(TIM_CCMR1_CC1S - 1)));
    static const unsigned ccer = static_cast<int> (polarity) * TIM_CCER_CC1E;
};

template<TimerInstance inst>
template<typename TimerHard<inst>::Filter filter,
    typename TimerHard<inst>::Map map,
    typename TimerHard<inst>::Polarity polarity>
struct TimerHard<inst>::OptionInputCapture<2, filter, map, polarity>
    : public TimerHard<inst>::Option
{
    static_assert (2 <= TimerHard<inst>::Hard::channels, "no such channel");
    static const unsigned ccmr1 =
        (static_cast<int> (filter)
         * (TIM_CCMR1_IC2F & ~(TIM_CCMR1_IC2F - 1)))
        | (static_cast<int> (map)
           * (TIM_CCMR1_CC2S & ~(TIM_CCMR1_CC2S - 1)));
    static const unsigned ccer = static_cast<int> (polarity) * TIM_CCER_CC2E;
};

template<TimerInstance inst>
template<typename TimerHard<inst>::Filter filter,
    typename TimerHard<inst>::Map map,
    typename TimerHard<inst>::Polarity polarity>
struct TimerHard<inst>::OptionInputCapture<3, filter, map, polarity>
    : public TimerHard<inst>::Option
{
    static_assert (3 <= TimerHard<inst>::Hard::channels, "no such channel");
    static const unsigned ccmr2 =
        (static_cast<int> (filter)
         * (TIM_CCMR2_IC3F & ~(TIM_CCMR2_IC3F - 1)))
        | (static_cast<int> (map)
           * (TIM_CCMR2_CC3S & ~(TIM_CCMR2_CC3S - 1)));
    static const unsigned ccer = static_cast<int> (polarity) * TIM_CCER_CC3E;
};

template<TimerInstance inst>
template<typename TimerHard<inst>::Filter filter,
    typename TimerHard<inst>::Map map,
    typename TimerHard<inst>::Polarity polarity>
struct TimerHard<inst>::OptionInputCapture<4, filter, map, polarity>
    : public TimerHard<inst>::Option
{
    static_assert (4 <= TimerHard<inst>::Hard::channels, "no such channel");
    static const unsigned ccmr2 =
        (static_cast<int> (filter)
         * (TIM_CCMR2_IC4F & ~(TIM_CCMR2_IC4F - 1)))
        | (static_cast<int> (map)
           * (TIM_CCMR2_CC4S & ~(TIM_CCMR2_CC4S - 1)));
    static const unsigned ccer = static_cast<int> (polarity) * TIM_CCER_CC4E;
};

template<TimerInstance inst>
template<int channel, typename TimerHard<inst>::Polarity polarity,
    typename TimerHard<inst>::Complementary complementary>
struct TimerHard<inst>::OptionOutputCompare
{
    static_assert (channel == 1, "no such channel");
};

template<TimerInstance inst>
template<typename TimerHard<inst>::Polarity polarity,
    typename TimerHard<inst>::Complementary complementary>
struct TimerHard<inst>::OptionOutputCompare<1, polarity, complementary>
    : public TimerHard<inst>::Option
{
    static_assert (1 <= TimerHard<inst>::Hard::channels, "no such channel");
    static_assert (complementary == TimerHard<inst>::Complementary::DO_NOT_USE
                   || TimerHard<inst>::Hard::advanced,
                   "no complementary output");
    static const unsigned ccmr1 = TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC1PE;
    static const unsigned ccer = static_cast<int> (complementary)
        * static_cast<int> (polarity) * TIM_CCER_CC1E;
};

template<TimerInstance inst>
template<typename TimerHard<inst>::Polarity polarity,
    typename TimerHard<inst>::Complementary complementary>
struct TimerHard<inst>::OptionOutputCompare<2, polarity, complementary>
    : public TimerHard<inst>::Option
{
    static_assert (2 <= TimerHard<inst>::Hard::channels, "no such channel");
    static_assert (complementary == TimerHard<inst>::Complementary::DO_NOT_USE
                   || TimerHard<inst>::Hard::advanced,
                   "no complementary output");
    static const unsigned ccmr1 = TIM_CCMR1_OC2M_PWM1 | TIM_CCMR1_OC2PE;
    static const unsigned ccer = static_cast<int> (complementary)
        * static_cast<int> (polarity) * TIM_CCER_CC2E;
};

template<TimerInstance inst>
template<typename TimerHard<inst>::Polarity polarity,
    typename TimerHard<inst>::Complementary complementary>
struct TimerHard<inst>::OptionOutputCompare<3, polarity, complementary>
    : public TimerHard<inst>::Option
{
    static_assert (3 <= TimerHard<inst>::Hard::channels, "no such channel");
    static_assert (complementary == TimerHard<inst>::Complementary::DO_NOT_USE
                   || TimerHard<inst>::Hard::advanced,
                   "no complementary output");
    static const unsigned ccmr2 = TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE;
    static const unsigned ccer = static_cast<int> (complementary)
        * static_cast<int> (polarity) * TIM_CCER_CC3E;
};

template<TimerInstance inst>
template<typename TimerHard<inst>::Polarity polarity,
    typename TimerHard<inst>::Complementary complementary>
struct TimerHard<inst>::OptionOutputCompare<4, polarity, complementary>
    : public TimerHard<inst>::Option
{
    static_assert (4 <= TimerHard<inst>::Hard::channels, "no such channel");
    static_assert (complementary == TimerHard<inst>::Complementary::DO_NOT_USE
                   || TimerHard<inst>::Hard::advanced,
                   "no complementary output");
    static const unsigned ccmr2 = TIM_CCMR2_OC4M_PWM1 | TIM_CCMR2_OC4PE;
    static const unsigned ccer = static_cast<int> (complementary)
        * static_cast<int> (polarity) * TIM_CCER_CC4E;
};

template<TimerInstance inst>
template<int channel, unsigned int value>
struct TimerHard<inst>::OptionOutputCompareValue
    : public TimerHard<inst>::Option
{
    static_assert (channel == 1, "no such channel");
};

template<TimerInstance inst>
template<unsigned int value>
struct TimerHard<inst>::OptionOutputCompareValue<1, value>
    : public TimerHard<inst>::Option
{
    static_assert (1 <= TimerHard<inst>::Hard::channels, "no such channel");
    static_assert (value <= TimerHard<inst>::Hard::max, "value too large");
    static const unsigned ccr1 = value;
};

template<TimerInstance inst>
template<unsigned int value>
struct TimerHard<inst>::OptionOutputCompareValue<2, value>
    : public TimerHard<inst>::Option
{
    static_assert (2 <= TimerHard<inst>::Hard::channels, "no such channel");
    static_assert (value <= TimerHard<inst>::Hard::max, "value too large");
    static const unsigned ccr2 = value;
};

template<TimerInstance inst>
template<unsigned int value>
struct TimerHard<inst>::OptionOutputCompareValue<3, value>
    : public TimerHard<inst>::Option
{
    static_assert (3 <= TimerHard<inst>::Hard::channels, "no such channel");
    static_assert (value <= TimerHard<inst>::Hard::max, "value too large");
    static const unsigned ccr3 = value;
};

template<TimerInstance inst>
template<unsigned int value>
struct TimerHard<inst>::OptionOutputCompareValue<4, value>
    : public TimerHard<inst>::Option
{
    static_assert (4 <= TimerHard<inst>::Hard::channels, "no such channel");
    static_assert (value <= TimerHard<inst>::Hard::max, "value too large");
    static const unsigned ccr4 = value;
};

template<TimerInstance inst>
template<typename... Options>
struct TimerHard<inst>::CombinedOptions : public TimerHard<inst>::Option
{
};

template<TimerInstance inst>
template<typename Option, typename... Options>
struct TimerHard<inst>::CombinedOptions<Option, Options...>
{
    static_assert (inst == Option::opinst, "option for another timer");
    static const TimerInstance opinst = inst;
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
