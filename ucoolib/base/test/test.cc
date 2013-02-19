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
#include "test.hh"

#include "ucoolib/common.hh"
#include "ucoolib/base/stdio/stdio.hh"
#include "ucoolib/utils/delay.hh"

#include "config/base/test.hh"

#include <cstdarg>

namespace ucoo {

TestSuite::TestSuite (const char *test_suite)
    : test_suite_ (test_suite), test_group_ ("none"),
      test_nb_ (0), fail_nb_ (0), in_test_ (false)
{
    test_stream_setup ();
    if (UCOO_CONFIG_BASE_TEST_WAIT)
    {
        Stream &s = test_stream ();
        int n;
        char b;
        s.block (false);
        do
        {
            printf ("waiting to start...\n");
            delay (1);
            n = s.read (&b, 1);
        } while (n <= 0);
        s.block ();
    }
}

bool
TestSuite::report ()
{
    assert (!in_test_);
    printf ("test results: %d tests, %d fails\n", test_nb_, fail_nb_);
    return fail_nb_ == 0;
}

void
TestSuite::group (const char *test_group)
{
    test_group_ = test_group;
}

Test::Test (TestSuite &suite, const char *test)
    : suite_ (suite), test_ (test), failed_ (false)
{
    assert (!suite_.in_test_);
    suite_.in_test_ = true;
    suite_.test_nb_++;
}

Test::~Test (void)
{
    if (!failed_)
        printf ("%s:%s:%s:P: pass\n", suite_.test_suite_, suite_.test_group_,
                test_);
    suite_.in_test_ = false;
}

void
Test::fail ()
{
    fail ("fail");
}

void
Test::fail (const char *fail_fmt, ...)
{
    va_list ap;
    assert (!failed_);
    printf ("%s:%s:%s:F: ", suite_.test_suite_, suite_.test_group_, test_);
    va_start (ap, fail_fmt);
    vprintf (fail_fmt, ap);
    va_end (ap);
    putchar ('\n');
    suite_.fail_nb_++;
    failed_ = true;
}

void
Test::info (const char *info_fmt, ...)
{
    va_list ap;
    printf ("%s:%s:%s:I: ", suite_.test_suite_, suite_.test_group_, test_);
    va_start (ap, info_fmt);
    vprintf (info_fmt, ap);
    va_end (ap);
    putchar ('\n');
}

} // namespace ucoo
