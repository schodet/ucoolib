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
#include "i2c_slave_data_buffer.hh"

#include <algorithm>

namespace ucoo {

I2cSlaveDataBuffer::I2cSlaveDataBuffer (char *send_buf, int send_size,
                                        char *recv_buf, int recv_size)
    : send_buf_ (send_buf), send_size_ (send_size), send_count_ (0),
      recv_buf_ (recv_buf), recv_size_ (recv_size), recv_count_ (0)
{
}

void
I2cSlaveDataBuffer::update (const char *buf, int count)
{
    int r = std::min (count, send_size_);
    irq_flags_t flags = irq_lock ();
    std::copy (buf, buf + r, send_buf_);
    send_count_ = r;
    irq_restore (flags);
}

int
I2cSlaveDataBuffer::poll (char *buf, int count)
{
    // Test to avoid superfluous irq lock.  This is not a problem if
    // condition become false before irq is locked.
    if (recv_count_)
    {
        irq_flags_t flags = irq_lock ();
        int r = std::min (count, recv_count_);
        recv_count_ = 0;
        std::copy (recv_buf_, recv_buf_ + r, buf);
        irq_restore (flags);
        return r;
    }
    else return 0;
}

int
I2cSlaveDataBuffer::to_send (char *buf, int count)
{
    int r = std::min (count, send_count_);
    std::copy (send_buf_, send_buf_ + r, buf);
    return r;
}

void
I2cSlaveDataBuffer::to_recv (const char *buf, int count)
{
    int r = std::min (count, recv_size_);
    std::copy (buf, buf + r, recv_buf_);
    recv_count_ = r;
}

} // namespace ucoo
