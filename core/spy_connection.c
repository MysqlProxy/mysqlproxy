#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

spy_os_io_t spy_io;

spy_listening_t *
spy_create_listening(spy_global_t *global, void *sockaddr, socklen_t socklen) {

	size_t len;
	spy_listening_t *ls;
	struct sockaddr *sa;
	u_char text[SPY_SOCKADDR_STRLEN];

	// 取一个listen_t
	ls = spy_array_push(&global->listening);
	if (ls == NULL) {
		return NULL;
	}

	spy_memzero(ls, sizeof(spy_listening_t));

	sa = spy_palloc(global->pool, socklen);
	if (sa == NULL) {
		return NULL;
	}

	spy_memcpy(sa, sockaddr, socklen);

	ls->sockaddr = sa;
	ls->socklen = socklen;

	len = spy_sock_ntop(sa, text, SPY_SOCKADDR_STRLEN, 1);
	ls->addr_text.len = len;
	ls->addr_text_max_len = SPY_INET_ADDRSTRLEN;

	ls->addr_text.data = spy_pnalloc(global->pool, len);
	if (ls->addr_text.data == NULL) {
		return NULL;
	}

	spy_memcpy(ls->addr_text.data, text, len);

	ls->fd = (spy_socket_t) -1;
	ls->type = SOCK_STREAM;

	ls->backlog = SPY_LISTEN_BACKLOG;
	ls->rcvbuf = -1;
	ls->sndbuf = -1;

	return ls;

}

spy_int_t spy_open_listening_sockets(spy_global_t *global) {

	int reuseaddr;
	spy_uint_t i, tries, failed;
	spy_err_t err;
	spy_socket_t s;
	spy_listening_t *ls;
	spy_log_t *log;

	reuseaddr = 1;
	log = global->log;

	for (tries = 5; tries; tries--) {
		failed = 0;

		/* for each listening socket */

		ls = global->listening.elts;
		for (i = 0; i < global->listening.nelts; i++) {

			if (ls[i].fd != -1) {
				continue;
			}

			// 建立套接字
			s = spy_socket(ls[i].sockaddr->sa_family, ls[i].type, 0);

			if (s == -1) {
				spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
						spy_socket_n " %V failed", &ls[i].addr_text);
				return SPY_ERROR;
			}

			// 重用端口
			if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
					(const void *) &reuseaddr, sizeof(int)) == -1) {
				spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
						"setsockopt(SO_REUSEADDR) %S failed", &ls[i].addr_text);

				if (spy_close_socket(s) == -1) {
					spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
							spy_close_socket_n, " %S failed", &ls[i].addr_text);
				}

				return SPY_ERROR;
			}

			// 设置套接字非阻塞
			if (spy_nonblocking(s) == -1) {
				spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
						spy_nonblocking_n " %S failed", ls[i].addr_text);

				if (spy_close_socket(s) == -1) {
					spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
							spy_close_socket_n " %S failed", ls[i].addr_text);
				}

				return SPY_ERROR;
			}

			spy_log_debug(SPY_LOG_DEBUG_CORE, log, 0, "bind() %S #%d ",
					&ls[i].addr_text, s);

			// 绑定
			if (bind(s, ls[i].sockaddr, ls[i].socklen) == -1) {
				err = spy_socket_errno;

				if (err == SPY_EADDRINUSE) {
					continue;
				}

				spy_log_error(SPY_LOG_EMERG, log, err, "bind() to %V failed",
						&ls[i].addr_text);

				if (spy_close_socket(s) == -1) {
					spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
							spy_close_socket_n, " %S failed", &ls[i].addr_text);
				}

				if (err != SPY_EADDRINUSE) {
					return SPY_ERROR;
				}

				failed = 1;

				continue;
			}

			// 监听
			if (listen(s, ls[i].backlog) == -1) {
				spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
						"listen() to %V, backlog %d failed", &ls[i].addr_text,
						ls[i].backlog);

				if (spy_close_socket(s) == -1) {
					spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
							spy_close_socket_n, " %S failed", &ls[i].addr_text);
				}

				return SPY_ERROR;
			}

			ls[i].listen = 1;
			ls[i].fd = s;

		}

		if (!failed) {
			break;
		}

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

