#ifndef ucoolib_arch_arch_common_arm_hh
#define ucoolib_arch_arch_common_arm_hh
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

namespace ucoo {

/// Type used to save irq state.
typedef unsigned int irq_flags_t;

static inline irq_flags_t
irq_lock (void)
{
    irq_flags_t flags;
    __asm__ __volatile__ ("mrs %0, PRIMASK\n\t"
                          "cpsid i"
                          : "=r" (flags) :
                          : "memory", "cc");
    return flags;
}

inline void
irq_restore (irq_flags_t flags)
{
    __asm__ __volatile__ ("msr PRIMASK, %0"
                          : : "r" (flags)
                          : "memory", "cc");
}

inline void
yield ()
{
    // Nothing, the CPU is ours!
}

} // namespace ucoo

#endif // ucoolib_arch_arch_common_arm_hh
