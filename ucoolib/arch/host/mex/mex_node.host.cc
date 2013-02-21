// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include "mex_node.hh"

#include <cstring>

namespace ucoo {
namespace mex {

Node *Node::instance_;

Node::Node ()
    : date_ (0),
      req_ (-1),
      handler_date (*this, &Node::handle_date),
      handler_req (*this, &Node::handle_req)
{
    assert (!instance_);
    instance_ = this;
    // Connect.
    socket_.connect ();
    // Setup default handlers.
    handler_register (MTYPE_DATE, handler_date);
    handler_register (MTYPE_REQ, handler_req);
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

Node::~Node ()
{
    instance_ = 0;
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
Node::handler_register (mtype_t mtype, Handler &handler)
{
    std::pair<handlers_type::iterator, bool> r =
        handlers_.insert (handlers_type::value_type (mtype, &handler));
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
    handlers_type::const_iterator it = handlers_.find (msg.mtype ());
    if (it != handlers_.end ())
        it->second->handle (msg);
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
