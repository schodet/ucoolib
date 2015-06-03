#ifndef ucoo_arch_host_mex_mex_hh
#define ucoo_arch_host_mex_mex_hh
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

namespace ucoo {
namespace mex {

/// Message type identifiers.
enum mtype_t
{
    /// IDLE message, sent by nodes to the hub to signal it had handled all
    /// messages.  It can have an optional parameter: the date at which the
    /// node will not be idle anymore.
    MTYPE_IDLE = 0,
    /// DATE message, sent by the hub to the nodes to update the current date
    /// value, sent by a node to request the current date (useful at startup).
    MTYPE_DATE = 1,
    /// REQ (request) message, sent from a node to other nodes to request a
    /// response message.
    MTYPE_REQ = 2,
    /// RSP (response) message, response to a REQ message, only sent to the
    /// requesting node.
    MTYPE_RSP = 3,
    /// RES (reserve) message, sent to the hub to request a message type, sent
    /// to a node with the allocated message type.
    MTYPE_RES = 4,
    /// First unreserved message identifier, to be used by user messages.
    MTYPE_USER_MIN = 0x20,
};

} // namespace mex
} // namespace ucoo

#endif // ucoo_arch_host_mex_mex_hh
