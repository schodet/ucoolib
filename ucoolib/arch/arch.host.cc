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
