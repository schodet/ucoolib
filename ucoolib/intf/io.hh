#ifndef ucoolib_ucoolib_intf_io_hh
#define ucoolib_ucoolib_intf_io_hh
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

/// One bit input/output interface.
class Io
{
  public:
    /// Set output to 1.
    virtual void set () = 0;
    /// Reset output to 0.
    virtual void reset () = 0;
    /// Set or reset output.
    virtual void set (bool state) = 0;
    /// Toggle output.
    virtual void toggle () = 0;
    /// Get input state (not the same as output latch!).
    virtual bool get () const = 0;
    /// Set as input.
    virtual void input () = 0;
    /// Set as output.
    virtual void output () = 0;
  protected:
    /// Default constructor.
    Io () { }
};

} // namespace ucoo

#endif // ucoolib_ucoolib_intf_io_hh
