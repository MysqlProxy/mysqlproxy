#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

ssize_t spy_unix_send(spy_connection_t *c, u_char *buf, size_t size) {
	ssize_t n;
	spy_err_t err;
	spy_event_t *wev;

	wev = c->write;

	for (;;) {
		n = send(c->fd, buf, size, 0);

		if (n > 0) {
			if (n < (ssize_t) size) {
				wev->ready = 0;
			}

			c->sent += n;

			return n;
		}

		err = spy_socket_errno;

		if (n == 0) {
			spy_log_error(SPY_LOG_ALERT, c->log, err, "%s",
					"send() returned zero");
			wev->ready = 0;
			return n;
		}

		if (err == SPY_EAGAIN || err == SPY_EINTR) {
			wev->ready = 0;

			if (err == SPY_EAGAIN) {
				return SPY_AGAIN;
			}

		} else {
			wev->error = 1;
			(void) spy_connection_error(c, err, "send() failed");
			return SPY_ERROR;
		}
	}
}
