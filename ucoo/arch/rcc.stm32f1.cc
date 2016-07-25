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
#include "ucoo/arch/rcc.stm32f1.hh"
#include "ucoo/common.hh"
#include "ucoo/utils/table_lookup.hh"

namespace ucoo {

static const int hsi_freq_hz = 8000000;

int rcc_sys_freq_hz = hsi_freq_hz;
int rcc_ahb_freq_hz = hsi_freq_hz;
int rcc_apb1_freq_hz = hsi_freq_hz;
int rcc_apb2_freq_hz = hsi_freq_hz;
int rcc_apb1_timer_freq_hz = hsi_freq_hz;
int rcc_apb2_timer_freq_hz = hsi_freq_hz;
int rcc_usb_freq_hz = 0;

void
rcc_sys_clock_setup_pll (int sys_freq_hz, int hse_freq_hz,
                         int prediv1, int pllmul,
                         int apb1_pre, int apb2_pre, int adc_pre)
{
    // Switch to HSI as a safe fall back.
    reg::RCC->CR |= RCC_CR_HSION;
    while (!(reg::RCC->CR & RCC_CR_HSIRDY))
        ;
    reg::RCC->CFGR = (reg::RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI;
    while ((reg::RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
        ;
    // Start HSE if needed.
    uint32_t pllsrc = 0;
    int pll_in_freq_hz = hsi_freq_hz / 2;
    if (hse_freq_hz)
    {
        pll_in_freq_hz = hse_freq_hz;
        pllsrc = RCC_CFGR_PLLSRC;
        reg::RCC->CR |= RCC_CR_HSEON;
        while (!(reg::RCC->CR & RCC_CR_HSERDY))
            ;
        // May divide by 2.
        assert (prediv1 == 1 || prediv1 == 2);
        if (prediv1 == 2)
        {
            pllsrc |= RCC_CFGR_PLLXTPRE;
            pll_in_freq_hz /= 2;
        }
    }
    // Change buses and ADC prescalers.
    static const LookupTable<int, uint32_t> adc_pre_table[] =
    {
        { 2, RCC_CFGR_ADCPRE_DIV2 },
        { 4, RCC_CFGR_ADCPRE_DIV4 },
        { 6, RCC_CFGR_ADCPRE_DIV6 },
        { 8, RCC_CFGR_ADCPRE_DIV8 },
    };
    static const LookupTable<int, uint32_t> apb1_pre_table[] =
    {
        { 1, RCC_CFGR_PPRE1_DIV1 },
        { 2, RCC_CFGR_PPRE1_DIV2 },
        { 4, RCC_CFGR_PPRE1_DIV4 },
        { 8, RCC_CFGR_PPRE1_DIV8 },
        { 16, RCC_CFGR_PPRE1_DIV16 },
    };
    static const LookupTable<int, uint32_t> apb2_pre_table[] =
    {
        { 1, RCC_CFGR_PPRE2_DIV1 },
        { 2, RCC_CFGR_PPRE2_DIV2 },
        { 4, RCC_CFGR_PPRE2_DIV4 },
        { 8, RCC_CFGR_PPRE2_DIV8 },
        { 16, RCC_CFGR_PPRE2_DIV16 },
    };
    uint32_t pre = simple_table_lookup (adc_pre_table, adc_pre)
        | simple_table_lookup (apb1_pre_table, apb1_pre)
        | simple_table_lookup (apb2_pre_table, apb2_pre);
    // Start PLL.
    static const LookupTable<int, uint32_t> pllmul_table[] =
    {
        { 4, RCC_CFGR_PLLMULL4 },
        { 5, RCC_CFGR_PLLMULL5 },
        { 6, RCC_CFGR_PLLMULL6 },
        { 7, RCC_CFGR_PLLMULL7 },
        { 8, RCC_CFGR_PLLMULL8 },
        { 9, RCC_CFGR_PLLMULL9 },
        // Other values (2, 3, 10...16 or 6.5 on connectivity line) not
        // handled.
    };
    reg::RCC->CFGR = (reg::RCC->CFGR & ~(
            RCC_CFGR_PLLMULL | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC
            | RCC_CFGR_ADCPRE_Msk | RCC_CFGR_PPRE1_Msk | RCC_CFGR_PPRE2_Msk))
        | pllsrc | pre | simple_table_lookup (pllmul_table, pllmul);
    reg::RCC->CR |= RCC_CR_PLLON;
    while (!(reg::RCC->CR & RCC_CR_PLLRDY))
        ;
    // Setup flash.
    int mhz_per_ws = 24;
    int ws = (sys_freq_hz - 1) / 1000000 / mhz_per_ws;
    reg::FLASH->ACR = (reg::FLASH->ACR & ~FLASH_ACR_LATENCY) | ws;
    // Switch to PLL.
    reg::RCC->CFGR = (reg::RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
    while ((reg::RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        ;
    // Update frequencies.
    assert (pll_in_freq_hz * pllmul == sys_freq_hz);
    rcc_ahb_freq_hz = rcc_sys_freq_hz = sys_freq_hz;
    rcc_apb1_freq_hz = sys_freq_hz / apb1_pre;
    rcc_apb2_freq_hz = sys_freq_hz / apb2_pre;
    rcc_apb1_timer_freq_hz = rcc_apb1_freq_hz * (apb1_pre == 1 ? 1 : 2);
    rcc_apb2_timer_freq_hz = rcc_apb2_freq_hz * (apb2_pre == 1 ? 1 : 2);
    rcc_usb_freq_hz = sys_freq_hz * 2 / 3;
}

} // namespace ucoo
