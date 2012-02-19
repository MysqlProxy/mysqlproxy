#include <spy_config.h>
#include <spy_core.h>

static void *spy_palloc_block(spy_pool_t *pool, size_t size);
static void *spy_palloc_large(spy_pool_t *pool, size_t size);

spy_pool_t *
spy_create_pool(size_t size, spy_log_t *log) {
	spy_pool_t *p;

	p = spy_memalign(SPY_POOL_ALIGNMENT, size, log);
	if (p == NULL) {
		return NULL;
	}

	p->d.last = (u_char *) p + sizeof(spy_pool_t);
	p->d.end = (u_char *) p + size;
	p->d.next = NULL;
	p->d.failed = 0;

	size = size - sizeof(spy_pool_t);
	p->max = (size < SPY_MAX_ALLOC_FROM_POOL) ? size : SPY_MAX_ALLOC_FROM_POOL;

	p->current = p;
	p->large = NULL;
	p->log = log;

	return p;
}

void spy_destroy_pool(spy_pool_t *pool) {
	spy_pool_t *p, *n;
	spy_pool_large_t *l;

	for (l = pool->large; l; l = l->next) {

		spy_log_debug(SPY_LOG_DEBUG_ALLOC, pool->log, 0, "free: %p", l->alloc);

		if (l->alloc) {
			spy_free(l->alloc);
		}
	}

	for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
		spy_free(p);

		if (n == NULL) {
			break;
		}
	}
}

void *
spy_palloc(spy_pool_t *pool, size_t size) {
	u_char *m;
	spy_pool_t *p;

	if (size <= pool->max) {

		p = pool->current;

		do {
			m = spy_align_ptr(p->d.last, SPY_ALIGNMENT);

			if ((size_t) (p->d.end - m) >= size) {
				p->d.last = m + size;

				return m;
			}

			p = p->d.next;

		} while (p);

		return spy_palloc_block(pool, size);
	}

	return spy_palloc_large(pool, size);
}

void *
spy_pnalloc(spy_pool_t *pool, size_t size) {
	u_char *m;
	spy_pool_t *p;

	if (size <= pool->max) {

		p = pool->current;

		do {
			m = p->d.last;

			if ((size_t) (p->d.end - m) >= size) {
				p->d.last = m + size;

				return m;
			}

			p = p->d.next;

		} while (p);

		return spy_palloc_block(pool, size);
	}

	return spy_palloc_large(pool, size);
}

static void *
spy_palloc_block(spy_pool_t *pool, size_t size) {
	u_char *m;
	size_t psize;
	spy_pool_t *p, *new, *current;

	psize = (size_t) (pool->d.end - (u_char *) pool);

	m = spy_memalign(SPY_POOL_ALIGNMENT, psize, pool->log);
	if (m == NULL) {
		return NULL;
	}

	new = (spy_pool_t *) m;

	new->d.end = m + psize;
	new->d.next = NULL;
	new->d.failed = 0;

	m += sizeof(spy_pool_data_t);
	m = spy_align_ptr(m, SPY_ALIGNMENT);
	new->d.last = m + size;

	current = pool->current;

	for (p = current; p->d.next; p = p->d.next) {
		if (p->d.failed++ > 4) {
			current = p->d.next;
		}
	}

	p->d.next = new;

	pool->current = current ? current : new;

	return m;
}

static void *
spy_palloc_large(spy_pool_t *pool, size_t size) {
	void *p;
	spy_uint_t n;
	spy_pool_large_t *large;

	p = spy_alloc(size, pool->log);
	if (p == NULL) {
		return NULL;
	}

	n = 0;

	for (large = pool->large; large; large = large->next) {
		if (large->alloc == NULL) {
			large->alloc = p;
			return p;
		}

		if (n++ > 3) {
			break;
		}
	}

	large = spy_palloc(pool, sizeof(spy_pool_large_t));
	if (large == NULL) {
		spy_free(p);
		return NULL;
	}

	large->alloc = p;
	large->next = pool->large;
	pool->large = large;

	return p;
}

void *
spy_pmemalign(spy_pool_t *pool, size_t size, size_t alignment) {
	void *p;
	spy_pool_large_t *large;

	p = spy_memalign(alignment, size, pool->log);
	if (p == NULL) {
		return NULL;
	}

	large = spy_palloc(pool, sizeof(spy_pool_large_t));
	if (large == NULL) {
		spy_free(p);
		return NULL;
	}

	large->alloc = p;
	large->next = pool->large;
	pool->large = large;

	return p;
}

spy_int_t spy_pfree(spy_pool_t *pool, void *p) {
	spy_pool_large_t *l;

	for (l = pool->large; l; l = l->next) {
		if (p == l->alloc) {
			spy_log_debug(SPY_LOG_DEBUG_ALLOC, pool->log, 0, "free: %p",
					l->alloc);
			spy_free(l->alloc);
			l->alloc = NULL;

			return SPY_OK;
		}
	}

	return SPY_DECLINED;
}

void *
spy_pcalloc(spy_pool_t *pool, size_t size) {
	void *p;

	p = spy_palloc(pool, size);
	if (p) {
		spy_memzero(p, size);
	}

	return p;
}
