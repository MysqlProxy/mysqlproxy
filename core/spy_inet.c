#include <spy_config.h>
#include <spy_core.h>

size_t spy_inet_ntop(int family, void *addr, u_char *text, size_t len) {
	u_char *p;

	switch (family) {

	case AF_INET:

		p = addr;

		return spy_snprintf(text, len, "%u.%u.%u.%u", p[0], p[1], p[2], p[3])
				- text;

	default:
		return 0;
	}
}

size_t spy_sock_ntop(struct sockaddr *sa, u_char *text, size_t len,
		spy_uint_t port) {
	u_char *p;
	struct sockaddr_in *sin;

	switch (sa->sa_family) {

	case AF_INET:

		sin = (struct sockaddr_in *) sa;
		p = (u_char *) &sin->sin_addr;

		if (port) {
			p = spy_snprintf(text, len, "%u.%u.%u.%u:%d", p[0], p[1], p[2],
					p[3], ntohs(sin->sin_port));
		} else {
			p = spy_snprintf(text, len, "%u.%u.%u.%u", p[0], p[1], p[2], p[3]);
		}

		return (p - text);

	default:
		return 0;
	}
}

spy_int_t spy_get_endian() {
	union {
		short s;
		char c[sizeof(short)];
	} un;

	un.s = 0x0102;

	if (2 == sizeof(short)) {
		if (1 == un.c[0] && 2 == un.c[1]) {
			return 0;
		} else if (2 == un.c[0] && 1 == un.c[1]) {
			return 1;
		} else {
			return -1;
		}
	}
	return 0;
}
