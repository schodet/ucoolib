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
#include "ucoolib/arch/arch.hh"
#include "ucoolib/common.hh"

#include <cstdlib>
#include <cstdio>
#include <time.h>
#include <errno.h>

namespace ucoo {

static int arch_stored_argc;
static const char **arch_stored_argv;

void
arch_init (int argc, const char **argv)
{
    arch_stored_argc = argc;
    arch_stored_argv = argv;
}

void
arch_reset ()
{
    // Not supported.
    halt ();
}

void
arch_get_args (int &argc, const char **&argv)
{
    argc = arch_stored_argc;
    argv = arch_stored_argv;
}

void
halt ()
{
    fprintf (stderr, "halt\n");
    abort ();
}

void
halt_perror ()
{
    perror ("halt");
    abort ();
}

void
yield ()
{
    // Do a small delay.
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 100 * 1000 * 1000;
    while (nanosleep (&ts, &ts) == -1 && errno == EINTR)
	;
}

} // namespace ucoo
