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
#include "stream.hh"

namespace ucoo {

Stream::Stream ()
    : block_ (true)
{
}

void
Stream::block (bool block/*true*/)
{
    block_ = block;
}

int
Stream::getc ()
{
    int r;
    char c;
    r = read (&c, 1);
    if (r == 1)
        return static_cast<unsigned char> (c);
    else
        return -1;
}

int
Stream::putc (int c)
{
    int r;
    char b = c;
    r = write (&b, 1);
    if (r == 1)
        return 0;
    else
        return -1;
}

} // namespace ucoo
