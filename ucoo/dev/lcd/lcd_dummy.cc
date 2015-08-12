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
#include "ucoo/dev/lcd/lcd_dummy.hh"
#include "ucoo/base/test/test.hh"

#include <cstdio>
#include <cstdarg>

namespace ucoo {

void
LcdDummy::enable ()
{
    test_stream_setup ();
}

void
LcdDummy::disable ()
{
}

void
LcdDummy::puts (const char *str)
{
    std::puts (str);
}

void
LcdDummy::go (int line, int column)
{
}

void
LcdDummy::clear ()
{
}

int
LcdDummy::get_lines () const
{
    return 0;
}

int
LcdDummy::get_columns () const
{
    return 0;
}

void
LcdDummy::printf (const char *fmt, ...)
{
    std::va_list ap;
    va_start (ap, fmt);
    std::vprintf (fmt, ap);
    va_end (ap);
}

} // namespace ucoo
