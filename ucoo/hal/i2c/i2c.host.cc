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
#include "i2c.host.hh"

#include "config/ucoo/hal/i2c.hh"

namespace ucoo {

/// Shared context between instances.
class I2cHostShared
{
  public:
    /// Constructor, connect to mex node.
    I2cHostShared (Host &host);
    /// Register a new driver instance.
    void register_instance (Host &host, I2cHost &instance);
    /// Send message, return new master status.
    int send (uint8_t addr, const char *buf, int count);
    /// Receive message, return new master status.
    int recv (uint8_t addr, char *buf, int count);
    /// Handle read requests from master.
    void handle_read (mex::Msg &msg);
    /// Handle write requests from master.
    void handle_write (mex::Msg &msg);
  private:
    Host &host_;
    mex::Node &node_;
    mex::mtype_t read_mtype_, write_mtype_;
    typedef std::list<I2cHost *> Instances;
    Instances instances_;
};

I2cHostShared::I2cHostShared (Host &host)
    : host_ (host), node_ (host.get_node ())
{
    std::string instance = host_.get_instance (1);
    read_mtype_ = node_.reserve (instance + ":read");
    write_mtype_ = node_.reserve (instance + ":write");
    node_.handler_register (read_mtype_, *this, &I2cHostShared::handle_read);
    node_.handler_register (write_mtype_, *this, &I2cHostShared::handle_write);
}

void
I2cHostShared::register_instance (Host &host, I2cHost &instance)
{
    assert (&host == &host_);
    instances_.push_back (&instance);
}

int
I2cHostShared::send (uint8_t addr, const char *buf, int count)
{
    // Test for this slave in the same program.
    for (Instances::const_iterator i = instances_.begin ();
         i != instances_.end (); ++i)
    {
        if (addr == (*i)->slave_addr_)
        {
            assert (count <= CONFIG_UCOO_HAL_I2C_SLAVE_BUFFER_SIZE);
            (*i)->slave_data_handler_->to_recv (buf, count);
            return count;
        }
    }
    // Else, send message.
    mex::Msg msg (write_mtype_);
    msg.push ("B") << addr;
    msg.push (buf, count);
    node_.send (msg);
    return count;
}

int
I2cHostShared::recv (uint8_t addr, char *buf, int count)
{
    // Test for this slave in the same program.
    for (Instances::const_iterator i = instances_.begin ();
         i != instances_.end (); ++i)
    {
        if (addr == (*i)->slave_addr_)
        {
            assert (count <= CONFIG_UCOO_HAL_I2C_SLAVE_BUFFER_SIZE);
            return (*i)->slave_data_handler_->to_send (buf, count);
        }
    }
    // Else, send request and wait for response.
    mex::Msg msg (read_mtype_);
    msg.push ("BB") << addr << count;
    std::auto_ptr<mex::Msg> rsp = node_.request (msg);
    int rcount = rsp->len ();
    assert (rcount <= count);
    const char *rbuf = rsp->pop (rcount);
    std::copy (rbuf, rbuf + rcount, buf);
    return rcount;
}

void
I2cHostShared::handle_read (mex::Msg &msg)
{
    uint8_t addr;
    int size;
    msg.pop ("BB") >> addr >> size;
    for (Instances::const_iterator i = instances_.begin ();
         i != instances_.end (); ++i)
    {
        if (addr == (*i)->slave_addr_)
        {
            assert (size <= CONFIG_UCOO_HAL_I2C_SLAVE_BUFFER_SIZE);
            char buf[size];
            int n = (*i)->slave_data_handler_->to_send (buf, size);
            mex::Msg rsp (read_mtype_);
            rsp.push (buf, n);
            node_.response (rsp);
            break;
        }
    }
}

void
I2cHostShared::handle_write (mex::Msg &msg)
{
    uint8_t addr;
    msg.pop ("B") >> addr;
    for (Instances::const_iterator i = instances_.begin ();
         i != instances_.end (); ++i)
    {
        if (addr == (*i)->slave_addr_)
        {
            int size = msg.len ();
            assert (size <= CONFIG_UCOO_HAL_I2C_SLAVE_BUFFER_SIZE);
            (*i)->slave_data_handler_->to_recv (msg.pop (size), size);
            break;
        }
    }
}

I2cHostShared *I2cHost::shared_;

I2cHost::I2cHost (Host &host, int n)
    : n_ (n), slave_addr_ (0), slave_data_handler_ (0),
      master_status_ (STATUS_ERROR)
{
    if (!shared_)
    {
        static I2cHostShared shared (host);
        shared_ = &shared;
    }
    shared_->register_instance (host, *this);
    // n is not used, there is no support for several buses in the MEX
    // messages at the moment.
}

void
I2cHost::send (uint8_t addr, const char *buf, int count)
{
    // Update status, there is no background task.
    master_status_ = shared_->send (addr, buf, count);
    // If needed, call callback.
    if (finished_handler_)
        finished_handler_->finished (master_status_);
}

void
I2cHost::recv (uint8_t addr, char *buf, int count)
{
    // Update status, there is no background task.
    master_status_ = shared_->recv (addr, buf, count);
    // If needed, call callback.
    if (finished_handler_)
        finished_handler_->finished (master_status_);
}

void
I2cHost::send_recv (uint8_t addr, const char *send_buf, int send_count,
                    char *recv_buf, int recv_count)
{
    // No restart in host implementation, do regular send/recv.
    int r = shared_->send (addr, send_buf, send_count);
    if (r == send_count)
    {
        master_status_ = STATUS_ERROR;
        if (finished_handler_)
            finished_handler_->finished (master_status_);
    }
    else
        recv (addr, recv_buf, recv_count);
}

int
I2cHost::status ()
{
    return master_status_;
}

int
I2cHost::wait ()
{
    // Transfers are immediate.
    return status ();
}

void
I2cHost::register_data (uint8_t addr, DataHandler &data_handler)
{
    slave_addr_ = addr;
    slave_data_handler_ = &data_handler;
}

} // namespace ucoo
