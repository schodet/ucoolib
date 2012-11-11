#ifndef ucoolib_ucoolib_intf_io_hh
#define ucoolib_ucoolib_intf_io_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
