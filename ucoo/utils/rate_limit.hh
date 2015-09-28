#ifndef ucoo_utils_rate_limit_hh
#define ucoo_utils_rate_limit_hh
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

namespace ucoo {

/// Rate limiting object, call it to ask for permission to issue an event.
/// Care should be taken to ask for permission often enough to avoid timer
/// overflow problems or to call refresh.
template<typename Timer>
class RateLimit
{
  public:
    /// Constructor.
    RateLimit (const Timer &timer);
    /// Constructor.  Set rate limit, in Hz.  Timer frequency must be known
    /// yet.
    RateLimit (const Timer &timer, int rate_num, int rate_denum = 1);
    /// Set rate limit, in Hz.
    inline void set_limit (int rate_num, int rate_denum = 1);
    /// Ask for permission, return true if allowed.
    bool operator () ();
    /// Refresh timestamp to avoid overflow, must be called periodically when
    /// permission is not requested often enough.
    void refresh ();
  private:
    /// Associated timer.
    const Timer &timer_;
    /// Minimum interval between events.
    unsigned int interval_ = 0;
    /// Next event timer value.
    unsigned int next_ = 0;
};

} // namespace ucoo

#include "ucoo/utils/rate_limit.tcc"

#endif // ucoo_utils_rate_limit_hh
