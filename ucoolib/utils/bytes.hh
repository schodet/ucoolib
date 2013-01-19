#ifndef ucoolib_ucoolib_utils_bytes_hh
#define ucoolib_ucoolib_utils_bytes_hh
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

/// Pack two bytes to one 16 bit word.
static inline uint16_t
bytes_pack (uint8_t b1, uint8_t b0)
{
    return b1 << 8 | b0;
}

/// Pack four bytes to one 32 bit word.
static inline uint32_t
bytes_pack (uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0)
{
    return b3 << 24 | b2 << 16 | b1 << 8 | b0;
}

/// Unpack one byte from one 32 bit word.
static inline uint8_t
bytes_unpack (uint32_t w, int n)
{
    return (w >> (n * 8)) & 0xff;
}

} // namespace ucoo

#endif // ucoolib_ucoolib_utils_bytes_hh
