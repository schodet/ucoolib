// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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
#include "ucoo/hal/i2c/i2c_hard.stm32.hh"
#include "ucoo/arch/interrupt.arm.hh"
#include "ucoo/arch/rcc.stm32.hh"

#include "ucoo/utils/trace.hh"

namespace ucoo {

/// Local trace.
static Trace<CONFIG_UCOO_HAL_I2C_TRACE> i2c_trace;

/// Information on I2C hardware structure.
struct i2c_hardware_t
{
    /// I2C base address.
    I2C_TypeDef *base;
    /// RCC identifier, to enable clock.
    Rcc rcc;
    /// Corresponding event IRQ.
    Irq ev_irq;
    /// Corresponding error IRQ.
    Irq er_irq;
};

/// Information on I2C hardware array, this is zero indexed, I2C1 is at index
/// 0.
static const i2c_hardware_t i2c_hardware[] =
{
    { reg::I2C1, Rcc::I2C1, Irq::I2C1_EV, Irq::I2C1_ER },
    { reg::I2C2, Rcc::I2C2, Irq::I2C2_EV, Irq::I2C2_ER },
#ifdef I2C3_BASE
    { reg::I2C3, Rcc::I2C3, Irq::I2C3_EV, Irq::I2C3_ER },
#endif
};

static I2cHard *i2c_instances[lengthof (i2c_hardware)];

template<>
void interrupt<Irq::I2C1_EV> () { I2cHard::ev_isr (0); }

template<>
void interrupt<Irq::I2C1_ER> () { I2cHard::er_isr (0); }

template<>
void interrupt<Irq::I2C2_EV> () { I2cHard::ev_isr (1); }

template<>
void interrupt<Irq::I2C2_ER> () { I2cHard::er_isr (1); }

#ifdef I2C3_BASE

template<>
void interrupt<Irq::I2C3_EV> () { I2cHard::ev_isr (2); }

template<>
void interrupt<Irq::I2C3_ER> () { I2cHard::er_isr (2); }

#endif

I2cHard::I2cHard (Instance inst)
    : n_ (static_cast<int> (inst))
{
    assert (n_ < lengthof (i2c_instances));
    assert (!i2c_instances[n_]);
    i2c_instances[n_] = this;
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
    auto base = i2c_hardware[n_].base;
    // Turn on.
    rcc_peripheral_clock_enable (i2c_hardware[n_].rcc);
    // Reset.
    base->CR1 = I2C_CR1_SWRST;
    // TODO: make sure the bus is free!!! How!
    base->CR1 = 0;
    // Compute clock parameters.
    int pclk_hz = rcc_apb1_freq_hz;
    int pclk_mhz = pclk_hz / 1000000;
    uint16_t ccr, tris;
    if (speed <= 100000)
    {
        ccr = pclk_hz / speed / 2;
        tris = pclk_mhz + 1;
    }
    else
    {
        assert (speed <= 400000);
        ccr = I2C_CCR_FS | I2C_CCR_DUTY | (pclk_hz / speed / 25);
        tris = pclk_mhz * 3 / 10 + 1;
    }
    // Set all parameters.
    base->CCR = ccr;
    base->TRISE = tris;
    base->OAR1 = slave_addr_ | (1 << 14);
    base->OAR2 = 0;
    base->CR2 = I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | pclk_mhz;
    // Enable.
    interrupt_enable (i2c_hardware[n_].ev_irq);
    interrupt_enable (i2c_hardware[n_].er_irq);
    base->CR1 = I2C_CR1_ACK | I2C_CR1_PE;

}

void
I2cHard::disable ()
{
    if (enabled_)
    {
        enabled_ = false;
        auto base = i2c_hardware[n_].base;
        // TODO: wait for end of transfer?
        // Disable.
        interrupt_disable (i2c_hardware[n_].ev_irq);
        interrupt_disable (i2c_hardware[n_].er_irq);
        base->CR1 = 0;
        // Turn off.
        rcc_peripheral_clock_disable (i2c_hardware[n_].rcc);
    }
}

void
I2cHard::transfer (uint8_t addr, char *buf, int count)
{
    assert (enabled_ && count);
    // No need to lock, master is not busy.
    assert (master_status_ != STATUS_BUSY);
    auto base = i2c_hardware[n_].base;
    // Wait until STOP condition terminated, polling is the only way.
    while (base->CR1 & I2C_CR1_STOP)
        barrier ();
    // Now, program next transfer.
    master_status_ = STATUS_BUSY;
    master_slave_addr_ = addr;
    master_buf_ = buf;
    master_count_ = count;
    // TODO: multimaster: about ACK, may have to lock IRQ for multimaster.
    base->CR1 |= I2C_CR1_START;
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
        auto base = i2c_hardware[n_].base;
        // Just in case a transfer is triggered right now.
        barrier ();
        // According to datasheet, bit 14 should be 1!
        base->OAR1 = addr | (1 << 14);
    }
}

