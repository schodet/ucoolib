#ifndef ucoo_base_proto_proto_hh
#define ucoo_base_proto_proto_hh
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
#include "ucoo/common.hh"
#include "ucoo/intf/stream.hh"

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

#endif // ucoo_base_proto_proto_hh
