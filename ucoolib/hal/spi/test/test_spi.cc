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
#include "ucoolib/hal/spi/spi_soft.hh"
#include "ucoolib/hal/gpio/gpio.hh"
#include "ucoolib/utils/delay.hh"

#include "ucoolib/arch/arch.hh"
#include "ucoolib/base/test/test.hh"

#include <libopencm3/stm32/f4/rcc.h>

#include <algorithm>
#include <cstdio>

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::Stream &ts = ucoo::test_stream ();
    // Use connection to LIS302DL device on discovery board.
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPAEN
                                 | RCC_AHB1ENR_IOPEEN);
    ucoo::Gpio ss (GPIOE, 3);
    ss.set ();
    gpio_mode_setup (GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
    ucoo::Gpio sck (GPIOA, 5), mosi (GPIOA, 7), miso (GPIOA, 6);
    ucoo::SpiSoftMaster spi (sck, mosi, miso, 1000000, ucoo::SPI_MODE_3);
    // Loop with simple IU.
    char buf[64];
    unsigned int r;
    int8_t x, y, z;
    while (1)
    {
        ucoo::delay_ms (200);
        // Read X, Y, Z.
        ss.reset ();
        spi.send (0xe9);
        x = spi.recv ();
        spi.recv ();
        y = spi.recv ();
        spi.recv ();
        z = spi.recv ();
        ss.set ();
        ucoo::delay_ns (100);
        r = snprintf (buf, sizeof (buf), "x: %4d, y: %4d, z: %4d\r", x, y, z);
        r = std::min (sizeof (buf) - 1, r);
        ts.write (buf, r);
        // Simple UI.
        while (ts.poll ())
        {
            char c = ts.getc ();
            switch (c)
            {
            case '?':
                static const char help[] =
                    "\n? - help\n"
                    "w - who am I\n"
                    "o - turn on\n"
                    "f - turn off\n";
                ts.write (help, sizeof (help));
                break;
            case 'w':
                {
                    // Read Who am I register.
                    ss.reset ();
                    spi.send (0x8f);
                    char rsp = spi.recv ();
                    ss.set ();
                    ucoo::delay_ns (100);
                    // Report result.
                    r = snprintf (buf, sizeof (buf), "\nI am 0x%02x\n",
                                  static_cast<unsigned char> (rsp));
                    r = std::min (sizeof (buf) - 1, r);
                    ts.write (buf, r);
                }
                break;
            case 'o':
            case 'f':
                {
                    char cmd[] = { 0x20, (char) (c == 'o' ? 0x47 : 0x07) };
                    ss.reset ();
                    spi.send (cmd, sizeof (cmd));
                    ss.set ();
                    ucoo::delay_ns (100);
                }
                break;
            }
        }
    }
}
