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
#include "proto.hh"

#include <cctype>

namespace ucoo {

Proto::Proto (Handler &handler, Stream &stream)
    : handler_ (handler), stream_ (stream), step_ (IDLE)
{
}

void
Proto::accept ()
{
    int c;
    while ((c = stream_.getc ()) != -1)
    {
        if (c == '!')
            step_ = BANG;
        else
        {
            switch (step_)
            {
            case IDLE:
                // Nothing received yet.
                break;
            case BANG:
                // Bang received yet.
                if (std::isalpha (c))
                {
                    cmd_ = c;
                    size_ = 0;
                    step_ = COMMAND;
                }
                else
                {
                    handler_.proto_handle (*this, '?', 0, 0);
                    step_ = IDLE;
                }
                break;
            case COMMAND:
                // Command received yet.
                if (c == '\r' || c == '\n')
                {
                    handler_.proto_handle (*this, cmd_, args_, size_);
                    step_ = IDLE;
                }
                else if (c == '\'')
                    step_ = ARG_CHAR;
                else if (c == '"')
                    step_ = ARG_STRING;
                else
                {
                    step_ = ARG_DIGIT;
                    accept_digit (c);
                }
                break;
            case ARG_DIGIT:
                step_ = COMMAND;
                accept_digit (c);
                break;
            case ARG_CHAR:
                step_ = COMMAND;
                accept_char (c);
                break;
            case ARG_STRING:
                if (c == '\r' || c == '\n')
                {
                    handler_.proto_handle (*this, cmd_, args_, size_);
                    step_ = IDLE;
                }
                else
                {
                    accept_char (c);
                }
                break;
            }
        }
    }
}

void
Proto::send (char cmd)
{
    char buf[3];
    char *p = buf;
    *p++ = '!';
    *p++ = cmd;
    *p++ = '\n';
    stream_.write (buf, p - buf);
}

void
Proto::send (char cmd, const char *fmt, int a0)
{
    send (cmd, fmt, a0, 0, 0, 0);
}

void
Proto::send (char cmd, const char *fmt, int a0, int a1)
{
    send (cmd, fmt, a0, a1, 0, 0);
}

void
Proto::send (char cmd, const char *fmt, int a0, int a1, int a2)
{
    send (cmd, fmt, a0, a1, a2, 0);
}

static inline void
send_byte (char *buf, uint8_t b)
{
    static const char hexchars[] = "0123456789abcdef";
    buf[0] = hexchars[b >> 4];
    buf[1] = hexchars[b & 0xf];
}

static char *
send_arg (char *buf, char fmt, int v)
{
    switch (fmt)
    {
    case 'L':
    case 'l':
        send_byte (buf, (v >> 24) & 0xff);
        buf += 2;
        send_byte (buf, (v >> 16) & 0xff);
        buf += 2;
    case 'H':
    case 'h':
        send_byte (buf, (v >> 8) & 0xff);
        buf += 2;
    case 'B':
    case 'b':
        send_byte (buf, v & 0xff);
        buf += 2;
        break;
    default:
        assert_unreachable ();
    }
    return buf;
}

void
Proto::send (char cmd, const char *fmt, int a0, int a1, int a2, int a3)
{
    char buf[3 + 4 * 4 * 2]; // Large enough for the largest frame.
    char *p = buf;
    *p++ = '!';
    *p++ = cmd;
    if (*fmt)
    {
        p = send_arg (p, *fmt++, a0);
        if (*fmt)
        {
            p = send_arg (p, *fmt++, a1);
            if (*fmt)
            {
                p = send_arg (p, *fmt++, a2);
                if (*fmt)
                {
                    p = send_arg (p, *fmt++, a3);
                }
            }
        }
    }
    *p++ = '\n';
    stream_.write (buf, p - buf);
}

void
Proto::send_buf (char cmd, const uint8_t *args, int size)
{
    int i;
    char buf[3 + 2 * size];
    char *p = buf;
    *p++ = '!';
    *p++ = cmd;
    for (i = 0; i < size; i++)
    {
        send_byte (p, args[i]);
        p += 2;
    }
    *p++ = '\n';
    stream_.write (buf, p - buf);
}

void
Proto::accept_digit (int c)
{
    // Test for argument list overflow.
    if (size_ >= UCOO_CONFIG_BASE_PROTO_ARGS_MAX_SIZE)
      {
	handler_.proto_handle (*this, '?', 0, 0);
	step_ = IDLE;
	return;
      }
    // Convert from hexa.
    if ('0' <= c && c <= '9')
	c -= '0';
    else if ('a' <= c && c <= 'f')
	c -= 'a' - 10;
    else if ('A' <= c && c <= 'F')
	c -= 'A' - 10;
    else
      {
	handler_.proto_handle (*this, '?', 0, 0);
	step_ = IDLE;
	return;
      }
    // Add digit.
    args_[size_] <<= 4;
    args_[size_] |= c;
    if (step_ == COMMAND)
	size_++;
}

void
Proto::accept_char (int c)
{
    // Test for argument list overflow or unwanted char.
    if (size_ >= UCOO_CONFIG_BASE_PROTO_ARGS_MAX_SIZE || !std::isprint (c))
      {
	handler_.proto_handle (*this, '?', 0, 0);
	step_ = IDLE;
	return;
      }
    // Add char.
    args_[size_] = c;
    size_++;
}

} // namespace ucoo
