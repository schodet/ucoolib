// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2015 Nicolas Schodet
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

#include "ucoo/dev/lcd/lcd_spi.hh"

#include "ucoo/arch/arch.hh"

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::GPIOB.enable ();
    ucoo::Gpio rw (ucoo::GPIOB[10]);
    ucoo::Gpio rs (ucoo::GPIOB[11]);
    ucoo::Gpio cs (ucoo::GPIOB[12]);
    ucoo::Gpio sck (ucoo::GPIOB[13]);
    ucoo::Gpio miso (ucoo::GPIOB[14]);
    ucoo::Gpio mosi (ucoo::GPIOB[15]);
    ucoo::SpiSoftMaster spi (sck, mosi, miso);
    rw.reset ();
    rw.output ();
    ucoo::LcdSpi lcd (spi, cs, rs);
    ucoo::delay (1);
    lcd.enable ();
    int i = 0;
    while (1)
    {
        lcd.printf ("Hello world!\n%02d:%02d\n", i / 60, i % 60);
        i++;
        ucoo::delay (1);
    }
}
