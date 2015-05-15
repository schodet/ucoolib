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
#include "spi_soft.hh"

#include "ucoo/utils/delay.hh"
#include "ucoo/common.hh"

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
        if (miso_.get ())
            rx |= i;
        sck_.toggle ();
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
