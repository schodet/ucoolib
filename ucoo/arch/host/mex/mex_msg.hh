#ifndef ucoo_arch_host_mex_mex_msg_hh
#define ucoo_arch_host_mex_mex_msg_hh
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

#include "ucoo/arch/host/mex/mex.hh"

#include <vector>

namespace ucoo {
namespace mex {

class MsgPusher;
class MsgPoper;
class MsgReader;
class MsgWriter;

/// Mex message.
///
/// Format used for payload push and pop:
///  - b: 8 bits.
///  - h: 16 bits.
///  - l: 32 bits.
/// Uppercase is used for unsigned.
class Msg
{
  public:
    /// New message with the given MTYPE.
    explicit Msg (mtype_t mtype);
    /// Read a new message.
    explicit Msg (MsgReader &reader);
    /// Write a message.
    void write (MsgWriter &writer);
    /// Add data to the message payload.  FMT string describes the provided
    /// data.  Return a pusher which will do the actual push, using the
    /// operator<<.
    MsgPusher push (const char *fmt);
    /// Add data to the message payload from a buffer.
    void push (const char *buffer, int size);
    /// Extract data from the message payload.  FMT string describes the
    /// requested data.  Return a poper which will do the actual pop, using
    /// the operator>>.
    MsgPoper pop (const char *fmt);
    /// Pop buffer of given size.  Data is kept valid as long as this object
    /// is alive.
    const char *pop (int size);
    /// Encapsulate a message in another one, adding header data.  Return a
    /// pusher which will do the actual header data push, using the
    /// operator<<.
    MsgPusher encapsulate (mtype_t mtype, const char *fmt);
    /// Decapsulate a message from payload.  The current payload becomes the
    /// full message.
    void decapsulate ();
    /// Get remaining length.
    int len () const;
    /// Get message type.
    mtype_t mtype () const { return mtype_; }
  private:
    /// Message payload, not including message type.
    std::vector<char> payload_;
    /// Unused space at head of payload.
    int head_skip_;
    /// Message type.
    mtype_t mtype_;
};

/// Helper used to push data to message.
class MsgPusher
{
  public:
    /// Push an integer.
    MsgPusher &operator<< (int32_t v);
    /// Push an unsigned integer, used when an int is not enough.
    MsgPusher &operator<< (uint32_t v);
  private:
    MsgPusher (std::vector<char>::iterator out, const char *fmt);
    void push (int64_t v);
  private:
    friend class Msg;
    std::vector<char>::iterator out_;
    const char *fmt_;
};

/// Helper used to pop data from message.
class MsgPoper
{
  public:
    /// Pop for each supported types.
    MsgPoper &operator>> (int32_t &v);
    MsgPoper &operator>> (uint32_t &v);
    MsgPoper &operator>> (int16_t &v);
    MsgPoper &operator>> (uint16_t &v);
    MsgPoper &operator>> (int8_t &v);
    MsgPoper &operator>> (uint8_t &v);
  private:
    MsgPoper (std::vector<char>::const_iterator in, const char *fmt);
    uint32_t pop_bytes (int bytes);
    int64_t pop (int bits, bool sign);
  private:
    friend class Msg;
    std::vector<char>::const_iterator in_;
    const char *fmt_;
};

/// Abstract Msg reader.
class MsgReader
{
  public:
    /// Get message size.
    virtual int size () = 0;
    /// Read message bytes to provided buffer.
    virtual void read (char *buf) = 0;
};

/// Abstract Msg writer.
class MsgWriter
{
  public:
    /// Write COUNT bytes from provided buffer.
    virtual void write (const char *buf, int count) = 0;
};

} // namespace mex
} // namespace ucoo

#endif // ucoo_arch_host_mex_mex_msg_hh
