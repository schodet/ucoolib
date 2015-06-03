#ifndef ucoo_hal_i2c_i2c_slave_data_buffer_hh
#define ucoo_hal_i2c_i2c_slave_data_buffer_hh
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

#endif // ucoo_hal_i2c_i2c_slave_data_buffer_hh
