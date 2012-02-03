#ifndef _SPY_ALLOC_INCLUDE_H_
#define _SPY_ALLOC_INCLUDE_H_

#include <spy_config.h>
#include <spy_core.h>

#define spy_free          free

void *
spy_alloc(size_t size, spy_log_t *log);

void *
spy_calloc(size_t size, spy_log_t *log);

#endif
