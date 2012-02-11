#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

spy_minheap_t spy_event_timer_heap;

spy_int_t spy_event_timer_init() {

	spy_minheap_init(&spy_event_timer_heap, spy_global->connection_n * 2);
	return SPY_OK;
}

spy_msec_t spy_event_find_timer(void) {
	spy_int_t timer;
	spy_minheap_node_t *node;
	spy_minheap_t *heap;

	heap = &spy_event_timer_heap;

	node = spy_minheap_min(heap);

	if (!node)
		return SPY_TIMER_INFINITE;

	timer = (spy_msec_int_t) node->key - (spy_msec_int_t) spy_current_msec;

	return (spy_msec_t) (timer > 0 ? timer : 0);
}

void spy_event_expire_timers(void) {
	spy_event_t *ev;
	spy_minheap_node_t *node;

	for (;;) {

		node = spy_minheap_min(&spy_event_timer_heap);

		if (!node)
			break;

		/* node->key <= ngx_current_time */

		if ((spy_msec_int_t) node->key - (spy_msec_int_t) spy_current_msec <= 0) {
			ev = (spy_event_t *) ((char *) node - offsetof(spy_event_t, timer));

			spy_log_debug(SPY_LOG_DEBUG_EVENT, ev->log, 0,
					"event timer del: %d: %M", spy_event_ident(ev->data),
					ev->timer.key);

			spy_minheap_delete(&spy_event_timer_heap, ev->timer.index);

			ev->timer_set = 0;

			ev->timedout = 1;

			ev->handler(ev);

			continue;
		}

		break;
	}
}

