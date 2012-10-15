#ifndef ucoolib_intf_stream_hh
#define ucoolib_intf_stream_hh
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

namespace ucoo {

/// Interface to an object providing a stream oriented flow, like serial port,
/// connected socket, or higher encapsulated protocol.
class Stream
{
  public:
    /// Default constructor.
    Stream ();
    /// Set whether operations on this stream should block when no data is
    /// available.
    virtual void block (bool block = true);
    /// Read up to COUNT bytes of data from stream and store them in BUF.
    /// Return the number of read bytes, -1 on error or -2 on EOF.
    ///
    /// If blocking, will return as soon as there is some data available.  If
    /// not blocking, may return 0 if no data is available.
    virtual int read (char *buf, int count) = 0;
    /// Write up to COUNT bytes of data from BUF to stream.  Return the number
    /// of written bytes or -1 on error.
    ///
    /// If blocking, will try its best to write all data provided.
    virtual int write (const char *buf, int count) = 0;
    /// Shortcut to read one character.  Return -1 on error, on EOF, or if no
    /// character is available.
    int getc ();
    /// Shortcut to write one character.  Return -1 on error.
    int putc (int c);
  protected:
    /// Current blocking operation flag.
    bool block_;
};

} // namespace ucoo

#endif // ucoolib_intf_stream_hh
