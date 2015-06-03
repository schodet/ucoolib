#ifndef ucoo_hal_i2c_i2c_host_hh
#define ucoo_hal_i2c_i2c_host_hh
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
#include "ucoo/arch/host/host.hh"

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


#endif // ucoo_hal_i2c_i2c_host_hh
