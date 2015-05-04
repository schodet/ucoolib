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
#include "uart.stm32.hh"

#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

#ifndef TARGET_stm32f4
// Need RCC adaptations and USART6 different handling for F1.
# error "it's a trap, only implemented for F4 for the moment"
#endif

namespace ucoo {

static const int uart_nb = 6;

/// Information on UART hardware structure.
struct uart_hardware_t
{
    /// UART base address.
    uint32_t base;
    /// APB number.
    int apb;
    /// Clock enable identifier.
    enum rcc_periph_clken clken;
    /// Corresponding IRQ.
    int irq;
};

/// Information on UART hardware array, this is zero indexed, USART1 is at
/// index 0.
static const uart_hardware_t uart_hardware[uart_nb] =
{
    { USART1, 2, RCC_USART1, NVIC_USART1_IRQ },
    { USART2, 1, RCC_USART2, NVIC_USART2_IRQ },
    { USART3, 1, RCC_USART3, NVIC_USART3_IRQ },
    { UART4, 1, RCC_UART4, NVIC_UART4_IRQ },
    { UART5, 1, RCC_UART5, NVIC_UART5_IRQ },
    { USART6, 2, RCC_USART6, NVIC_USART6_IRQ },
};

static Uart *uart_instances[uart_nb];

} // namespace ucoo

extern "C" {

void usart1_isr () { ucoo::Uart::isr (0); }

void usart2_isr () { ucoo::Uart::isr (1); }

void usart3_isr () { ucoo::Uart::isr (2); }

void uart4_isr () { ucoo::Uart::isr (3); }

void uart5_isr () { ucoo::Uart::isr (4); }

void usart6_isr () { ucoo::Uart::isr (5); }

}

namespace ucoo {

Uart::Uart (int n)
    : n_ (n), error_char_ (default_error_char), enabled_ (false)
{
    assert (n < uart_nb);
    assert (!uart_instances[n]);
    uart_instances[n] = this;
}

Uart::~Uart ()
{
    disable ();
    uart_instances[n_] = 0;
}

void
Uart::enable (int speed, Parity parity, int stop_bits)
{
    enabled_ = true;
    uint32_t base = uart_hardware[n_].base;
    // Turn on.
    rcc_periph_clock_enable (uart_hardware[n_].clken);
    // Set speed, rounded to nearest.
    int apb_freq = uart_hardware[n_].apb == 1 ? rcc_apb1_frequency
        : rcc_apb2_frequency;
    USART_BRR (base) = (2 * apb_freq + speed) / (2 * speed);
    // Set parameters and enable.
    if (stop_bits == 1)
        USART_CR2 (base) = USART_CR2_STOPBITS_1;
    else if (stop_bits == 2)
        USART_CR2 (base) = USART_CR2_STOPBITS_2;
    else
        assert_unreachable ();
    USART_CR3 (base) = 0;
    uint32_t cr1 = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;
    if (parity != Parity::NONE)
        cr1 |= USART_CR1_M | USART_CR1_PCE;
    if (parity == Parity::ODD)
        cr1 |= USART_CR1_PS;
    USART_CR1 (base) = cr1;
    // Reset status.
    (void) USART_SR (base);
    (void) USART_DR (base);
    // Enable interrupts.
    nvic_enable_irq (uart_hardware[n_].irq);
}

void
Uart::disable ()
{
    if (enabled_)
    {
        enabled_ = false;
        uint32_t base = uart_hardware[n_].base;
        // Stop UART.
        nvic_disable_irq (uart_hardware[n_].irq);
        USART_CR1 (base) = 0;
        // Turn off.
        rcc_periph_clock_disable (uart_hardware[n_].clken);
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
            USART_CR1 (uart_hardware[n_].base) |= USART_CR1_TXEIE;
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
    uint32_t base = uart_hardware[n].base;
    uint32_t sr = USART_SR (base);
    uint32_t dr = USART_DR (base);
    assert (uart_instances[n]);
    Uart &uart = *uart_instances[n];
    if (sr & (USART_SR_ORE | USART_SR_NE | USART_SR_FE  | USART_SR_PE))
    {
        dr = uart.error_char_;
        // Datasheet is not really clear about this when error bits are set.
        sr |= USART_SR_RXNE;
    }
    if (sr & USART_SR_RXNE)
    {
        if (!uart.rx_fifo_.full ())
            uart.rx_fifo_.push (dr);
    }
    if (sr & USART_SR_TXE)
    {
        if (!uart.tx_fifo_.empty ())
            USART_DR (base) = static_cast<uint8_t> (uart.tx_fifo_.pop ());
        if (uart.tx_fifo_.empty ())
            USART_CR1 (base) &= ~USART_CR1_TXEIE;
    }
}

} // namespace ucoo
