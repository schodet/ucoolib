#ifndef ucoo_arch_reg_stm32f4_hh
#define ucoo_arch_reg_stm32f4_hh
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
// Use a superset of all supported chips.
#include "ST/STM32F4xx/Include/stm32f479xx.h"
#include "ucoo/arch/reg.stm32.hh"

#define SCB_CPACR_CP10_Pos 20
#define SCB_CPACR_CP10_Denied (0 << SCB_CPACR_CP10_Pos)
#define SCB_CPACR_CP10_Privileged (1 << SCB_CPACR_CP10_Pos)
#define SCB_CPACR_CP10_Full (3 << SCB_CPACR_CP10_Pos)
#define SCB_CPACR_CP10_Msk (3 << SCB_CPACR_CP10_Pos)

#define SCB_CPACR_CP11_Pos 22
#define SCB_CPACR_CP11_Denied (0 << SCB_CPACR_CP11_Pos)
#define SCB_CPACR_CP11_Privileged (1 << SCB_CPACR_CP11_Pos)
#define SCB_CPACR_CP11_Full (3 << SCB_CPACR_CP11_Pos)
#define SCB_CPACR_CP11_Msk (3 << SCB_CPACR_CP11_Pos)

#define PWR_CR_VOS_Scale3 (PWR_CR_VOS_0)
#define PWR_CR_VOS_Scale2 (PWR_CR_VOS_1)
#define PWR_CR_VOS_Scale1 (PWR_CR_VOS_1 | PWR_CR_VOS_0)

#define RCC_PLLCFGR_PLLM_Pos 0
#define RCC_PLLCFGR_PLLN_Pos 6
#define RCC_PLLCFGR_PLLP_Pos 16
#define RCC_PLLCFGR_PLLQ_Pos 24
#ifdef RCC_PLLCFGR_PLLR
# define RCC_PLLCFGR_PLLR_Pos 28
#endif

#define RCC_DCKCFGR_PLLSAIDIVQ_Pos 8
#define RCC_DCKCFGR_PLLSAIDIVR_Div2 0
#define RCC_DCKCFGR_PLLSAIDIVR_Div4 (1 << 16)
#define RCC_DCKCFGR_PLLSAIDIVR_Div8 (2 << 16)
#define RCC_DCKCFGR_PLLSAIDIVR_Div16 (3 << 16)

#define GPIO_MODER_Input 0
#define GPIO_MODER_Output 1
#define GPIO_MODER_AF 2
#define GPIO_MODER_Analog 3

#define SPI_CR1_BR_Pos 3

#define USB_OTG_GRXSTSP_BCNT_Pos 4
#define USB_OTG_GRXSTSP_PKTSTS_Pos 17
#define USB_OTG_DOEPTSIZ_PKTCNT_Pos 19
#define USB_OTG_DIEPTSIZ_PKTCNT_Pos 19

#define FLASH_CR_PSIZE_x32 FLASH_CR_PSIZE_1
#define FLASH_CR_PSIZE_x8 0

#define FMC_SDCR_RPIPE_None 0
#define FMC_SDCR_RPIPE_1Clk FMC_SDCR1_RPIPE_0
#define FMC_SDCR_RPIPE_2Clk FMC_SDCR1_RPIPE_1
#define FMC_SDCR_RBURST FMC_SDCR1_RBURST
#define FMC_SDCR_SDCLK_Disabled 0
#define FMC_SDCR_SDCLK_2Hclk FMC_SDCR1_SDCLK_1
#define FMC_SDCR_SDCLK_3Hclk (FMC_SDCR1_SDCLK_1 | FMC_SDCR1_SDCLK_0)
#define FMC_SDCR_CAS_Pos 7
#define FMC_SDCR_NB FMC_SDCR1_NB
#define FMC_SDCR_MWID_8B 0
#define FMC_SDCR_MWID_16B FMC_SDCR1_MWID_0
#define FMC_SDCR_MWID_32B FMC_SDCR1_MWID_1
#define FMC_SDCR_NR_Pos 2
#define FMC_SDCR_NC_Pos 0

