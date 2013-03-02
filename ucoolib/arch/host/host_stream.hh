#ifndef ucoolib_arch_host_stream_hh
#define ucoolib_arch_host_stream_hh
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

namespace ucoo {

class HostStream : public Stream
{
  public:
    /// Default constructor, use stdin/stdout.
    HostStream ();
    /// PTY constructor, will make a link to PTY at the given name.
    HostStream (const char *name);
    /// Close if needed.
    ~HostStream ();
    /// See Stream::block.
    void block (bool block = true);
    /// See Stream::read.
    int read (char *buf, int count);
    /// See Stream::write.
    int write (const char *buf, int count);
    /// See Stream::poll.
    int poll ();
  private:
    /// Input and output file descriptors.
    int fdi_, fdo_;
};

} // namespace ucoo

#endif // ucoolib_arch_host_stream_hh
