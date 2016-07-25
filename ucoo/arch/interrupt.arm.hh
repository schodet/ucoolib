#ifndef ucoo_arch_interrupt_arm_hh
#define ucoo_arch_interrupt_arm_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2016 Nicolas Schodet
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
#include "ucoo/arch/reg.hh"

#if defined (TARGET_stm32f4)
# include "ucoo/arch/interrupt.stm32f4.hh"
#elif defined (TARGET_stm32f1)
# include "ucoo/arch/interrupt.stm32f1.hh"
#else
# error "not implemented for this target"
#endif

namespace ucoo {

/// Standard ARM Cortex exceptions.
enum class Exception
{
    RESET = 1,
    NMI = 2,
    HARD_FAULT = 3,
    MEMORY_MANAGEMENT_FAULT = 4,
    BUS_FAULT = 5,
    USAGE_FAULT = 6,
    SV_CALL = 11,
    PEND_SV = 14,
    SYSTICK = 15,
    COUNT
};

/// Exception vectors are a specialisation of this template.
template<Exception>
void
interrupt ();

/// IRQ vectors are a specialisation of this template.
template<Irq>
void
interrupt ();

/// Enable interrupt source.
static inline void
interrupt_enable (Irq irq)
{
    const unsigned int n = static_cast<unsigned int> (irq);
    NVIC->ISER[n / 32] = 1 << (n % 32);
}

/// Disable interrupt source.
static inline void
interrupt_disable (Irq irq)
{
    const unsigned int n = static_cast<unsigned int> (irq);
    NVIC->ICER[n / 32] = 1 << (n % 32);
}

/// Test if a interrupt source is enabled.
static inline bool
interrupt_is_enabled (Irq irq)
{
    const unsigned int n = static_cast<unsigned int> (irq);
    return NVIC->ISER[n / 32] & (1 << (n % 32));
}

/// Set interrupt priority. Warning: a limited number of MSB bits are used
/// depending on the hardware implementation.
static inline void
interrupt_set_priority (Irq irq, uint8_t priority)
{
    const unsigned int n = static_cast<unsigned int> (irq);
    NVIC->IP[n] = priority;
}

/// Set exception priority. Warning: a limited number of MSB bits are used
/// depending on the hardware implementation.
static inline void
interrupt_set_priority (Exception ex, uint8_t priority)
{
    const unsigned int n = static_cast<unsigned int> (ex);
    SCB->SHP[n - 4] = priority;
}

} // namespace ucoo

#endif // ucoo_arch_interrupt_arm_hh
