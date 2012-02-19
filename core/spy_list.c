/*
 * Copyright (C) Igor Sysoev
 */

#include <spy_config.h>
#include <spy_core.h>

spy_list_t *
spy_list_create(spy_pool_t *pool, spy_uint_t n, size_t size) {
	spy_list_t *list;

	list = spy_palloc(pool, sizeof(spy_list_t));
	if (list == NULL) {
		return NULL;
	}

	list->part.elts = spy_palloc(pool, n * size);
	if (list->part.elts == NULL) {
		return NULL;
	}

	list->part.nelts = 0;
	list->part.next = NULL;
	list->last = &list->part;
	list->size = size;
	list->nalloc = n;
	list->pool = pool;

	return list;
}

void *
spy_list_push(spy_list_t *l) {
	void *elt;
	spy_list_part_t *last;

	last = l->last;

	if (last->nelts == l->nalloc) {

		/* the last part is full, allocate a new list part */

		last = spy_palloc(l->pool, sizeof(spy_list_part_t));
		if (last == NULL) {
			return NULL;
		}

		last->elts = spy_palloc(l->pool, l->nalloc * l->size);
		if (last->elts == NULL) {
			return NULL;
		}

		last->nelts = 0;
		last->next = NULL;

		l->last->next = last;
		l->last = last;
	}

	elt = (char *) last->elts + l->size * last->nelts;
	last->nelts++;

	return elt;
}
