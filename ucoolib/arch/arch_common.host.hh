#ifndef ucoolib_arch_arch_common_host_hh
#define ucoolib_arch_arch_common_host_hh
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

/// Type used to save irq state.
typedef unsigned int irq_flags_t;

static inline irq_flags_t
irq_lock (void)
{
    // Nothing on host, there is no interrupts.
    return 0;
}

inline void
irq_restore (irq_flags_t)
{
    // Nothing on host, there is no interrupts.
}

} // namespace ucoo

#endif // ucoolib_arch_arch_common_host_hh
