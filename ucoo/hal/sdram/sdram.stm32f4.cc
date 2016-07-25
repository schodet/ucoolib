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
#include "ucoo/arch/rcc.stm32.hh"
#include "ucoo/utils/delay.hh"

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
        for (int i = 0; i < 16; i++)
        {
            if (io.pins & (1 << i))
            {
                Gpio gpio (io.port[i]);
                gpio.af (12);
                gpio.speed (Gpio::Speed::SPEED_50MHZ);
            }
        }
    }
    rcc_peripheral_clock_enable (Rcc::FMC);
    // Prepare SDRAM clock, can only be HCLK/2 or HCLK/3. Can go slower than
    // requested, but not faster.
    int clock_div = (rcc_ahb_freq_hz + params_.clock_hz - 1)
        / params_.clock_hz;
    if (clock_div < 2)
        clock_div = 2;
    assert (clock_div <= 3);
    int clock_hz = rcc_ahb_freq_hz / clock_div;
    int clock_ns = 1000000000 / clock_hz;
    // Prepare and set parameters.
    assert (params_.bank == 1 || params_.bank == 2);
    assert (params_.cas >= 2 && params_.cas <= 3);
    assert (params_.banks == 2 || params_.banks == 4);
    assert (params_.bits == 8 || params_.bits == 16 || params_.bits == 32);
    assert (params_.row_bits >= 11 && params_.row_bits <= 13);
    assert (params_.col_bits >= 8 && params_.col_bits <= 11);
    uint32_t sdcr =
        FMC_SDCR_RPIPE_None
        | FMC_SDCR_RBURST
        | (clock_div == 2 ? FMC_SDCR_SDCLK_2Hclk : FMC_SDCR_SDCLK_3Hclk)
        | (params_.cas << FMC_SDCR_CAS_Pos)
        | (params_.banks == 2 ? 0 : FMC_SDCR_NB)
        | (params_.bits == 8 ? FMC_SDCR_MWID_8B
           : (params_.bits == 16 ? FMC_SDCR_MWID_16B : FMC_SDCR_MWID_32B))
        | ((params_.row_bits - 11) << FMC_SDCR_NR_Pos)
        | ((params_.col_bits - 8) << FMC_SDCR_NC_Pos);
    uint32_t sdtr =
        ((params_.trcd - 1) << FMC_SDTR_TRCD_Pos)
        | ((params_.trp - 1) << FMC_SDTR_TRP_Pos)
        | ((params_.twr - 1) << FMC_SDTR_TWR_Pos)
        | ((params_.trc - 1) << FMC_SDTR_TRC_Pos)
        | ((params_.tras - 1) << FMC_SDTR_TRAS_Pos)
        | ((params_.txsr - 1) << FMC_SDTR_TXSR_Pos)
        | ((params_.tmrd - 1) << FMC_SDTR_TMRD_Pos);
    if (params_.bank == 1)
    {
        reg::FMC_Bank5_6->SDCR[0] = sdcr;
        reg::FMC_Bank5_6->SDTR[0] = sdtr;
    }
    else
    {
        reg::FMC_Bank5_6->SDCR[0] = sdcr & FMC_SDCR_DNC_Mask;
        reg::FMC_Bank5_6->SDCR[1] = sdcr;
        reg::FMC_Bank5_6->SDTR[0] = sdtr & FMC_SDTR_DNC_Mask;
        reg::FMC_Bank5_6->SDTR[1] = sdtr;
    }
    // Initialise SDRAM.
    uint32_t bank = params_.bank == 1 ? FMC_SDCMR_CTB1 : FMC_SDCMR_CTB2;
    while (reg::FMC_Bank5_6->SDSR & FMC_SDSR_BUSY)
        ;
    reg::FMC_Bank5_6->SDCMR = bank | FMC_SDCMR_MODE_ClockConfigEna;
    delay_us (params_.init_clock_delay_us);
    while (reg::FMC_Bank5_6->SDSR & FMC_SDSR_BUSY)
        ;
    reg::FMC_Bank5_6->SDCMR = bank | FMC_SDCMR_MODE_Pall;
    while (reg::FMC_Bank5_6->SDSR & FMC_SDSR_BUSY)
        ;
    reg::FMC_Bank5_6->SDCMR = bank
        | ((params_.init_auto_refresh - 1) << FMC_SDCMR_NRFS_Pos)
        | FMC_SDCMR_MODE_AutoRefresh;
    while (reg::FMC_Bank5_6->SDSR & FMC_SDSR_BUSY)
        ;
    uint32_t sdram_mode =
        SDRAM_MODE_BURST_LENGTH_1
        | SDRAM_MODE_BURST_TYPE_SEQUENTIAL
        | (params_.cas == 2 ? SDRAM_MODE_CAS_LATENCY_2
           : SDRAM_MODE_CAS_LATENCY_3)
        | SDRAM_MODE_OPERATING_MODE_STANDARD
        | SDRAM_MODE_WRITEBURST_MODE_SINGLE;
    reg::FMC_Bank5_6->SDCMR = bank | (sdram_mode << FMC_SDCMR_MRD_Pos)
        | FMC_SDCMR_MODE_LoadModeRegister;
    // Set refresh rate.
    int refresh_interval_ns = params_.tref_ms * 1000000
        / (1 << params_.row_bits);
    reg::FMC_Bank5_6->SDRTR = refresh_interval_ns / clock_ns - 20;
}

void *
Sdram::addr () const
{
    return params_.bank == 1 ? reinterpret_cast<void *> (FMC_BANK5_BASE)
        : reinterpret_cast<void *> (FMC_BANK6_BASE);
}

int
Sdram::size () const
{
    return params_.bits / 8 * (1 << params_.col_bits)
        * (1 << params_.row_bits) * params_.banks;
}

} // namespace ucoo
