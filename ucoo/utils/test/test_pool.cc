// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2015 Nicolas Schodet
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
#include "ucoo/utils/pool.hh"
#include "ucoo/arch/arch.hh"
#include "ucoo/base/test/test.hh"
#include "ucoo/common.hh"

struct A
{
    A () { n++; };
    ~A () { n--; };
    static int n;
};

int A::n = 0;

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::TestSuite tsuite ("pool");
    do {
        ucoo::Test test (tsuite, "int pool");
        ucoo::Pool<int, 4> pool;
        int *ar[5];
        for (int i = 0; i < ucoo::lengthof (ar); i++)
            ar[i] = pool.construct (i);
        test_fail_break_unless (test, ar[0] && *ar[0] == 0);
        test_fail_break_unless (test, ar[1] && *ar[1] == 1);
        test_fail_break_unless (test, ar[2] && *ar[2] == 2);
        test_fail_break_unless (test, ar[3] && *ar[3] == 3);
        test_fail_break_unless (test, !ar[4]);
        for (int i = 0; i < ucoo::lengthof (ar) - 1; i++)
            pool.destroy (ar[i]);
        for (int i = 0; i < ucoo::lengthof (ar); i++)
            ar[i] = pool.construct (i);
        test_fail_break_unless (test, ar[0] && *ar[0] == 0);
        test_fail_break_unless (test, ar[1] && *ar[1] == 1);
        test_fail_break_unless (test, ar[2] && *ar[2] == 2);
        test_fail_break_unless (test, ar[3] && *ar[3] == 3);
        test_fail_break_unless (test, !ar[4]);
        for (int i = 0; i < ucoo::lengthof (ar) - 1; i++)
            pool.destroy (ar[i]);
    } while (0);
    do {
        ucoo::Test test (tsuite, "object pool");
        ucoo::Pool<A, 4> pool;
        A *ar[5];
        for (int i = 0; i < ucoo::lengthof (ar); i++)
            ar[i] = pool.construct ();
        test_fail_break_unless (test, A::n == 4);
        test_fail_break_unless (test, ar[0] && ar[1] && ar[2] && ar[3]);
        test_fail_break_unless (test, !ar[4]);
        for (int i = 0; i < ucoo::lengthof (ar) - 1; i++)
            pool.destroy (ar[i]);
        test_fail_break_unless (test, A::n == 0);
        for (int i = 0; i < ucoo::lengthof (ar); i++)
            ar[i] = pool.construct ();
        test_fail_break_unless (test, A::n == 4);
        test_fail_break_unless (test, ar[0] && ar[1] && ar[2] && ar[3]);
        test_fail_break_unless (test, !ar[4]);
        for (int i = 0; i < ucoo::lengthof (ar) - 1; i++)
            pool.destroy (ar[i]);
    } while (0);
    return tsuite.report () ? 0 : 1;
}
