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
#include "ucoo/hal/spi/spi_hard.stm32.hh"
#include "ucoo/arch/rcc.stm32.hh"
#include "ucoo/common.hh"

namespace ucoo {

/// Information on SPI hardware.
struct spi_hardware_t
{
    /// SPI base address.
    SPI_TypeDef *base;
    /// APB bus.
    Bus bus;
    /// RCC identifier.
    Rcc rcc;
};

/// Information on SPI hardware array, this is zero indexed.
static const spi_hardware_t spi_hardware[] =
{
    { reg::SPI1, Bus::APB2, Rcc::SPI1 },
    { reg::SPI2, Bus::APB1, Rcc::SPI2 },
    { reg::SPI3, Bus::APB1, Rcc::SPI3 },
#ifdef SPI4_BASE
    { reg::SPI4, Bus::APB2, Rcc::SPI4 },
#endif
#ifdef SPI5_BASE
    { reg::SPI5, Bus::APB2, Rcc::SPI5 },
#endif
#ifdef SPI6_BASE
    { reg::SPI6, Bus::APB2, Rcc::SPI6 },
#endif
};

SpiHardMaster::SpiHardMaster (SpiHardMaster::Instance inst)
    : n_ (static_cast<int> (inst)), enabled_ (false)
{
}

SpiHardMaster::~SpiHardMaster ()
{
    disable ();
}

void
SpiHardMaster::enable (int speed_hz, SpiMode mode)
{
    enabled_ = true;
    auto base = spi_hardware[n_].base;
    // Turn on.
    rcc_peripheral_clock_enable (spi_hardware[n_].rcc);
    // Compute speed, rounded down.
    int apb_freq_hz = spi_hardware[n_].bus == Bus::APB1 ? rcc_apb1_freq_hz
        : rcc_apb2_freq_hz;
    int freq_hz = apb_freq_hz / 2;
    int br = 0;
    while (freq_hz > speed_hz)
    {
        freq_hz = freq_hz / 2;
        br++;
        assert (br <= 7);
    }
    // Set parameters and enable.
    base->CR2 = 0;
    base->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE
        | (br << SPI_CR1_BR_Pos) | SPI_CR1_MSTR | mode;
}

void
SpiHardMaster::disable ()
{
    if (enabled_)
    {
        enabled_ = false;
        auto base = spi_hardware[n_].base;
        // Stop SPI.
        base->CR1 = 0;
        // Turn off.
        rcc_peripheral_clock_disable (spi_hardware[n_].rcc);
    }
}

void
SpiHardMaster::send_and_recv (const char *tx_buf, char *rx_buf, int count)
{
    assert (enabled_);
    auto base = spi_hardware[n_].base;
    while (count--)
    {
        base->DR = *tx_buf++;
        while (!(base->SR & SPI_SR_RXNE))
            ;
        *rx_buf++ = base->DR;
    }
}

void
SpiHardMaster::send (const char *tx_buf, int count)
{
    assert (enabled_);
    auto base = spi_hardware[n_].base;
    while (count--)
    {
        base->DR = *tx_buf++;
        while (!(base->SR & SPI_SR_TXE))
            ;
    }
    // Wait for end of transfer.
    while (base->SR & SPI_SR_BSY)
        ;
    // Clear RXNE.
    (void) base->DR;
}

void
SpiHardMaster::recv (char *rx_buf, int count)
{
    assert (enabled_);
    auto base = spi_hardware[n_].base;
    while (count--)
    {
        base->DR = 0;
        while (!(base->SR & SPI_SR_RXNE))
            ;
        *rx_buf++ = base->DR;
    }
}

} // namespace ucoo
