#ifndef ucoolib_hal_i2c_i2c_host_hh
#define ucoolib_hal_i2c_i2c_host_hh
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
#include "ucoolib/arch/host/host.hh"

namespace ucoo {

class I2cHostShared;

/// I2C interface, host version.
class I2cHost : public I2c
{
  public:
    /// Initialise the Nth I2C.
    I2cHost (Host &host, int n);
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
  private:
    /// I2C number.
    int n_;
    /// Slave address.
    uint8_t slave_addr_;
    /// Handler called to source or sink data for slave exchanges.
    DataHandler *slave_data_handler_;
    /// Current master transfer status.
    int master_status_;
    /// Shared context.
    static I2cHostShared *shared_;
    friend class I2cHostShared;
};

} // namespace ucoo


#endif // ucoolib_hal_i2c_i2c_host_hh
