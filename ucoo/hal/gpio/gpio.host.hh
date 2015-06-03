#ifndef ucoo_hal_gpio_gpio_host_hh
#define ucoo_hal_gpio_gpio_host_hh
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
#include "ucoo/intf/io.hh"
#include "ucoo/arch/host/host.hh"

namespace ucoo {

class GpioShared;

/// General purpose input/output on host.
class Gpio : public Io
{
  public:
    /// Initialise GPIO.
    Gpio (Host &host, const char *name);
    /// Initialise GPIO, with no mex support.
    Gpio ();
    /// See Io::set.
    void set ();
    /// See Io::reset.
    void reset ();
    /// See Io::set.
    void set (bool state);
    /// See Io::toggle.
    void toggle ();
    /// See Io::get.
    bool get () const;
    /// See Io::input.
    void input ();
    /// See Io::output.
    void output ();
  private:
    /// Name.
    const char *name_;
    /// Current input/output states.
    bool input_, output_;
    /// Current direction, true for output.
    bool direction_output_;
    /// Shared context.
    static GpioShared *shared_;
    friend class GpioShared;
};

} // namespace ucoo

#endif // ucoo_hal_gpio_gpio_host_hh
