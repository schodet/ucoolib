#ifndef ucoolib_base_proto_proto_hh
#define ucoolib_base_proto_proto_hh
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
#include "ucoolib/common.hh"
#include "ucoolib/intf/stream.hh"

#include "config/base/proto.hh"

namespace ucoo {

/// Support for old proto protocol.
class Proto
{
  public:
    /// Receivers should implement this interface.
    class Handler
    {
      public:
        /// Handle a received message.
        virtual void proto_handle (Proto &proto, char cmd,
                                   const uint8_t *args, int size) = 0;
    };
  public:
    /// Constructor.
    Proto (Handler &handler, Stream &stream);
    /// Read from stream and handle any received message.
    void accept ();
    /// Send a message with no argument.
    void send (char cmd);
    /// Send a message with one argument.
    void send (char cmd, const char *fmt, int a0);
    /// Send a message with two arguments.
    void send (char cmd, const char *fmt, int a0, int a1);
    /// Send a message with three arguments.
    void send (char cmd, const char *fmt, int a0, int a1, int a2);
    /// Send a message with four arguments.
    void send (char cmd, const char *fmt, int a0, int a1, int a2, int a3);
    /// Send a message, with a byte buffer.
    void send_buf (char cmd, const uint8_t *args, int size);
  private:
    /// Accept a digit to be used for args.
    void accept_digit (int c);
    /// Accept a quoted char to be used for args.
    void accept_char (int c);
  private:
    /// Handler interface.
    Handler &handler_;
    /// Connected stream.
    Stream &stream_;
    /// Decoding step.
    enum Step
    {
        /// Nothing received yet.
        IDLE,
        /// Exclamation mark received.
        BANG,
        /// Expecting argument or end of frame.
        COMMAND,
        /// Expecting second hex digit of an argument.
        ARG_DIGIT,
        /// Quote received, expect any char.
        ARG_CHAR,
        /// Double quote received, read a string.
        ARG_STRING,
    };
    Step step_;
    /// Received message current size.
    int size_;
    /// Message arguments being received.
    uint8_t args_[UCOO_CONFIG_BASE_PROTO_ARGS_MAX_SIZE];
    /// Command being received.
    char cmd_;
};

} // namespace ucoo

#endif // ucoolib_base_proto_proto_hh
