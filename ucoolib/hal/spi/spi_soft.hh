#ifndef ucoolib_hal_spi_spi_soft_hh
#define ucoolib_hal_spi_spi_soft_hh
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
#include "ucoolib/intf/spi_master.hh"
#include "ucoolib/intf/io.hh"

namespace ucoo {

/// Software implementation of a SPI master.
class SpiSoftMaster : public SpiMaster
{
  public:
    /// Constructor, need all SPI pins.  Speed is given in Hz.
    SpiSoftMaster (Io &sck, Io &mosi, Io &miso, int speed = 0,
                   SpiMode mode = SPI_MODE_0);
    /// Destructor, release SPI pins.
    ~SpiSoftMaster ();
    /// Change settings, use speed 0 to stop.
    void setup (int speed, SpiMode mode = SPI_MODE_0);
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
};

} // namespace ucoo

#endif // ucoolib_hal_spi_spi_soft_hh
