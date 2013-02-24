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
#include "ucoolib/hal/i2c/i2c.hh"

#include "ucoolib/arch/arch.hh"
#include "ucoolib/hal/gpio/gpio.hh"
#include "ucoolib/utils/delay.hh"
#include "ucoolib/base/test/test.hh"

#include <libopencm3/stm32/f4/rcc.h>

#include <algorithm>
#include <cstring>

static const int buffer_size = 16;
static const int margin = 5;
static const uint8_t a1 = 0x2c, a2 = 0x2e;

void
test_basic (ucoo::TestSuite &tsuite, ucoo::I2cMaster &m,
            ucoo::I2cSlaveDataBuffer &d, uint8_t addr)
{
    tsuite.group ("basic");
    {
        ucoo::Test test (tsuite, "recv");
        // Slave is not able to signal its buffer end. Extra bytes will be
        // read as 0xff.
        const char *hello = "Hello world!";
        char buf[buffer_size + margin];
        d.update (hello, std::strlen (hello) + 1);
        char ref[buffer_size + margin];
        std::copy (hello, hello + std::strlen (hello) + 1, ref);
        std::fill (ref + std::strlen (hello) + 1, ref + sizeof (ref), 0xff);
        for (int len = 2; len < (int) sizeof (buf); len++)
        {
            std::fill (buf, buf + sizeof (buf), 42);
            m.recv (addr, buf, len);
            int r = m.wait ();
            test_fail_break_unless (test, r == len);
            test_fail_break_unless (test, std::equal (buf, buf + len, ref));
            test_fail_break_unless (test, std::count (buf, buf + sizeof (buf), 42)
                                    == (int) sizeof (buf) - len);
        }
    }
    {
        ucoo::Test test (tsuite, "send");
        // Slave is supposed to signal when it received enough data, but this
        // is not implemented.
        char buf[buffer_size + margin];
        for (int len = 1; len < (int) sizeof (buf); len++)
        {
            int r;
            // Before transfer, no data.
            r = d.poll (buf, sizeof (buf));
            test_fail_break_unless (test, r == 0);
            // Send data.
            char c = '0' + len % 10;
            std::fill (buf, buf + len, c);
            m.send (addr, buf, len);
            r = m.wait ();
            test_fail_break_unless (test, r == len);
            // Let some time for slave to finish reception.
            ucoo::delay_ms (1);
            // Check what is received.
            r = d.poll (buf, sizeof (buf));
            test_fail_break_unless (test, r == std::min (len, buffer_size));
            test_fail_break_unless (test, std::count (buf, buf + r, c) == r);
        }
    }
    do {
        ucoo::Test test (tsuite, "callback");
        // Callback object will start a reception, then a transmission of what
        // was received.
        class TestCallback : public ucoo::I2cMaster::FinishedHandler
        {
            ucoo::I2cMaster &m_;
            uint8_t addr_;
            int step_;
            char buf_[buffer_size];
          public:
            bool failed;
          public:
            TestCallback (ucoo::I2cMaster &m, uint8_t addr)
                : m_ (m), addr_ (addr), step_ (0), failed (false) { }
            void finished (int status)
            {
                if (status != buffer_size)
                    failed = true;
                else
                {
                    switch (step_)
                    {
                    case 0:
                        m_.recv (addr_, buf_, buffer_size);
                        step_++;
                        break;
                    case 1:
                        m_.send (addr_, buf_, buffer_size);
                        step_++;
                        break;
                    case 2:
                        // Nothing, stop.
                        break;
                    }
                }
            }
        };
        TestCallback callback (m, addr);
        m.register_finished (callback);
        // Set slave data.
        char buf[buffer_size];
        std::fill (buf, buf + sizeof (buf), 42);
        d.update (buf, sizeof (buf));
        // Start transfers.
        std::fill (buf, buf + sizeof (buf), 21);
        m.send (addr, buf, sizeof (buf));
        // Will only return after the last transfer.
        m.wait ();
        m.unregister_finished ();
        test_fail_break_unless (test, !callback.failed);
        // Let some time for slave to finish reception.
        ucoo::delay_ms (1);
        // Check what is received by slave (master should have read 42 from
        // slave and send it back).
        int r = d.poll (buf, sizeof (buf));
        test_fail_break_unless (test, r == buffer_size);
        test_fail_break_unless (test, std::count (buf, buf + r, 42) == r);
    } while (0);
}

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::TestSuite tsuite ("i2c");
    // I2C1: B6: SCL, B9: SDA
    // I2C3: A8: SCL, C9: SDA
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPAEN);
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPBEN);
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPCEN);
    gpio_mode_setup (GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO9);
    gpio_mode_setup (GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
    gpio_mode_setup (GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);
    gpio_set_output_options (GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO6 | GPIO9);
    gpio_set_output_options (GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO8);
    gpio_set_output_options (GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO9);
    gpio_set_af (GPIOB, GPIO_AF4, GPIO6 | GPIO9);
    gpio_set_af (GPIOA, GPIO_AF4, GPIO8);
    gpio_set_af (GPIOC, GPIO_AF4, GPIO9);
    ucoo::I2cSlaveDataBufferSize<16, 16> data1, data2;
    ucoo::I2cHard i2c1 (0);
    ucoo::I2cHard i2c2 (2);
    i2c1.register_data (a1, data1);
    i2c2.register_data (a2, data2);
    i2c1.enable ();
    i2c2.enable ();
    // Run tests.
    test_basic (tsuite, i2c1, data2, a2);
    test_basic (tsuite, i2c2, data1, a1);
    return tsuite.report () ? 0 : 1;
}
