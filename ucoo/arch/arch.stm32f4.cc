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
#include "ucoo/arch/rcc.stm32.hh"

#include "config/ucoo/arch.hh"

namespace ucoo {

void
arch_init (int argc, const char **argv)
{
    rcc_sys_clock_setup_pll (120000000, 8000000,
                             4, // pllm => 8 MHz / 4 = 2 MHz
                             120, // plln => 2 MHz * 120 = 240 MHz
                             2, // pllp => 240 MHz / 2 = 120 MHz
                             5, // pllq => 240 MHz / 5 = 48 MHz
                             4, // apb1_pre => 30 MHz
                             2, // apb2_pre => 60 MHz
                             CONFIG_UCOO_ARCH_SUPPLY_RANGE);
    rcc_peripheral_clock_enable (Rcc::SYSCFG);
}

} // namespace ucoo
