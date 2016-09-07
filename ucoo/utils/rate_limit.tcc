#ifndef ucoo_utils_rate_limit_tcc
#define ucoo_utils_rate_limit_tcc
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

template<typename Timer>
RateLimit<Timer>::RateLimit (const Timer &timer)
    : timer_ (timer)
{
    static_assert ((Timer::max & (Timer::max + 1)) == 0,
                   "timer max value plus one should be a power of two");
}

template<typename Timer>
RateLimit<Timer>::RateLimit (const Timer &timer, int rate_num, int rate_denum)
    : RateLimit (timer)
{
    set_limit (rate_num, rate_denum);
}

template<typename Timer>
void
RateLimit<Timer>::set_limit (int rate_num, int rate_denum)
{
    int freq = timer_.get_freq_hz ();
    interval_ = freq * rate_denum / rate_num;
    ucoo::assert (interval_ <= Timer::max / 2 + 1);
    next_ = timer_.get_value ();
}

template<typename Timer>
bool
RateLimit<Timer>::operator () ()
{
    unsigned int val = timer_.get_value ();
    const unsigned int sign_bit = Timer::max & ~(Timer::max >> 1);
    if ((val - next_) & sign_bit)
        return false;
    else
    {
        next_ = val + interval_;
        return true;
    }
}

template<typename Timer>
void
RateLimit<Timer>::refresh ()
{
    unsigned int val = timer_.get_value ();
    const unsigned int sign_bit = Timer::max & ~(Timer::max >> 1);
    if (!((val - next_) & sign_bit))
        next_ = val;
}

template<typename Timer>
void
RateLimit<Timer>::reset ()
{
    next_ = timer_.get_value () + interval_;
}

} // namespace ucoo

#endif // ucoo_utils_rate_limit_tcc
