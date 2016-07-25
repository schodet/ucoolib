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
#include "ucoo/common.hh"
#include "ucoo/arch/interrupt.arm.hh"

#if defined (TARGET_stm32f4)
# include "ucoo/arch/vector.stm32f4.hh"
#elif defined (TARGET_stm32f1)
# include "ucoo/arch/vector.stm32f1.hh"
#else
# error "not implemented for this target"
#endif

extern uint32_t _stack;

extern "C" void entry ();

namespace ucoo {

typedef void (*vector_t) (void);

struct vector_table_t
{
    uint32_t *initial_stack;
    vector_t exception[static_cast<int> (Exception::COUNT) - 1];
    vector_t irq[static_cast<int> (Irq::COUNT)];
};

template<> void interrupt<Exception::NMI> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Exception::HARD_FAULT> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Exception::MEMORY_MANAGEMENT_FAULT> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Exception::BUS_FAULT> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Exception::USAGE_FAULT> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Exception::SV_CALL> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Exception::PEND_SV> ()
    __attribute__ ((weak, alias ("undefined_handler")));
template<> void interrupt<Exception::SYSTICK> ()
    __attribute__ ((weak, alias ("undefined_handler")));

vector_table_t vector_table __attribute__ ((section (".vectors"))) =
{
    .initial_stack = &_stack,
    {
        entry,
        interrupt<Exception::NMI>,
        interrupt<Exception::HARD_FAULT>,
        interrupt<Exception::MEMORY_MANAGEMENT_FAULT>,
        interrupt<Exception::BUS_FAULT>,
        interrupt<Exception::USAGE_FAULT>,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        interrupt<Exception::SV_CALL>,
        nullptr,
        nullptr,
        interrupt<Exception::PEND_SV>,
        interrupt<Exception::SYSTICK>
    },
    {
        UCOO_VECTOR_IRQ
    }
};

extern "C" void
undefined_handler ()
{
    halt ();
}

} // namespace ucoo

int main ();

extern uint32_t _data_loadaddr;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _ebss;
extern uint32_t __preinit_array_start;
extern uint32_t __preinit_array_end;
extern uint32_t __init_array_start;
extern uint32_t __init_array_end;
extern uint32_t __fini_array_start;
extern uint32_t __fini_array_end;

extern "C" void __attribute__ ((naked))
entry ()
{
    uint32_t *src = &_data_loadaddr, *dst = &_data;
    while (dst < &_edata)
        *dst++ = *src++;
    while (dst < &_ebss)
        *dst++ = 0;
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;
#if __FPU_USED
    SCB->CPACR |= SCB_CPACR_CP10_Full | SCB_CPACR_CP11_Full;
#endif
    uint32_t *fp;
    fp = &__preinit_array_start;
    while (fp < &__preinit_array_end)
        reinterpret_cast<void (*) ()> (*fp++) ();
    fp = &__init_array_start;
    while (fp < &__init_array_end)
        reinterpret_cast<void (*) ()> (*fp++) ();
    main ();
    fp = &__fini_array_start;
    while (fp < &__fini_array_end)
        reinterpret_cast<void (*) ()> (*fp++) ();
}
