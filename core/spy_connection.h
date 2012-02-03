#ifndef _SPY_CONNECTION_INCLUDE_H_
#define _SPY_CONNECTION_INCLUDE_H_

#include <spy_config.h>
#include <spy_core.h>

struct spy_listening_s {
	spy_socket_t fd;

	struct sockaddr *sockaddr;
	socklen_t socklen; /* size of sockaddr */

	spy_str_t addr_text;
	size_t addr_text_max_len;
	unsigned addr_ntop :1;
	spy_connection_handler_pt handler;

	spy_connection_t *connection;
	int type;

	int backlog;
	int rcvbuf;
	int sndbuf;

	unsigned listen :1;
	unsigned handshake :1; /* MySQL first init package */
};

struct spy_connection_s {
	void *data;
	spy_event_t *read;
	spy_event_t *write;

	spy_socket_t fd;
	size_t addr_text_max_len;
	spy_str_t addr_text;

	spy_listening_t *listening;

	off_t sent;

	spy_log_t *log;

	struct sockaddr *sockaddr;
	socklen_t socklen;

	struct sockaddr *local_sockaddr;
};

/*
 spy_listening_t *
 spy_create_listening(void *sockaddr, socklen_t socklen);
 */

void
spy_free_connection(spy_connection_t *c);

spy_connection_t *
spy_get_connection(spy_socket_t s, spy_log_t *log);

spy_int_t
spy_open_listening_sockets(spy_global_t *proxy);

spy_listening_t *
spy_create_listening(void *sockaddr, socklen_t socklen, size_t index);

#endif
