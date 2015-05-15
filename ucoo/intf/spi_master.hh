#ifndef ucoo_intf_spi_master_hh
#define ucoo_intf_spi_master_hh
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

/// SPI clock polarity and phase modes.
enum SpiMode
{
    /// Mode 0, sample on rising edge, setup on falling edge.
    /// CPOL = 0, CPHA = 0
    SPI_MODE_0,
    /// Mode 1, setup on rising edge, sample on falling edge.
    /// CPOL = 0, CPHA = 1
    SPI_MODE_1,
    /// Mode 2, sample on falling edge, setup on rising edge.
    /// CPOL = 1, CPHA = 0
    SPI_MODE_2,
    /// Mode 3, setup on falling edge, sample on rising edge.
    /// CPOL = 1, CPHA = 1
    SPI_MODE_3,
};

/// Interface to the master side of an SPI bus.  Do not handle slave select,
/// this should be done with a regular IO.
class SpiMaster
{
  public:
    /// Enable and setup.
    virtual void enable (int speed_hz, SpiMode mode = SPI_MODE_0) = 0;
    /// Disable.
    virtual void disable () = 0;
    /// Send and receive COUNT bytes of data.  Send data from TX_BUF, store
    /// received data in RX_BUF.  TX_BUF and RX_BUF can be the same memory.
    virtual void send_and_recv (const char *tx_buf, char *rx_buf, int count)
        = 0;
    /// Send and receive one byte, shortcut.
    virtual char send_and_recv (char tx);
    /// Send, and ignore received data.  Same as send_and_recv, but received
    /// data is discarded.
    virtual void send (const char *tx_buf, int count) = 0;
    /// Send one byte, shortcut.
    virtual void send (char tx);
    /// Send zeros, and receive data.  Same as send_and_recv, but transmit
    /// zeros.
    virtual void recv (char *rx_buf, int count) = 0;
    /// Receive one byte, shortcut.
    virtual char recv ();
  protected:
    /// Default constructor.
    SpiMaster () { }
};

} // namespace ucoo

#endif // ucoo_intf_spi_master_hh
