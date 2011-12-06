#include <spy_core.h>
#include <spy_config.h>

static spy_log_t spy_log;
static spy_log_file_t spy_log_file;
spy_uint_t spy_use_stderr = 1;

void spy_cdecl
spy_log_stderr(spy_err_t err, const char *fmt, ...) {
	u_char *p, *last;
	va_list args;
	u_char errstr[SPY_MAX_ERROR_STR];

	last = errstr + SPY_MAX_ERROR_STR;
	p = errstr + 5;

	spy_memcpy(errstr, "spy: ", 5);

	va_start(args, fmt);
	p = spy_vslprintf(p, last, fmt, args);
	va_end(args);

	if (err) {
		p = spy_log_errno(p, last, err);
	}

	if (p > (last - SPY_LINEFEED_SIZE)) {
		p = last - SPY_LINEFEED_SIZE;
	}
	*p++ = LF;

	spy_write_fd(spy_stderr, (void *) errstr, (p - errstr));
}

spy_log_t *
spy_log_init(u_char *name) {

	spy_log.file = &spy_log_file;

	// 空名字直接打拼到标准错误输出
	if (spy_strlen(name)) {
		spy_log_file.fd = spy_stderr;
		return &spy_log;
	}

	// 非绝对路径，使用SPY_LOG_PREFIX
	if (name[0] != '/') {

	} else {

	}

	// 打开文件
	spy_log.file->fd
			= spy_open_file(name, SPY_FILE_APPEND | SPY_FILE_CREATE_OR_OPEN,
					SPY_FILE_DEFAULT_ACCESS);
	if (spy_log_file.fd == SPY_INVALID_FILE) {
		spy_log_stderr(spy_errno, "[error] could not open error log file: "
		spy_open_file_n " \"%s\" failed", name);
	}

	return &spy_log;
}

u_char *spy_log_errno(u_char *buf, u_char *last, spy_err_t err) {
	{
		if (buf > last - 50) {

			/* leave a space for an error code */

			buf = last - 50;
			*buf++ = '.';
			*buf++ = '.';
			*buf++ = '.';
		}

		buf = spy_slprintf(buf, last, " (%d: ", err);

		buf = spy_strerror(err, buf, last - buf);

		if (buf < last) {
			*buf++ = ')';
		}

		return buf;
	}

#ifdef _SPY_LOG_UNIT_TEST_

	int main()
	{

		exit(EXIT_SUCCESS);
	}

#endif

