// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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
#include "i2c_hard.stm32.hh"

#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/nvic.h>

#include "ucoolib/utils/trace.hh"

namespace ucoo {

/// Local trace.
static Trace<UCOO_CONFIG_HAL_I2C_TRACE> i2c_trace;

static const int i2c_nb = 3;

/// Information on I2C hardware structure.
struct i2c_hardware_t
{
    /// I2C base address.
    uint32_t base;
    /// RCC enable bit.
    uint32_t rcc_en;
    /// Corresponding event IRQ (error IRQ is next one).
    int ev_irq;
};

/// Information on I2C hardware array, this is zero indexed, I2C1 is at index
/// 0.
static const i2c_hardware_t i2c_hardware[i2c_nb] =
{
    { I2C1_BASE, RCC_APB1ENR_I2C1EN, NVIC_I2C1_EV_IRQ },
    { I2C2_BASE, RCC_APB1ENR_I2C2EN, NVIC_I2C2_EV_IRQ },
    { I2C3_BASE, RCC_APB1ENR_I2C3EN, NVIC_I2C3_EV_IRQ },
};

static I2cHard *i2c_instances[i2c_nb];

} // namespace ucoo

extern "C" {

void i2c1_ev_isr () { ucoo::I2cHard::ev_isr (0); }

void i2c1_er_isr () { ucoo::I2cHard::er_isr (0); }

void i2c2_ev_isr () { ucoo::I2cHard::ev_isr (1); }

void i2c2_er_isr () { ucoo::I2cHard::er_isr (1); }

void i2c3_ev_isr () { ucoo::I2cHard::ev_isr (2); }

void i2c3_er_isr () { ucoo::I2cHard::er_isr (2); }

}

