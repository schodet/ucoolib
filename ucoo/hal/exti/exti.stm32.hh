#ifndef ucoo_hal_exti_exti_stm32_hh
#define ucoo_hal_exti_exti_stm32_hh
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
#include "ucoo/intf/exti.hh"

#include <libopencm3/stm32/exti.h>

namespace ucoo {

/// External interrupt on STM32.
class ExtiHard : public Exti
{
  public:
    enum class Edge
    {
        RISING = 1,
        FALLING = 2,
        BOTH = 3
    };
  public:
    /// Constructor, take the EXTI number.
    ExtiHard (int n);
    /// Constructor, shortcut to set source.
    ExtiHard (uint32_t gpio_port, int n);
    /// Constructor, shortcut to set source and trigger.
    ExtiHard (uint32_t gpio_port, int n, Edge edge);
    /// Destructor.
    ~ExtiHard ();
    /// See Exti::enable.
    void enable ();
    /// See Exti::disable.
    void disable ();
    /// See Exti::mask.
    bool mask ();
    /// See Exti::unmask.
    void unmask (bool saved_state);
    /// Set trigger edge.
    void set_trigger (Edge edge);
    /// Select source, take GPIO port address.
    void set_source (uint32_t gpio_port);
    /// Handle interrupts.
    void isr ();
  private:
    /// EXTI line number.
    int n_;
};

inline
ExtiHard::ExtiHard (uint32_t gpio_port, int n)
    : ExtiHard (n)
{
    set_source (gpio_port);
}

inline
ExtiHard::ExtiHard (uint32_t gpio_port, int n, Edge edge)
    : ExtiHard (gpio_port, n)
{
    set_trigger (edge);
}

} // namespace ucoo

#endif // ucoo_hal_exti_exti_stm32_hh
