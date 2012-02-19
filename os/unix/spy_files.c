#include <spy_config.h>
#include <spy_core.h>

ssize_t spy_read_file(spy_file_t *file, u_char *buf, size_t size, off_t offset) {
	ssize_t n;

	spy_log_debug(SPY_LOG_DEBUG_CORE, file->log, 0, "read: %d, %p, %uz, %O",
			file->fd, buf, size, offset);

	if (file->sys_offset != offset) {
		if (lseek(file->fd, offset, SEEK_SET) == -1) {
			spy_log_error(SPY_LOG_CRIT, file->log, spy_errno,
					"lseek() \"%s\" failed", file->name.data);
			return SPY_ERROR;
		}

		file->sys_offset = offset;
	}

	n = read(file->fd, buf, size);

	if (n == -1) {
		spy_log_error(SPY_LOG_CRIT, file->log, spy_errno,
				"read() \"%s\" failed", file->name.data);
		return SPY_ERROR;
	}

	file->sys_offset += n;

	file->offset += n;

	return n;
}

ssize_t spy_write_file(spy_file_t *file, u_char *buf, size_t size, off_t offset) {
	ssize_t n, written;

	spy_log_debug(SPY_LOG_DEBUG_CORE, file->log, 0, "write: %d, %p, %uz, %O",
			file->fd, buf, size, offset);

	written = 0;

	if (file->sys_offset != offset) {
		if (lseek(file->fd, offset, SEEK_SET) == -1) {
			spy_log_error(SPY_LOG_CRIT, file->log, spy_errno,
					"lseek() \"%s\" failed", file->name.data);
			return SPY_ERROR;
		}

		file->sys_offset = offset;
	}

	for (;;) {
		n = write(file->fd, buf + written, size);

		if (n == -1) {
			spy_log_error(SPY_LOG_CRIT, file->log, spy_errno,
					"write() \"%s\" failed", file->name.data);
			return SPY_ERROR;
		}

		file->offset += n;
		written += n;

		if ((size_t) n == size) {
			return written;
		}

		size -= n;
	}
}
