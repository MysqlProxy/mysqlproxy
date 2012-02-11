#ifndef _SPY_OS_H_INCLUDED_
#define _SPY_OS_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

#define SPY_IO_SENDFILE    1

spy_int_t spy_os_specific_init(spy_log_t *log);

typedef ssize_t (*spy_recv_pt)(spy_connection_t *c, u_char *buf, size_t size);
typedef ssize_t (*spy_send_pt)(spy_connection_t *c, u_char *buf, size_t size);

ssize_t spy_unix_recv(spy_connection_t *c, u_char *buf, size_t size);
ssize_t spy_unix_send(spy_connection_t *c, u_char *buf, size_t size);

typedef struct {
	spy_recv_pt recv;
	spy_send_pt send;
	spy_uint_t flags;
} spy_os_io_t;

extern spy_os_io_t spy_os_io;

#endif
