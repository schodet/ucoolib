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
#include "spi_soft.hh"

#include "ucoolib/utils/delay.hh"
#include "ucoolib/common.hh"

namespace ucoo {

SpiSoftMaster::SpiSoftMaster (Io &sck, Io &mosi, Io &miso)
    : sck_ (sck), mosi_ (mosi), miso_ (miso), enabled_ (false)
{
}

SpiSoftMaster::~SpiSoftMaster ()
{
    disable ();
}

void
SpiSoftMaster::enable (int speed, SpiMode mode)
{
    enabled_ = true;
    // Take pins ownership, set SCK according to CPOL.
    sck_.set (mode >= SPI_MODE_2);
    sck_.output ();
    mosi_.reset ();
    mosi_.output ();
    miso_.input ();
    // Set clock period and clock phase.
    half_period_ns_ = 1000 * 1000 * 1000 / 2 / speed;
    cpha_ = mode & 1;
}

void
SpiSoftMaster::disable ()
{
    enabled_ = false;
    // Release pins.
    sck_.input ();
    sck_.reset ();
    mosi_.input ();
}

void
SpiSoftMaster::send_and_recv (const char *tx_buf, char *rx_buf, int count)
{
    while (count--)
        *rx_buf++ = send_and_recv (*tx_buf++);
}

char
SpiSoftMaster::send_and_recv (char tx)
{
    assert (enabled_);
    uint8_t i, rx = 0;
    // Depending on clock phase, sample is done on first transition or second
    // transition.
    for (i = 0x80; i; i >>= 1)
    {
        // Setup stage.
        if (cpha_)
            sck_.toggle ();
        mosi_.set (tx & i);
        delay_ns (half_period_ns_);
        // Sample stage.
        sck_.toggle ();
        if (miso_.get ())
            rx |= i;
        delay_ns (half_period_ns_);
        // SCK toggle for next setup stage.
        if (!cpha_)
            sck_.toggle ();
    }
    return rx;
}

void
SpiSoftMaster::send (const char *tx_buf, int count)
{
    while (count--)
        send (*tx_buf++);
}

void
SpiSoftMaster::send (char tx)
{
    send_and_recv (tx);
}

void
SpiSoftMaster::recv (char *rx_buf, int count)
{
    while (count--)
        *rx_buf++ = recv ();
}

char
SpiSoftMaster::recv ()
{
    return send_and_recv (0);
}

} // namespace ucoo
