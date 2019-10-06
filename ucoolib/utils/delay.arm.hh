#ifndef ucoolib_utils_delay_arm_hh
#define ucoolib_utils_delay_arm_hh
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

namespace ucoo {

/// Wait for an integral number of microseconds, used to implement delay.
void
delay_us (int us);

/// Wait for a small integral number of nanoseconds, used to implement short
/// delay.
void
delay_ns (int us);

/// Wait for the specified delay in seconds.
extern inline void
delay (double s)
{
    if (s < 1e-6)
        delay_ns (static_cast<int> (s * 1e9));
    else
        delay_us (static_cast<int> (s * 1e6));
}
extern inline void
delay (double s) __attribute__ ((always_inline));

} // namespace ucoo

#endif // ucoolib_utils_delay_arm_hh
