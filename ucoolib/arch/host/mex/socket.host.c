/* imported from mysock library and modified for ucoolib.
 * mysock library - client functions. {{{
 *
 * Copyright (C) 2001 Nicolas Schodet
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
 * Contact :
 *        Web: http://ni.fr.eu.org/
 *
 * }}} */
#include "socket.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

int
socket_client (const char *addr, const char *port)
{
    int s;
    struct sockaddr_in saddr;
    struct servent *port_info;
    struct hostent *host_info;
    unsigned short int iport;
    unsigned int iaddr;
    iport = htons (atoi (port));
    if (!iport)
      {
	port_info = getservbyname (port, "tcp");
	if (!port_info)
	  {
	    fprintf (stderr, "Service %s unknown\n", port);
	    exit (EXIT_FAILURE);
	  }
	iport = port_info->s_port;
      }
    iaddr = inet_addr (addr);
    if (iaddr == INADDR_NONE)
      {
	host_info = gethostbyname (addr);
	if (!host_info)
	  {
	    fprintf (stderr, "Host %s unknown\n", addr);
	    exit (EXIT_FAILURE);
	  }
	iaddr = * (int *) host_info->h_addr;
      }
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = iaddr;
    saddr.sin_port = iport;
    s = socket (AF_INET, SOCK_STREAM, 0);
    if (s < 0)
      {
	perror ("socket");
	exit (EXIT_FAILURE);
      }
    int on = 1;
    if (setsockopt (s, IPPROTO_TCP, TCP_NODELAY, (void *) &on, sizeof (on))
	< 0)
      {
	perror ("setsockopt (NODELAY)");
	exit (EXIT_FAILURE);
      }
    if (connect (s, (struct sockaddr *) &saddr, sizeof (saddr)) < 0)
      {
	perror ("connect");
	exit (EXIT_FAILURE);
      }
    return s;
}
