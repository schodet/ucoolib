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
#include "ucoo/hal/spi/spi_soft.hh"
#include "ucoo/hal/gpio/gpio.hh"
#include "ucoo/utils/delay.hh"

#include "ucoo/arch/arch.hh"
#include "ucoo/base/test/test.hh"

#include <libopencm3/stm32/f4/rcc.h>

#include <algorithm>
#include <cstdio>

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::Stream &ts = ucoo::test_stream ();
    // Use connection to LIS302DL device on discovery board revision MB997B.
    // Revision MB997C uses a different device.
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPAEN
                                 | RCC_AHB1ENR_IOPEEN);
    ucoo::Gpio ss (GPIOE, 3);
    ss.set ();
    ss.output ();
    ucoo::Gpio sck (GPIOA, 5), mosi (GPIOA, 7), miso (GPIOA, 6);
    ucoo::SpiSoftMaster spi (sck, mosi, miso);
    spi.enable (1000000, ucoo::SPI_MODE_3);
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
