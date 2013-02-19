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
#include "ucoolib/dev/avrisp/avrisp_frame.hh"

#include "ucoolib/arch/arch.hh"
#include "ucoolib/base/test/test.hh"
#include "ucoolib/hal/gpio/gpio.hh"
#include "ucoolib/hal/spi/spi_soft.hh"
#include "ucoolib/utils/delay.hh"

#include <libopencm3/stm32/f4/rcc.h>

class TestAvrIspIntf : public ucoo::AvrIspIntf
{
  public:
    TestAvrIspIntf (ucoo::Io &reset, ucoo::Io &sck, ucoo::Io &mosi,
                    ucoo::Io &miso)
        : reset_ (reset), sck_ (sck), spi_ (sck, mosi, miso) { }
    uint8_t send_and_recv (uint8_t tx)
    {
        return spi_.send_and_recv (tx);
    }
    void enable (uint8_t sck_duration_us)
    {
        reset_.output ();
        reset_.reset ();
        sck_.output ();
        sck_.reset ();
        ucoo::delay_us (1000);
        reset_.set ();
        ucoo::delay_us (100);
        reset_.reset ();
        int freq = 1000000 / sck_duration_us;
        spi_.setup (freq, ucoo::SPI_MODE_0);
    }
    void disable ()
    {
        spi_.setup (0);
        reset_.input ();
    }
    void sck_pulse ()
    {
        sck_.toggle ();
        ucoo::delay_us (100);
        sck_.toggle ();
    }
  private:
    ucoo::Io &reset_;
    ucoo::Io &sck_;
    ucoo::SpiSoftMaster spi_;
};

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::Stream &ts = ucoo::test_stream ();
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPDEN
                                 | RCC_AHB1ENR_IOPEEN);
    ucoo::Gpio reset (GPIOD, 10), sck (GPIOE, 7), mosi (GPIOD, 8),
        miso (GPIOD, 9);
    TestAvrIspIntf intf (reset, sck, mosi, miso);
    ucoo::AvrIsp isp (intf);
    ucoo::AvrIspProto proto (isp);
    ucoo::AvrIspFrame frame (proto);
    while (1)
        frame.read_and_write (ts);
}
