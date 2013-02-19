#ifndef ucoolib_hal_i2c_i2c_slave_data_buffer_hh
#define ucoolib_hal_i2c_i2c_slave_data_buffer_hh
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

namespace ucoo {

/// Data sink and source for I2C slave.  Store any received data and data to
/// send in buffers.
class I2cSlaveDataBuffer : public I2cSlave::DataHandler
{
  public:
    /// Constructor, take buffers.
    I2cSlaveDataBuffer (char *send_buf, int send_size,
                        char *recv_buf, int recv_size);
    /// Update slave internal buffer, will be sent to master on request.
    void update (const char *buf, int count);
    /// If data has been received, copy it to provided buffer and return its
    /// size.  Else, return 0.
    int poll (char *buf, int count);
    /// See I2cSlave::DataHandler::to_send.
    int to_send (char *buf, int count);
    /// See I2cSlave::DataHandler::to_recv.
    void to_recv (const char *buf, int count);
  private:
    char *send_buf_;
    int send_size_, send_count_;
    char *recv_buf_;
    int recv_size_, recv_count_;
};

/// Same as I2cSlaveDataBuffer, but include buffers.
template<int send_size, int recv_size>
class I2cSlaveDataBufferSize : public I2cSlaveDataBuffer
{
  public:
    /// Default constructor.
    I2cSlaveDataBufferSize ()
        : I2cSlaveDataBuffer (send_array_, sizeof (send_array_),
                              recv_array_, sizeof (recv_array_))
        { }
  private:
    char send_array_[send_size];
    char recv_array_[recv_size];
};

} // namespace ucoo

#endif // ucoolib_hal_i2c_i2c_slave_data_buffer_hh
