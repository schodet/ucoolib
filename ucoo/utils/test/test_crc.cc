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
#include "ucoo/utils/crc.hh"
#include "ucoo/arch/arch.hh"
#include "ucoo/base/test/test.hh"

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
