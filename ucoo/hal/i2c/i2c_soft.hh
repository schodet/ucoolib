#ifndef ucoo_hal_i2c_i2c_soft_hh
#define ucoo_hal_i2c_i2c_soft_hh
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
#include "ucoo/intf/i2c.hh"
#include "ucoo/intf/io.hh"

namespace ucoo {

/// I2C interface, using IO.
class I2cSoft : public I2cMaster
{
  public:
    /// Constructor.
    I2cSoft (Io &scl, Io &sda);
    /// Destructor.
    ~I2cSoft ();
    /// Enable and setup.
    void enable (int speed = 100000);
    /// Disable.
    void disable ();
    /// See I2cMaster::send.
    void send (uint8_t addr, const char *buf, int count);
    /// See I2cMaster::recv.
    void recv (uint8_t addr, char *buf, int count);
    /// See I2cMaster::send_recv
    void send_recv (uint8_t addr, const char *send_buf, int send_count,
                    char *recv_buf, int recv_count);
    /// See I2cMaster::status.
    int status ();
    /// See I2cMaster::wait.
    int wait ();
  private:
    /// Wait half a bit period.
    void delay ();
    /// Wait until SCL goes high (clock stretching).
    void wait_scl ();
    /// Send start condition.
    void send_start ();
    /// Send restart condition.
    void send_restart ();
    /// Send stop condition.
    void send_stop ();
    /// Send one bit.
    void send_bit (bool bit);
    /// Receive one bit.
    bool recv_bit ();
    /// Send one byte, return nack bit.
    bool send_byte (char b);
    /// Receive one byte.
    char recv_byte (bool nack);
  private:
    /// I2C signals.
    Io &scl_, &sda_;
    /// Half period.
    int half_period_ns_;
    /// Current status.
    int current_status_;
};

} // namespace ucoo

#endif // ucoo_hal_i2c_i2c_soft_hh
