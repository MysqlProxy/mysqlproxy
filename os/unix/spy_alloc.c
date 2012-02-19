#include <spy_config.h>
#include <spy_core.h>

spy_uint_t spy_pagesize;

void *
spy_alloc(size_t size, spy_log_t *log) {
	void *p;

	p = malloc(size);
	if (p == NULL) {
		spy_log_error(SPY_LOG_EMERG, log, spy_errno, "malloc(%uz) failed", size);
	}

	spy_log_debug(SPY_LOG_DEBUG_ALLOC, log, 0, "malloc: %p:%uz", p, size);

	return p;
}

void *
spy_calloc(size_t size, spy_log_t *log) {
	void *p;

	p = spy_alloc(size, log);

	if (p) {
		spy_memzero(p, size);
	}

	return p;
}

void *
spy_memalign(size_t alignment, size_t size, spy_log_t *log) {
	void *p;
	int err;

	err = posix_memalign(&p, alignment, size);

	if (err) {
		spy_log_error(SPY_LOG_EMERG, log, err,
				"posix_memalign(%uz, %uz) failed", alignment, size);
		p = NULL;
	}

	spy_log_debug(SPY_LOG_DEBUG_ALLOC, log, 0, "posix_memalign: %p:%uz @%uz",
			p, size, alignment);

	return p;
}
