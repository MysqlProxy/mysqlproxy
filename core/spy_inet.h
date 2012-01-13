#ifndef _SPY_INET_H_INCLUDED_
#define _SPY_INET_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

#define SPY_INET_ADDRSTRLEN   (sizeof("255.255.255.255") - 1)
#define SPY_SOCKADDR_STRLEN   (SPY_INET_ADDRSTRLEN + sizeof(":65535") - 1)

size_t spy_inet_ntop(int family, void *addr, u_char *text, size_t len);

size_t spy_sock_ntop(struct sockaddr *sa, u_char *text, size_t len,
		spy_uint_t port);

#endif
