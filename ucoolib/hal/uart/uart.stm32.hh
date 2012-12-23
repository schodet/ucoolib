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
#include "ucoolib/utils/fifo.hh"

#include "config/hal/uart.hh"

namespace ucoo {

/// Universal asynchronous receiver transmitter (UART).
///
/// When an error is detected on RX, error character is inserted in receive
/// FIFO.
class Uart : public Stream
{
  public:
    /// Parity setting.
    enum Parity { ODD, EVEN, NONE };
    /// Default error character.
    static const char default_error_char = '~';
  public:
    /// Initialise the Nth UART with given parameters.
    Uart (int n, int speed = 0, Parity parity = NONE, int stop_bits = 1);
    /// Shutdown UART.
    ~Uart ();
    /// Change UART settings, use speed 0 to stop UART.
    void setup (int speed, Parity parity = NONE, int stop_bits = 1);
    /// Change the error character.
    void set_error_char (char c);
    /// See Stream::read.
    int read (char *buf, int count);
    /// See Stream::write.
    int write (const char *buf, int count);
    /// See Stream::poll.
    int poll ();
    /// Handle interrupts.
    static void isr (int n);
  private:
    /// UART number.
    int n_;
    /// RX FIFO, filled by interrupt handler.
    Fifo<char, UCOO_CONFIG_HAL_UART_RX_BUFFER> rx_fifo_;
    /// TX FIFO, emptied by interrupt handler.
    Fifo<char, UCOO_CONFIG_HAL_UART_TX_BUFFER> tx_fifo_;
    /// Error character, inserted in case of error.
    char error_char_;
};

} // namespace ucoo

#endif // ucoolib_hal_uart_uart_stm32_hh
