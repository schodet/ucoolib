#ifndef ucoo_arch_vector_stm32f4_hh
#define ucoo_arch_vector_stm32f4_hh
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

#define UCOO_VECTOR_IRQ \
    interrupt<Irq::WWDG>, \
    interrupt<Irq::PVD>, \
    interrupt<Irq::TAMP_STAMP>, \
    interrupt<Irq::RTC_WKUP>, \
    interrupt<Irq::FLASH>, \
    interrupt<Irq::RCC>, \
    interrupt<Irq::EXTI0>, \
    interrupt<Irq::EXTI1>, \
    interrupt<Irq::EXTI2>, \
    interrupt<Irq::EXTI3>, \
    interrupt<Irq::EXTI4>, \
    interrupt<Irq::DMA1_Stream0>, \
    interrupt<Irq::DMA1_Stream1>, \
    interrupt<Irq::DMA1_Stream2>, \
    interrupt<Irq::DMA1_Stream3>, \
    interrupt<Irq::DMA1_Stream4>, \
    interrupt<Irq::DMA1_Stream5>, \
    interrupt<Irq::DMA1_Stream6>, \
    interrupt<Irq::ADC>, \
    interrupt<Irq::CAN1_TX>, \
    interrupt<Irq::CAN1_RX0>, \
    interrupt<Irq::CAN1_RX1>, \
    interrupt<Irq::CAN1_SCE>, \
    interrupt<Irq::EXTI9_5>, \
    interrupt<Irq::TIM1_BRK_TIM9>, \
    interrupt<Irq::TIM1_UP_TIM10>, \
    interrupt<Irq::TIM1_TRG_COM_TIM11>, \
    interrupt<Irq::TIM1_CC>, \
    interrupt<Irq::TIM2>, \
    interrupt<Irq::TIM3>, \
    interrupt<Irq::TIM4>, \
    interrupt<Irq::I2C1_EV>, \
    interrupt<Irq::I2C1_ER>, \
    interrupt<Irq::I2C2_EV>, \
    interrupt<Irq::I2C2_ER>, \
    interrupt<Irq::SPI1>, \
    interrupt<Irq::SPI2>, \
    interrupt<Irq::USART1>, \
    interrupt<Irq::USART2>, \
    interrupt<Irq::USART3>, \
    interrupt<Irq::EXTI15_10>, \
    interrupt<Irq::RTC_Alarm>, \
    interrupt<Irq::OTG_FS_WKUP>, \
    interrupt<Irq::TIM8_BRK_TIM12>, \
    interrupt<Irq::TIM8_UP_TIM13>, \
    interrupt<Irq::TIM8_TRG_COM_TIM14>, \
    interrupt<Irq::TIM8_CC>, \
    interrupt<Irq::DMA1_Stream7>, \
    interrupt<Irq::FMC>, \
    interrupt<Irq::SDIO>, \
    interrupt<Irq::TIM5>, \
    interrupt<Irq::SPI3>, \
    interrupt<Irq::UART4>, \
    interrupt<Irq::UART5>, \
    interrupt<Irq::TIM6_DAC>, \
    interrupt<Irq::TIM7>, \
    interrupt<Irq::DMA2_Stream0>, \
    interrupt<Irq::DMA2_Stream1>, \
    interrupt<Irq::DMA2_Stream2>, \
    interrupt<Irq::DMA2_Stream3>, \
    interrupt<Irq::DMA2_Stream4>, \
    interrupt<Irq::ETH>, \
    interrupt<Irq::ETH_WKUP>, \
    interrupt<Irq::CAN2_TX>, \
    interrupt<Irq::CAN2_RX0>, \
    interrupt<Irq::CAN2_RX1>, \
    interrupt<Irq::CAN2_SCE>, \
    interrupt<Irq::OTG_FS>, \
    interrupt<Irq::DMA2_Stream5>, \
    interrupt<Irq::DMA2_Stream6>, \
    interrupt<Irq::DMA2_Stream7>, \
    interrupt<Irq::USART6>, \
    interrupt<Irq::I2C3_EV>, \
    interrupt<Irq::I2C3_ER>, \
    interrupt<Irq::OTG_HS_EP1_OUT>, \
    interrupt<Irq::OTG_HS_EP1_IN>, \
    interrupt<Irq::OTG_HS_WKUP>, \
    interrupt<Irq::OTG_HS>, \
    interrupt<Irq::DCMI>, \
    interrupt<Irq::CRYP>, \
    interrupt<Irq::HASH_RNG>, \
    interrupt<Irq::FPU>, \
    interrupt<Irq::UART7>, \
    interrupt<Irq::UART8>, \
    interrupt<Irq::SPI4>, \
    interrupt<Irq::SPI5>, \
    interrupt<Irq::SPI6>, \
    interrupt<Irq::SAI1>, \
    interrupt<Irq::LTDC>, \
    interrupt<Irq::LTDC_ER>, \
    interrupt<Irq::DMA2D>, \
    UCOO_VECTOR_IRQ_QUADSPI \
    UCOO_VECTOR_IRQ_DSI

