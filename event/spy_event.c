#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>
#include <spy_event_select.c>

spy_event_actions_t spy_event_actions;

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

	spy_init_event(global);

	/*
	 // 初始化时间计数器
	 if (spy_event_timer_init() == SPY_ERROR) {
	 return SPY_ERROR;
	 }
	 */

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
