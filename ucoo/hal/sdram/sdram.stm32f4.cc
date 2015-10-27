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
#include "ucoo/utils/delay.hh"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/fsmc.h>

namespace ucoo {

Sdram::Sdram (std::initializer_list<Io> ios, const Param &params)
    : ios_ (ios), params_ (params)
{
}

void
Sdram::enable ()
{
    // Setup GPIO, turn controller on.
    for (auto io : ios_)
    {
        gpio_mode_setup (io.gpio, GPIO_MODE_AF, GPIO_PUPD_NONE, io.pins);
        gpio_set_output_options (io.gpio, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                                 io.pins);
        gpio_set_af (io.gpio, GPIO_AF12, io.pins);
    }
    rcc_periph_clock_enable (RCC_FSMC);
    // Prepare SDRAM clock, can only be HCLK/2 or HCLK/3. Can go slower than
    // requested, but not faster.
    int clock_div = (rcc_ahb_frequency + params_.clock_hz - 1)
        / params_.clock_hz;
    if (clock_div < 2)
        clock_div = 2;
    assert (clock_div <= 3);
    int clock_hz = rcc_ahb_frequency / clock_div;
    int clock_ns = 1000000000 / clock_hz;
    // Prepare and set parameters.
    assert (params_.bank == 1 || params_.bank == 2);
    assert (params_.cas >= 2 && params_.cas <= 3);
    assert (params_.banks == 2 || params_.banks == 4);
    assert (params_.bits == 8 || params_.bits == 16 || params_.bits == 32);
    assert (params_.row_bits >= 11 && params_.row_bits <= 13);
    assert (params_.col_bits >= 8 && params_.col_bits <= 11);
    uint32_t sdcr =
        FMC_SDCR_RPIPE_NONE
        | (clock_div == 2 ? FMC_SDCR_SDCLK_2HCLK : FMC_SDCR_SDCLK_3HCLK)
        | (params_.cas << FMC_SDCR_CAS_SHIFT)
        | (params_.banks == 2 ? FMC_SDCR_NB2 : FMC_SDCR_NB4)
        | (params_.bits == 8 ? FMC_SDCR_MWID_8b
           : (params_.bits == 16 ? FMC_SDCR_MWID_16b : FMC_SDCR_MWID_32b))
        | ((params_.row_bits - 11) << FMC_SDCR_NR_SHIFT)
        | ((params_.col_bits - 8) << FMC_SDCR_NC_SHIFT);
    uint32_t sdtr =
        ((params_.trcd - 1) << FMC_SDTR_TRCD_SHIFT)
        | ((params_.trp - 1) << FMC_SDTR_TRP_SHIFT)
        | ((params_.twr - 1) << FMC_SDTR_TWR_SHIFT)
        | ((params_.trc - 1) << FMC_SDTR_TRC_SHIFT)
        | ((params_.tras - 1) << FMC_SDTR_TRAS_SHIFT)
        | ((params_.txsr - 1) << FMC_SDTR_TXSR_SHIFT)
        | ((params_.tmrd - 1) << FMC_SDTR_TMRD_SHIFT);
    if (params_.bank == 1)
    {
        FMC_SDCR1 = sdcr;
        FMC_SDTR1 = sdtr;
    }
    else
    {
        FMC_SDCR1 = sdcr & FMC_SDCR_DNC_MASK;
        FMC_SDCR2 = sdcr;
        FMC_SDTR1 = sdtr & FMC_SDTR_DNC_MASK;
        FMC_SDTR2 = sdtr;
    }
    // Initialise SDRAM.
    uint32_t bank = params_.bank == 1 ? FMC_SDCMR_CTB1 : FMC_SDCMR_CTB2;
    while (FMC_SDSR & FMC_SDSR_BUSY)
        ;
    FMC_SDCMR = bank | FMC_SDCMR_MODE_CLOCK_CONFIG_ENA;
    delay_us (params_.init_clock_delay_us);
    while (FMC_SDSR & FMC_SDSR_BUSY)
        ;
    FMC_SDCMR = bank | FMC_SDCMR_MODE_PALL;
    while (FMC_SDSR & FMC_SDSR_BUSY)
        ;
    FMC_SDCMR = bank | ((params_.init_auto_refresh - 1)
                        << FMC_SDCMR_NRFS_SHIFT)
        | FMC_SDCMR_MODE_AUTO_REFRESH;
    while (FMC_SDSR & FMC_SDSR_BUSY)
        ;
    uint32_t sdram_mode =
        SDRAM_MODE_BURST_LENGTH_1
        | SDRAM_MODE_BURST_TYPE_SEQUENTIAL
        | (params_.cas == 2 ? SDRAM_MODE_CAS_LATENCY_2
           : SDRAM_MODE_CAS_LATENCY_3)
        | SDRAM_MODE_OPERATING_MODE_STANDARD
        | SDRAM_MODE_WRITEBURST_MODE_SINGLE;
    FMC_SDCMR = bank | (sdram_mode << FMC_SDCMR_MRD_SHIFT)
        | FMC_SDCMR_MODE_LOAD_MODE_REGISTER;
    // Set refresh rate.
    int refresh_interval_ns = params_.tref_ms * 1000000
        / (1 << params_.row_bits);
    FMC_SDRTR = refresh_interval_ns / clock_ns - 20;
}

void *
Sdram::addr () const
{
    return params_.bank == 1 ? reinterpret_cast<void *> (FMC_BANK7_BASE)
        : reinterpret_cast<void *> (FMC_BANK8_BASE);
}

int
Sdram::size () const
{
    return params_.bits / 8 * (1 << params_.col_bits)
        * (1 << params_.row_bits) * params_.banks;
}

} // namespace ucoo