#ifdef QSPI_R_BASE
# define UCOO_VECTOR_IRQ_QUADSPI interrupt<Irq::QUADSPI>,
#else
# define UCOO_VECTOR_IRQ_QUADSPI
#endif

#ifdef DSI_BASE
# define UCOO_VECTOR_IRQ_DSI interrupt<Irq::DSI>,
#else
# define UCOO_VECTOR_IRQ_DSI
#endif

namespace ucoo {

template<> void interrupt<Irq::WWDG> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::PVD> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TAMP_STAMP> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::RTC_WKUP> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::FLASH> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::RCC> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::EXTI0> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::EXTI1> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::EXTI2> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::EXTI3> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::EXTI4> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Stream0> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Stream1> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Stream2> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Stream3> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Stream4> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Stream5> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Stream6> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::ADC> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::CAN1_TX> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::CAN1_RX0> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::CAN1_RX1> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::CAN1_SCE> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::EXTI9_5> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM1_BRK_TIM9> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM1_UP_TIM10> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM1_TRG_COM_TIM11> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM1_CC> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM2> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM3> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM4> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::I2C1_EV> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::I2C1_ER> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::I2C2_EV> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::I2C2_ER> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::SPI1> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::SPI2> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::USART1> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::USART2> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::USART3> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::EXTI15_10> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::RTC_Alarm> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::OTG_FS_WKUP> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM8_BRK_TIM12> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM8_UP_TIM13> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM8_TRG_COM_TIM14> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM8_CC> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Stream7> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::FMC> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::SDIO> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM5> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::SPI3> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::UART4> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::UART5> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM6_DAC> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM7> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Stream0> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Stream1> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Stream2> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Stream3> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Stream4> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::ETH> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::ETH_WKUP> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::CAN2_TX> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::CAN2_RX0> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::CAN2_RX1> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::CAN2_SCE> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::OTG_FS> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Stream5> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Stream6> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Stream7> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::USART6> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::I2C3_EV> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::I2C3_ER> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::OTG_HS_EP1_OUT> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::OTG_HS_EP1_IN> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::OTG_HS_WKUP> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::OTG_HS> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DCMI> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::CRYP> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::HASH_RNG> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::FPU> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::UART7> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::UART8> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::SPI4> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::SPI5> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::SPI6> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::SAI1> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::LTDC> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::LTDC_ER> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2D> ()
    __attribute__ ((weak, alias ("undefined_handler")));
#ifdef QSPI_R_BASE
template<> void interrupt<Irq::QUADSPI> ()
    __attribute__ ((weak, alias ("undefined_handler")));
#endif
#ifdef DSI_BASE
template<> void interrupt<Irq::DSI> ()
    __attribute__ ((weak, alias ("undefined_handler")));
#endif

} // namespace ucoo

#endif // ucoo_arch_vector_stm32f4_hh
