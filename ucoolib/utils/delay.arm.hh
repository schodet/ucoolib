#ifndef ucoolib_utils_delay_arm_hh
#define ucoolib_utils_delay_arm_hh
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

/// Wait for an integral number of microseconds, used to implement delay.
void
delay_us (int us);

/// Wait for a small integral number of nanoseconds, used to implement short
/// delay.
void
delay_ns (int us);

/// Wait for the specified delay in seconds.
extern inline void
delay (double s)
{
    if (s < 1e-6)
        delay_ns (static_cast<int> (s * 1e9));
    else
        delay_us (static_cast<int> (s * 1e6));
}
extern inline void
delay (double s) __attribute__ ((always_inline));

} // namespace ucoo

#endif // ucoolib_utils_delay_arm_hh
