#ifndef ucoolib_intf_adc_hh
#define ucoolib_intf_adc_hh
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

namespace ucoo {

/// Analog to digital converter.  This is a really simple interface which
/// should be enough for simple needs.
class Adc
{
    /// Make a conversion and return the value.
    virtual int read () = 0;
    /// Get resolution (number of possible different values).
    virtual int get_resolution () const = 0;
  protected:
    /// Default constructor.
    Adc () { }
};

} // namespace ucoo

#endif // ucoolib_intf_adc_hh
