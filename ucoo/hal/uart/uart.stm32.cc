// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include "ucoo/hal/uart/uart.stm32.hh"
#include "ucoo/arch/interrupt.arm.hh"
#include "ucoo/arch/rcc.stm32.hh"

namespace ucoo {

/// Information on UART hardware structure.
struct uart_hardware_t
{
    /// UART base address.
    USART_TypeDef *base;
    /// APB number.
    Bus apb;
    /// RCC identifier, to enable clock.
    Rcc rcc;
    /// Corresponding IRQ.
    Irq irq;
};

/// Information on UART hardware array, this is zero indexed, USART1 is at
/// index 0.
static const uart_hardware_t uart_hardware[] =
{
    { reg::USART1, Bus::APB2, Rcc::USART1, Irq::USART1 },
    { reg::USART2, Bus::APB1, Rcc::USART2, Irq::USART2 },
    { reg::USART3, Bus::APB1, Rcc::USART3, Irq::USART3 },
    { reg::UART4, Bus::APB1, Rcc::UART4, Irq::UART4 },
    { reg::UART5, Bus::APB1, Rcc::UART5, Irq::UART5 },
#ifdef USART6_BASE
    { reg::USART6, Bus::APB2, Rcc::USART6, Irq::USART6 },
#endif
};

static Uart *uart_instances[lengthof (uart_hardware)];

template<>
void interrupt<Irq::USART1> () { Uart::isr (0); }

template<>
void interrupt<Irq::USART2> () { Uart::isr (1); }

template<>
void interrupt<Irq::USART3> () { Uart::isr (2); }

template<>
void interrupt<Irq::UART4> () { Uart::isr (3); }

template<>
void interrupt<Irq::UART5> () { Uart::isr (4); }

#ifdef USART6_BASE
template<>
void interrupt<Irq::USART6> () { Uart::isr (5); }
#endif

Uart::Uart (Uart::Instance inst)
    : n_ (static_cast<int> (inst)),
      error_char_ (default_error_char), enabled_ (false)
{
    assert (!uart_instances[n_]);
    uart_instances[n_] = this;
}

Uart::~Uart ()
{
    disable ();
    uart_instances[n_] = nullptr;
}

void
Uart::enable (int speed, Parity parity, int stop_bits)
{
    enabled_ = true;
    auto base = uart_hardware[n_].base;
    // Turn on.
    rcc_peripheral_clock_enable (uart_hardware[n_].rcc);
    // Set speed, rounded to nearest.
    int apb_freq = uart_hardware[n_].apb == Bus::APB1 ? rcc_apb1_freq_hz
        : rcc_apb2_freq_hz;
    base->BRR = (2 * apb_freq + speed) / (2 * speed);
    // Set parameters and enable.
    if (stop_bits == 1)
        base->CR2 = USART_CR2_STOP_Bits_1;
    else if (stop_bits == 2)
        base->CR2 = USART_CR2_STOP_Bits_2;
    else
        assert_unreachable ();
    base->CR3 = 0;
    uint32_t cr1 = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE
        | USART_CR1_RE;
    if (parity != Parity::NONE)
        cr1 |= USART_CR1_M | USART_CR1_PCE;
    if (parity == Parity::ODD)
        cr1 |= USART_CR1_PS;
    base->CR1 = cr1;
    // Reset status.
    (void) base->SR;
    (void) base->DR;
    // Enable interrupts.
    interrupt_enable (uart_hardware[n_].irq);
}

void
Uart::disable ()
{
    if (enabled_)
    {
        enabled_ = false;
        auto base = uart_hardware[n_].base;
        // Stop UART.
        interrupt_disable (uart_hardware[n_].irq);
        base->CR1 = 0;
        // Turn off.
        rcc_peripheral_clock_disable (uart_hardware[n_].rcc);
    }
}

void
Uart::set_error_char (char c)
{
    error_char_ = c;
}

int
Uart::read (char *buf, int count)
{
    assert (enabled_);
    if (block_)
        while (rx_fifo_.empty ())
            barrier ();
    return rx_fifo_.read (buf, count);
}

int
Uart::write (const char *buf, int count)
{
    assert (enabled_);
    int left = count;
    while (left)
    {
        int r = tx_fifo_.write (buf, left);
        if (r)
        {
            uart_hardware[n_].base->CR1 |= USART_CR1_TXEIE;
            buf += r;
            left -= r;
        }
        if (!block_)
            break;
    }
    return count - left;
}

int
Uart::poll ()
{
    return rx_fifo_.empty () ? 0 : 1;
}

void
Uart::isr (int n)
{
    auto base = uart_hardware[n].base;
    uint32_t sr = base->SR;
    uint32_t dr = base->DR;
    assert (uart_instances[n]);
    Uart &uart = *uart_instances[n];
    if (sr & (USART_SR_ORE | USART_SR_NE | USART_SR_FE | USART_SR_PE))
    {
        dr = uart.error_char_;
        // Datasheet is not really clear about this when error bits are set.
        sr |= USART_SR_RXNE;
    }
    if (sr & USART_SR_RXNE)
    {
        if (!uart.rx_fifo_.full ())
        {
            bool was_empty = uart.rx_fifo_.empty ();
            uart.rx_fifo_.push (dr);
            if (was_empty && uart.handler_)
                uart.handler_ (true);
        }
    }
    if (sr & USART_SR_TXE)
    {
        bool was_full = uart.tx_fifo_.full ();
        if (!uart.tx_fifo_.empty ())
            base->DR = static_cast<uint8_t> (uart.tx_fifo_.pop ());
        if (uart.tx_fifo_.empty ())
            base->CR1 &= ~USART_CR1_TXEIE;
        if (was_full && uart.handler_)
            uart.handler_ (false);
    }
}

} // namespace ucoo
