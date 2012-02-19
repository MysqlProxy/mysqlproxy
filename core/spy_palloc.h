#ifndef _SPY_PALLOC_H_INCLUDED_
#define _SPY_PALLOC_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

#define SPY_MAX_ALLOC_FROM_POOL  (spy_pagesize - 1)
#define SPY_POOL_ALIGNMENT       16

typedef struct spy_pool_large_s spy_pool_large_t;

struct spy_pool_large_s {
	spy_pool_large_t *next;
	void *alloc;
};

typedef struct {
	u_char *last;
	u_char *end;
	spy_pool_t *next;
	spy_uint_t failed;
} spy_pool_data_t;

struct spy_pool_s {
	spy_pool_data_t d;
	size_t max;
	spy_pool_t *current;
	spy_pool_large_t *large;
	spy_log_t *log;
};

void *spy_alloc(size_t size, spy_log_t *log);
void *spy_calloc(size_t size, spy_log_t *log);

spy_pool_t *spy_create_pool(size_t size, spy_log_t *log);
void spy_destroy_pool(spy_pool_t *pool);

void *spy_palloc(spy_pool_t *pool, size_t size);
void *spy_pnalloc(spy_pool_t *pool, size_t size);
void *spy_pcalloc(spy_pool_t *pool, size_t size);
void *spy_pmemalign(spy_pool_t *pool, size_t size, size_t alignment);
spy_int_t spy_pfree(spy_pool_t *pool, void *p);

#endif
