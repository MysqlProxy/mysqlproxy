#include <spy_config.h>
#include <spy_core.h>


static spy_str_t *spy_sys_errlist;
static spy_str_t spy_unknown_error = spy_string("Unknown error");

u_char *
spy_strerror(spy_err_t err, u_char *errstr, size_t size) {
	spy_str_t *msg;

	msg = ((spy_uint_t) err < SPY_SYS_NERR) ? &spy_sys_errlist[err]
			: &spy_unknown_error; // not modify
	size = spy_min(size, msg->len);

	return spy_cpymem(errstr, msg->data, size);
}

spy_uint_t spy_strerror_init(void) {
	char *msg;
	u_char *p;
	size_t len;
	spy_err_t err;

	/*
	 * ngx_strerror() is not ready to work at this stage, therefore,
	 * malloc() is used and possible errors are logged using strerror().
	 */

	len = SPY_SYS_NERR * sizeof(spy_str_t);

	spy_sys_errlist = malloc(len);
	if (spy_sys_errlist == NULL) {
		goto failed;
	}

	for (err = 0; err < SPY_SYS_NERR; err++) {
		msg = strerror(err);
		len = spy_strlen(msg);

		p = malloc(len);
		if (p == NULL) {
			goto failed;
		}

		spy_memcpy(p, msg, len);
		spy_sys_errlist[err].len = len;
		spy_sys_errlist[err].data = p;
	}

	return SPY_OK;

	failed:

	err = spy_errno;
	spy_log_stderr(0, "malloc(%uz) failed (%d: %s)", len, err, strerror(err));

	return SPY_ERROR;
}
