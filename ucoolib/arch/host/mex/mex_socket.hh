#ifndef ucoolib_arch_host_mex_mex_socket_hh
#define ucoolib_arch_host_mex_mex_socket_hh
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
#include "mex_msg.hh"

namespace ucoo {
namespace mex {

/// Mex socket, able to read and write messages.
class Socket : public MsgReader, public MsgWriter
{
  public:
    /// Default constructor.
    Socket () : fd_ (-1), read_size_ (-1) { }
    /// Destructor, close the connection.
    ~Socket ();
    /// Connect to Mex hub.
    void connect ();
    /// See MsgReader::size.
    int size ();
    /// See MsgReader::read.
    void read (char *buf);
    /// See MsgWriter::write.
    void write (const char *buf, int count);
  private:
    /// File descriptor to open socket, or -1 if not open.
    int fd_;
    /// Size of the next message to be read, or -1 if not known yet.
    int read_size_;
    /// This socket sequence number.
    int seq_;
};

} // namespace mex
} // namespace ucoo

#endif // ucoolib_arch_host_mex_mex_socket_hh
