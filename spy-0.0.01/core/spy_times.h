
#ifndef _SPY_TIMES_H_INCLUDED_
#define _SPY_TIMES_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

/*
 * 使用ring buffer方案 实现lock-free
 */
#define SPY_TIME_SLOTS   64

extern volatile spy_str_t spy_cached_err_log_time;

typedef struct {
	time_t sec;
	spy_uint_t msec;
	spy_int_t gmtoff;
} spy_time_t;

void spy_time_update_r(void);
void spy_time_init(void);

#endif