void
I2cHard::ev_isr (int n)
{
    auto base = i2c_hardware[n].base;
    assert (i2c_instances[n]);
    I2cHard &i2c = *i2c_instances[n];
    i2c_trace ("<%d> event isr", n);
    while (1)
    {
        uint16_t sr1 = base->SR1;
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
                    base->CR1 = I2C_CR1_PE;
                    sr2 = base->SR2;
                    base->CR1 = I2C_CR1_STOP | I2C_CR1_PE;
                    // TODO: what to wait now? Unsupported for now.
                }
                else if ((i2c.master_slave_addr_ & 1) && i2c.buf_count_ == 2)
                {
                    base->CR1 = I2C_CR1_POS | I2C_CR1_PE;
                    sr2 = base->SR2;
                    // Wait for BTF.
                }
                else
                {
                    sr2 = base->SR2;
                    base->CR2 |= I2C_CR2_ITBUFEN;
                }
                i2c_trace ("<%d> master sr2=%04x", n, sr2);
            }
            else if (sr1 & I2C_SR1_TXE
                     && i2c.buf_index_ < i2c.buf_count_)
            {
                i2c_trace ("<%d> master tx index=%d", n, i2c.buf_index_);
                // Send next byte.
                base->DR = i2c.master_buf_[i2c.buf_index_++];
                // Wait for BTF if last one.
                if (i2c.buf_index_ == i2c.buf_count_)
                    base->CR2 &= ~I2C_CR2_ITBUFEN;
            }
            else if (sr1 & I2C_SR1_RXNE
                     && i2c.buf_count_ - i2c.buf_index_ > 3)
            {
                i2c_trace ("<%d> master rx index=%d", n, i2c.buf_index_);
                i2c.master_buf_[i2c.buf_index_++] = base->DR;
                if (i2c.buf_count_ - i2c.buf_index_ == 3)
                    // Wait for BTF.
                    base->CR2 &= ~I2C_CR2_ITBUFEN;
            }
            else if (sr1 & I2C_SR1_BTF)
            {
                i2c_trace ("<%d> master btf index=%d", n, i2c.buf_index_);
                if (!(i2c.master_slave_addr_ & 1))
                {
                    // End of transmission.
                    base->CR1 = I2C_CR1_ACK | I2C_CR1_STOP | I2C_CR1_PE;
                    i2c.master_ = false;
                    i2c.master_status_ = i2c.buf_index_;
                    if (i2c.finished_handler_)
                        i2c.finished_handler_->finished (i2c.master_status_);
                }
                else if (i2c.buf_count_ - i2c.buf_index_ == 3)
                {
                    // Near end of reception.
                    base->CR1 = I2C_CR1_PE;
                    i2c.master_buf_[i2c.buf_index_++] = base->DR;
                    // Wait for BTF.
                }
                else
                {
                    // End of reception.
                    base->CR1 = I2C_CR1_ACK | I2C_CR1_STOP | I2C_CR1_PE;
                    if (i2c.buf_count_ - i2c.buf_index_ == 2)
                        i2c.master_buf_[i2c.buf_index_++] = base->DR;
                    i2c.master_buf_[i2c.buf_index_++] = base->DR;
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
                uint16_t sr2 = base->SR2;
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
                base->CR2 |= I2C_CR2_ITBUFEN;
            }
            else if (sr1 & I2C_SR1_TXE)
            {
                i2c_trace ("<%d> slave tx index=%d", n, i2c.buf_index_);
                uint8_t b = 0xff;
                if (i2c.buf_index_ < i2c.buf_count_)
                    b = i2c.slave_buf_[i2c.buf_index_++];
                base->DR = b;
            }
            else if (sr1 & I2C_SR1_RXNE)
            {
                i2c_trace ("<%d> slave rx index=%d", n, i2c.buf_index_);
                uint8_t b = base->DR;
                if (i2c.buf_index_ < i2c.buf_count_)
                    i2c.slave_buf_[i2c.buf_index_++] = b;
            }
            else if (sr1 & I2C_SR1_STOPF)
            {
                i2c_trace ("<%d> slave stop", n);
                i2c.slave_data_handler_->to_recv (i2c.slave_buf_, i2c.buf_index_);
                // TODO: multimaster: there is no way to write in this
                // register if a START was requested to switch to master mode!
                base->CR1 = I2C_CR1_ACK | I2C_CR1_PE;
                base->CR2 &= ~I2C_CR2_ITBUFEN;
            }
            else if (sr1 & I2C_SR1_SB)
            {
                i2c_trace ("<%d> master start", n);
                // Starting master mode.
                base->DR = i2c.master_slave_addr_;
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
    auto base = i2c_hardware[n].base;
    assert (i2c_instances[n]);
    I2cHard &i2c = *i2c_instances[n];
    uint16_t sr1 = base->SR1;
    base->SR1 = 0;
    i2c_trace ("<%d> error isr sr1=%04x", n, sr1);
    if (i2c.master_)
    {
        if (sr1 & I2C_SR1_ARLO)
        {
            // Try again.
            base->CR1 = I2C_CR1_ACK | I2C_CR1_START | I2C_CR1_PE;
            i2c.master_ = false;
        }
        else if (sr1 & I2C_SR1_AF)
        {
            base->CR1 = I2C_CR1_ACK | I2C_CR1_STOP | I2C_CR1_PE;
            i2c.master_ = false;
            i2c.master_status_ = i2c.buf_index_;
            if (i2c.finished_handler_)
                i2c.finished_handler_->finished (i2c.master_status_);
        }
    }
    base->CR2 &= ~I2C_CR2_ITBUFEN;
    // TODO: handle misplaced STOP errata.
}

} // namespace ucoo
