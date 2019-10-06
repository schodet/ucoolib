#ifndef ucoolib_utils_crc_hh
#define ucoolib_utils_crc_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
