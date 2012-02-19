#ifndef _SPY_CONNECTION_INCLUDE_H_
#define _SPY_CONNECTION_INCLUDE_H_

#include <spy_config.h>
#include <spy_core.h>

struct spy_listening_s {
	spy_socket_t fd;

	spy_log_t *log;
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

	unsigned open :1;
	unsigned listen :1;
	unsigned handshake :1; /* MySQL first init package */
	unsigned remain :1;
};

struct spy_connection_s {
	void *data;
	spy_event_t *read;
	spy_event_t *write;

	spy_socket_t fd;
	spy_recv_pt recv;
	spy_send_pt send;

	size_t addr_text_max_len;
	spy_str_t addr_text;

	off_t sent;

	spy_listening_t *listening;
	spy_connection_t *pool;
	size_t pool_size;

	spy_log_t *log;

	struct sockaddr *sockaddr;
	socklen_t socklen;

	struct sockaddr *local_sockaddr;

	unsigned sndlowat :1;
	unsigned error :1;
	unsigned ignore :1;
	unsigned log_error :3; /* ngx_connection_log_error_e */
};

/*
 spy_listening_t *
 spy_create_listening(void *sockaddr, socklen_t socklen);
 */

void
spy_free_connection(spy_connection_t *c);

spy_connection_t *
spy_get_connection(spy_socket_t s, spy_log_t *log);

spy_int_t spy_open_listening_sockets(spy_global_t *global);

spy_listening_t *
spy_create_listening(spy_global_t *global, void *sockaddr, socklen_t socklen);

spy_int_t
spy_connection_error(spy_connection_t *c, spy_err_t err, char *text);

void spy_close_connection(spy_connection_t *c);

void spy_close_listening_sockets(spy_global_t *global);

#endif
