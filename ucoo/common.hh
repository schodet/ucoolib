#ifndef ucoo_common_hh
#define ucoo_common_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include <stdint.h>

#include "ucoo/arch/arch_common.hh"

namespace ucoo {

/// Integer which is read and written atomically.
typedef int int_atomic_t;

/// Compiler barrier.  Prevents the compiler from moving the memory accesses
/// from one side of it to the other side.
extern inline void
barrier ()
{
    __asm__ __volatile__("": : : "memory");
}

/// Ensure a single access is done, avoid compiler optimisations.
template<typename T>
extern inline volatile T &
access_once (T &x)
{
    return *static_cast<volatile T *> (&x);
}

/// Stop, abruptly.
void
halt () __attribute__ ((noreturn));

/// Stop, try to output error description corresponding to errno.
void
halt_perror () __attribute__ ((noreturn));

/// To be used to swear that the given condition is true.  If this is not the
/// case... well... you swore!
extern inline void
assert (bool condition)
{
    if (!__builtin_expect (condition, 1))
        halt ();
}

/// To be used to swear that this code can never be reached.
void
assert_unreachable () __attribute__ ((noreturn));
extern inline void
assert_unreachable ()
{
    halt ();
}

/// To be used to swear that the given condition is true, but print errno
/// description anyway in case you lied.
extern inline void
assert_perror (bool condition)
{
    if (!__builtin_expect (condition, 1))
        halt_perror ();
}

} // namespace ucoo

/// Get array length at compile time, must be a macro until c++11.
#define lengthof(array) (sizeof (array) / sizeof((array)[0]))

#endif // ucoo_common_h
