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
#include "ucoo/arch/arch.hh"
#include "ucoo/hal/gpio/gpio.hh"
#include "ucoo/utils/delay.hh"

void
test (ucoo::Io &loop_in, ucoo::Io &led1, ucoo::Io &led2, ucoo::Io &led3,
      ucoo::Io &led4)
{
    loop_in.input ();
    led1.output ();
    led2.output ();
    led3.output ();
    led4.output ();
    led1.set ();
    led4.reset ();
    bool state = false;
    while (1)
    {
        led1.toggle ();
        led2.set (state);
        led3.set (loop_in.get ());
        led4.toggle ();
        state = !state;
        ucoo::delay (1);
    }
}

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::GPIOC.enable ();
    ucoo::Gpio loop_in (ucoo::GPIOC[5]);
    ucoo::Gpio led1 (ucoo::GPIOC[6]);
    ucoo::Gpio led2 (ucoo::GPIOC[7]);
    ucoo::Gpio led3 (ucoo::GPIOC[8]);
    ucoo::Gpio led4 (ucoo::GPIOC[9]);
    test (loop_in, led1, led2, led3, led4);
    return 0;
}
