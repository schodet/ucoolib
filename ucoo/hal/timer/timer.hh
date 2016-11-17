#ifndef ucoo_hal_timer_timer_hh
#define ucoo_hal_timer_timer_hh
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
#include <cstdio>

#if defined TARGET_stm32
# include "timer.stm32.hh"
#else
# error "not implemented for this target"
#endif

namespace ucoo {

/// Use a timer as a trace time stamp.
template<typename Timer>
struct TimerTraceTimestamp
{
    struct Entry
    {
        uint32_t timestamp;
    };
    void operator () (Entry &e)
    {
        e.timestamp = Timer::get_value ();
    }
    int dump (const Entry &e, const Entry &eprev, char *buf) const
    {
        return sprintf (buf, "[%+8d] ", static_cast<int> (e.timestamp - eprev.timestamp));
    }
};

} // namespace ucoo

#endif // ucoo_hal_timer_timer_hh
