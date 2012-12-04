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
#define __STDC_LIMIT_MACROS 1
#include "mex_msg.hh"

namespace ucoo {
namespace mex {

const int default_head_skip = 3;

/// Compute needed size to store given format.
static int
calcsize (const char *fmt)
{
    int size = 0;
    for (; *fmt; fmt++)
    {
        switch (*fmt)
        {
        case 'l':
        case 'L':
            size += 4;
            break;
        case 'h':
        case 'H':
            size += 2;
            break;
        case 'b':
        case 'B':
            size += 1;
            break;
        }
    }
    return size;
}

Msg::Msg (mtype_t mtype)
    : head_skip_ (default_head_skip),
      mtype_ (mtype)
{
    payload_.resize (head_skip_);
}

Msg::Msg (MsgReader &reader)
    : head_skip_ (default_head_skip),
      mtype_ (MTYPE_IDLE)
{
    int size = reader.size ();
    payload_.resize (head_skip_ - 1 + size);
    reader.read (&payload_.front () + head_skip_ - 1);
    mtype_ = static_cast<mtype_t> (payload_[head_skip_ - 1]);
}

void
Msg::write (MsgWriter &writer)
{
    assert (head_skip_ >= 1);
    payload_[head_skip_ - 1] = static_cast<int> (mtype_);
    writer.write (&payload_.front () + head_skip_ - 1,
                  payload_.size () - head_skip_ + 1);
}

MsgPusher
Msg::push (const char *fmt)
{
    int push_index = payload_.size ();
    payload_.resize (push_index + calcsize (fmt));
    return MsgPusher (payload_.begin () + push_index, fmt);
}

void
Msg::push (const char *buffer, int size)
{
    payload_.insert (payload_.end (), buffer, buffer + size);
}

MsgPoper
Msg::pop (const char *fmt)
{
    int pop_size = calcsize (fmt);
    assert (pop_size <= (int) payload_.size () - head_skip_);
    int pop_index = head_skip_;
    head_skip_ += pop_size;
    return MsgPoper (payload_.begin () + pop_index, fmt);
}

const char *
Msg::pop (int size)
{
    assert (size <= (int) payload_.size () - head_skip_);
    const char *r = &payload_.front () + head_skip_;
    head_skip_ += size;
    return r;
}

MsgPusher
Msg::encapsulate (mtype_t mtype, const char *fmt)
{
    int head_size = calcsize (fmt) + 1;
    assert (head_size <= head_skip_);
    payload_[head_skip_ - 1] = static_cast<int> (mtype_);
    mtype_ = mtype;
    head_skip_ -= head_size;
    return MsgPusher (payload_.begin () + head_skip_, fmt);
}

void
Msg::decapsulate ()
{
    assert (1 <= payload_.size () - head_skip_);
    mtype_ = static_cast<mtype_t> (payload_[head_skip_]);
    head_skip_++;
}

int
Msg::len () const
{
    return payload_.size () - head_skip_;
}

MsgPusher &
MsgPusher::operator<< (int32_t v)
{
    push (v);
    return *this;
}

MsgPusher &
MsgPusher::operator<< (uint32_t v)
{
    push (v);
    return *this;
}

MsgPusher::MsgPusher (std::vector<char>::iterator out, const char *fmt)
    : out_ (out),
      fmt_ (fmt)
{
}

void
MsgPusher::push (int64_t v)
{
    switch (*fmt_)
    {
    case 'l':
        assert (v >= INT32_MIN && v <= INT32_MAX);
        break;
    case 'L':
        assert (v >= 0 && v <= UINT32_MAX);
        break;
    case 'h':
        assert (v >= INT16_MIN && v <= INT16_MAX);
        break;
    case 'H':
        assert (v >= 0 && v <= UINT16_MAX);
        break;
    case 'b':
        assert (v >= INT8_MIN && v <= INT8_MAX);
        break;
    case 'B':
        assert (v >= 0 && v <= UINT8_MAX);
        break;
    default:
        assert (0);
    }
    switch (*fmt_)
    {
    case 'l':
    case 'L':
        *out_++ = v >> 24;
        *out_++ = v >> 16;
    case 'h':
    case 'H':
        *out_++ = v >> 8;
    case 'b':
    case 'B':
        *out_++ = v >> 0;
    }
    fmt_++;
}

MsgPoper &
MsgPoper::operator>> (int32_t &v)
{
    v = pop (31, true);
    return *this;
}

MsgPoper &
MsgPoper::operator>> (uint32_t &v)
{
    v = pop (32, false);
    return *this;
}

MsgPoper &
MsgPoper::operator>> (int16_t &v)
{
    v = pop (15, true);
    return *this;
}

MsgPoper &
MsgPoper::operator>> (uint16_t &v)
{
    v = pop (16, false);
    return *this;
}

MsgPoper &
MsgPoper::operator>> (int8_t &v)
{
    v = pop (7, true);
    return *this;
}

MsgPoper &
MsgPoper::operator>> (uint8_t &v)
{
    v = pop (8, false);
    return *this;
}

MsgPoper::MsgPoper (std::vector<char>::const_iterator in, const char *fmt)
    : in_ (in),
      fmt_ (fmt)
{
}

uint32_t
MsgPoper::pop_bytes (int bytes)
{
    uint32_t r = 0;
    while (bytes--)
        r = r << 8 | static_cast<unsigned char> (*in_++);
    return r;
}

int64_t
MsgPoper::pop (int bits, bool sign)
{
    switch (*fmt_++)
    {
    case 'l':
        assert (bits >= 31 && sign);
        return static_cast<int32_t> (pop_bytes (4));
    case 'L':
        assert (bits >= 32);
        return static_cast<uint32_t> (pop_bytes (4));
    case 'h':
        assert (bits >= 15 && sign);
        return static_cast<int16_t> (pop_bytes (2));
    case 'H':
        assert (bits >= 16);
        return static_cast<uint16_t> (pop_bytes (2));
    case 'b':
        assert (bits >= 7 && sign);
        return static_cast<int8_t> (pop_bytes (1));
    case 'B':
        assert (bits >= 8);
        return static_cast<uint8_t> (pop_bytes (1));
    default:
        assert_unreachable ();
    }
}

} // namespace mex
} // namespace ucoo
