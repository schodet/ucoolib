// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include "mex_node.hh"

#include <cstring>

namespace ucoo {
namespace mex {

Node::Node ()
    : date_ (0),
      req_ (-1)
{
    // Connect.
    socket_.connect ();
    // Setup default handlers.
    handler_register (MTYPE_DATE, *this, &Node::handle_date);
    handler_register (MTYPE_REQ, *this, &Node::handle_req);
    // Synchronise with the hub.
    bool sync = false;
    while (!sync)
    {
        std::auto_ptr<Msg> msg = recv ();
        if (msg->mtype () == MTYPE_DATE)
            sync = true;
        dispatch (*msg);
    }
}

void
Node::wait ()
{
    while (1)
    {
        // Signal IDLE state.
        Msg idle (MTYPE_IDLE);
        send (idle);
        // Receive and dispatch messages.
        std::auto_ptr<Msg> msg = recv ();
        dispatch (*msg);
    }
}

void
Node::wait (uint32_t date)
{
    while (date_ != date)
    {
        // Signal IDLE state.
        Msg idle (MTYPE_IDLE);
        idle.push ("L") << date;
        send (idle);
        // Receive and dispatch messages.
        std::auto_ptr<Msg> msg = recv ();
        dispatch (*msg);
    }
}

void
Node::send (Msg &msg)
{
    msg.write (socket_);
}

std::auto_ptr<Msg>
Node::request (Msg &msg)
{
    // Send request.
    msg.encapsulate (MTYPE_REQ, "B") << 0;
    send (msg);
    // Wait for response.
    std::auto_ptr<Msg> rsp;
    rsp = recv ();
    while (rsp->mtype () != MTYPE_RSP)
    {
        dispatch (*rsp);
        rsp = recv ();
    }
    // Eat unused request identifier.
    rsp->pop ("B");
    rsp->decapsulate ();
    return rsp;
}

void
Node::response (Msg &msg)
{
    assert (req_ != -1);
    msg.encapsulate (MTYPE_RSP, "B") << req_;
    send (msg);
    req_ = -1;
}

mtype_t
Node::reserve (const std::string &name)
{
    // Send request.
    Msg msg (MTYPE_RES);
    msg.push (name.data (), name.size ());
    send (msg);
    // Wait for response.
    std::auto_ptr<Msg> rsp;
    rsp = recv ();
    while (rsp->mtype () != MTYPE_RES)
    {
        dispatch (*rsp);
        rsp = recv ();
    }
    // Return allocated message type.
    int mtype;
    rsp->pop ("B") >> mtype;
    return static_cast<mtype_t> (mtype);
}

void
Node::handler_register (mtype_t mtype, Handler handler)
{
    std::pair<Handlers::iterator, bool> r =
        handlers_.insert (Handlers::value_type (mtype, handler));
    assert (r.second);
}

std::auto_ptr<Msg>
Node::recv ()
{
    return std::auto_ptr<Msg> (new Msg (socket_));
}

void
Node::dispatch (Msg &msg)
{
    Handlers::const_iterator it = handlers_.find (msg.mtype ());
    if (it != handlers_.end ())
        it->second (*this, msg);
}

void
Node::handle_date (Msg &msg)
{
    msg.pop ("L") >> date_;
}

void
Node::handle_req (Msg &msg)
{
    msg.pop ("B") >> req_;
    msg.decapsulate ();
    dispatch (msg);
    req_ = -1;
}

} // namespace mex
} // namespace ucoo
