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
#include "ucoo/base/test/test.hh"
#include "ucoo/arch/arch.hh"
#include "ucoo/hal/timer/timer.hh"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include <cstdio>

int
main (int argc, const char **argv)
{
    // PD13 (T4_CH2) is used as input capture.
    // PWM signal is output on PA1 (T2_CH2), there is a button on PA0.
    // Connect PD13 on PA0 or PA1.  On each edge, there will be a pulse on
    // PD15 (T4_CH4).
    ucoo::arch_init (argc, argv);
    ucoo::test_stream_setup ();
    using Timer = ucoo::TimerHard<TIM4>;
    using TimerRef = ucoo::TimerHard<TIM2>;
    Timer timer;
    TimerRef timerref;
    // AF setup.
    rcc_periph_clock_enable (RCC_GPIOA);
    rcc_periph_clock_enable (RCC_GPIOD);
    gpio_mode_setup (GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO1);
    gpio_set_af (GPIOA, GPIO_AF1, GPIO1);
    gpio_mode_setup (GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO13 | GPIO15);
    gpio_set_af (GPIOD, GPIO_AF2, GPIO13 | GPIO15);
    // Timers setup.
    timerref.enable<TimerRef::OptionReloadValue<5000>,
        TimerRef::OptionOutputCompare<2> > (2000);
    timerref.set_output_compare (2, 2500);
    timer.enable<Timer::OptionOnePulse,
        Timer::OptionReloadValue<1000>,
        Timer::OptionTrigger<2>,
        Timer::OptionInputCapture<2, Timer::Filter::OFF, Timer::Map::SAME,
            Timer::Polarity::BOTH>,
        Timer::OptionOutputCompare<4, Timer::Polarity::INVERTED> > (2000);
    timer.set_output_compare (4, 500);
    while (1)
        printf ("%u\n", timer.get_value ());
}

