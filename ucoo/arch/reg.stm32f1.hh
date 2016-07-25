#ifndef ucoo_arch_reg_stm32f1_hh
#define ucoo_arch_reg_stm32f1_hh
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
#include "ST/STM32F1xx/Include/stm32f105xc.h"
#include "ucoo/arch/reg.stm32.hh"

#define GPIO_CNF_Input_Analog 0
#define GPIO_CNF_Input_Float 1
#define GPIO_CNF_Input_PullUpDown 2

#define GPIO_CNF_Output_PushPull 0
#define GPIO_CNF_Output_OpenDrain 1
#define GPIO_CNF_Output_AFPushPull 2
#define GPIO_CNF_Output_AFOpenDrain 3

#define GPIO_MODE_Input 0
#define GPIO_MODE_Output_2MHz 1
#define GPIO_MODE_Output_10MHz 2
#define GPIO_MODE_Output_50MHz 3

#define DESIG_BASE 0x1ffff7e0

struct DESIG_TypeDef
{
    __IO uint16_t FLASH_SIZE;
    uint16_t reserved1;
    uint32_t reserved2;
    __IO uint32_t U_ID[3];
};

#undef SysTick

#undef TIM2
#undef TIM3
#undef TIM4
#undef TIM5
#undef TIM6
#undef TIM7
#undef RTC
#undef WWDG
#undef IWDG
#undef SPI2
#undef SPI3
#undef USART2
#undef USART3
#undef UART4
#undef UART5
#undef I2C1
#undef I2C2
#undef CAN1
#undef CAN2
#undef BKP
#undef PWR
#undef DAC
#undef AFIO
#undef EXTI
#undef GPIOA
#undef GPIOB
#undef GPIOC
#undef GPIOD
#undef GPIOE
#undef ADC1
#undef ADC2
#undef ADC12_COMMON
#undef TIM1
#undef SPI1
#undef USART1
#undef SDIO
#undef DMA1
#undef DMA2
#undef DMA1_Channel1
#undef DMA1_Channel2
#undef DMA1_Channel3
#undef DMA1_Channel4
#undef DMA1_Channel5
#undef DMA1_Channel6
#undef DMA1_Channel7
#undef DMA2_Channel1
#undef DMA2_Channel2
#undef DMA2_Channel3
#undef DMA2_Channel4
#undef DMA2_Channel5
#undef RCC
#undef CRC
#undef FLASH
#undef OB
#undef DBGMCU
#undef USB_OTG_FS

