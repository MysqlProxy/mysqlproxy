/*
 * Copyright (C) Igor Sysoev
 */

#ifndef _SPY_ARRAY_H_INCLUDED_
#define _SPY_ARRAY_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

struct spy_array_s {
	void *elts;
	spy_uint_t nelts;
	size_t size;
	spy_uint_t nalloc;
	spy_pool_t *pool;
};

spy_array_t *spy_array_create(spy_pool_t *p, spy_uint_t n, size_t size);
void spy_array_destroy(spy_array_t *a);
void *spy_array_push(spy_array_t *a);
void *spy_array_push_n(spy_array_t *a, spy_uint_t n);

static spy_inline spy_int_t spy_array_init(spy_array_t *array,
		spy_pool_t *pool, spy_uint_t n, size_t size) {
	/*
	 * set "array->nelts" before "array->elts", otherwise MSVC thinks
	 * that "array->nelts" may be used without having been initialized
	 */

	array->nelts = 0;
	array->size = size;
	array->nalloc = n;
	array->pool = pool;

	array->elts = spy_palloc(pool, n * size);
	if (array->elts == NULL) {
		return SPY_ERROR;
	}

	return SPY_OK;
}

#endif /* _SPY_ARRAY_H_INCLUDED_ */