#define FMC_SDTR_TRCD_Pos 24
#define FMC_SDTR_TRP_Pos 20
#define FMC_SDTR_TWR_Pos 16
#define FMC_SDTR_TRC_Pos 12
#define FMC_SDTR_TRAS_Pos 8
#define FMC_SDTR_TXSR_Pos 4
#define FMC_SDTR_TMRD_Pos 0

#define FMC_SDCR_DNC_Mask (FMC_SDCR1_SDCLK | FMC_SDCR1_RPIPE \
                           | FMC_SDCR1_RBURST)
#define FMC_SDTR_DNC_Mask (FMC_SDTR1_TRP | FMC_SDTR1_TRC)

#define FMC_SDCMR_MODE_Normal 0
#define FMC_SDCMR_MODE_ClockConfigEna 1
#define FMC_SDCMR_MODE_Pall 2
#define FMC_SDCMR_MODE_AutoRefresh 3
#define FMC_SDCMR_MODE_LoadModeRegister 4
#define FMC_SDCMR_MODE_SelfRefresh 5
#define FMC_SDCMR_MODE_PowerDown 6

#define FMC_SDCMR_NRFS_Pos 5
#define FMC_SDCMR_MRD_Pos 9

#define SDRAM_MODE_BURST_LENGTH_1 0x0000
#define SDRAM_MODE_BURST_LENGTH_2 0x0001
#define SDRAM_MODE_BURST_LENGTH_4 0x0002
#define SDRAM_MODE_BURST_LENGTH_8 0x0004
#define SDRAM_MODE_BURST_TYPE_SEQUENTIAL 0x0000
#define SDRAM_MODE_BURST_TYPE_INTERLEAVED 0x0008
#define SDRAM_MODE_CAS_LATENCY_2 0x0020
#define SDRAM_MODE_CAS_LATENCY_3 0x0030
#define SDRAM_MODE_OPERATING_MODE_STANDARD 0x0000
#define SDRAM_MODE_WRITEBURST_MODE_PROGRAMMED 0x0000
#define SDRAM_MODE_WRITEBURST_MODE_SINGLE 0x0200

#define FMC_BANK5_BASE 0xc0000000
#define FMC_BANK6_BASE 0xd0000000

#define DESIG_BASE 0x1fff7a10
#define DESIG_FLASH_SIZE_BASE 0x1fff7a22
#define DESIG_UNIQUE_ID_BASE 0x1fff7a10

#define DMA2D_CR_MODE_MemoryToMemory 0
#define DMA2D_CR_MODE_MemoryToMemoryFPC DMA2D_CR_MODE_0
#define DMA2D_CR_MODE_MemoryToMemoryBlend DMA2D_CR_MODE_1
#define DMA2D_CR_MODE_RegisterToMemory (DMA2D_CR_MODE_1 | DMA2D_CR_MODE_0)

