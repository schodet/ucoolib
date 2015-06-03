#ifndef ucoo_arch_host_mex_mex_socket_hh
#define ucoo_arch_host_mex_mex_socket_hh
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

#endif // ucoo_arch_host_mex_mex_socket_hh
