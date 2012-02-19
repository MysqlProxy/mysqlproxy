/*
 * Copyright (C) Igor Sysoev
 */

#ifndef _SPY_LIST_H_INCLUDED_
#define _SPY_LIST_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

typedef struct spy_list_part_s spy_list_part_t;

struct spy_list_part_s {
	void *elts;
	spy_uint_t nelts;
	spy_list_part_t *next;
};

typedef struct {
	spy_list_part_t *last;
	spy_list_part_t part;
	size_t size;
	spy_uint_t nalloc;
	spy_pool_t *pool;
} spy_list_t;

spy_list_t *spy_list_create(spy_pool_t *pool, spy_uint_t n, size_t size);

static spy_inline spy_int_t spy_list_init(spy_list_t *list, spy_pool_t *pool,
		spy_uint_t n, size_t size) {
	list->part.elts = spy_palloc(pool, n * size);
	if (list->part.elts == NULL) {
		return SPY_ERROR;
	}

	list->part.nelts = 0;
	list->part.next = NULL;
	list->last = &list->part;
	list->size = size;
	list->nalloc = n;
	list->pool = pool;

	return SPY_OK;
}

/*
 *
 *  the iteration through the list:
 *
 *  part = &list.part;
 *  data = part->elts;
 *
 *  for (i = 0 ;; i++) {
 *
 *      if (i >= part->nelts) {
 *          if (part->next == NULL) {
 *              break;
 *          }
 *
 *          part = part->next;
 *          data = part->elts;
 *          i = 0;
 *      }
 *
 *      ...  data[i] ...
 *
 *  }
 */

void *spy_list_push(spy_list_t *list);

#endif /* _SPY_LIST_H_INCLUDED_ */
