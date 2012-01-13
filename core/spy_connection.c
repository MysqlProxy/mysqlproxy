#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

spy_listening_t *
spy_create_listening(void *sockaddr, socklen_t socklen) {

	size_t len;
	spy_listening_t *ls;
	struct sockaddr *sa;
	u_char text[SPY_SOCKADDR_STRLEN];

	ls = spy_global->listening;
	sa = malloc(sizeof(socklen));
	spy_memcpy(sa, sockaddr, socklen);
	spy_memzero(ls, sizeof(spy_listening_t));

	ls->sockaddr = sa;
	ls->socklen = socklen;
	ls->fd = (spy_socket_t) -1;
	ls->type = SOCK_STREAM;
	ls->backlog = SPY_LISTEN_BACKLOG;

	len = spy_sock_ntop(sa, text, SPY_SOCKADDR_STRLEN, 1);
	ls->addr_text.len = len;

	ls->addr_text.data = malloc(len);
	if (ls->addr_text.data == NULL) {
		return NULL;
	}

	spy_memcpy(ls->addr_text.data, text, len);

	ls->rcvbuf = -1;
	ls->sndbuf = -1;

	return ls;

}

spy_int_t spy_open_listening_sockets(spy_global_t *proxy) {

	int reuseaddr;
	spy_uint_t i, tries, failed;
	spy_err_t err;
	spy_socket_t s;
	spy_listening_t *ls;
	spy_log_t *log;

	reuseaddr = 1;
	log = proxy->log;

	for (tries = 5; tries; tries--) {
		failed = 0;

		/* for each listening socket */

		ls = proxy->listening;
		for (i = 0; i < proxy->listening_n; i++) {

			if (ls[i].fd != -1) {
				continue;
			}

			s = spy_socket(ls[i].sockaddr->sa_family, ls[i].type, 0);

			if (s == -1) {
				spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
						spy_socket_n " %V failed", &ls[i].addr_text);
				return SPY_ERROR;
			}

			if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
					(const void *) &reuseaddr, sizeof(int)) == -1) {
				spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
						"setsockopt(SO_REUSEADDR) %V failed", &ls[i].addr_text);

				if (spy_close_socket(s) == -1) {
					spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
							spy_close_socket_n, " %V failed", &ls[i].addr_text);
				}

				return SPY_ERROR;
			}

			spy_log_debug(SPY_LOG_DEBUG_CORE, log, 0, "bind() %V #%d ",
					&ls[i].addr_text, s);

			if (bind(s, ls[i].sockaddr, ls[i].socklen) == -1) {
				err = spy_socket_errno;

				if (err == SPY_EADDRINUSE) {
					continue;
				}

				spy_log_error(SPY_LOG_EMERG, log, err, "bind() to %V failed",
						&ls[i].addr_text);

				if (spy_close_socket(s) == -1) {
					spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
							spy_close_socket_n, " %V failed", &ls[i].addr_text);
				}

				if (err != SPY_EADDRINUSE) {
					return SPY_ERROR;
				}

				failed = 1;

				continue;
			}

			if (listen(s, ls[i].backlog) == -1) {
				spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
						"listen() to %V, backlog %d failed", &ls[i].addr_text,
						ls[i].backlog);

				if (spy_close_socket(s) == -1) {
					spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
							spy_close_socket_n, " %V failed", &ls[i].addr_text);
				}

				return SPY_ERROR;
			}

			ls[i].listen = 1;

			ls[i].fd = s;
		}

		if (!failed) {
			break;
		}

		/* TODO: delay configurable */

		spy_log_error(SPY_LOG_NOTICE, log, 0, "try again to bind() after 500ms");

		spy_msleep(500);
	}

	if (failed) {
		spy_log_error(SPY_LOG_EMERG, log, 0, "still could not bind()");
		return SPY_ERROR;
	}

	return SPY_OK;
}

void spy_free_connection(spy_connection_t *c) {

	// 把当前data指向free的connection
	c->data = spy_global->free_connections;
	// free_connections 指向c
	spy_global->free_connections = c;
	// 空闲计数+1
	spy_global->free_connection_n++;

}

spy_connection_t *
spy_get_connection(spy_socket_t s, spy_log_t *log) {
	spy_event_t *rev, *wev;
	spy_connection_t *c;

	// 先获取空闲slot的connection
	c = spy_global->free_connections;

	// 还没有就报错
	if (c == NULL) {
		spy_log_error(SPY_LOG_ALERT, log, 0,
				"%ui worker_connections are not enough",
				spy_global->connection_n);

		return NULL;
	}

	// 赋值给data
	spy_global->free_connections = c->data;

	// 可用connection总数-1
	spy_global->free_connection_n--;

	// 保存 之前的读写事件
	rev = c->read;
	wev = c->write;

	// 清空connection
	spy_memzero(c, sizeof(spy_connection_t));

	// 赋值读写事件，fd信息，log
	c->read = rev;
	c->write = wev;
	c->fd = s;
	c->log = log;

	spy_memzero(rev, sizeof(spy_event_t));
	spy_memzero(wev, sizeof(spy_event_t));

	rev->index = SPY_INVALID_INDEX;
	wev->index = SPY_INVALID_INDEX;

	// 保存connection到事件data中
	rev->data = c;
	wev->data = c;

	// 事件可写
	wev->write = 1;

	return c;
}

