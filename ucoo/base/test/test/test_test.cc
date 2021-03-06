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
#include "ucoo/base/test/test.hh"
#include "ucoo/arch/arch.hh"

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::TestSuite test_suite ("test_test");
    // Really, what a dumb test!
    test_suite.group ("group one");
    {
        ucoo::Test test (test_suite, "the first test");
        test.fail ("Oh no! test %d!", 123);
    }
    {
        ucoo::Test test (test_suite, "try again");
        test.info ("working harder...");
    }
    test_suite.group ("group two");
    {
        ucoo::Test test (test_suite, "simple test");
    }
    return test_suite.report () ? 0 : 1;
}

