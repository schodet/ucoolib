/* imported from mysock library and modified for ucoolib.
 * mysock library - client functions. {{{
 *
 * Copyright (C) 2001 Nicolas Schodet
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
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
