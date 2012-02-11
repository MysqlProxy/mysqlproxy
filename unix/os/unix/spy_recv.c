#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

ssize_t spy_unix_recv(spy_connection_t *c, u_char *buf, size_t size) {
	ssize_t n;
	spy_err_t err;
	spy_event_t *rev;

	rev = c->read;

	do {
		n = recv(c->fd, buf, size, 0);

		if (n == 0) {
			rev->ready = 0;
			rev->eof = 1;
			return n;

		} else if (n > 0) {

			if ((size_t) n < size) {
				rev->ready = 0;
			}

			return n;
		}

		err = spy_socket_errno;

		if (err == SPY_EAGAIN || err == SPY_EINTR) {
			n = SPY_AGAIN;

		} else {
			n = spy_connection_error(c, err, "recv() failed");
			break;
		}

	} while (err == SPY_EINTR);

	rev->ready = 0;

	if (n == SPY_ERROR) {
		rev->error = 1;
	}

	return n;
}