#ifdef DSI_BASE
# define DSI_VMCR_VMT_NonBurstSyncPulses 0
# define DSI_VMCR_VMT_NonBurstSyncEvents 1
# define DSI_VMCR_VMT_Burst 2
# define DSI_LCOLCR_COLC_16BitConf1 0
# define DSI_LCOLCR_COLC_16BitConf2 DSI_LCOLCR_COLC0
# define DSI_LCOLCR_COLC_16BitConf3 DSI_LCOLCR_COLC1
# define DSI_LCOLCR_COLC_18BitConf1 (DSI_LCOLCR_COLC1 | DSI_LCOLCR_COLC0)
# define DSI_LCOLCR_COLC_18BitConf2 DSI_LCOLCR_COLC2
# define DSI_LCOLCR_COLC_24Bit (DSI_LCOLCR_COLC2 | DSI_LCOLCR_COLC0)
# define DSI_WCFGR_COLMUX_16BitConf1 0
# define DSI_WCFGR_COLMUX_16BitConf2 DSI_WCFGR_COLMUX0
# define DSI_WCFGR_COLMUX_16BitConf3 DSI_WCFGR_COLMUX1
# define DSI_WCFGR_COLMUX_18BitConf1 (DSI_WCFGR_COLMUX1 | DSI_WCFGR_COLMUX0)
# define DSI_WCFGR_COLMUX_18BitConf2 DSI_WCFGR_COLMUX2
# define DSI_WCFGR_COLMUX_24Bit (DSI_WCFGR_COLMUX2 | DSI_WCFGR_COLMUX0)
# define DSI_WRPCR_PLL_NDIV_Pos 2
# define DSI_WRPCR_PLL_IDF_Pos 11
# define DSI_WRPCR_PLL_ODF_Div1 0
# define DSI_WRPCR_PLL_ODF_Div2 DSI_WRPCR_PLL_ODF0
# define DSI_WRPCR_PLL_ODF_Div4 DSI_WRPCR_PLL_ODF1
# define DSI_WRPCR_PLL_ODF_Div8 (DSI_WRPCR_PLL_ODF1 | DSI_WRPCR_PLL_ODF0)
#endif

struct DESIG_TypeDef
{
    __IO uint32_t U_ID[3];
    uint32_t reserved0x1c;
    uint16_t reserved0x20;
    __IO uint16_t FLASH_SIZE;
};

#undef FPU
#undef SysTick

#undef TIM2
#undef TIM3
#undef TIM4
#undef TIM5
#undef TIM6
#undef TIM7
#undef TIM12
#undef TIM13
#undef TIM14
#undef RTC
#undef WWDG
#undef IWDG
#undef I2S2ext
#undef SPI2
#undef SPI3
#undef I2S3ext
#undef USART2
#undef USART3
#undef UART4
#undef UART5
#undef I2C1
#undef I2C2
#undef I2C3
#undef CAN1
#undef CAN2
#undef PWR
#undef DAC
#undef UART7
#undef UART8
#undef TIM1
#undef TIM8
#undef USART1
#undef USART6
#undef ADC
#undef ADC1
#undef ADC2
#undef ADC3
#undef SDIO
#undef SPI1
#undef SPI4
#undef SYSCFG
#undef EXTI
#undef TIM9
#undef TIM10
#undef TIM11
#undef SPI5
#undef SPI6
#undef SAI1
#undef SAI1_Block_A
#undef SAI1_Block_B
#undef LTDC
#undef LTDC_Layer1
#undef LTDC_Layer2
#undef DSI
#undef GPIOA
#undef GPIOB
#undef GPIOC
#undef GPIOD
#undef GPIOE
#undef GPIOF
#undef GPIOG
#undef GPIOH
#undef GPIOI
#undef GPIOJ
#undef GPIOK
#undef CRC
#undef RCC
#undef FLASH
#undef DMA1
#undef DMA1_Stream0
#undef DMA1_Stream1
#undef DMA1_Stream2
#undef DMA1_Stream3
#undef DMA1_Stream4
#undef DMA1_Stream5
#undef DMA1_Stream6
#undef DMA1_Stream7
#undef DMA2
#undef DMA2_Stream0
#undef DMA2_Stream1
#undef DMA2_Stream2
#undef DMA2_Stream3
#undef DMA2_Stream4
#undef DMA2_Stream5
#undef DMA2_Stream6
#undef DMA2_Stream7
#undef ETH
#undef DMA2D
#undef DCMI
#undef CRYP
#undef HASH
#undef HASH_DIGEST
#undef RNG
#undef FMC_Bank1
#undef FMC_Bank1E
#undef FMC_Bank2_3
#undef FMC_Bank4
#undef FMC_Bank5_6
#undef QUADSPI
#undef DBGMCU
#undef USB_OTG_FS
#undef USB_OTG_HS

