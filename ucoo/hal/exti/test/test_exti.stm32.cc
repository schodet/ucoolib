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
#include "ucoo/hal/exti/exti.hh"
#include "ucoo/utils/delay.hh"

ucoo::Gpio *led4p;

void
led4_handler ()
{
    led4p->toggle ();
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
    ucoo::Gpio led3 (ucoo::GPIOD[13]);
    ucoo::Gpio led4 (ucoo::GPIOD[12]);
    led4p = &led4;
    ucoo::ExtiHard exti (7);
    exti.set_source (ucoo::GPIOB);
    exti.set_trigger (ucoo::ExtiHard::Edge::RISING);
    exti.register_event (led4_handler);
    exti.enable ();
    loop_out.output ();
    led3.output ();
    led4.output ();
    while (1)
    {
        loop_out.toggle ();
        led3.set (loop_in.get ());
        ucoo::delay (1);
    }
    return 0;
}
