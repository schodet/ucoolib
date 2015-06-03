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
#include "ucoo/base/proto/proto.hh"

#include "ucoo/arch/arch.hh"
#include "ucoo/base/test/test.hh"

class TestProto : public ucoo::Proto::Handler
{
  public:
    void proto_handle (ucoo::Proto &proto, char cmd,
                       const uint8_t *args, int size)
    {
#define c(cmd, size) ((cmd) << 8 | (size))
        switch (c (cmd, size))
        {
            // Receive various format.
        case c ('a', 1):
        case c ('a', 4):
        case c ('b', 8):
            // Send various formats.
            proto.send ('h', "hh", -1, 1);
            proto.send ('e', "B", 0xf0);
            proto.send ('l', "HBB", 0x1234, 0x56, 0x78);
            proto.send ('o', "L", 0x87654321u);
            break;
        default:
            // Error, unknown command.
            proto.send ('?');
            return;
        }
        proto.send_buf (cmd, args, size);
    }
};

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::Stream &s = ucoo::test_stream ();
    TestProto test_proto;
    ucoo::Proto proto (test_proto, s);
    while (1)
        proto.accept ();
}

