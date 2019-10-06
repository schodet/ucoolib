#ifndef ucoolib_utils_delay_hh
#define ucoolib_utils_delay_hh
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

#if defined (TARGET_host)

namespace ucoo {

/// No delay on host.
extern inline void
delay (double s)
{
}

} // namespace ucoo

#elif defined (TARGET_arm)
# include "delay.arm.hh"
#else
# error "not implemented for this target"
#endif

namespace ucoo {

/// Shortcut for delay with nanosecond.
extern inline void
delay_ns (double ns)
{
    delay (ns * 1e-9);
}
extern inline void
delay_ns (double ns) __attribute__ ((always_inline));

/// Shortcut for delay with microsecond.
extern inline void
delay_us (double us)
{
    delay (us * 1e-6);
}
extern inline void
delay_us (double us) __attribute__ ((always_inline));

/// Shortcut for delay with millisecond.
extern inline void
delay_ms (double ms)
{
    delay (ms * 1e-3);
}
extern inline void
delay_ms (double ms) __attribute__ ((always_inline));

} // namespace ucoo

#endif // ucoolib_utils_delay_hh
