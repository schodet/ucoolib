// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include "ucoo/arch/arch.hh"
#include "ucoo/hal/gpio/gpio.hh"
#include "ucoo/utils/delay.hh"

void
test (ucoo::Io &loop_out, ucoo::Io &loop_in, ucoo::Io &led3, ucoo::Io &led4,
      ucoo::Io &led5, ucoo::Io &led6)
{
    loop_in.input ();
    loop_out.output ();
    led3.output ();
    led4.output ();
    led5.output ();
    led6.output ();
    led3.set ();
    led6.reset ();
    bool state = false;
    while (1)
    {
        led3.toggle ();
        led4.set (state);
        led5.set (loop_in.get ());
        led6.toggle ();
        state = !state;
        loop_out.set (state);
        ucoo::delay (1);
    }
}

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::GPIOB.enable ();
    ucoo::GPIOD.enable ();
    // For this test, shorten B6 & B7 to have loopback.
    ucoo::Gpio loop_out (ucoo::GPIOB[6]);
    ucoo::Gpio loop_in (ucoo::GPIOB[7]);
    loop_in.pull (ucoo::Gpio::Pull::UP);
    ucoo::Gpio led3 (ucoo::GPIOD[13]);
    ucoo::Gpio led4 (ucoo::GPIOD[12]);
    ucoo::Gpio led5 (ucoo::GPIOD[14]);
    ucoo::Gpio led6 (ucoo::GPIOD[15]);
    test (loop_out, loop_in, led3, led4, led5, led6);
    return 0;
}
