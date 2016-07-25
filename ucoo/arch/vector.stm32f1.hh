#ifndef ucoo_arch_vector_stm32f1_hh
#define ucoo_arch_vector_stm32f1_hh
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
    interrupt<Irq::TAMPER>, \
    interrupt<Irq::RTC>, \
    interrupt<Irq::FLASH>, \
    interrupt<Irq::RCC>, \
    interrupt<Irq::EXTI0>, \
    interrupt<Irq::EXTI1>, \
    interrupt<Irq::EXTI2>, \
    interrupt<Irq::EXTI3>, \
    interrupt<Irq::EXTI4>, \
    interrupt<Irq::DMA1_Channel1>, \
    interrupt<Irq::DMA1_Channel2>, \
    interrupt<Irq::DMA1_Channel3>, \
    interrupt<Irq::DMA1_Channel4>, \
    interrupt<Irq::DMA1_Channel5>, \
    interrupt<Irq::DMA1_Channel6>, \
    interrupt<Irq::DMA1_Channel7>, \
    interrupt<Irq::ADC1_2>, \
    interrupt<Irq::CAN1_TX>, \
    interrupt<Irq::CAN1_RX0>, \
    interrupt<Irq::CAN1_RX1>, \
    interrupt<Irq::CAN1_SCE>, \
    interrupt<Irq::EXTI9_5>, \
    interrupt<Irq::TIM1_BRK>, \
    interrupt<Irq::TIM1_UP>, \
    interrupt<Irq::TIM1_TRG_COM>, \
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
    nullptr, \
    nullptr, \
    nullptr, \
    nullptr, \
    nullptr, \
    nullptr, \
    nullptr, \
    interrupt<Irq::TIM5>, \
    interrupt<Irq::SPI3>, \
    interrupt<Irq::UART4>, \
    interrupt<Irq::UART5>, \
    interrupt<Irq::TIM6>, \
    interrupt<Irq::TIM7>, \
    interrupt<Irq::DMA2_Channel1>, \
    interrupt<Irq::DMA2_Channel2>, \
    interrupt<Irq::DMA2_Channel3>, \
    interrupt<Irq::DMA2_Channel4>, \
    interrupt<Irq::DMA2_Channel5>, \
    nullptr, \
    nullptr, \
    interrupt<Irq::CAN2_TX>, \
    interrupt<Irq::CAN2_RX0>, \
    interrupt<Irq::CAN2_RX1>, \
    interrupt<Irq::CAN2_SCE>, \
    interrupt<Irq::OTG_FS>

namespace ucoo {

template<> void interrupt<Irq::WWDG> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::PVD> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TAMPER> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::RTC> ()
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
template<> void interrupt<Irq::DMA1_Channel1> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Channel2> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Channel3> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Channel4> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Channel5> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Channel6> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA1_Channel7> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::ADC1_2> ()
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
template<> void interrupt<Irq::TIM1_BRK> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM1_UP> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM1_TRG_COM> ()
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
template<> void interrupt<Irq::TIM5> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::SPI3> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::UART4> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::UART5> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM6> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::TIM7> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Channel1> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Channel2> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Channel3> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Channel4> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Irq::DMA2_Channel5> ()
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

} // namespace ucoo

#endif // ucoo_arch_vector_stm32f1_hh
