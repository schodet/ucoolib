#ifndef ucoo_arch_reg_stm32_hh
#define ucoo_arch_reg_stm32_hh
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

#define TIM_SMCR_SMS_TM (6 << 0)

#define TIM_SMCR_TS_TI1FP1 (5 << 4)
#define TIM_SMCR_TS_TI1FP2 (6 << 4)

#define TIM_CCMR1_OC1M_PWM1 (6 << 4)
#define TIM_CCMR1_OC2M_PWM1 (6 << 12)

#define TIM_CCMR2_OC3M_PWM1 (6 << 4)
#define TIM_CCMR2_OC4M_PWM1 (6 << 12)

#define USART_CR2_STOP_Bits_1 0
#define USART_CR2_STOP_Bits_0_5 USART_CR2_STOP_0
#define USART_CR2_STOP_Bits_2 USART_CR2_STOP_1
#define USART_CR2_STOP_Bits_1_5 (USART_CR2_STOP_1 | USART_CR2_STOP_0)

#ifndef USB_OTG_GCCFG_VBUSBSEN
// OTG version 0x1100
# define USB_OTG_GCCFG_VBUSBSEN (1 << 19)
#endif
#ifndef USB_OTG_GCCFG_VBDEN
// OTG version 0x2000
# define USB_OTG_GCCFG_VBDEN (1 << 21)
#endif

#define USB_OTG_GRXSTSP_PKTSTS_GlobalOutNak (1 << 17)
#define USB_OTG_GRXSTSP_PKTSTS_Out (2 << 17)
#define USB_OTG_GRXSTSP_PKTSTS_OutCompleted (3 << 17)
#define USB_OTG_GRXSTSP_PKTSTS_SetupCompleted (4 << 17)
#define USB_OTG_GRXSTSP_PKTSTS_Setup (6 << 17)

#define FLASH_KEYR_KEY1 0x45670123
#define FLASH_KEYR_KEY2 0xcdef89ab

namespace ucoo {

struct USB_OTG_FIFOTypeDef
{
    __IO uint32_t FIFO;
    uint32_t reserved[(0x1000 - sizeof (uint32_t)) / 4];
};

/// USB OTG registers, only define device registers.
struct USB_OTG_TypeDef
{
    USB_OTG_GlobalTypeDef global;
    uint32_t reserved0[(USB_OTG_DEVICE_BASE - USB_OTG_GLOBAL_BASE
                        - sizeof (USB_OTG_GlobalTypeDef)) / 4];
    USB_OTG_DeviceTypeDef device;
    uint32_t reserved1[(USB_OTG_IN_ENDPOINT_BASE - USB_OTG_DEVICE_BASE
                        - sizeof (USB_OTG_DeviceTypeDef)) / 4];
    USB_OTG_INEndpointTypeDef ep_in[4];
    uint32_t reserved2[(USB_OTG_OUT_ENDPOINT_BASE - USB_OTG_IN_ENDPOINT_BASE
                        - sizeof (USB_OTG_INEndpointTypeDef[4])) / 4];
    USB_OTG_OUTEndpointTypeDef ep_out[4];
    uint32_t reserved3[(USB_OTG_FIFO_BASE - USB_OTG_OUT_ENDPOINT_BASE
                        - sizeof (USB_OTG_OUTEndpointTypeDef[4])) / 4];
    USB_OTG_FIFOTypeDef fifo[4];
};

} // namespace ucoo

#endif // ucoo_arch_reg_stm32_hh
