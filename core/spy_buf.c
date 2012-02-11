#include <spy_config.h>
#include <spy_core.h>

spy_buf_t *
spy_create_temp_buf(size_t size) {
	spy_buf_t *b;

	b = spy_alloc(sizeof(spy_buf_t *), spy_global->log);
	if (b == NULL) {
		return NULL;
	}

	b->start = spy_alloc(size, spy_global->log);
	if (b->start == NULL) {
		return NULL;
	}

	/*
	 * set by ngx_calloc_buf():
	 *
	 *     b->file_pos = 0;
	 *     b->file_last = 0;
	 *     b->file = NULL;
	 *     b->shadow = NULL;
	 *     b->tag = 0;
	 *     and flags
	 */

	b->pos = b->start;
	b->last = b->start;
	b->end = b->last + size;
	b->temporary = 1;

	return b;
}
