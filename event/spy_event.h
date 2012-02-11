#ifndef _SPY_EVENT_H_INCLUDED_
#define _SPY_EVENT_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

#define SPY_READ_EVENT     0
#define SPY_WRITE_EVENT    1

#define SPY_CLOSE_EVENT    1

#define SPY_INVALID_INDEX  0xd0d0d0d0
#define SPY_USE_GREEDY_EVENT     0x00000020

struct spy_event_s {
	void *data;

	unsigned write :1;

	unsigned accept :1;

	unsigned active :1;
	unsigned ready :1;
	unsigned eof :1;
	unsigned error :1;

	unsigned timedout :1;
	unsigned timer_set :1;

	spy_uint_t index;
	unsigned available :1;

	spy_log_t *log;

	spy_minheap_node_t timer;

	spy_event_handler_pt handler;

	unsigned closed :1;

	/* the links of the posted queue */
	spy_event_t *next;
	spy_event_t **prev;

};

typedef struct {
	spy_int_t (*add)(spy_event_t *ev, spy_int_t event);
	spy_int_t (*del)(spy_event_t *ev, spy_int_t event);
	spy_int_t (*init)(spy_global_t *global);
	void (*done)(spy_global_t *global);
	spy_int_t (*proc)(spy_global_t *global, spy_msec_t timer);
} spy_event_actions_t;

extern spy_event_actions_t spy_event_actions;
extern spy_uint_t spy_event_flags;

#define spy_process_events   spy_event_actions.proc
#define spy_done_events      spy_event_actions.done

#define spy_add_event        spy_event_actions.add
#define spy_del_event        spy_event_actions.del
#define spy_init_event		 spy_event_actions.init

extern spy_os_io_t spy_io;

#define spy_recv             spy_io.recv
#define spy_send             spy_io.send

spy_int_t spy_event_init(spy_global_t *global);
void spy_event_accept(spy_event_t *ev);
spy_int_t spy_send_lowat(spy_connection_t *c, size_t lowat);
spy_int_t spy_handle_read_event(spy_event_t *rev, spy_uint_t flags);
spy_int_t spy_handle_write_event(spy_event_t *wev, size_t lowat);
void spy_process_events_and_timers(spy_global_t *global);

#define spy_add_timer        spy_event_add_timer
#define spy_del_timer        spy_event_del_timer

/* used in ngx_log_debugX() */
#define spy_event_ident(p)  ((spy_connection_t *) (p))->fd

#include <spy_event_posted.h>
#include <spy_event_timer.h>

#endif