namespace ucoo {

namespace reg {

constexpr auto FPU = reinterpret_cast<FPU_Type *> (FPU_BASE);
constexpr auto SysTick = reinterpret_cast<SysTick_Type *> (SysTick_BASE);

constexpr auto TIM2 = reinterpret_cast<TIM_TypeDef *> (TIM2_BASE);
constexpr auto TIM3 = reinterpret_cast<TIM_TypeDef *> (TIM3_BASE);
constexpr auto TIM4 = reinterpret_cast<TIM_TypeDef *> (TIM4_BASE);
constexpr auto TIM5 = reinterpret_cast<TIM_TypeDef *> (TIM5_BASE);
constexpr auto TIM6 = reinterpret_cast<TIM_TypeDef *> (TIM6_BASE);
constexpr auto TIM7 = reinterpret_cast<TIM_TypeDef *> (TIM7_BASE);
constexpr auto TIM12 = reinterpret_cast<TIM_TypeDef *> (TIM12_BASE);
constexpr auto TIM13 = reinterpret_cast<TIM_TypeDef *> (TIM13_BASE);
constexpr auto TIM14 = reinterpret_cast<TIM_TypeDef *> (TIM14_BASE);
constexpr auto RTC = reinterpret_cast<RTC_TypeDef *> (RTC_BASE);
constexpr auto WWDG = reinterpret_cast<WWDG_TypeDef *> (WWDG_BASE);
constexpr auto IWDG = reinterpret_cast<IWDG_TypeDef *> (IWDG_BASE);
constexpr auto I2S2ext = reinterpret_cast<SPI_TypeDef *> (I2S2ext_BASE);
constexpr auto SPI2 = reinterpret_cast<SPI_TypeDef *> (SPI2_BASE);
constexpr auto SPI3 = reinterpret_cast<SPI_TypeDef *> (SPI3_BASE);
constexpr auto I2S3ext = reinterpret_cast<SPI_TypeDef *> (I2S3ext_BASE);
constexpr auto USART2 = reinterpret_cast<USART_TypeDef *> (USART2_BASE);
constexpr auto USART3 = reinterpret_cast<USART_TypeDef *> (USART3_BASE);
constexpr auto UART4 = reinterpret_cast<USART_TypeDef *> (UART4_BASE);
constexpr auto UART5 = reinterpret_cast<USART_TypeDef *> (UART5_BASE);
constexpr auto I2C1 = reinterpret_cast<I2C_TypeDef *> (I2C1_BASE);
constexpr auto I2C2 = reinterpret_cast<I2C_TypeDef *> (I2C2_BASE);
constexpr auto I2C3 = reinterpret_cast<I2C_TypeDef *> (I2C3_BASE);
constexpr auto CAN1 = reinterpret_cast<CAN_TypeDef *> (CAN1_BASE);
constexpr auto CAN2 = reinterpret_cast<CAN_TypeDef *> (CAN2_BASE);
constexpr auto PWR = reinterpret_cast<PWR_TypeDef *> (PWR_BASE);
constexpr auto DAC = reinterpret_cast<DAC_TypeDef *> (DAC_BASE);
constexpr auto UART7 = reinterpret_cast<USART_TypeDef *> (UART7_BASE);
constexpr auto UART8 = reinterpret_cast<USART_TypeDef *> (UART8_BASE);
constexpr auto TIM1 = reinterpret_cast<TIM_TypeDef *> (TIM1_BASE);
constexpr auto TIM8 = reinterpret_cast<TIM_TypeDef *> (TIM8_BASE);
constexpr auto USART1 = reinterpret_cast<USART_TypeDef *> (USART1_BASE);
constexpr auto USART6 = reinterpret_cast<USART_TypeDef *> (USART6_BASE);
constexpr auto ADC = reinterpret_cast<ADC_Common_TypeDef *> (ADC_BASE);
constexpr auto ADC1 = reinterpret_cast<ADC_TypeDef *> (ADC1_BASE);
constexpr auto ADC2 = reinterpret_cast<ADC_TypeDef *> (ADC2_BASE);
constexpr auto ADC3 = reinterpret_cast<ADC_TypeDef *> (ADC3_BASE);
constexpr auto SDIO = reinterpret_cast<SDIO_TypeDef *> (SDIO_BASE);
constexpr auto SPI1 = reinterpret_cast<SPI_TypeDef *> (SPI1_BASE) ;
constexpr auto SPI4 = reinterpret_cast<SPI_TypeDef *> (SPI4_BASE);
constexpr auto SYSCFG = reinterpret_cast<SYSCFG_TypeDef *> (SYSCFG_BASE);
constexpr auto EXTI = reinterpret_cast<EXTI_TypeDef *> (EXTI_BASE);
constexpr auto TIM9 = reinterpret_cast<TIM_TypeDef *> (TIM9_BASE);
constexpr auto TIM10 = reinterpret_cast<TIM_TypeDef *> (TIM10_BASE);
constexpr auto TIM11 = reinterpret_cast<TIM_TypeDef *> (TIM11_BASE);
constexpr auto SPI5 = reinterpret_cast<SPI_TypeDef *> (SPI5_BASE);
constexpr auto SPI6 = reinterpret_cast<SPI_TypeDef *> (SPI6_BASE);
constexpr auto SAI1 = reinterpret_cast<SAI_TypeDef *> (SAI1_BASE);
constexpr auto SAI1_Block_A = reinterpret_cast<SAI_Block_TypeDef *> (SAI1_Block_A_BASE);
constexpr auto SAI1_Block_B = reinterpret_cast<SAI_Block_TypeDef *> (SAI1_Block_B_BASE);
constexpr auto LTDC = reinterpret_cast<LTDC_TypeDef *> (LTDC_BASE);
constexpr auto LTDC_Layer1 = reinterpret_cast<LTDC_Layer_TypeDef *> (LTDC_Layer1_BASE);
constexpr auto LTDC_Layer2 = reinterpret_cast<LTDC_Layer_TypeDef *> (LTDC_Layer2_BASE);
#ifdef DSI_BASE
constexpr auto DSI = reinterpret_cast<DSI_TypeDef *> (DSI_BASE);
#endif
constexpr auto GPIOA = reinterpret_cast<GPIO_TypeDef *> (GPIOA_BASE);
constexpr auto GPIOB = reinterpret_cast<GPIO_TypeDef *> (GPIOB_BASE);
constexpr auto GPIOC = reinterpret_cast<GPIO_TypeDef *> (GPIOC_BASE);
constexpr auto GPIOD = reinterpret_cast<GPIO_TypeDef *> (GPIOD_BASE);
constexpr auto GPIOE = reinterpret_cast<GPIO_TypeDef *> (GPIOE_BASE);
constexpr auto GPIOF = reinterpret_cast<GPIO_TypeDef *> (GPIOF_BASE);
constexpr auto GPIOG = reinterpret_cast<GPIO_TypeDef *> (GPIOG_BASE);
constexpr auto GPIOH = reinterpret_cast<GPIO_TypeDef *> (GPIOH_BASE);
constexpr auto GPIOI = reinterpret_cast<GPIO_TypeDef *> (GPIOI_BASE);
constexpr auto GPIOJ = reinterpret_cast<GPIO_TypeDef *> (GPIOJ_BASE);
constexpr auto GPIOK = reinterpret_cast<GPIO_TypeDef *> (GPIOK_BASE);
constexpr auto CRC = reinterpret_cast<CRC_TypeDef *> (CRC_BASE);
constexpr auto RCC = reinterpret_cast<RCC_TypeDef *> (RCC_BASE);
constexpr auto FLASH = reinterpret_cast<FLASH_TypeDef *> (FLASH_R_BASE);
constexpr auto DMA1 = reinterpret_cast<DMA_TypeDef *> (DMA1_BASE);
constexpr auto DMA1_Stream0 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA1_Stream0_BASE);
constexpr auto DMA1_Stream1 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA1_Stream1_BASE);
constexpr auto DMA1_Stream2 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA1_Stream2_BASE);
constexpr auto DMA1_Stream3 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA1_Stream3_BASE);
constexpr auto DMA1_Stream4 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA1_Stream4_BASE);
constexpr auto DMA1_Stream5 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA1_Stream5_BASE);
constexpr auto DMA1_Stream6 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA1_Stream6_BASE);
constexpr auto DMA1_Stream7 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA1_Stream7_BASE);
constexpr auto DMA2 = reinterpret_cast<DMA_TypeDef *> (DMA2_BASE);
constexpr auto DMA2_Stream0 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA2_Stream0_BASE);
constexpr auto DMA2_Stream1 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA2_Stream1_BASE);
constexpr auto DMA2_Stream2 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA2_Stream2_BASE);
constexpr auto DMA2_Stream3 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA2_Stream3_BASE);
constexpr auto DMA2_Stream4 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA2_Stream4_BASE);
constexpr auto DMA2_Stream5 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA2_Stream5_BASE);
constexpr auto DMA2_Stream6 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA2_Stream6_BASE);
constexpr auto DMA2_Stream7 = reinterpret_cast<DMA_Stream_TypeDef *> (DMA2_Stream7_BASE);
constexpr auto ETH = reinterpret_cast<ETH_TypeDef *> (ETH_BASE)  ;
constexpr auto DMA2D = reinterpret_cast<DMA2D_TypeDef *> (DMA2D_BASE);
constexpr auto DCMI = reinterpret_cast<DCMI_TypeDef *> (DCMI_BASE);
constexpr auto CRYP = reinterpret_cast<CRYP_TypeDef *> (CRYP_BASE);
constexpr auto HASH = reinterpret_cast<HASH_TypeDef *> (HASH_BASE);
constexpr auto HASH_DIGEST = reinterpret_cast<HASH_DIGEST_TypeDef *> (HASH_DIGEST_BASE);
constexpr auto RNG = reinterpret_cast<RNG_TypeDef *> (RNG_BASE);
constexpr auto FMC_Bank1 = reinterpret_cast<FMC_Bank1_TypeDef *> (FMC_Bank1_R_BASE);
constexpr auto FMC_Bank1E = reinterpret_cast<FMC_Bank1E_TypeDef *> (FMC_Bank1E_R_BASE);
#ifdef FMC_Bank2_3_R_BASE
constexpr auto FMC_Bank2_3 = reinterpret_cast<FMC_Bank2_3_TypeDef *> (FMC_Bank2_3_R_BASE);
#endif
#ifdef FMC_Bank4_R_BASE
constexpr auto FMC_Bank4 = reinterpret_cast<FMC_Bank4_TypeDef *> (FMC_Bank4_R_BASE);
#endif
constexpr auto FMC_Bank5_6 = reinterpret_cast<FMC_Bank5_6_TypeDef *> (FMC_Bank5_6_R_BASE);
#ifdef QSPI_R_BASE
constexpr auto QUADSPI = reinterpret_cast<QUADSPI_TypeDef *> (QSPI_R_BASE);
#endif
constexpr auto DBGMCU = reinterpret_cast<DBGMCU_TypeDef *> (DBGMCU_BASE);
constexpr auto USB_OTG_FS = reinterpret_cast<USB_OTG_TypeDef *> (USB_OTG_FS_PERIPH_BASE);
constexpr auto USB_OTG_HS = reinterpret_cast<USB_OTG_TypeDef *> (USB_OTG_HS_PERIPH_BASE);
constexpr auto DESIG = reinterpret_cast<DESIG_TypeDef *> (DESIG_BASE);

} // namespace reg

} // namespace ucoo

#endif // ucoo_arch_reg_stm32f4_hh
