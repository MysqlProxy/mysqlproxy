#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>
#include <spy_event_select.c>

spy_uint_t spy_event_flags;
spy_event_actions_t spy_event_actions;

void spy_process_events_and_timers(spy_global_t *global) {
	spy_msec_t timer, delta;

	/*
	 if (spy_timer_resolution) {
	 timer = NGX_TIMER_INFINITE;
	 flags = 0;

	 } else {

	 flags = NGX_UPDATE_TIME;

	 }


	 if (timer == NGX_TIMER_INFINITE || timer > 500) {
	 timer = 500;
	 }
	 */

	timer = spy_event_find_timer();
	delta = spy_current_msec;

	(void) spy_process_events(global, timer);

	delta = spy_current_msec - delta;

	spy_log_debug(SPY_LOG_DEBUG_EVENT, global->log, 0, "timer delta: %M", delta);

	if (spy_posted_accept_events) {
		spy_event_process_posted(global, &spy_posted_accept_events);
	}

	if (delta) {
		spy_event_expire_timers();
	}

	spy_log_debug(SPY_LOG_DEBUG_EVENT, global->log, 0, "posted events %p",
			spy_posted_events);

	if (spy_posted_events) {

		spy_event_process_posted(global, &spy_posted_events);
	}
}

spy_int_t spy_send_lowat(spy_connection_t *c, size_t lowat) {
	int sndlowat;

	if (lowat == 0 || c->sndlowat) {
		return SPY_OK;
	}

	sndlowat = (int) lowat;

	if (setsockopt(c->fd, SOL_SOCKET, SO_SNDLOWAT, (const void *) &sndlowat,
			sizeof(int)) == -1) {
		spy_connection_error(c, spy_socket_errno,
				"setsockopt(SO_SNDLOWAT) failed");
		return SPY_ERROR;
	}

	c->sndlowat = 1;

	return SPY_OK;
}

spy_int_t spy_event_init(spy_global_t *global) {

	spy_uint_t i;
	spy_event_t *rev, *wev;
	spy_connection_t *c, *next;
	spy_listening_t **ls;

	// IO复用模块初始化
	spy_event_actions.init = spy_select_init;
	spy_event_actions.add = spy_select_add_event;
	spy_event_actions.del = spy_select_del_event;
	spy_event_actions.done = spy_select_done;
	spy_event_actions.proc = spy_select_process_events;

	spy_init_event(global);

	// 初始化时间计数器
	if (spy_event_timer_init() == SPY_ERROR) {
		return SPY_ERROR;
	}

	// 初始化连接
	global->connections = malloc(sizeof(spy_connection_t)
			* global->connection_n);
	if (global->connections == NULL) {
		return SPY_ERROR;
	}

	c = global->connections;

	// 初始化读事件
	global->read_events = malloc(sizeof(spy_event_t) * global->connection_n);
	if (global->read_events == NULL) {
		return SPY_ERROR;
	}

	// 初始化写事件
	global->write_events = malloc(sizeof(spy_event_t) * global->connection_n);
	if (global->write_events == NULL) {
		return SPY_ERROR;
	}

	rev = global->read_events;
	wev = global->write_events;
	for (i = 0; i < global->connection_n; i++) {
		rev[i].closed = 1;
		wev[i].closed = 1;
	}

	i = global->connection_n;
	next = NULL;

	do {

		i--;

		c[i].data = next;
		c[i].read = &global->read_events[i];
		c[i].write = &global->write_events[i];
		c[i].fd = (spy_socket_t) -1;

		next = &c[i];

	} while (i);

	// 初始化可用连接
	global->free_connections = next;
	global->free_connection_n = global->connection_n;

	// 为监听分配连接
	ls = global->listening;

	for (i = 0; i < global->listening_n; i++) {

		c = spy_get_connection(ls[i]->fd, global->log);

		if (c == NULL) {
			return SPY_ERROR;
		}

		c->listening = ls[i];
		ls[i]->connection = c;

		rev = c->read;
		rev->accept = 1;

		rev->handler = spy_event_accept;

		if (spy_add_event(rev, SPY_READ_EVENT) == SPY_ERROR) {
			return SPY_ERROR;
		}

	}

	return SPY_OK;
}

spy_int_t spy_handle_read_event(spy_event_t *rev, spy_uint_t flags) {
	/* select, poll, /dev/poll */

	if (!rev->active && !rev->ready) {
		if (spy_add_event(rev, SPY_READ_EVENT) == SPY_ERROR) {
			return SPY_ERROR;
		}

		return SPY_OK;
	}

	if (rev->active && (rev->ready || (flags & SPY_CLOSE_EVENT))) {
		if (spy_del_event(rev, SPY_READ_EVENT) == SPY_ERROR) {
			return SPY_ERROR;
		}

		return SPY_OK;
	}

	return SPY_OK;
}

spy_int_t spy_handle_write_event(spy_event_t *wev, size_t lowat) {
	spy_connection_t *c;

	if (lowat) {
		c = wev->data;

		if (spy_send_lowat(c, lowat) == SPY_ERROR) {
			return SPY_ERROR;
		}
	}

	if (!wev->active && !wev->ready) {
		if (spy_add_event(wev, SPY_WRITE_EVENT) == SPY_ERROR) {
			return SPY_ERROR;
		}

		return SPY_OK;
	}

	if (wev->active && wev->ready) {
		if (spy_del_event(wev, SPY_WRITE_EVENT) == SPY_ERROR) {
			return SPY_ERROR;
		}

		return SPY_OK;
	}

	return SPY_OK;
}
