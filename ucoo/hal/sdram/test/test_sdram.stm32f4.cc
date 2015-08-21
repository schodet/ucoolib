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
#include "ucoo/hal/sdram/sdram.hh"

#include "ucoo/arch/arch.hh"
#include "ucoo/base/test/test.hh"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include <cstdio>
#include <cinttypes>

int
memtest (void *addr, int size, uint32_t pattern)
{
    int errors = 0;
    uint32_t *begin = reinterpret_cast<uint32_t *> (addr);
    uint32_t *end = begin + size / 4;
    printf ("memtest %dM, pattern 0x%08" PRIx32 "\n", size / 1024 / 1024, pattern);
    printf (" fill...\n");
    for (uint32_t *p = begin; p != end; p++)
        *p = pattern;
    printf (" check increasing...\n");
    for (uint32_t *p = begin; p != end; p++)
    {
        if (*p != pattern)
            errors++;
        *p = ~pattern;
    }
    printf (" check decreasing...\n");
    for (uint32_t *p = end; p != begin; p--)
    {
        if (*(p - 1) != ~pattern)
            errors++;
        *(p - 1) = pattern;
    }
    printf (" done, %d errors\n", errors);
    return errors;
}

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::test_stream_setup ();
    rcc_periph_clock_enable (RCC_GPIOD);
    rcc_periph_clock_enable (RCC_GPIOE);
    rcc_periph_clock_enable (RCC_GPIOF);
    rcc_periph_clock_enable (RCC_GPIOG);
    rcc_periph_clock_enable (RCC_GPIOH);
    rcc_periph_clock_enable (RCC_GPIOI);
    std::initializer_list<ucoo::Sdram::Io> sdram_ios {
        { GPIOD, GPIO0 | GPIO1 | GPIO8 | GPIO9 | GPIO10 | GPIO14 | GPIO15 },
        { GPIOE, GPIO0 | GPIO1 | GPIO7 | GPIO8 | GPIO9 | GPIO10 | GPIO11
            | GPIO12 | GPIO13 | GPIO14 | GPIO15 },
        { GPIOF, GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO4 | GPIO5 | GPIO11
            | GPIO12 | GPIO13 | GPIO14 | GPIO15 },
        { GPIOG, GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO4 | GPIO5 | GPIO8
            | GPIO15 },
        { GPIOH, GPIO2 | GPIO3 | GPIO5 | GPIO8 | GPIO9 | GPIO10 | GPIO11
            | GPIO12 | GPIO13 | GPIO14 | GPIO15 },
        { GPIOI, GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO4 | GPIO5 | GPIO6 | GPIO7
            | GPIO9 | GPIO10 },
    };
    ucoo::Sdram::Param sdram_param {
        .bank = 1,
        .clock_hz = 133333333,
        .cas = 3,
        .banks = 4,
        .bits = 32,
        .row_bits = 12,
        .col_bits = 8,
        .trcd = 3,
        .trp = 3,
        .twr = 2,
        .trc = 8,
        .tras = 6,
        .txsr = 9,
        .tmrd = 2,
        .tref_ms = 64,
        .init_clock_delay_us = 100,
        .init_auto_refresh = 2,
    };
    ucoo::Sdram sdram (sdram_ios, sdram_param);
    sdram.enable ();
    long long errors = 0;
    for (int i = 0; i < 100; i++)
    {
        errors += memtest (sdram.addr (), sdram.size (), 0x00000000);
        errors += memtest (sdram.addr (), sdram.size (), 0xffffffff);
        errors += memtest (sdram.addr (), sdram.size (), 0x55aaa55a);
        errors += memtest (sdram.addr (), sdram.size (), 0xff00ff00);
        errors += memtest (sdram.addr (), sdram.size (), 0x00ff00ff);
        printf ("total errors: %lld\n", errors);
    }
    return errors == 0 ? 0 : 1;
}
