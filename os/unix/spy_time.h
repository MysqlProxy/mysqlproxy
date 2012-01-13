#ifndef _SPY_TIME_H_INCLUDED_
#define _SPY_TIME_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

typedef spy_minheap_key_t spy_msec_t;

typedef struct tm spy_tm_t;

#define spy_tm_sec            tm_sec
#define spy_tm_min            tm_min
#define spy_tm_hour           tm_hour
#define spy_tm_mday           tm_mday
#define spy_tm_mon            tm_mon
#define spy_tm_year           tm_year
#define spy_tm_wday           tm_wday
#define spy_tm_isdst          tm_isdst

#define spy_tm_sec_t          int
#define spy_tm_min_t          int
#define spy_tm_hour_t         int
#define spy_tm_mday_t         int
#define spy_tm_mon_t          int
#define spy_tm_year_t         int
#define spy_tm_wday_t         int

void spy_localtime(time_t s, spy_tm_t *tm);
#define spy_gettimeofday(tp)  (void) gettimeofday(tp, NULL)
#define spy_msleep(ms)        (void) usleep(ms * 1000)

#endif
