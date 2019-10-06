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
