#ifndef ucoolib_dev_usdist_usdist_hh
#define ucoolib_dev_usdist_usdist_hh
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
