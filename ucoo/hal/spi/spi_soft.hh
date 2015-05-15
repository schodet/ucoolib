#ifndef ucoo_hal_spi_spi_soft_hh
#define ucoo_hal_spi_spi_soft_hh
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
#include "ucoo/intf/spi_master.hh"
#include "ucoo/intf/io.hh"

namespace ucoo {

/// Software implementation of a SPI master.
class SpiSoftMaster : public SpiMaster
{
  public:
    /// Constructor, need all SPI pins.
    SpiSoftMaster (Io &sck, Io &mosi, Io &miso);
    /// Destructor, disable.
    ~SpiSoftMaster ();
    /// See SpiMaster::enable.
    void enable (int speed_hz, SpiMode mode = SPI_MODE_0);
    /// See SpiMaster::disable.
    void disable ();
    /// See SpiMaster::send_and_recv.
    void send_and_recv (const char *tx_buf, char *rx_buf, int count);
    /// Send and receive one byte.
    char send_and_recv (char tx);
    /// See SpiMaster::send.
    void send (const char *tx_buf, int count);
    /// Send one byte.
    void send (char tx);
    /// See SpiMaster::recv.
    void recv (char *rx_buf, int count);
    /// Receive one byte.
    char recv ();
  private:
    Io &sck_, &mosi_, &miso_;
    int half_period_ns_;
    bool cpha_;
    bool enabled_;
};

} // namespace ucoo

#endif // ucoo_hal_spi_spi_soft_hh
