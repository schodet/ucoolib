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
#include "ucoo/hal/exti/exti.stm32.hh"
#include "ucoo/common.hh"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#if !defined AFIO_BASE
# include <libopencm3/stm32/syscfg.h>
#endif

namespace ucoo {

/// Information on EXTI hardware structure.
struct exti_hardware_t
{
    /// Corresponding IRQ.
    int irq;
    /// Pointer to number of enabled EXTI for shared vectors, or NULL.
    int *enabled;
};

/// Number of enabled EXTI for vector 9 down to 5.
static int exti_vector_9_5_enabled;

/// Number of enabled EXTI for vector 15 down to 10.
static int exti_vector_15_10_enabled;

/// Information on EXTI hardware array.
struct exti_hardware_t exti_hardware[] =
{
    { NVIC_EXTI0_IRQ, nullptr },
    { NVIC_EXTI1_IRQ, nullptr },
    { NVIC_EXTI2_IRQ, nullptr },
    { NVIC_EXTI3_IRQ, nullptr },
    { NVIC_EXTI4_IRQ, nullptr },
    { NVIC_EXTI9_5_IRQ, &exti_vector_9_5_enabled },
    { NVIC_EXTI9_5_IRQ, &exti_vector_9_5_enabled },
    { NVIC_EXTI9_5_IRQ, &exti_vector_9_5_enabled },
    { NVIC_EXTI9_5_IRQ, &exti_vector_9_5_enabled },
    { NVIC_EXTI9_5_IRQ, &exti_vector_9_5_enabled },
    { NVIC_EXTI15_10_IRQ, &exti_vector_15_10_enabled },
    { NVIC_EXTI15_10_IRQ, &exti_vector_15_10_enabled },
    { NVIC_EXTI15_10_IRQ, &exti_vector_15_10_enabled },
    { NVIC_EXTI15_10_IRQ, &exti_vector_15_10_enabled },
    { NVIC_EXTI15_10_IRQ, &exti_vector_15_10_enabled },
    { NVIC_EXTI15_10_IRQ, &exti_vector_15_10_enabled },
};

static ExtiHard *exti_instances[lengthof (exti_hardware)];

} // namespace ucoo

extern "C" {

// If the interrupt is triggered again after being cleared, the interrupt
// handler will be reentered.  There is no need to loop.

void
exti0_isr ()
{
    EXTI_PR = 1u << 0;
    assert (ucoo::exti_instances[0]);
    ucoo::exti_instances[0]->isr ();
}

void
exti1_isr ()
{
    EXTI_PR = 1u << 1;
    assert (ucoo::exti_instances[1]);
    ucoo::exti_instances[1]->isr ();
}

void
exti2_isr ()
{
    EXTI_PR = 1u << 2;
    assert (ucoo::exti_instances[2]);
    ucoo::exti_instances[2]->isr ();
}

void
exti3_isr ()
{
    EXTI_PR = 1u << 3;
    assert (ucoo::exti_instances[3]);
    ucoo::exti_instances[3]->isr ();
}

void
exti4_isr ()
{
    EXTI_PR = 1u << 4;
    assert (ucoo::exti_instances[4]);
    ucoo::exti_instances[4]->isr ();
}

void
exti9_5_isr ()
{
    uint32_t pending = EXTI_PR & 0x3e0;
    EXTI_PR = pending;
    for (int i = 5; i <= 9; i++)
    {
        if (pending & (1u << i))
        {
            assert (ucoo::exti_instances[i]);
            ucoo::exti_instances[i]->isr ();
        }
    }
}

void
exti15_10_isr ()
{
    uint32_t pending = EXTI_PR & 0xfc00;
    EXTI_PR = pending;
    for (int i = 10; i <= 15; i++)
    {
        if (pending & (1u << i))
        {
            assert (ucoo::exti_instances[i]);
            ucoo::exti_instances[i]->isr ();
        }
    }
}

}

namespace ucoo {

ExtiHard::ExtiHard (int n)
    : n_ (n)
{
    assert (n < lengthof (exti_instances));
    assert (!exti_instances[n]);
    exti_instances[n] = this;
}

ExtiHard::~ExtiHard ()
{
    disable ();
    exti_instances[n_] = 0;
}

void
ExtiHard::enable ()
{
    assert (handler_);
    EXTI_IMR |= 1u << n_;
    if (exti_hardware[n_].enabled)
    {
        if (!(*exti_hardware[n_].enabled)++)
            nvic_enable_irq (exti_hardware[n_].irq);
    }
    else
        nvic_enable_irq (exti_hardware[n_].irq);
}

void
ExtiHard::disable ()
{
    if (exti_hardware[n_].enabled)
    {
        if (!--*exti_hardware[n_].enabled)
            nvic_disable_irq (exti_hardware[n_].irq);
    }
    else
        nvic_disable_irq (exti_hardware[n_].irq);
    EXTI_IMR &= ~(1u << n_);
    EXTI_PR = 1u << n_;
}

bool
ExtiHard::mask ()
{
    bool irq_enabled = nvic_get_irq_enabled (exti_hardware[n_].irq);
    if (irq_enabled)
        nvic_disable_irq (exti_hardware[n_].irq);
    return irq_enabled;
}

void
ExtiHard::unmask (bool saved_state)
{
    if (saved_state)
        nvic_enable_irq (exti_hardware[n_].irq);
}

void
ExtiHard::set_trigger (Edge edge)
{
    switch (edge)
    {
    case Edge::RISING:
        EXTI_RTSR |= 1u << n_;
        EXTI_FTSR &= ~(1u << n_);
        break;
    case Edge::FALLING:
        EXTI_RTSR &= ~(1u << n_);
        EXTI_FTSR |= 1u << n_;
        break;
    case Edge::BOTH:
        EXTI_RTSR |= (1u << n_);
        EXTI_FTSR |= (1u << n_);
        break;
    }
}

void
ExtiHard::set_source (uint32_t gpio_port)
{
    int port;
    switch (gpio_port)
    {
    case GPIOA:
        port = 0;
        break;
    case GPIOB:
        port = 1;
        break;
    case GPIOC:
        port = 2;
        break;
    case GPIOD:
        port = 3;
        break;
#if defined GPIOE
    case GPIOE:
        port = 4;
        break;
#endif
#if defined GPIOF
    case GPIOF:
        port = 5;
        break;
#endif
#if defined GPIOG
    case GPIOG:
        port = 6;
        break;
#endif
#if defined GPIOH
    case GPIOH:
        port = 7;
        break;
#endif
#if defined GPIOI
    case GPIOI:
        port = 8;
        break;
#endif
    default:
        assert_unreachable ();
    }
    int shift = (n_ % 4) * 4;
    int reg = n_ / 4;
    uint32_t mask = 0xf << shift;
    uint32_t bits = port << shift;
#if defined AFIO_BASE
    AFIO_EXTICR (reg) = (AFIO_EXTICR (reg) & ~mask) | bits;
#else
    SYSCFG_EXTICR (reg) = (SYSCFG_EXTICR (reg) & ~mask) | bits;
#endif
}

void
ExtiHard::isr ()
{
    handler_ ();
}

} // namespace ucoo
