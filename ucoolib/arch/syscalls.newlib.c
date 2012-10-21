/* ucoolib - Microcontroller object oriented library. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
 *
 * APBTeam:
 *        Web: http://apbteam.org/
 *      Email: team AT apbteam DOT org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * }}} */

/** This is needed by C++ ABI, this simple definition will do.  See:
 * http://lists.debian.org/debian-gcc/2003/07/msg00057.html */
void *__dso_handle = (void*) &__dso_handle;

/** This function is called when a pure virtual function is called.  This is
 * needed by linker because when a abstrat class constructor or destructor is
 * called, object is not complete.  Replace the one provided by the toolchain
 * to avoid including the world. */
void
__cxa_pure_virtual (void)
{
    while (1)
	;
}
