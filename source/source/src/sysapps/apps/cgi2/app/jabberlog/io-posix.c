/* iksemel (XML parser for Jabber)
** Copyright (C) 2004 Gurer Ozen <madcat@e-kolay.net>
** This code is free software; you can redistribute it and/or
** modify it under the terms of GNU Lesser General Public License.
*/

#include "common.h"
#include "iksemel.h"

#ifdef _WIN32
#include <winsock.h>
#else
#include <netdb.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

static void
io_close (void *socket)
{
	int sock = (int) socket;
#ifdef _WIN32
	closesocket (sock);
#else
	close (sock);
#endif
}

static int
io_connect (iksparser *prs, void **socketptr, const char *server, int port)
{
	int sock = -1;
	int tmp;
#if SC_SSL_PROXY == 1
	char proxy_server[]="127.0.0.1";
	server = proxy_server;
#endif
#ifdef HAVE_GETADDRINFO
	struct addrinfo hints;
	struct addrinfo *addr_res, *addr_ptr;
	char port_str[6];

	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_addrlen = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	sprintf (port_str, "%i", port);

	if (getaddrinfo (server, port_str, &hints, &addr_res) != 0)
		return IKS_NET_NODNS;

	addr_ptr = addr_res;
	while (addr_ptr) {
		sock = socket (addr_ptr->ai_family, addr_ptr->ai_socktype, addr_ptr->ai_protocol);
		if (sock != -1) break;
		addr_ptr = addr_ptr->ai_next;
	}
	if (sock == -1) return IKS_NET_NOSOCK;

	tmp = connect (sock, addr_ptr->ai_addr, addr_ptr->ai_addrlen);
	freeaddrinfo (addr_res);
#else
	struct hostent *host;
	struct sockaddr_in sin;

	host = gethostbyname (server);
	if (!host) return IKS_NET_NODNS;

	memcpy (&sin.sin_addr, host->h_addr, host->h_length);
	sin.sin_family = host->h_addrtype;
	sin.sin_port = htons (port);
	sock = socket (host->h_addrtype, SOCK_STREAM, 0);
	if (sock == -1) return IKS_NET_NOSOCK;

	tmp = connect (sock, (struct sockaddr *)&sin, sizeof (struct sockaddr_in));
#endif
	if (tmp != 0) {
		io_close ((void *) sock);
		return IKS_NET_NOCONN;
	}

	*socketptr = (void *) sock;

	return IKS_OK;
}

static int
io_send (void *socket, const char *data, size_t len)
{
	int sock = (int) socket;

	if (send (sock, data, len, 0) == -1) return IKS_NET_RWERR;
	return IKS_OK;
}

static int
io_recv (void *socket, char *buffer, size_t buf_len, int timeout)
{
	int sock = (int) socket;
	fd_set fds;
	struct timeval tv, *tvptr;
	int len;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO (&fds);
	FD_SET (sock, &fds);
	tv.tv_sec = timeout;
	if (timeout != -1) tvptr = &tv; else tvptr = NULL;
	if (select (sock + 1, &fds, NULL, NULL, tvptr) > 0) {
		len = recv (sock, buffer, buf_len, 0);
		if (len > 0) {
			return len;
		} else if (len <= 0) {
			return -1;
		}
	}
	return 0;
}

ikstransport iks_default_transport = {
	IKS_TRANSPORT_V1,
	io_connect,
	io_send,
	io_recv,
	io_close,
	NULL
};
