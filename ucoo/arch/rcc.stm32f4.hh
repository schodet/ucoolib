#ifndef ucoo_arch_rcc_stm32f4_hh
#define ucoo_arch_rcc_stm32f4_hh
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
#include "ucoo/arch/arch.hh"
#include "ucoo/arch/reg.hh"

namespace ucoo {

enum class Bus
{
    AHB1 = 0,
    AHB2 = 1,
    AHB3 = 2,
    APB1 = 4,
    APB2 = 5,
};

/// Supply range.
enum class SupplyRange
{
    /// 1.8 V to 2.1 V.
    V1_8,
    /// 2.1 V to 2.4 V.
    V2_1,
    /// 2.4 V to 2.7 V.
    V2_4,
    /// 2.7 V to 3.6 V.
    V2_7,
};

namespace details {

constexpr uint32_t
rcc_enum (Bus bus, uint32_t bit, int pos = 0)
{
    return bit == 1
        ? (static_cast<uint32_t> (bus) << 5) | pos
        : rcc_enum (bus, (bit >> 1) | (bit << 31), pos + 1);
}

} // namespace details

/// Constants to handle reset and clock for each peripheral.
enum class Rcc
{
    GPIOA = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_GPIOAEN),
    GPIOB = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_GPIOBEN),
    GPIOC = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_GPIOCEN),
    GPIOD = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_GPIODEN),
    GPIOE = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_GPIOEEN),
    GPIOF = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_GPIOFEN),
    GPIOG = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_GPIOGEN),
    GPIOH = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_GPIOHEN),
    GPIOI = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_GPIOIEN),
    GPIOJ = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_GPIOJEN),
    GPIOK = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_GPIOKEN),
    CRC = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_CRCEN),
    BKPSRAM = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_BKPSRAMEN),
    CCMDATARAM = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_CCMDATARAMEN),
    DMA1 = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_DMA1EN),
    DMA2 = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_DMA2EN),
    DMA2D = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_DMA2DEN),
    ETHMAC = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_ETHMACEN),
    ETHMACTX = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_ETHMACTXEN),
    ETHMACRX = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_ETHMACRXEN),
    ETHMACPTP = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_ETHMACPTPEN),
    OTGHS = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_OTGHSEN),
    OTGHSULPI = details::rcc_enum (Bus::AHB1, RCC_AHB1ENR_OTGHSULPIEN),
    DCMI = details::rcc_enum (Bus::AHB2, RCC_AHB2ENR_DCMIEN),
    CRYP = details::rcc_enum (Bus::AHB2, RCC_AHB2ENR_CRYPEN),
    HASH = details::rcc_enum (Bus::AHB2, RCC_AHB2ENR_HASHEN),
    RNG = details::rcc_enum (Bus::AHB2, RCC_AHB2ENR_RNGEN),
    OTGFS = details::rcc_enum (Bus::AHB2, RCC_AHB2ENR_OTGFSEN),
    FMC = details::rcc_enum (Bus::AHB3, RCC_AHB3ENR_FMCEN),
#ifdef RCC_AHB3ENR_QSPIEN
    QUADSPI = details::rcc_enum (Bus::AHB3, RCC_AHB3ENR_QSPIEN),
