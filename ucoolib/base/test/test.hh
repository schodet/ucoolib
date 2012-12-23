#ifndef ucoolib_base_test_test_hh
#define ucoolib_base_test_test_hh
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
#include "ucoolib/intf/stream.hh"

/// Test helpers to fail and break to end of current block.
#define test_fail_break_unless(test, cond) \
    ({ if (!(cond)) { \
        test.fail (#cond); \
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

/// Test context.
class Test
{
  public:
    /// Create a new test suite.
    Test (const char *test_suite);
    /// Report the overall test results, return true if all tests passed.
    bool report ();
    /// Enter a new test group.
    void group (const char *test_group);
    /// Start a new test, should be followed by pass or fail.
    void begin (const char *test);
    /// End the current test with a pass status.
    void pass ();
    /// End the current test with a fail status.
    void fail ();
    /// End the current test with a fail status and message.
    void __attribute__ ((format (printf, 2, 3)))
        fail (const char *fail_fmt, ...);
    /// Give info about the current test.
    void __attribute__ ((format (printf, 2, 3)))
        info (const char *info_fmt, ...);
  private:
    /// Test suite name.
    const char *test_suite_;
    /// Test group name.
    const char *test_group_;
    /// Test name.
    const char *test_;
    /// Number of attempted tests.
    int test_nb_;
    /// Number of test failures.
    int fail_nb_;
};

} // namespace ucoo

#endif // ucoolib_base_test_test_hh
