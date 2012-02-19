#include <spy_config.h>
#include <spy_core.h>

spy_buf_t *
spy_create_temp_buf(spy_pool_t *pool, size_t size) {

	spy_buf_t *b;

	b = spy_pcalloc(pool, sizeof(spy_buf_t));
	if (b == NULL) {
		return NULL;
	}

	b->start = spy_palloc(pool, size);
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

#if 0
#ifdef _SPY_BUF_UNIT_TEST_

int main() {
	u_char *p;
	spy_buf_t *buf = spy_create_temp_buf(10);

	p = buf->start;

	*p++ = '1';
	*p++ = '2';
	*p++ = '3';
	*p = '\0';

	spy_log_stdout("%s\n", buf->start);

	return 0;
}

#endif
#endif
