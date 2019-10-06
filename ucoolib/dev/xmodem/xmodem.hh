#ifndef ucoolib_dev_xmodem_xmodem_hh
#define ucoolib_dev_xmodem_xmodem_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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
#include "ucoolib/intf/stream.hh"

namespace ucoo {

/// Handle data received from XMODEM.
class XmodemReceiver
{
  public:
    /// Write up to COUNT bytes of data from BUF to stream.  Return the number
    /// of written bytes or -1 on error.
    virtual int write (const char *buf, int count) = 0;
};

/// Make a XMODEM reception from stream and write any received data to
/// receiver.
int
xmodem_receive (Stream &s, XmodemReceiver &receiver);

} // namespace ucoo

#endif // ucoolib_dev_xmodem_xmodem_hh
