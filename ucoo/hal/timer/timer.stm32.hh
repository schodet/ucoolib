#ifndef ucoo_hal_timer_timer_stm32_hh
#define ucoo_hal_timer_timer_stm32_hh
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
#include "ucoo/common.hh"

namespace ucoo {

/// STM32 timer very basic support.
template<uint32_t Base>
class TimerHard
{
  public:
    /// Maximum timer value.
    static const unsigned int max;
  public:
    /// Destructor, disable.
    ~TimerHard ();
    /// Enable timer.
    template <typename... Options>
    void enable (int freq, bool start = true);
    /// Disable timer.
    void disable ();
    /// Manually start timer.
    void start ();
    /// Set reload value.
    void set_reload (unsigned int value);
    /// Set output compare value.
    void set_output_compare (int ch, unsigned int value);
    /// Read timer current value.
    static unsigned int get_value ();
    /// Read input capture value.
    unsigned int get_input_capture (int ch) const;
    /// Wait until update event.
    void wait_update () const;
    /// Wait until a new input capture value is available.
    void wait_input_capture (int ch) const;
    /// Get timer frequency.
    int get_freq () const { return freq_; }
    /// Enable interrupts on update event.
    void enable_interrupt ();
    /// Disable interrupts on update event.
    void disable_interrupt ();
    /// Clear update event interrupt flag.
    void clear_interrupt ();
  private:
    /// Enable updates (reload value, output compare values...).
    void enable_updates ();
    /// Disable updates.
    void disable_updates ();
  public:
    /// Filter for timer input.
    enum class Filter { OFF, CK_INT_N_2, CK_INT_N_4, CK_INT_N_8,
        DTF_DIV_2_N_6, DTF_DIV_2_N_8, DTF_DIV_4_N_6, DTF_DIV_4_N_8,
        DTF_DIV_8_N_6, DTF_DIV_8_N_8, DTF_DIV_16_N_5, DTF_DIV_16_N_6,
        DTF_DIV_16_N_8, DTF_DIV_32_N_5, DTF_DIV_32_N_6, DTF_DIV_32_N_8 };
    /// Mapping from timer input to input capture.
    enum class Map {
        SAME = 1, // Same timer input (for example TI1 for IC1).
        OTHER = 2, // Other timer input (for example TI2 for IC1).
        TRC = 3, // Internal trigger.
    };
    /// Polarity for input capture or output compare.  Edge is only used for
    /// input.
    enum class Polarity {
        NON_INVERTED = 1,
        INVERTED = 3,
        RISING = NON_INVERTED,
        FALLING = INVERTED,
        BOTH = 11,
    };
    /// Temporarily disable updates.
    class UpdateDisabled
    {
      public:
        /// Disable updates.
        UpdateDisabled (TimerHard<Base> &timer);
        /// Enable updates.
        ~UpdateDisabled ();
      private:
        TimerHard<Base> &timer_;
    };
    friend class UpdateDisabled;
    /// Base class for options.
    struct Option;
    /// Set the automatic reload value.
    template<unsigned int value>
    struct OptionReloadValue;
    /// Set the timer in one pulse mode.  You may want to unset start
    /// parameter.
    struct OptionOnePulse;
    /// Set timer trigger input.
    template<int timer_input>
    struct OptionTrigger;
    /// Set input capture options.
    template<int channel, Filter filter = Filter::OFF, Map map = Map::SAME,
        Polarity polarity = Polarity::NON_INVERTED>
    struct OptionInputCapture;
    /// Set output compare options.  Setup PWM mode 1.
    template<int channel, Polarity polarity = Polarity::NON_INVERTED>
    struct OptionOutputCompare;
    /// Set output compare value.
    template<int channel, unsigned int value>
    struct OptionOutputCompareValue;
  private:
    /// Helper to aggregate all options.
    template<typename... Options>
    struct CombinedOptions;
  private:
    /// When enabled, programmed frequency.
    int freq_ = 0;
};

} // namespace ucoo

#include "ucoo/hal/timer/timer.stm32.tcc"

#endif // ucoo_hal_timer_timer_stm32_hh
