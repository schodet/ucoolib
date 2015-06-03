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
#include "test.hh"

#include "ucoo/common.hh"
#include "ucoo/base/stdio/stdio.hh"
#include "ucoo/utils/delay.hh"

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
