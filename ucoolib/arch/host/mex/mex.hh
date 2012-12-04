#ifndef ucoolib_arch_host_mex_mex_hh
#define ucoolib_arch_host_mex_mex_hh
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
namespace mex {

/// Message type identifiers.
enum mtype_t
{
    /// IDLE message, sent by nodes to the hub to signal it had handled all
    /// messages.  It can have an optional parameter: the date at which the
    /// node will not be idle anymore.
    MTYPE_IDLE = 0,
    /// DATE message, sent by the hub to the nodes to update the current date
    /// value, sent by a node to request the current date (useful at startup).
    MTYPE_DATE = 1,
    /// REQ (request) message, sent from a node to other nodes to request a
    /// response message.
    MTYPE_REQ = 2,
    /// RSP (response) message, response to a REQ message, only sent to the
    /// requesting node.
    MTYPE_RSP = 3,
    /// RES (reserve) message, sent to the hub to request a message type, sent
    /// to a node with the allocated message type.
    MTYPE_RES = 4,
    /// First unreserved message identifier, to be used by user messages.
    MTYPE_USER_MIN = 0x20,
};

} // namespace mex
} // namespace ucoo

#endif // ucoolib_arch_host_mex_mex_hh
