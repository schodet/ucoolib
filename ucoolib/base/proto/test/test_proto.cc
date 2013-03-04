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
#include "ucoolib/base/proto/proto.hh"

#include "ucoolib/arch/arch.hh"
#include "ucoolib/base/test/test.hh"

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

