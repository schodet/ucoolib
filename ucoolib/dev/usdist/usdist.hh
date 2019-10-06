#ifndef ucoolib_dev_usdist_usdist_hh
#define ucoolib_dev_usdist_usdist_hh
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
#include "ucoolib/intf/adc.hh"
#include "ucoolib/intf/io.hh"

namespace ucoo {

class UsDistControl;

/// Single US distance sensor.
class UsDist
{
  public:
    /// Constructor.
    UsDist (UsDistControl &ctrl, Adc &adc, Io &io,
            int distance_min, int distance_max, int distance_too_far = -1,
            int resolution = -1);
    /// Enable measure.
    void enable ();
    /// Disable measure.
    void disable ();
    /// Read current value.
    int read ();
    /// Return last measured distance.
    int get () const { return distance_; }
  private:
    Adc &adc_;
    Io &io_;
    UsDist *next_;
    int distance_min_, distance_max_, distance_too_far_, resolution_;
    int distance_;
    friend class UsDistControl;
};

/// Handle a set of US distance sensors.
class UsDistControl
{
  public:
    /// Constructor.
    UsDistControl (int period);
    /// Make measurements, return true if a new measure is available.
    bool update ();
  private:
    /// Number of updates between measures.
    int period_;
    /// Pointer to linked list of sensors.
    UsDist *sensors_;
    /// Current measuring sensor.
    UsDist *current_;
    /// Number of updates before the next action.
    int wait_;
    friend class UsDist;
};

} // namespace ucoo

#endif // ucoolib_dev_usdist_usdist_hh
