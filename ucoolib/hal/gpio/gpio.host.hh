#ifndef ucoolib_hal_gpio_gpio_host_hh
#define ucoolib_hal_gpio_gpio_host_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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
#include "ucoolib/intf/io.hh"
#include "ucoolib/arch/host/host.hh"

namespace ucoo {

class GpioShared;

/// General purpose input/output on host.
class Gpio : public Io
{
  public:
    /// Initialise GPIO.
    Gpio (Host &host, const char *name);
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

#endif // ucoolib_hal_gpio_gpio_host_hh
