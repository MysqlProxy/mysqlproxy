#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

static fd_set master_read_fd_set;
static fd_set master_write_fd_set;
static fd_set work_read_fd_set;
static fd_set work_write_fd_set;

static spy_int_t max_fd;

static spy_uint_t nevents;
static spy_event_t **event_index;

static void spy_select_repair_fd_sets(spy_global_t *global);

static spy_int_t spy_select_init(spy_global_t *global) {
	spy_event_t * *index;

	// 设置读写FD
	if (event_index == NULL) {
		FD_ZERO(&master_read_fd_set);
		FD_ZERO(&master_write_fd_set);
		nevents = 0;
	}

	if (global->old_global == NULL || global->old_global->connection_n
			< global->connection_n) {
		// 分配读 + 写时间的event
		index = spy_alloc(sizeof(spy_event_t *) * 2 * global->connection_n,
				global->log);
		if (index == NULL) {
			return SPY_ERROR;
		}
	}

	// 分配n个event
	if (event_index) {
		spy_memcpy(index, event_index, sizeof(spy_event_t *) * nevents);
		spy_free(event_index);
	}

	event_index = index;

	max_fd = -1;

	spy_io = spy_os_io;

	return SPY_OK;
}

static void spy_select_done(spy_global_t *global) {
	spy_free(event_index);

	event_index = NULL;
}

static spy_int_t spy_select_add_event(spy_event_t *ev, spy_int_t event) {
	spy_connection_t *c;

	c = ev->data;

	/*
	 spy_log_debug(SPY_LOG_DEBUG_EVENT, ev->log, 0,
	 "select add event fd:%d ev:%i", c->fd, event);
	 */
	if (ev->index != SPY_INVALID_INDEX) {
		spy_log_error(SPY_LOG_ALERT, ev->log, 0,
				"select event fd:%d ev:%i is already set", c->fd, event);
		return SPY_OK;
	}

	if ((event == SPY_READ_EVENT && ev->write) || (event == SPY_WRITE_EVENT
			&& !ev->write)) {
		spy_log_error(SPY_LOG_ALERT, ev->log, 0,
				"invalid select %s event fd:%d ev:%i", ev->write ? "write"
						: "read", c->fd, event);
		return SPY_ERROR;
	}

	// 添加事件监听
	if (event == SPY_READ_EVENT) {
		FD_SET(c->fd, &master_read_fd_set);

	} else if (event == SPY_WRITE_EVENT) {
		FD_SET(c->fd, &master_write_fd_set);
	}

	if (max_fd != -1 && max_fd < c->fd) {
		max_fd = c->fd;
	}

	ev->active = 1;
	event_index[nevents] = ev;
	ev->index = nevents;
	nevents++;

	return SPY_OK;
}

static spy_int_t spy_select_del_event(spy_event_t *ev, spy_int_t event) {
	spy_event_t *e;
	spy_connection_t *c;

	c = ev->data;
	ev->active = 0;

	if (ev->index == SPY_INVALID_INDEX) {
		return SPY_OK;
	}

	spy_log_debug(SPY_LOG_DEBUG_EVENT, ev->log, 0,
			"select del event fd:%d ev:%i", c->fd, event);

	if (event == SPY_READ_EVENT) {
		FD_CLR(c->fd, &master_read_fd_set);

	} else if (event == SPY_WRITE_EVENT) {
		FD_CLR(c->fd, &master_write_fd_set);
	}

	if (max_fd == c->fd) {
		max_fd = -1;
	}

	if (ev->index < --nevents) {
		e = event_index[nevents];
		event_index[ev->index] = e;
		e->index = ev->index;
	}

	ev->index = SPY_INVALID_INDEX;

	return SPY_OK;
}