void spy_close_connection(spy_connection_t *c) {
	spy_err_t err;
	spy_socket_t fd;

	if (c->fd == -1) {
		spy_log_error(SPY_LOG_ALERT, c->log, 0, "connection already closed");
		return;
	}

	/*
	 if (c->read->timer_set) {
	 spy_del_timer(c->read);
	 }

	 if (c->write->timer_set) {
	 spy_del_timer(c->write);
	 }
	 */

	if (c->read->active) {
		spy_del_event(c->read, SPY_READ_EVENT);
	}

	if (c->write->active) {
		spy_del_event(c->write, SPY_WRITE_EVENT);
	}

	/*
	 if (c->read->prev) {
	 spy_delete_posted_event(c->read);
	 }

	 if (c->write->prev) {
	 ngx_delete_posted_event(c->write);
	 }
	 */

	c->read->closed = 1;
	c->write->closed = 1;

	spy_free_connection(c);

	fd = c->fd;
	c->fd = (spy_socket_t) -1;

	if (spy_close_socket(fd) == -1) {

		err = spy_socket_errno;

		/* we use ngx_cycle->log because c->log was in c->pool */

		spy_log_error(SPY_LOG_ERR, spy_global->log, err,
				spy_close_socket_n " %d failed", fd);
	}
}

void spy_close_listening_sockets(spy_global_t *global) {
	spy_uint_t i;
	spy_listening_t *ls;
	spy_connection_t *c;

	ngx_accept_mutex_held = 0;
	ngx_use_accept_mutex = 0;

	ls = global->listening.elts;
	for (i = 0; i < global->listening.nelts; i++) {

		c = ls[i].connection;

		if (c) {

			if (c->read->active) {
				spy_del_event(c->read, SPY_READ_EVENT);
			}

			spy_free_connection(c);

			c->fd = (spy_socket_t) -1;
		}

		spy_log_debug(SPY_LOG_DEBUG_CORE, global->log, 0,
				"close listening %S #%d ", &ls[i].addr_text, ls[i].fd);

		if (spy_close_socket(ls[i].fd) == -1) {
			spy_log_error(SPY_LOG_EMERG, global->log, spy_socket_errno,
					spy_close_socket_n " %S failed", &ls[i].addr_text);
		}

		ls[i].fd = (spy_socket_t) -1;
	}
}

spy_int_t spy_connection_error(spy_connection_t *c, spy_err_t err, char *text) {

#if 0
	spy_uint_t level;
	/* Winsock may return NGX_ECONNABORTED instead of NGX_ECONNRESET */

	if ((err == NGX_ECONNRESET) && c->log_error == SPY_ERROR_IGNORE_ECONNRESET) {
		return 0;
	}

	if (err == 0 || err == SPY_ECONNRESET || err == SPY_EPIPE || err
			== SPY_ENOTCONN || err == SPY_ETIMEDOUT || err == SPY_ECONNREFUSED
			|| err == SPY_ENETDOWN || err == SPY_ENETUNREACH || err
			== SPY_EHOSTDOWN || err == SPY_EHOSTUNREACH) {
		switch (c->log_error) {

			case SPY_ERROR_IGNORE_EINVAL:
			case SPY_ERROR_IGNORE_ECONNRESET:
			case SPY_ERROR_INFO:
			level = SPY_LOG_INFO;
			break;

			default:
			level = SPY_LOG_ERR;
		}

	} else {
		level = SPY_LOG_ALERT;
	}

	ngx_log_error(level, c->log, err, text);
#endif
	return SPY_ERROR;
}
