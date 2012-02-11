#ifndef _SPY_EVENT_TIMER_H_INCLUDED_
#define _SPY_EVENT_TIMER_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

extern spy_minheap_t spy_event_timer_heap;

#define SPY_TIMER_LAZY_DELAY  300
#define SPY_TIMER_INFINITE  (spy_msec_t) -1

spy_int_t spy_event_timer_init();
spy_msec_t spy_event_find_timer(void);
void spy_event_expire_timers(void);

static spy_inline void spy_event_del_timer(spy_event_t *ev) {
	spy_log_debug(SPY_LOG_DEBUG_EVENT, ev->log, 0, "event timer del: %d: %M",
			spy_event_ident(ev->data), ev->timer.key);

	spy_minheap_delete(&spy_event_timer_heap, ev->timer.index);

	ev->timer_set = 0;
}

static spy_inline void spy_event_add_timer(spy_event_t *ev, spy_msec_t timer) {

	spy_msec_t key;
	spy_msec_int_t diff;

	key = spy_current_msec + timer;

	if (ev->timer_set) {

		/*
		 * Use a previous timer value if difference between it and a new
		 * value is less than NGX_TIMER_LAZY_DELAY milliseconds: this allows
		 * to minimize the rbtree operations for fast connections.
		 */

		diff = (spy_msec_int_t) (key - ev->timer.key);

		if (spy_abs(diff) < SPY_TIMER_LAZY_DELAY) {
			spy_log_debug(SPY_LOG_DEBUG_EVENT, ev->log, 0,
					"event timer: %d, old: %M, new: %M", spy_event_ident(
							ev->data), ev->timer.key, key);
			return;
		}

		spy_del_timer(ev);
	}

	ev->timer.key = key;

	spy_log_debug(SPY_LOG_DEBUG_EVENT, ev->log, 0,
			"event timer add: %d: %M:%M", spy_event_ident(ev->data), timer,
			ev->timer.key);

	spy_minheap_insert(&spy_event_timer_heap, &ev->timer,
			spy_event_timer_heap.last);

	ev->timer_set = 1;

}

#endif

