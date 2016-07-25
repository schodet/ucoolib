// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2016 Nicolas Schodet
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
#include "ucoo/arch/rcc.stm32f4.hh"
#include "ucoo/common.hh"
#include "ucoo/utils/table_lookup.hh"

namespace ucoo {

static const int hsi_freq_hz = 16000000;

int rcc_sys_freq_hz = hsi_freq_hz;
int rcc_ahb_freq_hz = hsi_freq_hz;
int rcc_apb1_freq_hz = hsi_freq_hz;
int rcc_apb2_freq_hz = hsi_freq_hz;
int rcc_apb1_timer_freq_hz = hsi_freq_hz;
int rcc_apb2_timer_freq_hz = hsi_freq_hz;
int rcc_pll48_freq_hz = 0;

void
rcc_sys_clock_setup_pll (int sys_freq_hz, int hse_freq_hz,
                         int pllm, int plln, int pllp, int pllq,
                         int apb1_pre, int apb2_pre,
                         SupplyRange vrange)
{
    // Switch to HSI as a safe fall back.
    reg::RCC->CR |= RCC_CR_HSION;
    while (!(reg::RCC->CR & RCC_CR_HSIRDY))
        ;
    reg::RCC->CFGR = (reg::RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI;
    while ((reg::RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
        ;
    // Start HSE if needed.
    uint32_t pllsrc = RCC_PLLCFGR_PLLSRC_HSI;
    int pll_in_freq_hz = hsi_freq_hz;
    if (hse_freq_hz)
    {
        pll_in_freq_hz = hse_freq_hz;
        pllsrc = RCC_PLLCFGR_PLLSRC_HSE;
        reg::RCC->CR |= RCC_CR_HSEON;
        while (!(reg::RCC->CR & RCC_CR_HSERDY))
            ;
    }
    // Adapt voltage regulator scale.
    if (sys_freq_hz <= 120000000)
        reg::PWR->CR = (reg::PWR->CR & ~PWR_CR_VOS) | PWR_CR_VOS_Scale3;
    else if (sys_freq_hz <= 144000000)
        reg::PWR->CR = (reg::PWR->CR & ~PWR_CR_VOS) | PWR_CR_VOS_Scale2;
    else
        reg::PWR->CR = (reg::PWR->CR & ~PWR_CR_VOS) | PWR_CR_VOS_Scale1;
    // Change buses prescalers.
    uint32_t ppre = 0;
    static const LookupTable<int, uint32_t> apb1_pre_table[] =
    {
        { 1, RCC_CFGR_PPRE1_DIV1 },
        { 2, RCC_CFGR_PPRE1_DIV2 },
        { 4, RCC_CFGR_PPRE1_DIV4 },
        { 8, RCC_CFGR_PPRE1_DIV8 },
        { 16, RCC_CFGR_PPRE1_DIV16 },
    };
    ppre |= simple_table_lookup (apb1_pre_table, apb1_pre);
    static const LookupTable<int, uint32_t> apb2_pre_table[] =
    {
        { 1, RCC_CFGR_PPRE2_DIV1 },
        { 2, RCC_CFGR_PPRE2_DIV2 },
        { 4, RCC_CFGR_PPRE2_DIV4 },
        { 8, RCC_CFGR_PPRE2_DIV8 },
        { 16, RCC_CFGR_PPRE2_DIV16 },
    };
    ppre |= simple_table_lookup (apb2_pre_table, apb2_pre);
    reg::RCC->CFGR = (reg::RCC->CFGR & ~(RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2))
        | ppre;
    // Start PLL.
    reg::RCC->PLLCFGR = pllsrc
        | pllm << RCC_PLLCFGR_PLLM_Pos
        | plln << RCC_PLLCFGR_PLLN_Pos
        | (pllp / 2 - 1) << RCC_PLLCFGR_PLLP_Pos
        | pllq << RCC_PLLCFGR_PLLQ_Pos;
    reg::RCC->CR |= RCC_CR_PLLON;
    while (!(reg::RCC->CR & RCC_CR_PLLRDY))
        ;
    // Setup flash.
    int mhz_per_ws_per_vrange[] = { 20, 22, 24, 30 };
    int mhz_per_ws = mhz_per_ws_per_vrange[static_cast<int> (vrange)];
    int ws = (sys_freq_hz - 1) / 1000000 / mhz_per_ws;
    reg::FLASH->ACR = ws
        | FLASH_ACR_ICEN | FLASH_ACR_DCEN
        | (ws ? FLASH_ACR_PRFTEN : 0); // XXX not for buggy F40x!
    // Switch to PLL.
    reg::RCC->CFGR = (reg::RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
    while ((reg::RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        ;
    // Disable unused HSI.
    reg::RCC->CR &= ~RCC_CR_HSION;
    // Update frequencies.
    assert (pll_in_freq_hz / pllm * plln / pllp == sys_freq_hz);
    rcc_ahb_freq_hz = rcc_sys_freq_hz = sys_freq_hz;
    rcc_apb1_freq_hz = sys_freq_hz / apb1_pre;
    rcc_apb2_freq_hz = sys_freq_hz / apb2_pre;
    rcc_apb1_timer_freq_hz = rcc_apb1_freq_hz * (apb1_pre == 1 ? 1 : 2);
    rcc_apb2_timer_freq_hz = rcc_apb2_freq_hz * (apb2_pre == 1 ? 1 : 2);
    rcc_pll48_freq_hz = pll_in_freq_hz / pllm * plln / pllq;
}

} // namespace ucoo
