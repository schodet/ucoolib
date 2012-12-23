// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
//
// APBTeam:
//        Web: http://apbteam.org/
//      Email: team AT apbteam DOT org
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// }}}
#include "uart.stm32.hh"

#include <libopencm3/stm32/f4/usart.h>
#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/nvic.h>

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
    /// RCC enable bit.
    uint32_t rcc_en;
    /// Corresponding IRQ.
    int irq;
};

/// Information on UART hardware array, this is zero indexed, USART1 is at
/// index 0.
static const uart_hardware_t uart_hardware[uart_nb] =
{
    { USART1, 2, RCC_APB2ENR_USART1EN, NVIC_USART1_IRQ },
    { USART2, 1, RCC_APB1ENR_USART2EN, NVIC_USART2_IRQ },
    { USART3, 1, RCC_APB1ENR_USART3EN, NVIC_USART3_IRQ },
    { UART4, 1, RCC_APB1ENR_UART4EN, NVIC_UART4_IRQ },
    { UART5, 1, RCC_APB1ENR_UART5EN, NVIC_UART5_IRQ },
    { USART6, 2, RCC_APB2ENR_USART6EN, NVIC_USART6_IRQ },
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

Uart::Uart (int n, int speed, Parity parity, int stop_bits)
    : n_ (n), error_char_ (default_error_char)
{
    assert (n < uart_nb);
    assert (!uart_instances[n]);
    uart_instances[n] = this;
    setup (speed, parity, stop_bits);
}

Uart::~Uart ()
{
    setup (0);
    uart_instances[n_] = 0;
}

void
Uart::setup (int speed, Parity parity, int stop_bits)
{
    uint32_t base = uart_hardware[n_].base;
    if (speed)
    {
        // Turn on.
        rcc_peripheral_enable_clock
            (uart_hardware[n_].apb == 1 ? &RCC_APB1ENR : &RCC_APB2ENR,
             uart_hardware[n_].rcc_en);
        // Set speed, rounded to nearest.
        int apb_freq = uart_hardware[n_].apb == 1 ? rcc_ppre1_frequency
            : rcc_ppre2_frequency;
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
        if (parity != NONE)
            cr1 |= USART_CR1_M | USART_CR1_PCE;
        if (parity == ODD)
            cr1 |= USART_CR1_PS;
        USART_CR1 (base) = cr1;
        // Reset status.
        (void) USART_SR (base);
        (void) USART_DR (base);
        // Enable interrupts.
        nvic_enable_irq (uart_hardware[n_].irq);
    }
    else
    {
        // Stop UART.
        nvic_disable_irq (uart_hardware[n_].irq);
        USART_CR1 (base) = 0;
        // Turn off.
        rcc_peripheral_disable_clock
            (uart_hardware[n_].apb == 1 ? &RCC_APB1ENR : &RCC_APB2ENR,
             uart_hardware[n_].rcc_en);
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
    if (block_)
        while (rx_fifo_.empty ())
            barrier ();
    return rx_fifo_.read (buf, count);
}

int
Uart::write (const char *buf, int count)
{
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
