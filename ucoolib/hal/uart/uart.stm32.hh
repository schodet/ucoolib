#ifndef ucoolib_hal_uart_uart_stm32_hh
#define ucoolib_hal_uart_uart_stm32_hh
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
    /// Constructor for the Nth UART.
    Uart (int n);
    /// Shutdown UART.
    ~Uart ();
    /// Enable and setup UART.
    void enable (int speed, Parity parity = NONE, int stop_bits = 1);
    /// Disable.
    void disable ();
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
    /// Is it enabled?
    bool enabled_;
};

} // namespace ucoo

#endif // ucoolib_hal_uart_uart_stm32_hh
