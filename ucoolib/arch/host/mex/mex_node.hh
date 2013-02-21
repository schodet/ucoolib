#ifndef ucoolib_arch_host_mex_mex_node_hh
#define ucoolib_arch_host_mex_mex_node_hh
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
#include "ucoolib/common.hh"
#include "mex_msg.hh"
#include "mex_socket.hh"

#include <memory>
#include <map>
#include <string>
#include <tr1/functional>

namespace ucoo {
namespace mex {

/// Mex node, singleton.
class Node
{
  public:
    /// Message handler type.
    typedef std::tr1::function<void (Node &, Msg &)> Handler;
  public:
    /// Connect to mex hub.
    Node ();
    /// Wait forever.
    void wait ();
    /// Wait until date is reached.
    void wait (uint32_t date);
    /// Return current date.
    uint32_t date () const { return date_; }
    /// Send a message.
    void send (Msg &msg);
    /// Send a request and return response.
    std::auto_ptr<Msg> request (Msg &msg);
    /// Send a response while handling a request.
    void response (Msg &msg);
    /// Reserve a message type.
    mtype_t reserve (const std::string &name);
    /// Register a handler for a message type.
    void handler_register (mtype_t mtype, Handler handler);
    /// Register a handler for a message type, member function version.
    template<class T>
    void handler_register (mtype_t mtype, T &obj,
                           void (T::*handler) (Node &, Msg &))
    {
        using namespace std::tr1::placeholders;
        handler_register (mtype, std::tr1::bind (handler, &obj, _1, _2));
    }
    /// Register a handler for a message type, member function version,
    /// without Node parameter.
    template<class T>
    void handler_register (mtype_t mtype, T &obj,
                           void (T::*handler) (Msg &))
    {
        using namespace std::tr1::placeholders;
        handler_register (mtype, std::tr1::bind (handler, &obj, _2));
    }
  private:
    /// Receive one message.
    std::auto_ptr<Msg> recv ();
    /// Dispatch message to the right handler.
    void dispatch (Msg &msg);
    /// Handle an incomming DATE message.
    void handle_date (Msg &msg);
    /// Handle an incomming REQ message.
    void handle_req (Msg &msg);
  private:
    /// Connection to hub.
    Socket socket_;
    /// Current date.
    uint32_t date_;
    /// When handling a request, this is the request identifier, else -1.
    int req_;
    /// Registered handlers.
    typedef std::map<mtype_t, Handler> Handlers;
    Handlers handlers_;
};

} // namespace mex
} // namespace ucoo

#endif // ucoolib_arch_host_mex_mex_node_hh
