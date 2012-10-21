#ifndef ucoolib_hal_uart_uart_stm32_hh
#define ucoolib_hal_uart_uart_stm32_hh
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

/// Universal asynchronous receiver transmitter (UART).
class Uart : public Stream
{
  public:
    /// Parity setting.
    enum Parity { ODD, EVEN, NONE };
  public:
    /// Initialise the Nth UART with given parameters.
    Uart (int n, int speed, Parity parity, int stop_bits);
    /// Shutdown UART.
    ~Uart ();
    /// Change UART settings.
    void setup (int speed, Parity parity, int stop_bits);
    /// See Stream::read.
    int read (char *buf, int count);
    /// See Stream::write.
    int write (const char *buf, int count);
};

} // namespace ucoo

#endif // ucoolib_hal_uart_uart_stm32_hh
