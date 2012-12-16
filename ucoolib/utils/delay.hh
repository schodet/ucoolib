#ifndef ucoolib_utils_delay_hh
#define ucoolib_utils_delay_hh
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

#if defined (TARGET_host)

namespace ucoo {

/// No delay on host.
extern inline void
delay (double s)
{
}

} // namespace ucoo

#elif defined (TARGET_arm)
# include "delay.arm.hh"
#else
# error "not implemented for this target"
#endif

namespace ucoo {

/// Shortcut for delay with nanosecond.
extern inline void
delay_ns (double ns)
{
    delay (ns * 1e-9);
}
extern inline void
delay_ns (double ns) __attribute__ ((always_inline));

/// Shortcut for delay with microsecond.
extern inline void
delay_us (double us)
{
    delay (us * 1e-6);
}
extern inline void
delay_us (double us) __attribute__ ((always_inline));

/// Shortcut for delay with millisecond.
extern inline void
delay_ms (double ms)
{
    delay (ms * 1e-3);
}
extern inline void
delay_ms (double ms) __attribute__ ((always_inline));

} // namespace ucoo

#endif // ucoolib_utils_delay_hh
