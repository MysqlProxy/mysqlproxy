#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

void spy_single_process_launch(spy_global_t *global) {
	//ngx_uint_t i;

	for (;;) {
		spy_log_debug(SPY_LOG_DEBUG_EVENT, global->log, 0, "%s", "worker cycle");

		spy_process_events_and_timers(global);
	}
}
