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
#include "mex_socket.hh"

#include "ucoolib/common.hh"

#include "config/arch/host/mex.hh"
#include "socket.h"

#include <unistd.h>
#include <cstdlib>

namespace ucoo {
namespace mex {

Socket::~Socket ()
{
    if (fd_ != -1)
        close (fd_);
}

void
Socket::connect ()
{
    fd_ = socket_client UCOO_CONFIG_ARCH_HOST_MEX_DEFAULT_ADDRESS;
}

int
Socket::size ()
{
    uint8_t header[3];
    int r = ::read (fd_, header, sizeof (header));
    assert_perror (r != -1);
    if (r == 0)
        exit (0);
    else
    {
        assert (r == sizeof (header));
        read_size_ = (header[0] << 8) | header[1];
        seq_ = header[2];
        return read_size_;
    }
}

void
Socket::read (char *buf)
{
    assert (fd_ != -1);
    assert (read_size_ > 0);
    int r = ::read (fd_, buf, read_size_);
    assert_perror (r != -1);
    assert (r == read_size_);
    read_size_ = -1;
}

void
Socket::write (const char *buf, int count)
{
    assert (fd_ != -1);
    assert (read_size_ == -1);
    uint8_t header[3];
    header[0] = count >> 8;
    header[1] = count;
    header[2] = seq_;
    int r = ::write (fd_, header, sizeof (header));
    assert_perror (r != -1);
    assert (r == sizeof (header));
    r = ::write (fd_, buf, count);
    assert_perror (r != -1);
    assert (r == count);
}

} // namespace mex
} // namespace ucoo
