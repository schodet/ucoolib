#ifndef ucoo_hal_i2c_i2c_hard_stm32_hh
#define ucoo_hal_i2c_i2c_hard_stm32_hh
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
#include "ucoo/intf/i2c.hh"

#include "config/hal/i2c.hh"

namespace ucoo {

/// I2C interface, using dedicated hardware.
class I2cHard : public I2c
{
  public:
    /// Constructor for the Nth I2C.
    I2cHard (int n);
    /// Shutdown.
    ~I2cHard ();
    /// Enable and setup
    void enable (int speed = 100000);
    /// Disable.
    void disable ();
    /// See I2cMaster::send.
    void send (uint8_t addr, const char *buf, int count);
    /// See I2cMaster::recv.
    void recv (uint8_t addr, char *buf, int count);
    /// See I2cMaster::status.
    int status ();
    /// See I2cMaster::wait.
    int wait ();
    /// See I2cSlave::register_data.
    void register_data (uint8_t addr, DataHandler &data_handler);
    /// Event ISR.
    static void ev_isr (int n);
    /// Error ISR.
    static void er_isr (int n);
  private:
    /// Start a master transfer, send or recv, depending on addr LSB.
    void transfer (uint8_t addr, char *buf, int count);
  private:
    /// I2C number.
    int n_;
    /// Is it enabled?
    bool enabled_;
    /// Slave address.
    uint8_t slave_addr_;
    /// Handler called to source or sink data for slave exchanges.
    DataHandler *slave_data_handler_;
    /// Slave buffer.
    char slave_buf_[UCOO_CONFIG_HAL_I2C_SLAVE_BUFFER_SIZE];
    /// Current buffer count (bytes to send), or buffer size (available room).
    int buf_count_;
    /// Current buffer index (position to read byte to send or write received
    /// byte).
    int buf_index_;
    /// Master access granted.
    bool master_;
    /// Current master transfer status.
    int master_status_;
    /// Current master transfer buffer.
    char *master_buf_;
    /// Current master transfer size, copied to buf_count_ when active.
    int master_count_;
    /// Current master transfer slave address, LSB is set for receiver mode.
    uint8_t master_slave_addr_;
};

} // namespace ucoo

#endif // ucoo_hal_i2c_i2c_hard_stm32_hh
