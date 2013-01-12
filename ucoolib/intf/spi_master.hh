#ifndef ucoolib_intf_spi_master_hh
#define ucoolib_intf_spi_master_hh
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

/// SPI clock polarity and phase modes.
enum SpiMode
{
    /// Mode 0, sample on rising edge, setup on falling edge.
    SPI_MODE_0,
    /// Mode 1, setup on rising edge, sample on falling edge.
    SPI_MODE_1,
    /// Mode 2, sample on falling edge, setup on rising edge.
    SPI_MODE_2,
    /// Mode 3, setup on falling edge, sample on rising edge.
    SPI_MODE_3,
};

/// Interface to the master side of an SPI bus.  Do not handle slave select,
/// this should be done with a regular IO.
class SpiMaster
{
  public:
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

#endif // ucoolib_intf_spi_master_hh
