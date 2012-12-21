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
#include "stdio.hh"

#include <unistd.h>

namespace ucoo {

/// Callback to read a stream.
ssize_t
stream_io_read (void *cookie, char *buf, size_t n)
{
    Stream *stream = static_cast<Stream *> (cookie);
    int r = stream->read (buf, n);
    if (r == -2)
        return 0;
    else
        return r;
}

/// Callback to write a stream.
ssize_t
stream_io_write (void *cookie, const char *buf, size_t n)
{
    Stream *stream = static_cast<Stream *> (cookie);
    int r = stream->write (buf, n);
    return r;
}

/// Table of callback for fopencookie.
static const cookie_io_functions_t stream_io_functions =
{
    stream_io_read,
    stream_io_write,
    NULL,
    NULL,
};

FILE *
fstreamopen (Stream &stream, const char *mode)
{
    // fopencookie is a glibc extension also implemented in newlib, nice!
    return fopencookie (static_cast<void *> (&stream), mode,
                        stream_io_functions);
}

} // namespace ucoo
