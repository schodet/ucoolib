// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include "ucoo/utils/fifo.hh"
#include "ucoo/arch/arch.hh"
#include "ucoo/base/test/test.hh"

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::TestSuite tsuite ("fifo");
    {
        ucoo::Test test (tsuite, "push pop");
        ucoo::Fifo<int, 15> fifo;
        fifo.push (1);
        fifo.push (2);
        fifo.push (3);
        if (fifo.pop () != 1 || fifo.pop () != 2 || fifo.pop () != 3)
            test.fail ();
    }
    {
        ucoo::Test test (tsuite, "full empty");
        ucoo::Fifo<int, 3> fifo;
        do
        {
            test_fail_break_unless (test, fifo.empty () && !fifo.full ());
            fifo.push (1);
            test_fail_break_unless (test, !fifo.empty () && !fifo.full ());
            fifo.push (2);
            test_fail_break_unless (test, !fifo.empty () && !fifo.full ());
            fifo.push (3);
            test_fail_break_unless (test, !fifo.empty () && fifo.full ());
        } while (0);
    }
    {
        ucoo::Test test (tsuite, "write read");
        ucoo::Fifo<int, 7> fifo;
        static const int b[] = { 1, 2, 3, 4, 5 };
        int c[8];
        int r;
        do
        {
            r = fifo.write (b, 5);
            test_fail_break_unless (test, r == 5);
            r = fifo.read (c, 2);
            test_fail_break_unless (test, r == 2 && c[0] == 1 && c[1] == 2);
            r = fifo.write (b, 5);
            test_fail_break_unless (test, r == 4);
            r = fifo.read (c, 8);
            test_fail_break_unless (test, r == 7);
            test_fail_break_unless (test, c[0] == 3 && c[1] == 4 && c[2] == 5);
            test_fail_break_unless (test, c[3] == 1 && c[4] == 2 && c[5] == 3
                                    && c[6] == 4);
        } while (0);
    }
    return tsuite.report () ? 0 : 1;
}
