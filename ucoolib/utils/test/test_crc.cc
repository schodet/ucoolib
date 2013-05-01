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
#include "ucoolib/utils/crc.hh"
#include "ucoolib/arch/arch.hh"
#include "ucoolib/base/test/test.hh"

#include <cstring>

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::TestSuite tsuite ("crc");
    {
        ucoo::Test test (tsuite, "crc8 test vector");
        static const uint8_t test_vector[] = { 0x02, 0x1c, 0xb8, 0x01, 0, 0, 0, 0xa2 };
        if (ucoo::crc8_compute (test_vector, lengthof (test_vector)) != 0)
            test.fail ();
    }
    {
        ucoo::Test test (tsuite, "crc32 test vector");
        const char *check_str = "123456789";
        if (ucoo::crc32_compute ((const uint8_t *) check_str,
                                 std::strlen (check_str)) != 0xCBF43926)
            test.fail ();
    }
    return tsuite.report () ? 0 : 1;
}
