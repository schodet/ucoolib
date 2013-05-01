#ifndef ucoolib_utils_crc_hh
#define ucoolib_utils_crc_hh
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
#include "ucoolib/common.hh"

/// Please read "A PAINLESS GUIDE TO CRC ERROR DETECTION ALGORITHMS",
/// http://www.ross.net/crc/download/crc_v3.txt

namespace ucoo {

/// Dallas/Maxim iButton 8bit CRC.
/// Polynomial: x^8 + x^5 + x^4 + 1
/// Initial value: 0
static inline uint8_t
crc8_update (uint8_t crc, uint8_t data)
{
    uint8_t i;
    crc = crc ^ data;
    for (i = 0; i < 8; i++)
    {
        if (crc & 0x01)
            crc = (crc >> 1) ^ 0x8C;
        else
            crc >>= 1;
    }
    return crc;
}

uint8_t
crc8_compute (const uint8_t *data, int size);

/// Name   : "CRC-32"
/// Width  : 32
/// Poly   : 04C11DB7
/// Init   : FFFFFFFF
/// RefIn  : True
/// RefOut : True
/// XorOut : FFFFFFFF
/// Check  : CBF43926
uint32_t
crc32_update (uint32_t crc, uint8_t data);

uint32_t
crc32_compute (const uint8_t *data, int size);

} // namespace ucoo

#endif // ucoolib_utils_crc_hh