namespace ucoo {

I2cHard::I2cHard (int n)
    : n_ (n), enabled_ (false), slave_addr_ (0), slave_data_handler_ (0),
      master_ (false), master_status_ (STATUS_ERROR), master_buf_ (0)
{
    assert (n < i2c_nb);
    assert (!i2c_instances[n]);
    i2c_instances[n] = this;
}

I2cHard::~I2cHard ()
{
    disable ();
    i2c_instances[n_] = 0;
}

void
I2cHard::enable (int speed)
{
    enabled_ = true;
    uint32_t base = i2c_hardware[n_].base;
    // Turn on.
    rcc_peripheral_enable_clock (&RCC_APB1ENR, i2c_hardware[n_].rcc_en);
    // Reset.
    I2C_CR1 (base) = I2C_CR1_SWRST;
    // TODO: make sure the bus is free!!! How!
    I2C_CR1 (base) = 0;
    // Compute clock parameters.
    int pclk = rcc_ppre1_frequency;
    int pclk_mhz = pclk / 1000000;
    uint16_t ccr, tris;
    if (speed <= 100000)
    {
        ccr = pclk / speed / 2;
        tris = pclk_mhz + 1;
    }
    else
    {
        assert (speed <= 400000);
        ccr = I2C_CCR_FS | I2C_CCR_DUTY | (pclk / speed / 25);
        tris = pclk_mhz * 3 / 10 + 1;
    }
    // Set all parameters.
    I2C_CCR (base) = ccr;
    I2C_TRISE (base) = tris;
    I2C_OAR1 (base) = slave_addr_ | (1 << 14);
    I2C_OAR2 (base) = 0;
    I2C_CR2 (base) = I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | pclk_mhz;
    // Enable.
    nvic_enable_irq (i2c_hardware[n_].ev_irq);
    nvic_enable_irq (i2c_hardware[n_].ev_irq + 1);
    I2C_CR1 (base) = I2C_CR1_ACK | I2C_CR1_PE;

}

void
I2cHard::disable ()
{
    if (enabled_)
    {
        enabled_ = false;
        uint32_t base = i2c_hardware[n_].base;
        // TODO: wait for end of transfer?
        // Disable.
        nvic_disable_irq (i2c_hardware[n_].ev_irq);
        nvic_disable_irq (i2c_hardware[n_].ev_irq + 1);
        I2C_CR1 (base) = 0;
        // Turn off.
        rcc_peripheral_disable_clock (&RCC_APB1ENR,
                                      i2c_hardware[n_].rcc_en);
    }
}

void
I2cHard::transfer (uint8_t addr, char *buf, int count)
{
    assert (enabled_ && count);
    // No need to lock, master is not busy.
    assert (master_status_ != STATUS_BUSY);
    uint32_t base = i2c_hardware[n_].base;
    // Wait until STOP condition terminated, polling is the only way.
    while (I2C_CR1 (base) & I2C_CR1_STOP)
        barrier ();
    // Now, program next transfer.
    master_status_ = STATUS_BUSY;
    master_slave_addr_ = addr;
    master_buf_ = buf;
    master_count_ = count;
    // TODO: multimaster: about ACK, may have to lock IRQ for multimaster.
    I2C_CR1 (base) |= I2C_CR1_START;
}

void
I2cHard::send (uint8_t addr, const char *buf, int count)
{
    transfer (addr, const_cast<char *> (buf), count);
}

void
I2cHard::recv (uint8_t addr, char *buf, int count)
{
    assert (count >= 2); // TODO: count = 1 not supported, there is no IT!
    // LSB = 1 for receiver mode.
    transfer (addr | 1, buf, count);
}

int
I2cHard::status ()
{
    return master_status_;
}

int
I2cHard::wait ()
{
    while (master_status_ == STATUS_BUSY)
        barrier ();
    return master_status_;
}

void
I2cHard::register_data (uint8_t addr, DataHandler &data_handler)
{
    assert ((addr & 1) == 0);
    slave_addr_ = addr;
    slave_data_handler_ = &data_handler;
    if (enabled_)
    {
        uint32_t base = i2c_hardware[n_].base;
        // Just in case a transfer is triggered right now.
        barrier ();
        // According to datasheet, bit 14 should be 1!
        I2C_OAR1 (base) = addr | (1 << 14);
    }
}

void
I2cHard::ev_isr (int n)
{
    uint32_t base = i2c_hardware[n].base;
    assert (i2c_instances[n]);
    I2cHard &i2c = *i2c_instances[n];
    i2c_trace ("<%d> event isr", n);
    while (1)
    {
        uint16_t sr1 = I2C_SR1 (base);
        i2c_trace ("<%d> sr1=%04x", n, sr1);
        // Can not read SR2 because doing so would clear the ADDR bit.
        if (i2c.master_)
        {
            if (sr1 & I2C_SR1_ADDR)
            {
                uint16_t sr2;
                // If only one or two bytes should be received, disable ACK
                // before reading SR2, and STOP after... Crappy hardware!
                if ((i2c.master_slave_addr_ & 1) && i2c.buf_count_ == 1)
                {
                    I2C_CR1 (base) = I2C_CR1_PE;
                    sr2 = I2C_SR2 (base);
                    I2C_CR1 (base) = I2C_CR1_STOP | I2C_CR1_PE;
                    // TODO: what to wait now? Unsupported for now.
                }
                else if ((i2c.master_slave_addr_ & 1) && i2c.buf_count_ == 2)
                {
                    I2C_CR1 (base) = I2C_CR1_POS | I2C_CR1_PE;
                    sr2 = I2C_SR2 (base);
                    // Wait for BTF.
                }
                else
                {
                    sr2 = I2C_SR2 (base);
                    I2C_CR2 (base) |= I2C_CR2_ITBUFEN;
                }
                i2c_trace ("<%d> master sr2=%04x", n, sr2);
            }
            else if (sr1 & I2C_SR1_TxE
                     && i2c.buf_index_ < i2c.buf_count_)
            {
                i2c_trace ("<%d> master tx index=%d", n, i2c.buf_index_);
                // Send next byte.
                I2C_DR (base) = i2c.master_buf_[i2c.buf_index_++];
                // Wait for BTF if last one.
                if (i2c.buf_index_ == i2c.buf_count_)
                    I2C_CR2 (base) &= ~I2C_CR2_ITBUFEN;
            }
            else if (sr1 & I2C_SR1_RxNE
                     && i2c.buf_count_ - i2c.buf_index_ > 3)
            {
                i2c_trace ("<%d> master rx index=%d", n, i2c.buf_index_);
                i2c.master_buf_[i2c.buf_index_++] = I2C_DR (base);
                if (i2c.buf_count_ - i2c.buf_index_ == 3)
                    // Wait for BTF.
                    I2C_CR2 (base) &= ~I2C_CR2_ITBUFEN;
            }
            else if (sr1 & I2C_SR1_BTF)
            {
                i2c_trace ("<%d> master btf index=%d", n, i2c.buf_index_);
                if (!(i2c.master_slave_addr_ & 1))
                {
                    // End of transmission.
                    I2C_CR1 (base) = I2C_CR1_ACK | I2C_CR1_STOP | I2C_CR1_PE;
                    i2c.master_ = false;
                    i2c.master_status_ = i2c.buf_index_;
                    if (i2c.finished_handler_)
                        i2c.finished_handler_->finished (i2c.master_status_);
                }
                else if (i2c.buf_count_ - i2c.buf_index_ == 3)
                {
                    // Near end of reception.
                    I2C_CR1 (base) = I2C_CR1_PE;
                    i2c.master_buf_[i2c.buf_index_++] = I2C_DR (base);
                    // Wait for BTF.
                }
                else
                {
                    // End of reception.
                    I2C_CR1 (base) = I2C_CR1_ACK | I2C_CR1_STOP | I2C_CR1_PE;
                    if (i2c.buf_count_ - i2c.buf_index_ == 2)
                        i2c.master_buf_[i2c.buf_index_++] = I2C_DR (base);
                    i2c.master_buf_[i2c.buf_index_++] = I2C_DR (base);
                    i2c.master_ = false;
                    i2c.master_status_ = i2c.buf_index_;
                    if (i2c.finished_handler_)
                        i2c.finished_handler_->finished (i2c.master_status_);
                }
            }
            else
                break;
        }
        else
        {
            if (sr1 & I2C_SR1_ADDR)
            {
                uint16_t sr2 = I2C_SR2 (base);
                i2c_trace ("<%d> slave sr2=%04x", n, sr2);
                // Initiate new slave transfer.
                if (sr2 & I2C_SR2_TRA)
                {
                    i2c.buf_count_ = i2c.slave_data_handler_->to_send
                        (i2c.slave_buf_, sizeof (i2c.slave_buf_));
                }
                else
                    i2c.buf_count_ = sizeof (i2c.slave_buf_);
                i2c.buf_index_ = 0;
                I2C_CR2 (base) |= I2C_CR2_ITBUFEN;
            }
            else if (sr1 & I2C_SR1_TxE)
            {
                i2c_trace ("<%d> slave tx index=%d", n, i2c.buf_index_);
                uint8_t b = 0xff;
                if (i2c.buf_index_ < i2c.buf_count_)
                    b = i2c.slave_buf_[i2c.buf_index_++];
                I2C_DR (base) = b;
            }
            else if (sr1 & I2C_SR1_RxNE)
            {
                i2c_trace ("<%d> slave rx index=%d", n, i2c.buf_index_);
                uint8_t b = I2C_DR (base);
                if (i2c.buf_index_ < i2c.buf_count_)
                    i2c.slave_buf_[i2c.buf_index_++] = b;
            }
            else if (sr1 & I2C_SR1_STOPF)
            {
                i2c_trace ("<%d> slave stop", n);
                i2c.slave_data_handler_->to_recv (i2c.slave_buf_, i2c.buf_index_);
                // TODO: multimaster: there is no way to write in this
                // register if a START was requested to switch to master mode!
                I2C_CR1 (base) = I2C_CR1_ACK | I2C_CR1_PE;
                I2C_CR2 (base) &= ~I2C_CR2_ITBUFEN;
            }
            else if (sr1 & I2C_SR1_SB)
            {
                i2c_trace ("<%d> master start", n);
                // Starting master mode.
                I2C_DR (base) = i2c.master_slave_addr_;
                i2c.master_ = true;
                i2c.buf_count_ = i2c.master_count_;
                i2c.buf_index_ = 0;
            }
            else
                break;
        }

    }
}

void
I2cHard::er_isr (int n)
{
    uint32_t base = i2c_hardware[n].base;
    assert (i2c_instances[n]);
    I2cHard &i2c = *i2c_instances[n];
    uint16_t sr1 = I2C_SR1 (base);
    I2C_SR1 (base) = 0;
    i2c_trace ("<%d> error isr sr1=%04x", n, sr1);
    if (i2c.master_)
    {
        if (sr1 & I2C_SR1_ARLO)
        {
            // Try again.
            I2C_CR1 (base) = I2C_CR1_ACK | I2C_CR1_START | I2C_CR1_PE;
            i2c.master_ = false;
        }
        else if (sr1 & I2C_SR1_AF)
        {
            I2C_CR1 (base) = I2C_CR1_ACK | I2C_CR1_STOP | I2C_CR1_PE;
            i2c.master_ = false;
            i2c.master_status_ = i2c.buf_index_;
            if (i2c.finished_handler_)
                i2c.finished_handler_->finished (i2c.master_status_);
        }
    }
    I2C_CR2 (base) &= ~I2C_CR2_ITBUFEN;
    // TODO: handle misplaced STOP errata.
}

} // namespace ucoo
