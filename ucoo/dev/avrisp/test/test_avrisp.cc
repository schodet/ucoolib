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
#include "ucoo/dev/avrisp/avrisp_frame.hh"

#include "ucoo/arch/arch.hh"
#include "ucoo/base/test/test.hh"
#include "ucoo/hal/gpio/gpio.hh"
#include "ucoo/hal/spi/spi_soft.hh"
#include "ucoo/utils/delay.hh"

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
        spi_.enable (freq, ucoo::SPI_MODE_0);
    }
    void disable ()
    {
        spi_.disable ();
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
    ucoo::GPIOD.enable ();
    ucoo::GPIOE.enable ();
    ucoo::Gpio reset (ucoo::GPIOD[10]), sck (ucoo::GPIOE[7]),
        mosi (ucoo::GPIOD[8]), miso (ucoo::GPIOD[9]);
    TestAvrIspIntf intf (reset, sck, mosi, miso);
    ucoo::AvrIsp isp (intf);
    ucoo::AvrIspProto proto (isp);
    ucoo::AvrIspFrame frame (proto);
    while (1)
        frame.read_and_write (ts);
}
