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
