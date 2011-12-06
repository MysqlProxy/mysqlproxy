

#ifndef _SPY_TIMES_H_INCLUDED_
#define _SPY_TIMES_H_INCLUDED_

/*
 * 使用ring buffer方案 实现lock-free
 */
#define SPY_TIME_SLOTS   64


extern volatile spy_str_t    spy_cached_err_log_time;


#endif
