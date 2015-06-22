#ifndef ucoo_base_test_test_hh
#define ucoo_base_test_test_hh
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
#include "ucoo/intf/stream.hh"

/// Test helpers to fail and break to end of current block.
#define test_fail_break_unless(test, cond) \
    ({ if (!(cond)) { \
        test.fail ("%s", #cond); \
        break; \
     } })

namespace ucoo {

/// Get a reference to test stream.  This stream is the preferred way to
/// interact with a tester.
Stream &
test_stream (void);

/// Setup stdin and stdout to use the reference test stream.  On host, this
/// function does nothing.
void
test_stream_setup (void);

/// Test suite context.
class TestSuite
{
  public:
    /// Create a new test suite.
    TestSuite (const char *test_suite);
    /// Report the overall test results, return true if all tests passed.
    bool report ();
    /// Enter a new test group.
    void group (const char *test_group);
  private:
    friend class Test;
    /// Test suite name.
    const char *test_suite_;
    /// Test group name.
    const char *test_group_;
    /// Number of attempted tests.
    int test_nb_;
    /// Number of test failures.
    int fail_nb_;
    /// Running a test.
    bool in_test_;
};

/// Test context.
class Test
{
  public:
    /// Start a new test, may be followed by fail, test stops at object
    /// destruction.
    Test (TestSuite &suite, const char *test);
    /// Stop a test.
    ~Test (void);
    /// End the current test with a fail status.
    void fail ();
    /// End the current test with a fail status and message.
    void __attribute__ ((format (printf, 2, 3)))
        fail (const char *fail_fmt, ...);
    /// Give info about the current test.
    void __attribute__ ((format (printf, 2, 3)))
        info (const char *info_fmt, ...);
  private:
    /// Attached test suite.
    TestSuite &suite_;
    /// Test name.
    const char *test_;
    /// Failed yet.
    bool failed_;
};

} // namespace ucoo

#endif // ucoo_base_test_test_hh
