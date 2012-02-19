#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

static void spy_close_accepted_connection(spy_connection_t *c);

void spy_event_accept(spy_event_t *ev) {

	socklen_t socklen;
	spy_err_t err;
	spy_socket_t s;
	spy_event_t *rev, *wev;
	spy_listening_t *ls;
	spy_connection_t *c, *lc;
	u_char sa[SPY_SOCKADDRLEN];

	lc = ev->data;
	ls = lc->listening;

	spy_log_debug(SPY_LOG_DEBUG_EVENT, ev->log, 0, "accept on %S, ready: %d",
			&ls->addr_text, ev->available);

	do {
		socklen = SPY_SOCKADDRLEN;

		s = accept(lc->fd, (struct sockaddr *) sa, &socklen);

		if (s == -1) {
			err = spy_socket_errno;

			if (err == SPY_EAGAIN) {
				spy_log_debug(SPY_LOG_DEBUG_EVENT, ev->log, err, "%s",
						"accept() not ready");
				return;
			}

			spy_log_error((spy_uint_t) ((err == SPY_ECONNABORTED) ? SPY_LOG_ERR
					: SPY_LOG_ALERT), ev->log, err, "accept() failed");

			if (err == SPY_ECONNABORTED) {

				if (ev->available) {
					continue;
				}
			}

			return;
		}

		c = spy_get_connection(s, ev->log);

		if (c == NULL) {
			if (spy_close_socket(s) == -1) {
				spy_log_error(SPY_LOG_ALERT, ev->log, spy_socket_errno,
						spy_close_socket_n " failed");
			}

			return;
		}

		c->pool = spy_create_pool(ls->pool_size, ev->log);
		if (c->pool == NULL) {
			spy_close_accepted_connection(c);
			return;
		}

		c->sockaddr = ngx_palloc(c->pool, socklen);
		if (c->sockaddr == NULL) {
			spy_close_accepted_connection(c);
			return;
		}

		spy_memcpy(c->sockaddr, sa, socklen);

		c->recv = spy_recv;
		c->send = spy_send;
		c->socklen = socklen;
		c->listening = ls;

		rev = c->read;
		wev = c->write;
		wev->ready = 1;

		rev->log = ev->log;
		wev->log = ev->log;

		if (ls->addr_ntop) {
			c->addr_text.data = spy_palloc(c->pool, ls->addr_text_max_len);
			if (c->addr_text.data == NULL) {
				spy_close_accepted_connection(c);
				return;
			}

			c->addr_text.len = spy_sock_ntop(c->sockaddr, c->addr_text.data,
					ls->addr_text_max_len, 0);
			if (c->addr_text.len == 0) {
				spy_close_accepted_connection(c);
				return;
			}
		}

		spy_log_debug(SPY_LOG_DEBUG_EVENT, log, 0, "*%d accept: %S fd:%d",
				c->number, &c->addr_text, s);

		ls->handler(c);

		ev->available = 0;
	} while (ev->available);
}

void spy_close_accepted_connection(spy_connection_t *c) {
	spy_socket_t fd;

	spy_free_connection(c);

	fd = c->fd;
	c->fd = (spy_socket_t) -1;

	if (spy_close_socket(fd) == -1) {
		spy_log_error(SPY_LOG_ALERT, c->log, spy_socket_errno,
				spy_close_socket_n " failed");
	}
}
