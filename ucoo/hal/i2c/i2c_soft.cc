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
#include "ucoo/hal/i2c/i2c_soft.hh"
#include "ucoo/utils/delay.hh"

namespace ucoo {

I2cSoft::I2cSoft (Io &scl, Io &sda)
    : scl_ (scl), sda_ (sda), half_period_ns_ (0),
      current_status_ (STATUS_ERROR)
{
}

I2cSoft::~I2cSoft ()
{
    disable ();
}

void
I2cSoft::enable (int speed)
{
    half_period_ns_ = 1000000000 / speed;
    sda_.reset ();
    scl_.reset ();
}

void
I2cSoft::disable ()
{
    half_period_ns_ = 0;
}

void
I2cSoft::send (uint8_t addr, const char *buf, int count)
{
    bool nack;
    int sent = 0;
    // Start.
    send_start ();
    // Send SLA+W.
    nack = send_byte (addr);
    // Send data.
    for (; !nack && sent < count; sent++)
        nack = send_byte (buf[sent]);
    // Stop.
    send_stop ();
    // Update status, there is no background task.
    current_status_ = sent;
    if (finished_handler_)
        finished_handler_->finished (sent);
}

void
I2cSoft::recv (uint8_t addr, char *buf, int count)
{
    bool nack;
    int recv = 0;
    // Start.
    send_start ();
    // Send SLA+R.
    nack = send_byte (addr | 1);
    if (!nack)
    {
        // Receive data, send nack in last byte.
        for (; recv < count; recv++)
            buf[recv] = recv_byte (recv == count - 1 ? true : false);
    }
    // Stop.
    send_stop ();
    // Update status, there is no background task.
    current_status_ = recv;
    if (finished_handler_)
        finished_handler_->finished (recv);
}

void
I2cSoft::send_recv (uint8_t addr, const char *send_buf, int send_count,
                    char *recv_buf, int recv_count)
{
    bool nack;
    int sent = 0, recv = 0;
    // Start.
    send_start ();
    // Send SLA+W.
    nack = send_byte (addr);
    // Send data.
    for (; !nack && sent < send_count; sent++)
        nack = send_byte (send_buf[sent]);
    // Update status, there is no background task.
    if (sent != send_count)
    {
        current_status_ = STATUS_ERROR;
        if (finished_handler_)
            finished_handler_->finished (STATUS_ERROR);
        return;
    }
    // Restart.
    send_restart ();
    // Send SLA+R.
    nack = send_byte (addr | 1);
    if (!nack)
    {
        // Receive data, send nack in last byte.
        for (; recv < recv_count; recv++)
            recv_buf[recv] = recv_byte (recv == recv_count - 1 ? true : false);
    }
    // Stop.
    send_stop ();
    // Update status, there is no background task.
    current_status_ = recv;
    if (finished_handler_)
        finished_handler_->finished (recv);
}

int
I2cSoft::status ()
{
    return current_status_;
}

int
I2cSoft::wait ()
{
    // No background task, nothing to wait.
    return current_status_;
}

void
I2cSoft::delay ()
{
    delay_ns (half_period_ns_);
}

void
I2cSoft::wait_scl ()
{
    while (!scl_.get ())
        ;
}

void
I2cSoft::send_start ()
{
    sda_.output ();
    delay ();
    scl_.output ();
    delay ();
}

void
I2cSoft::send_restart ()
{
    sda_.input ();
    delay ();
    scl_.input ();
    wait_scl ();
    delay ();
    send_start ();
}

void
I2cSoft::send_stop ()
{
    sda_.output ();
    delay ();
    scl_.input ();
    wait_scl ();
    delay ();
    sda_.input ();
    delay ();
}

void
I2cSoft::send_bit (bool bit)
{
    if (bit)
        sda_.input ();
    else
        sda_.output ();
    delay ();
    scl_.input ();
    wait_scl ();
    delay ();
    scl_.output ();
}

bool
I2cSoft::recv_bit ()
{
    sda_.input ();
    delay ();
    scl_.input ();
    wait_scl ();
    bool bit = sda_.get ();
    delay ();
    scl_.output ();
    return bit;
}

bool
I2cSoft::send_byte (char b)
{
    for (int i = 8; i; i--)
    {
        send_bit (b & 0x80);
        b <<= 1;
    }
    return recv_bit ();
}

char
I2cSoft::recv_byte (bool nack)
{
    char b = 0;
    for (int i = 8; i; i--)
    {
        b <<= 1;
        b |= recv_bit () ? 1 : 0;
    }
    send_bit (nack);
    return b;
}

} // namespace ucoo
