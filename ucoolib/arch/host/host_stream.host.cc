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
#include "host_stream.hh"

#include "ucoolib/common.hh"

#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pty.h>
#include <fcntl.h>
#include <errno.h>

namespace ucoo {

/// Setup non canonical mode.
static void
setup_raw (int fd)
{
    struct termios tc;
    tcgetattr (fd, &tc);
    tc.c_iflag &= ~(IGNPAR | PARMRK | ISTRIP | IGNBRK | BRKINT | IGNCR |
		    ICRNL | INLCR | IXON | IXOFF | IXANY | IMAXBEL);
    tc.c_iflag |= INPCK;
    tc.c_oflag &= ~(OPOST);
    tc.c_cflag &= ~(HUPCL | CSTOPB | PARENB | PARODD | CSIZE);
    tc.c_cflag |= CS8 | CLOCAL | CREAD;
    tc.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN | NOFLSH | TOSTOP);
    tc.c_cc[VTIME] = 0;
    tc.c_cc[VMIN] = 1;
    tcflush (fd, TCIFLUSH);
    tcsetattr (fd, TCSANOW, &tc);
}

HostStream::HostStream ()
    : fdi_ (0), fdo_ (1)
{
}

HostStream::HostStream (const char *name)
    : fdi_ (-1), fdo_ (-1)
{
    int fd, slave_fd, r;
    // Open and unlock pt.
    if (openpty (&fd, &slave_fd, 0, 0, 0) == -1
        || grantpt (fd) == -1
        || unlockpt (fd) == -1)
        halt_perror ();
    // Make a link to the slave pts.
    unlink (name);
    const char *slave_name = ptsname (fd);
    assert (slave_name);
    r = symlink (slave_name, name);
    assert_perror (r != -1);
    // Make slave raw.
    setup_raw (slave_fd);
    // Use as both in and out.
    fdi_ = fdo_ = fd;
    // slave_fd is left open.
}

HostStream::~HostStream ()
{
    if (fdi_ != -1 && fdi_ != 0)
        close (fdi_);
    if (fdo_ != -1 && fdo_ != 1 && fdo_ != fdi_)
        close (fdo_);
}

void
HostStream::block (bool block)
{
    Stream::block (block);
    // Do not care to use non blocking on output for host.  However, if
    // fdi_ == fdo_, this will change input and output.
    int r = fcntl (fdi_, F_SETFL, block ? 0 : O_NONBLOCK);
    assert_perror (r != -1);
}

int
HostStream::read (char *buf, int count)
{
    int r = ::read (fdi_, buf, count);
    if (r == 0)
        return -2;
    if (r == -1 && errno == EAGAIN)
        return 0;
    assert_perror (r != -1);
    return r;
}

int
HostStream::write (const char *buf, int count)
{
    int writen = 0;
    while (writen < count)
    {
        int r = ::write (fdo_, buf + writen, count - writen);
        if (r == -1 && errno == EAGAIN)
            break;
        assert_perror (r != -1);
        writen += r;
    }
    return writen;
}

int
HostStream::poll ()
{
    fd_set fds;
    struct timeval tv;
    int r;
    // Use select to poll.
    FD_ZERO (&fds);
    FD_SET (fdi_, &fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    r = select (FD_SETSIZE, &fds, 0, 0, &tv);
    // Check result.
    assert_perror (r != -1);
    return r;
}

} // namespace ucoo
