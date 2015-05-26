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

#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>

#include "ucoo/common.hh"

namespace ucoo {

/// Information on SPI hardware.
struct spi_hardware_t
{
    /// SPI base address.
    uint32_t base;
    /// APB number.
    int apb;
    /// Clock enable identifier.
    enum rcc_periph_clken clken;
};

/// Information on SPI hardware array, this is zero indexed.
static const spi_hardware_t spi_hardware[] =
{
    { SPI1, 2, RCC_SPI1 },
    { SPI2, 1, RCC_SPI2 },
    { SPI3, 1, RCC_SPI3 },
#ifdef SPI4_BASE
    { SPI4, 2, RCC_SPI4 },
#endif
#ifdef SPI5_BASE
    { SPI5, 2, RCC_SPI5 },
#endif
#ifdef SPI6_BASE
    { SPI6, 2, RCC_SPI6 },
#endif
};

SpiHardMaster::SpiHardMaster (int n)
    : n_ (n), enabled_ (false)
{
    assert (n < lengthof (spi_hardware));
}

SpiHardMaster::~SpiHardMaster ()
{
    disable ();
}

void
SpiHardMaster::enable (int speed_hz, SpiMode mode)
{
    enabled_ = true;
    uint32_t base = spi_hardware[n_].base;
    // Turn on.
    rcc_periph_clock_enable (spi_hardware[n_].clken);
    // Compute speed, rounded down.
    int apb_freq = spi_hardware[n_].apb == 1 ? rcc_apb1_frequency
        : rcc_apb2_frequency;
    int freq = apb_freq / 2;
    int br = 0;
    while (freq > speed_hz)
    {
        freq = freq / 2;
        br++;
        assert (br <= SPI_CR1_BR_FPCLK_DIV_256);
    }
    // Set parameters and enable.
    SPI_CR2 (base) = 0;
    SPI_CR1 (base) = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | (br << 3)
        | SPI_CR1_MSTR | mode;
}

void
SpiHardMaster::disable ()
{
    if (enabled_)
    {
        enabled_ = false;
        uint32_t base = spi_hardware[n_].base;
        // Stop SPI.
        SPI_CR1 (base) = 0;
        // Turn off.
        rcc_periph_clock_disable (spi_hardware[n_].clken);
    }
}

void
SpiHardMaster::send_and_recv (const char *tx_buf, char *rx_buf, int count)
{
    assert (enabled_);
    uint32_t base = spi_hardware[n_].base;
    while (count--)
    {
        SPI_DR (base) = *tx_buf++;
        while (!(SPI_SR (base) & SPI_SR_RXNE))
            ;
        *rx_buf++ = SPI_DR (base);
    }
}

void
SpiHardMaster::send (const char *tx_buf, int count)
{
    assert (enabled_);
    uint32_t base = spi_hardware[n_].base;
    while (count--)
    {
        SPI_DR (base) = *tx_buf++;
        while (!(SPI_SR (base) & SPI_SR_TXE))
            ;
    }
    // Wait for end of transfer.
    while (SPI_SR (base) & SPI_SR_BSY)
        ;
    // Clear RXNE.
    (void) SPI_DR (base);
}

void
SpiHardMaster::recv (char *rx_buf, int count)
{
    assert (enabled_);
    uint32_t base = spi_hardware[n_].base;
    while (count--)
    {
        SPI_DR (base) = 0;
        while (!(SPI_SR (base) & SPI_SR_RXNE))
            ;
        *rx_buf++ = SPI_DR (base);
    }
}

} // namespace ucoo
