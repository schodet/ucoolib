#ifndef ucoolib_hal_i2c_i2c_hard_stm32_hh
#define ucoolib_hal_i2c_i2c_hard_stm32_hh
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
#include "ucoolib/intf/i2c.hh"

#include "config/hal/i2c.hh"

namespace ucoo {

/// I2C interface, using dedicated hardware.
class I2cHard : public I2c
{
  public:
    /// Initialise the Nth I2C.
    I2cHard (int n, bool enable, int speed = 100000);
    /// Shutdown.
    ~I2cHard ();
    /// Enable or disable.
    void setup (bool enable, int speed = 100000);
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
    bool enable_;
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

#endif // ucoolib_hal_i2c_i2c_hard_stm32_hh