#endif
    TIM2 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM2EN),
    TIM3 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM3EN),
    TIM4 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM4EN),
    TIM5 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM5EN),
    TIM6 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM6EN),
    TIM7 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM7EN),
    TIM12 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM12EN),
    TIM13 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM13EN),
    TIM14 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_TIM14EN),
    WWDG = details::rcc_enum (Bus::APB1, RCC_APB1ENR_WWDGEN),
    SPI2 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_SPI2EN),
    SPI3 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_SPI3EN),
    USART2 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_USART2EN),
    USART3 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_USART3EN),
    UART4 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_UART4EN),
    UART5 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_UART5EN),
    I2C1 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_I2C1EN),
    I2C2 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_I2C2EN),
    I2C3 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_I2C3EN),
    CAN1 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_CAN1EN),
    CAN2 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_CAN2EN),
    PWR = details::rcc_enum (Bus::APB1, RCC_APB1ENR_PWREN),
    DAC = details::rcc_enum (Bus::APB1, RCC_APB1ENR_DACEN),
    UART7 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_UART7EN),
    UART8 = details::rcc_enum (Bus::APB1, RCC_APB1ENR_UART8EN),
    TIM1 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_TIM1EN),
    TIM8 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_TIM8EN),
    USART1 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_USART1EN),
    USART6 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_USART6EN),
    ADC1 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_ADC1EN),
    ADC2 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_ADC2EN),
    ADC3 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_ADC3EN),
    SDIO = details::rcc_enum (Bus::APB2, RCC_APB2ENR_SDIOEN),
    SPI1 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_SPI1EN),
    SPI4 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_SPI4EN),
    SYSCFG = details::rcc_enum (Bus::APB2, RCC_APB2ENR_SYSCFGEN),
    TIM9 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_TIM9EN),
    TIM10 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_TIM10EN),
    TIM11 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_TIM11EN),
    SPI5 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_SPI5EN),
    SPI6 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_SPI6EN),
    SAI1 = details::rcc_enum (Bus::APB2, RCC_APB2ENR_SAI1EN),
    LTDC = details::rcc_enum (Bus::APB2, RCC_APB2ENR_LTDCEN),
#ifdef RCC_APB2ENR_DSIEN
    DSI = details::rcc_enum (Bus::APB2, RCC_APB2ENR_DSIEN),
#endif
};

/// Enable clock for given peripheral.
static inline void
rcc_peripheral_clock_enable (Rcc rcc)
{
    int bus_index = static_cast<uint32_t> (rcc) >> 5;
    int bit_index = static_cast<uint32_t> (rcc) & 0x1f;
    (&reg::RCC->AHB1ENR)[bus_index] |= 1 << bit_index;
    (&reg::RCC->AHB1ENR)[bus_index];
}

/// Disable clock for given peripheral.
static inline void
rcc_peripheral_clock_disable (Rcc rcc)
{
    int bus_index = static_cast<uint32_t> (rcc) >> 5;
    int bit_index = static_cast<uint32_t> (rcc) & 0x1f;
    (&reg::RCC->AHB1ENR)[bus_index] &= ~(1 << bit_index);
    (&reg::RCC->AHB1ENR)[bus_index];
}

/// Frequency of the main system clock.
extern int rcc_sys_freq_hz;

/// Frequency of AHB bus.
extern int rcc_ahb_freq_hz;

/// Frequency of APB1 bus.
extern int rcc_apb1_freq_hz;

/// Frequency of APB2 bus.
extern int rcc_apb2_freq_hz;

/// Frequency of timers on APB1 bus.
extern int rcc_apb1_timer_freq_hz;

/// Frequency of timers on APB2 bus.
extern int rcc_apb2_timer_freq_hz;

/// Frequency of the should-be-48-MHz clock (used for USB, RNG & SDIO).
extern int rcc_pll48_freq_hz;

/// Frequency of LCD clock;
extern int rcc_lcd_freq_hz;

/// Setup system clock using PLL, from HSE clock if not 0, else HSI clock.
/// Also setup flash access and voltage regulator scale.
void
rcc_sys_clock_setup_pll (int sys_freq_hz, int hse_freq_hz,
                         int pllm, int plln, int pllp, int pllq,
                         int apb1_pre, int apb2_pre,
                         SupplyRange vrange);

/// Setup SAI PLL.
void
rcc_sai_pll_setup (int hse_freq_hz,
                   int pllm, int plln, int pllp, int pllq, int pllq_div,
                   int pllr, int pllr_div);

} // namespace ucoo

#endif // ucoo_arch_rcc_stm32f4_hh
