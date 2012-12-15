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
#include "ucoolib/utils/fifo.hh"
#include "ucoolib/arch/arch.hh"
#include "ucoolib/base/test/test.hh"

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::Test test ("fifo");
    {
        test.begin ("push pop");
        ucoo::Fifo<int, 16> fifo;
        fifo.push (1);
        fifo.push (2);
        fifo.push (3);
        if (fifo.pop () != 1 || fifo.pop () != 2 || fifo.pop () != 3)
            test.fail ();
        else
            test.pass ();
    }
    {
        test.begin ("full empty");
        ucoo::Fifo<int, 4> fifo;
        do
        {
            test_fail_break_unless (test, fifo.empty () && !fifo.full ());
            fifo.push (1);
            test_fail_break_unless (test, !fifo.empty () && !fifo.full ());
            fifo.push (2);
            test_fail_break_unless (test, !fifo.empty () && !fifo.full ());
            fifo.push (3);
            test_fail_break_unless (test, !fifo.empty () && fifo.full ());
            test.pass ();
        } while (0);
    }
    {
        test.begin ("write read");
        ucoo::Fifo<int, 8> fifo;
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
            test.pass ();
        } while (0);
    }
    return test.report () ? 0 : 1;
}
