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
