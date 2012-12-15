#ifndef ucoolib_common_hh
#define ucoolib_common_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include <stdint.h>

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
    if (!condition)
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
    if (!condition)
        halt_perror ();
}

} // namespace ucoo

/// Get array length at compile time, must be a macro until c++11.
#define lengthof(array) (sizeof (array) / sizeof((array)[0]))

#endif // ucoolib_common_h
