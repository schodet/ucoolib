#ifndef ucoo_intf_exti_hh
#define ucoo_intf_exti_hh
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
#include "ucoo/utils/function.hh"

namespace ucoo {

/// External interrupt interface.
class Exti
{
  public:
    /// Enable.
    virtual void enable () = 0;
    /// Disable.
    virtual void disable () = 0;
    /// Temporary mask interrupt, return previous state.
    virtual bool mask () = 0;
    /// Unmask interrupt.
    virtual void unmask (bool saved_state) = 0;
    /// Register a handler called on event.
    template<typename F>
    void register_event (F handler)
        { handler_ = handler; }
  protected:
    /// Default constructor.
    Exti () { }
  protected:
    /// Handler called on event.
    Function<void ()> handler_;
};

} // namespace ucoo

#endif // ucoo_intf_exti_hh
