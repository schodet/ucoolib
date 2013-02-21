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
#include "ucoolib/arch/host/host.hh"
#include "ucoolib/arch/arch.hh"

#include <cstdio>
#include <cstdlib>

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::Host host ("test_host");
    host.add_option ('c', "connect to mex hub");
    host.add_option ('p', "FOO", "print FOO", "Hello world!");
    host.add_option ('s', "STRIP", "print striped instance", "");
    host.parse_options ();
    puts (host.get_option ('p').c_str ());
    if (host.is_option_set ('s'))
    {
        const std::string &strips = host.get_option ('s');
        int strip = atoi (strips.c_str ()); // Quick'n dirty convertion.
        puts (host.get_instance (strip).c_str ());
    }
    if (host.is_option_set ('c'))
        host.get_node ();
    return 0;
}
