#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

spy_int_t spy_event_init(spy_global_t *proxy) {

	spy_uint_t i;
	spy_event_t *rev, *wev;
	spy_connection_t *c, *next;
	spy_listening_t *ls;

	/*
	 // 初始化时间计数器
	 if (spy_event_timer_init() == SPY_ERROR) {
	 return SPY_ERROR;
	 }
	 */

	// 初始化连接
	proxy->connections = malloc(sizeof(spy_connection_t) * proxy->connection_n);
	if (proxy->connections == NULL) {
		return SPY_ERROR;
	}

	c = proxy->connections;

	// 初始化读事件
	proxy->read_events = malloc(sizeof(spy_event_t) * proxy->connection_n);
	if (proxy->read_events == NULL) {
		return SPY_ERROR;
	}

	// 初始化写事件
	proxy->write_events = malloc(sizeof(spy_event_t) * proxy->connection_n);
	if (proxy->write_events == NULL) {
		return SPY_ERROR;
	}

	rev = proxy->read_events;
	wev = proxy->write_events;
	for (i = 0; i < proxy->connection_n; i++) {
		rev[i].closed = 1;
		wev[i].closed = 1;
	}

	i = proxy->connection_n;
	next = NULL;

	do {

		i--;

		c[i].data = next;
		c[i].read = &proxy->read_events[i];
		c[i].write = &proxy->write_events[i];
		c[i].fd = (spy_socket_t) -1;

		next = &c[i];

	} while (i);

	// 初始化可用连接
	proxy->free_connections = next;
	proxy->free_connection_n = proxy->connection_n;

	// 为监听分配连接
	ls = proxy->listening;

	for (i = 0; i < proxy->listening_n; i++) {

		c = spy_get_connection(ls[i].fd, proxy->log);

		if (c == NULL) {
			return SPY_ERROR;
		}

		c->listening = &ls[i];
		ls[i].connection = c;

		rev = c->read;
		rev->accept = 1;

		//rev->handler = ngx_event_accept;

		/*
		 if (spy_add_event(rev, SPY_READ_EVENT, 0) == SPY_ERROR) {
		 return SPY_ERROR;
		 }
		 */

	}

	return SPY_OK;
}
