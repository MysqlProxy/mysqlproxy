#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

spy_event_t *spy_posted_accept_events;
spy_event_t *spy_posted_events;

void spy_event_process_posted(spy_global_t *global, spy_event_t **posted) {
	spy_event_t *ev;

	for (;;) {

		ev = (spy_event_t *) *posted;

		spy_log_debug(SPY_LOG_DEBUG_EVENT, global->log, 0, "posted event %p",
				ev);

		if (ev == NULL) {
			return;
		}

		spy_delete_posted_event(ev);

		ev->handler(ev);
	}
}
