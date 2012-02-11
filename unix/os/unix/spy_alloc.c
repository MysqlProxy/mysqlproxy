#include <spy_config.h>
#include <spy_core.h>

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
