#ifndef ucoolib_dev_xmodem_xmodem_hh
#define ucoolib_dev_xmodem_xmodem_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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
