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
			p = spy_snprintf(text, len, "%ud.%ud.%ud.%ud:%d", p[0], p[1], p[2],
					p[3], ntohs(sin->sin_port));
		} else {
			p = spy_snprintf(text, len, "%ud.%ud.%ud.%ud", p[0], p[1], p[2],
					p[3]);
		}

		return (p - text);

	default:
		return 0;
	}
}
