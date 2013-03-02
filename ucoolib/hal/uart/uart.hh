#ifndef ucoolib_hal_uart_uart_hh
#define ucoolib_hal_uart_uart_hh
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

#if defined (TARGET_host)
# include "ucoolib/arch/host/host_stream.hh"
namespace ucoo { typedef HostStream Uart; }
#elif defined (TARGET_stm32)
# include "uart.stm32.hh"
#else
# error "not implemented for this target"
#endif

#endif // ucoolib_hal_uart_uart_hh