namespace ucoo {

namespace reg {

constexpr auto SysTick = reinterpret_cast<SysTick_Type *> (SysTick_BASE);

constexpr auto TIM2 = reinterpret_cast<TIM_TypeDef *> (TIM2_BASE);
constexpr auto TIM3 = reinterpret_cast<TIM_TypeDef *> (TIM3_BASE);
constexpr auto TIM4 = reinterpret_cast<TIM_TypeDef *> (TIM4_BASE);
constexpr auto TIM5 = reinterpret_cast<TIM_TypeDef *> (TIM5_BASE);
constexpr auto TIM6 = reinterpret_cast<TIM_TypeDef *> (TIM6_BASE);
constexpr auto TIM7 = reinterpret_cast<TIM_TypeDef *> (TIM7_BASE);
constexpr auto RTC = reinterpret_cast<RTC_TypeDef *> (RTC_BASE);
constexpr auto WWDG = reinterpret_cast<WWDG_TypeDef *> (WWDG_BASE);
constexpr auto IWDG = reinterpret_cast<IWDG_TypeDef *> (IWDG_BASE);
constexpr auto SPI2 = reinterpret_cast<SPI_TypeDef *> (SPI2_BASE);
constexpr auto SPI3 = reinterpret_cast<SPI_TypeDef *> (SPI3_BASE);
constexpr auto USART2 = reinterpret_cast<USART_TypeDef *> (USART2_BASE);
constexpr auto USART3 = reinterpret_cast<USART_TypeDef *> (USART3_BASE);
constexpr auto UART4 = reinterpret_cast<USART_TypeDef *> (UART4_BASE);
constexpr auto UART5 = reinterpret_cast<USART_TypeDef *> (UART5_BASE);
constexpr auto I2C1 = reinterpret_cast<I2C_TypeDef *> (I2C1_BASE);
constexpr auto I2C2 = reinterpret_cast<I2C_TypeDef *> (I2C2_BASE);
constexpr auto CAN1 = reinterpret_cast<CAN_TypeDef *> (CAN1_BASE);
constexpr auto CAN2 = reinterpret_cast<CAN_TypeDef *> (CAN2_BASE);
constexpr auto BKP = reinterpret_cast<BKP_TypeDef *> (BKP_BASE);
constexpr auto PWR = reinterpret_cast<PWR_TypeDef *> (PWR_BASE);
constexpr auto DAC = reinterpret_cast<DAC_TypeDef *> (DAC_BASE);
constexpr auto AFIO = reinterpret_cast<AFIO_TypeDef *> (AFIO_BASE);
constexpr auto EXTI = reinterpret_cast<EXTI_TypeDef *> (EXTI_BASE);
constexpr auto GPIOA = reinterpret_cast<GPIO_TypeDef *> (GPIOA_BASE);
constexpr auto GPIOB = reinterpret_cast<GPIO_TypeDef *> (GPIOB_BASE);
constexpr auto GPIOC = reinterpret_cast<GPIO_TypeDef *> (GPIOC_BASE);
constexpr auto GPIOD = reinterpret_cast<GPIO_TypeDef *> (GPIOD_BASE);
constexpr auto GPIOE = reinterpret_cast<GPIO_TypeDef *> (GPIOE_BASE);
constexpr auto ADC1 = reinterpret_cast<ADC_TypeDef *> (ADC1_BASE);
constexpr auto ADC2 = reinterpret_cast<ADC_TypeDef *> (ADC2_BASE);
constexpr auto ADC12_COMMON = reinterpret_cast<ADC_Common_TypeDef *> (ADC1_BASE);
constexpr auto TIM1 = reinterpret_cast<TIM_TypeDef *> (TIM1_BASE);
constexpr auto SPI1 = reinterpret_cast<SPI_TypeDef *> (SPI1_BASE);
constexpr auto USART1 = reinterpret_cast<USART_TypeDef *> (USART1_BASE);
constexpr auto SDIO = reinterpret_cast<SDIO_TypeDef *> (SDIO_BASE);
constexpr auto DMA1 = reinterpret_cast<DMA_TypeDef *> (DMA1_BASE);
constexpr auto DMA2 = reinterpret_cast<DMA_TypeDef *> (DMA2_BASE);
constexpr auto DMA1_Channel1 = reinterpret_cast<DMA_Channel_TypeDef *> (DMA1_Channel1_BASE);
constexpr auto DMA1_Channel2 = reinterpret_cast<DMA_Channel_TypeDef *> (DMA1_Channel2_BASE);
constexpr auto DMA1_Channel3 = reinterpret_cast<DMA_Channel_TypeDef *> (DMA1_Channel3_BASE);
constexpr auto DMA1_Channel4 = reinterpret_cast<DMA_Channel_TypeDef *> (DMA1_Channel4_BASE);
constexpr auto DMA1_Channel5 = reinterpret_cast<DMA_Channel_TypeDef *> (DMA1_Channel5_BASE);
constexpr auto DMA1_Channel6 = reinterpret_cast<DMA_Channel_TypeDef *> (DMA1_Channel6_BASE);
constexpr auto DMA1_Channel7 = reinterpret_cast<DMA_Channel_TypeDef *> (DMA1_Channel7_BASE);
constexpr auto DMA2_Channel1 = reinterpret_cast<DMA_Channel_TypeDef *> (DMA2_Channel1_BASE);
constexpr auto DMA2_Channel2 = reinterpret_cast<DMA_Channel_TypeDef *> (DMA2_Channel2_BASE);
constexpr auto DMA2_Channel3 = reinterpret_cast<DMA_Channel_TypeDef *> (DMA2_Channel3_BASE);
constexpr auto DMA2_Channel4 = reinterpret_cast<DMA_Channel_TypeDef *> (DMA2_Channel4_BASE);
constexpr auto DMA2_Channel5 = reinterpret_cast<DMA_Channel_TypeDef *> (DMA2_Channel5_BASE);
constexpr auto RCC = reinterpret_cast<RCC_TypeDef *> (RCC_BASE);
constexpr auto CRC = reinterpret_cast<CRC_TypeDef *> (CRC_BASE);
constexpr auto FLASH = reinterpret_cast<FLASH_TypeDef *> (FLASH_R_BASE);
constexpr auto OB = reinterpret_cast<OB_TypeDef *> (OB_BASE);
constexpr auto DBGMCU = reinterpret_cast<DBGMCU_TypeDef *> (DBGMCU_BASE);
constexpr auto USB_OTG_FS = reinterpret_cast<USB_OTG_TypeDef *> (USB_OTG_FS_PERIPH_BASE);
constexpr auto DESIG = reinterpret_cast<DESIG_TypeDef *> (DESIG_BASE);

} // namespace reg

} // namespace ucoo

#endif // ucoo_arch_reg_stm32f1_hh
