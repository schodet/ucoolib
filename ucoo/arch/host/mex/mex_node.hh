#ifndef ucoo_arch_host_mex_mex_node_hh
#define ucoo_arch_host_mex_mex_node_hh
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
#include "ucoo/common.hh"
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

#endif // ucoo_arch_host_mex_mex_node_hh
