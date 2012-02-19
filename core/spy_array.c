/*
 * Copyright (C) Igor Sysoev
 */

#include <spy_config.h>
#include <spy_core.h>

spy_array_t *
spy_array_create(spy_pool_t *p, spy_uint_t n, size_t size) {
	spy_array_t *a;

	a = spy_palloc(p, sizeof(spy_array_t));
	if (a == NULL) {
		return NULL;
	}

	a->elts = spy_palloc(p, n * size);
	if (a->elts == NULL) {
		return NULL;
	}

	a->nelts = 0;
	a->size = size;
	a->nalloc = n;
	a->pool = p;

	return a;
}

void spy_array_destroy(spy_array_t *a) {
	spy_pool_t *p;

	p = a->pool;

	if ((u_char *) a->elts + a->size * a->nalloc == p->d.last) {
		p->d.last -= a->size * a->nalloc;
	}

	if ((u_char *) a + sizeof(spy_array_t) == p->d.last) {
		p->d.last = (u_char *) a;
	}
}

void *
spy_array_push(spy_array_t *a) {
	void *elt, *new;
	size_t size;
	spy_pool_t *p;

	if (a->nelts == a->nalloc) {

		/* the array is full */

		size = a->size * a->nalloc;

		p = a->pool;

		if ((u_char *) a->elts + size == p->d.last && p->d.last + a->size
				<= p->d.end) {
			/*
			 * the array allocation is the last in the pool
			 * and there is space for new allocation
			 */

			p->d.last += a->size;
			a->nalloc++;

		} else {
			/* allocate a new array */

			new = spy_palloc(p, 2 * size);
			if (new == NULL) {
				return NULL;
			}

			spy_memcpy(new, a->elts, size);
			a->elts = new;
			a->nalloc *= 2;
		}
	}

	elt = (u_char *) a->elts + a->size * a->nelts;
	a->nelts++;

	return elt;
}

void *
spy_array_push_n(spy_array_t *a, spy_uint_t n) {
	void *elt, *new;
	size_t size;
	spy_uint_t nalloc;
	spy_pool_t *p;

	size = n * a->size;

	if (a->nelts + n > a->nalloc) {

		/* the array is full */

		p = a->pool;

		if ((u_char *) a->elts + a->size * a->nalloc == p->d.last && p->d.last
				+ size <= p->d.end) {
			/*
			 * the array allocation is the last in the pool
			 * and there is space for new allocation
			 */

			p->d.last += size;
			a->nalloc += n;

		} else {
			/* allocate a new array */

			nalloc = 2 * ((n >= a->nalloc) ? n : a->nalloc);

			new = spy_palloc(p, nalloc * a->size);
			if (new == NULL) {
				return NULL;
			}

			spy_memcpy(new, a->elts, a->nelts * a->size);
			a->elts = new;
			a->nalloc = nalloc;
		}
	}

	elt = (u_char *) a->elts + a->size * a->nelts;
	a->nelts += n;

	return elt;
}
