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
#include "uart.stm32.hh"

#include "config/hal/uart.hh"

namespace ucoo {

Uart::Uart (int n, int speed, Parity parity, int stop_bits)
{
}

Uart::~Uart ()
{
}

void
Uart::setup (int speed, Parity parity, int stop_bits)
{
}

int
Uart::read (char *buf, int count)
{
    return -1;
}

int
Uart::write (const char *buf, int count)
{
    return -1;
}

int
Uart::poll ()
{
    return 0;
}

} // namespace ucoo
