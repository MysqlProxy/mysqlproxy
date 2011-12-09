
#include <spy_config.h>
#include <spy_core.h>

void spy_localtime(time_t s, spy_tm_t *tm) {
	(void) localtime_r(&s, tm);
	tm->spy_tm_mon++;
	tm->spy_tm_year += 1900;
}
