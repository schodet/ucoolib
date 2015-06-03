#ifndef ucoo_intf_stream_hh
#define ucoo_intf_stream_hh
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

namespace ucoo {

/// Interface to an object providing a stream oriented flow, like serial port,
/// connected socket, or higher encapsulated protocol.
class Stream
{
  public:
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
    /// Return the number of available bytes to be read.
    virtual int poll () = 0;
  protected:
    /// Default constructor.
    Stream ();
  protected:
    /// Current blocking operation flag.
    bool block_;
};

} // namespace ucoo

#endif // ucoo_intf_stream_hh
