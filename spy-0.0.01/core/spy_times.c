#include <spy_config.h>
#include <spy_core.h>

#define SPY_TIME_SLOTS   64

static spy_uint_t slot;

static u_char
		cached_err_log_time[SPY_TIME_SLOTS][sizeof("1970-09-28 12:00:00")];

static spy_time_t cached_time[SPY_TIME_SLOTS];
volatile spy_str_t spy_cached_err_log_time;
static spy_atomic_t spy_time_lock;
volatile spy_time_t *spy_cached_time;
volatile spy_time_t *spy_cached_time;

void spy_time_init(void) {

	spy_cached_err_log_time.len = sizeof("1970-09-28 12:00:00") - 1;

	spy_cached_time = &cached_time[0];

	spy_time_update_r();

	struct sigaction sa;
	struct itimerval itv;

	ngx_memzero(&sa, sizeof(struct sigaction));
	sa.sa_handler = ngx_timer_signal_handler;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGALRM, &sa, NULL) == -1) {
		ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
				"sigaction(SIGALRM) failed");
		return NGX_ERROR;
	}

	itv.it_interval.tv_sec = ngx_timer_resolution / 1000;
	itv.it_interval.tv_usec = (ngx_timer_resolution % 1000) * 1000;
	itv.it_value.tv_sec = ngx_timer_resolution / 1000;
	itv.it_value.tv_usec = (ngx_timer_resolution % 1000) * 1000;

	if (setitimer(ITIMER_REAL, &itv, NULL) == -1) {
		ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
				"setitimer() failed");
	}
}

void spy_time_update_r(void) {
	u_char *p1;
	spy_tm_t tm;
	time_t sec;
	spy_time_t *tp;
	struct timeval tv;

	if (!spy_trylock(&spy_time_lock)) {
		return;
	}

	spy_gettimeofday(&tv);

	sec = tv.tv_sec;

	tp = &cached_time[slot];

	if (tp->sec == sec) {
		spy_unlock(&spy_time_lock);
		return;
	}

	if (slot == SPY_TIME_SLOTS - 1) {
		slot = 0;
	} else {
		slot++;
	}

	spy_localtime(sec, &tm);

	p1 = &cached_err_log_time[slot][0];

	// 2011-12-07 12:00:00
	(void) spy_sprintf(p1, "%4d-%-02d-%-02d %-02d:%-02d:%-02d", tm.spy_tm_year,
			tm.spy_tm_mon, tm.spy_tm_mday, tm.spy_tm_hour, tm.spy_tm_min,
			tm.spy_tm_sec);

	spy_memory_barrier();

	//spy_write_fd(spy_stderr, (void *) p1, 19);
	spy_cached_err_log_time.data = p1;

	spy_unlock(&spy_time_lock);
}

#if 0
void spy_gmtime(time_t t, spy_tm_t *tp) {
	spy_int_t yday;
	spy_uint_t n, sec, min, hour, mday, mon, year, wday, days, leap;

	/* the calculation is valid for positive time_t only */

	n = (spy_uint_t) t;

	days = n / 86400;

	/* Jaunary 1, 1970 was Thursday */

	wday = (4 + days) % 7;

	n %= 86400;
	hour = n / 3600;
	n %= 3600;
	min = n / 60;
	sec = n % 60;

	/*
	 * the algorithm based on Gauss' formula,
	 * see src/http/spy_http_parse_time.c
	 */

	/* days since March 1, 1 BC */
	days = days - (31 + 28) + 719527;

	/*
	 * The "days" should be adjusted to 1 only, however, some March 1st's go
	 * to previous year, so we adjust them to 2.  This causes also shift of the
	 * last Feburary days to next year, but we catch the case when "yday"
	 * becomes negative.
	 */

	year = (days + 2) * 400 / (365 * 400 + 100 - 4 + 1);

	yday = days - (365 * year + year / 4 - year / 100 + year / 400);

	if (yday < 0) {
		leap = (year % 4 == 0) && (year % 100 || (year % 400 == 0));
		yday = 365 + leap + yday;
		year--;
	}

	/*
	 * The empirical formula that maps "yday" to month.
	 * There are at least 10 variants, some of them are:
	 *     mon = (yday + 31) * 15 / 459
	 *     mon = (yday + 31) * 17 / 520
	 *     mon = (yday + 31) * 20 / 612
	 */

	mon = (yday + 31) * 10 / 306;

	/* the Gauss' formula that evaluates days before the month */

	mday = yday - (367 * mon / 12 - 30) + 1;

	if (yday >= 306) {

		year++;
		mon -= 10;

		/*
		 * there is no "yday" in Win32 SYSTEMTIME
		 *
		 * yday -= 306;
		 */

	} else {

		mon += 2;

		/*
		 * there is no "yday" in Win32 SYSTEMTIME
		 *
		 * yday += 31 + 28 + leap;
		 */
	}

	tp->spy_tm_sec = (spy_tm_sec_t) sec;
	tp->spy_tm_min = (spy_tm_min_t) min;
	tp->spy_tm_hour = (spy_tm_hour_t) hour;
	tp->spy_tm_mday = (spy_tm_mday_t) mday;
	tp->spy_tm_mon = (spy_tm_mon_t) mon;
	tp->spy_tm_year = (spy_tm_year_t) year;
	tp->spy_tm_wday = (spy_tm_wday_t) wday;
}
#endif

#ifdef _SPY_TIMES_UNIT_TEST

int main() {
	spy_time_init();
	u_char buf[50];
	u_char *p;
	p = buf;

	p = spy_slprintf(p, p + 19, "%S", spy_cached_err_log_time);

	spy_write_fd(1, buf, p - buf);
	exit(EXIT_SUCCESS);
}

#endif
