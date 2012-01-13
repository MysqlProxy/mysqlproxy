#ifndef _SPY_EVENT_H_INCLUDED_
#define _SPY_EVENT_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

#define SPY_READ_EVENT     0
#define SPY_WRITE_EVENT    1

#define SPY_INVALID_INDEX  0xd0d0d0d0

struct spy_event_s {
	void *data;

	unsigned write :1;

	unsigned accept :1;

	unsigned eof :1;
	unsigned error :1;

	unsigned timedout :1;
	unsigned timer_set :1;

	spy_event_handler_pt handler;

	spy_uint_t index;

	spy_log_t *log;

	spy_minheap_node_t timer;

	unsigned closed :1;

	/* the links of the posted queue */
	spy_event_t *next;
	spy_event_t **prev;

};

spy_int_t spy_event_init(spy_global_t *proxy);

#endif