static spy_int_t spy_select_process_events(spy_global_t *global,
		spy_msec_t timer) {

	int ready, nready;
	spy_err_t err;
	spy_uint_t i, found;
	spy_event_t *ev, **queue;
	struct timeval tv, *tp;
	spy_connection_t *c;

	if (max_fd == -1) {
		for (i = 0; i < nevents; i++) {
			c = event_index[i]->data;
			if (max_fd < c->fd) {
				max_fd = c->fd;
			}
		}

		spy_log_debug(SPY_LOG_DEBUG_EVENT, global->log, 0, "change max_fd: %d",
				max_fd);
	}

	if (timer == SPY_TIMER_INFINITE) {
		tp = NULL;

	} else {
		tv.tv_sec = (long) (timer / 1000);
		tv.tv_usec = (long) ((timer % 1000) * 1000);
		tp = &tv;
	}

	spy_log_debug(SPY_LOG_DEBUG_EVENT, global->log, 0, "select timer: %M",
			timer);

	work_read_fd_set = master_read_fd_set;
	work_write_fd_set = master_write_fd_set;

	ready = select(max_fd + 1, &work_read_fd_set, &work_write_fd_set, NULL, tp);

	err = (ready == -1) ? spy_errno : 0;

	spy_time_update_r();

	spy_log_debug(SPY_LOG_DEBUG_EVENT, global->log, 0, "select ready %d", ready);

	if (err) {
		spy_uint_t level;

		if (err == SPY_EINTR) {
			level = SPY_LOG_INFO;

		} else {
			level = SPY_LOG_ALERT;
		}

		spy_log_error(level, global->log, err, "select() failed");

		if (err == EBADF) {
			spy_select_repair_fd_sets(global);
		}

		return SPY_ERROR;
	}

	if (ready == 0) {
		if (timer != SPY_TIMER_INFINITE) {
			return SPY_OK;
		}

		spy_log_error(SPY_LOG_ALERT, global->log, 0,
				"select() returned no events without timeout");
		return SPY_ERROR;
	}

	//ngx_mutex_lock(ngx_posted_events_mutex);

	nready = 0;

	for (i = 0; i < nevents; i++) {
		ev = event_index[i];
		c = ev->data;
		found = 0;

		if (ev->write) {
			if (FD_ISSET(c->fd, &work_write_fd_set)) {
				found = 1;
				spy_log_debug(SPY_LOG_DEBUG_EVENT, global->log, 0,
						"select write %d", c->fd);
			}

		} else {
			if (FD_ISSET(c->fd, &work_read_fd_set)) {
				found = 1;
				spy_log_debug(SPY_LOG_DEBUG_EVENT, global->log, 0,
						"select read %d", c->fd);
			}
		}

		if (found) {
			ev->ready = 1;

			queue = (spy_event_t **) (ev->accept ? &spy_posted_accept_events
					: &spy_posted_events);
			spy_post_event(ev, queue);

			nready++;
		}
	}

	//ngx_mutex_unlock(ngx_posted_events_mutex);

	if (ready != nready) {
		spy_log_error(SPY_LOG_ALERT, global->log, 0,
				"select ready != events: %d:%d", ready, nready);

		spy_select_repair_fd_sets(global);
	}

	return SPY_OK;
}

static void spy_select_repair_fd_sets(spy_global_t *global) {
	int n;
	socklen_t len;
	spy_err_t err;
	spy_socket_t s;

	for (s = 0; s <= max_fd; s++) {

		if (FD_ISSET(s, &master_read_fd_set) == 0) {
			continue;
		}

		len = sizeof(int);

		if (getsockopt(s, SOL_SOCKET, SO_TYPE, &n, &len) == -1) {
			err = spy_socket_errno;

			spy_log_error(SPY_LOG_ALERT, global->log, err,
					"invalid descriptor #%d in read fd_set", s);

			FD_CLR(s, &master_read_fd_set);
		}
	}

	for (s = 0; s <= max_fd; s++) {

		if (FD_ISSET(s, &master_write_fd_set) == 0) {
			continue;
		}

		len = sizeof(int);

		if (getsockopt(s, SOL_SOCKET, SO_TYPE, &n, &len) == -1) {
			err = spy_socket_errno;

			spy_log_error(SPY_LOG_ALERT, global->log, err,
					"invalid descriptor #%d in write fd_set", s);

			FD_CLR(s, &master_write_fd_set);
		}
	}

	max_fd = -1;
}
