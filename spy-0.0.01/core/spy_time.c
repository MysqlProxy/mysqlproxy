
#include <spy_config.h>
#include <spy_core.h>


static spy_time_t        cached_time[NGX_TIME_SLOTS];

void spy_time_init(void) {

	spy_cached_err_log_time.len = sizeof("1970/09/28 12:00:00") - 1;

	ngx_cached_time = &cached_time[0];

	ngx_time_update();
}
