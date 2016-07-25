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

#include "ucoo/arch/interrupt.arm.hh"

namespace ucoo {

/// Information on EXTI hardware structure.
struct exti_hardware_t
{
    /// Corresponding IRQ.
    Irq irq;
    /// Pointer to number of enabled EXTI for shared vectors, or NULL.
    int *enabled;
};

/// Number of enabled EXTI for vector 9 down to 5.
static int exti_vector_9_5_enabled;

/// Number of enabled EXTI for vector 15 down to 10.
static int exti_vector_15_10_enabled;

/// Information on EXTI hardware array.
static const exti_hardware_t exti_hardware[] =
{
    { Irq::EXTI0, nullptr },
    { Irq::EXTI1, nullptr },
    { Irq::EXTI2, nullptr },
    { Irq::EXTI3, nullptr },
    { Irq::EXTI4, nullptr },
    { Irq::EXTI9_5, &exti_vector_9_5_enabled },
    { Irq::EXTI9_5, &exti_vector_9_5_enabled },
    { Irq::EXTI9_5, &exti_vector_9_5_enabled },
    { Irq::EXTI9_5, &exti_vector_9_5_enabled },
    { Irq::EXTI9_5, &exti_vector_9_5_enabled },
    { Irq::EXTI15_10, &exti_vector_15_10_enabled },
    { Irq::EXTI15_10, &exti_vector_15_10_enabled },
    { Irq::EXTI15_10, &exti_vector_15_10_enabled },
    { Irq::EXTI15_10, &exti_vector_15_10_enabled },
    { Irq::EXTI15_10, &exti_vector_15_10_enabled },
    { Irq::EXTI15_10, &exti_vector_15_10_enabled },
};

static ExtiHard *exti_instances[lengthof (exti_hardware)];

// If the interrupt is triggered again after being cleared, the interrupt
// handler will be reentered.  There is no need to loop.

template<>
void interrupt<Irq::EXTI0> ()
{
    reg::EXTI->PR = 1u << 0;
    assert (exti_instances[0]);
    exti_instances[0]->isr ();
}

template<>
void interrupt<Irq::EXTI1> ()
{
    reg::EXTI->PR = 1u << 1;
    assert (exti_instances[1]);
    exti_instances[1]->isr ();
}

template<>
void interrupt<Irq::EXTI2> ()
{
    reg::EXTI->PR = 1u << 2;
    assert (exti_instances[2]);
    exti_instances[2]->isr ();
}

template<>
void interrupt<Irq::EXTI3> ()
{
    reg::EXTI->PR = 1u << 3;
    assert (exti_instances[3]);
    exti_instances[3]->isr ();
}

template<>
void interrupt<Irq::EXTI4> ()
{
    reg::EXTI->PR = 1u << 4;
    assert (exti_instances[4]);
    exti_instances[4]->isr ();
}

template<>
void interrupt<Irq::EXTI9_5> ()
{
    uint32_t pending = reg::EXTI->PR & 0x3e0;
    reg::EXTI->PR = pending;
    for (int i = 5; i <= 9; i++)
    {
        if (pending & (1u << i))
        {
            assert (exti_instances[i]);
            exti_instances[i]->isr ();
        }
    }
}

template<>
void interrupt<Irq::EXTI15_10> ()
{
    uint32_t pending = reg::EXTI->PR & 0xfc00;
    reg::EXTI->PR = pending;
    for (int i = 10; i <= 15; i++)
    {
        if (pending & (1u << i))
        {
            assert (exti_instances[i]);
            exti_instances[i]->isr ();
        }
    }
}

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
    reg::EXTI->IMR |= 1u << n_;
    if (exti_hardware[n_].enabled)
    {
        if (!(*exti_hardware[n_].enabled)++)
            interrupt_enable (exti_hardware[n_].irq);
    }
    else
        interrupt_enable (exti_hardware[n_].irq);
}

void
ExtiHard::disable ()
{
    if (exti_hardware[n_].enabled)
    {
        if (!--*exti_hardware[n_].enabled)
            interrupt_disable (exti_hardware[n_].irq);
    }
    else
        interrupt_disable (exti_hardware[n_].irq);
    reg::EXTI->IMR &= ~(1u << n_);
    reg::EXTI->PR = 1u << n_;
}

bool
ExtiHard::mask ()
{
    bool irq_enabled = interrupt_is_enabled (exti_hardware[n_].irq);
    if (irq_enabled)
        interrupt_disable (exti_hardware[n_].irq);
    return irq_enabled;
}

void
ExtiHard::unmask (bool saved_state)
{
    if (saved_state)
        interrupt_enable (exti_hardware[n_].irq);
}

void
ExtiHard::set_trigger (Edge edge)
{
    switch (edge)
    {
    case Edge::RISING:
        reg::EXTI->RTSR |= 1u << n_;
        reg::EXTI->FTSR &= ~(1u << n_);
        break;
    case Edge::FALLING:
        reg::EXTI->RTSR &= ~(1u << n_);
        reg::EXTI->FTSR |= 1u << n_;
        break;
    case Edge::BOTH:
        reg::EXTI->RTSR |= (1u << n_);
        reg::EXTI->FTSR |= (1u << n_);
        break;
    }
}

void
ExtiHard::set_source (const GpioPort &gpio_port)
{
    int port = gpio_port.get_port_index ();
    int shift = (n_ % 4) * 4;
    int reg = n_ / 4;
    uint32_t mask = 0xf << shift;
    uint32_t bits = port << shift;
#if defined AFIO_BASE
    reg::AFIO->EXTICR[reg] = (reg::AFIO->EXTICR[reg] & ~mask) | bits;
#else
    reg::SYSCFG->EXTICR[reg] = (reg::SYSCFG->EXTICR[reg] & ~mask) | bits;
#endif
}

void
ExtiHard::isr ()
{
    handler_ ();
}

} // namespace ucoo
