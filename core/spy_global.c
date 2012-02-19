#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>
#include <spy_proxy.h>

volatile spy_global_t *spy_global;
spy_array_t spy_old_globals;
static spy_pool_t *spy_temp_pool;
static spy_event_t spy_cleaner_event;
static spy_str_t error_log = spy_string("spy.log");

static spy_connection_t dumb;

static spy_int_t spy_cmp_sockaddr(struct sockaddr *sa1, struct sockaddr *sa2);
static void spy_clean_old_global(spy_event_t *ev);

spy_int_t spy_create_pidfile(spy_str_t *name, spy_log_t *log) {
	size_t len;
	spy_file_t file;
	u_char pid[SPY_INT64_LEN + 2];

	spy_memzero(&file, sizeof(spy_file_t));

	file.name = *name;
	file.log = log;

	file.fd = spy_open_file(file.name.data, SPY_FILE_RDWR, SPY_FILE_TRUNCATE,
			SPY_FILE_DEFAULT_ACCESS);

	if (file.fd == SPY_INVALID_FILE) {
		spy_log_error(SPY_LOG_EMERG, log, spy_errno,
				spy_open_file_n " \"%s\" failed", file.name.data);
		return SPY_ERROR;
	}

	len = spy_snprintf(pid, SPY_INT64_LEN + 2, "%P%N", spy_pid) - pid;

	if (spy_write_file(&file, pid, len, 0) == SPY_ERROR) {
		return SPY_ERROR;
	}

	if (spy_close_file(file.fd) == SPY_FILE_ERROR) {
		spy_log_error(SPY_LOG_ALERT, log, spy_errno,
				spy_close_file_n " \"%s\" failed", file.name.data);
	}

	return SPY_OK;
}

spy_global_t *spy_global_init(spy_global_t *old_global) {

	spy_global_t *global;
	spy_conf_t *cf;
	spy_uint_t n, i;
	spy_list_part_t *part;
	spy_pool_t *pool;
	spy_open_file_t *file;
	spy_listening_t *ls, *nls;
	spy_log_t *log;

	// 更新时间
	spy_time_update_r();

	// 获取老的global 日志指针
	log = old_global->log;

	// 创建新的pool
	pool = spy_create_pool(10240, log);
	if (pool == NULL) {
		return NULL;
	}
	pool->log = log;

	// 创建新的global
	global = spy_pcalloc(pool, sizeof(spy_global_t));
	if (global == NULL) {
		spy_destroy_pool(pool);
		return NULL;
	}

	// 设置新global
	global->pool = pool;
	global->log = log;
	global->new_log.log_level = SPY_LOG_ERR;
	global->old_global = old_global;

	// 设置配置信息
	global->conf_prefix.len = old_global->conf_prefix.len;
	global->conf_prefix.data = spy_pstrdup(pool, &old_global->conf_prefix);
	if (global->conf_prefix.data == NULL) {
		spy_destroy_pool(pool);
		return NULL;
	}

	global->prefix.len = old_global->prefix.len;
	global->prefix.data = spy_pstrdup(pool, &old_global->prefix);
	if (global->prefix.data == NULL) {
		spy_destroy_pool(pool);
		return NULL;
	}

	global->conf_file.len = old_global->conf_file.len;
	global->conf_file.data = spy_pnalloc(pool, old_global->conf_file.len + 1);
	if (global->conf_file.data == NULL) {
		spy_destroy_pool(pool);
		return NULL;
	}
	spy_cpystrn(global->conf_file.data, old_global->conf_file.data,
			old_global->conf_file.len + 1);

	// 初始化配置文件
	if (spy_conf_init(global) != SPY_OK) {
		return NULL;
	}

	// 初始化打开文件list
	if (old_global->open_files.part.nelts) {
		n = old_global->open_files.part.nelts;
		for (part = old_global->open_files.part.next; part; part = part->next) {
			n += part->nelts;
		}

	} else {
		n = 20;
	}

	if (spy_list_init(&global->open_files, pool, n, sizeof(spy_open_file_t))
			!= SPY_OK) {
		spy_destroy_pool(pool);
		return NULL;
	}

	// 初始化监听
	n = old_global->listening.nelts ? old_global->listening.nelts : 10;

	global->listening.elts = spy_pcalloc(pool, n * sizeof(spy_listening_t));
	if (global->listening.elts == NULL) {
		spy_destroy_pool(pool);
		return NULL;
	}

	global->listening.nelts = 0;
	global->listening.size = sizeof(spy_listening_t);
	global->listening.nalloc = n;
	global->listening.pool = pool;

	// 打开文件
	part = &global->open_files.part;
	file = part->elts;

	for (i = 0; /* void */; i++) {

		if (i >= part->nelts) {
			if (part->next == NULL) {
				break;
			}
			part = part->next;
			file = part->elts;
			i = 0;
		}

		if (file[i].name.len == 0) {
			continue;
		}

		file[i].fd = spy_open_file(file[i].name.data, SPY_FILE_APPEND,
				SPY_FILE_CREATE_OR_OPEN, SPY_FILE_DEFAULT_ACCESS);

		spy_log_debug(SPY_LOG_DEBUG_CORE, log, 0, "log: %p %d \"%s\"",
				&file[i], file[i].fd, file[i].name.data);

		if (file[i].fd == SPY_INVALID_FILE) {
			spy_log_error(SPY_LOG_EMERG, log, spy_errno,
					spy_open_file_n " \"%s\" failed", file[i].name.data);
			goto failed;
		}

		if (fcntl(file[i].fd, F_SETFD, FD_CLOEXEC) == -1) {
			spy_log_error(SPY_LOG_EMERG, log, spy_errno,
					"fcntl(FD_CLOEXEC) \"%s\" failed", file[i].name.data);
			goto failed;
		}
	}

	// 设置新日志
	if (global->new_log.file == NULL) {
		global->new_log.file = spy_conf_open_file(global, &error_log);
		if (global->new_log.file == NULL) {
			goto failed;
		}
	}

	// 设置新的日志
	global->log = &global->new_log;
	pool->log = &global->new_log;

	// 处理监听
	if (old_global->listening.nelts) {
		ls = old_global->listening.elts;
		for (i = 0; i < old_global->listening.nelts; i++) {
			ls[i].remain = 0;
		}

		nls = global->listening.elts;
		for (n = 0; n < global->listening.nelts; n++) {

			for (i = 0; i < old_global->listening.nelts; i++) {

				// 如果存在一样的sockaddr，不必重新打开
				if (spy_cmp_sockaddr(nls[n].sockaddr, ls[i].sockaddr) == SPY_OK) {
					nls[n].fd = ls[i].fd;
					//nls[n].previous = &ls[i];
					ls[i].remain = 1;

					if (ls[n].backlog != nls[i].backlog) {
						nls[n].listen = 1;
					}

					break;
				}
			}

			if (nls[n].fd == -1) {
				nls[n].open = 1;
			}
		}

	} else {
		ls = global->listening.elts;
		for (i = 0; i < global->listening.nelts; i++) {
			ls[i].open = 1;
		}
	}

	// 代理初始化
	if (spy_proxy_init(global) != SPY_OK) {
		goto failed;
	}

	// 打开监听
	if (spy_open_listening_sockets(global) != SPY_OK) {
		goto failed;
	}

	// 提交新global
	if (!spy_use_stderr && global->log->file->fd != spy_stderr) {

		if (spy_set_stderr(global->log->file->fd) == SPY_FILE_ERROR) {
			spy_log_error(SPY_LOG_ALERT, global->log, spy_errno,
					spy_set_stderr_n " failed");
		}
	}

	pool->log = global->log;

	// 关闭老的listen
	ls = old_global->listening.elts;
	for (i = 0; i < old_global->listening.nelts; i++) {

		if (ls[i].remain || ls[i].fd == -1) {
			continue;
		}

		if (spy_close_socket(ls[i].fd) == -1) {
			spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
					spy_close_socket_n " listening socket on %V failed",
					&ls[i].addr_text);
		}
	}

	// 关闭不需要的文件
	part = &old_global->open_files.part;
	file = part->elts;

	for (i = 0; /* void */; i++) {

		if (i >= part->nelts) {
			if (part->next == NULL) {
				break;
			}
			part = part->next;
			file = part->elts;
			i = 0;
		}

		if (file[i].fd == SPY_INVALID_FILE || file[i].fd == spy_stderr) {
			continue;
		}

		if (spy_close_file(file[i].fd) == SPY_FILE_ERROR) {
			spy_log_error(SPY_LOG_EMERG, log, spy_errno,
					spy_close_file_n " \"%s\" failed", file[i].name.data);
		}
	}

	// 预删除old_global
	if (spy_temp_pool == NULL) {
		spy_temp_pool = spy_create_pool(128, global->log);
		if (spy_temp_pool == NULL) {
			spy_log_error(SPY_LOG_EMERG, global->log, 0,
					"could not create ngx_temp_pool");
			exit(1);
		}

		n = 10;
		spy_old_globals.elts = spy_pcalloc(spy_temp_pool, n
				* sizeof(spy_global_t *));
		if (spy_old_globals.elts == NULL) {
			exit(1);
		}
		spy_old_globals.nelts = 0;
		spy_old_globals.size = sizeof(spy_global_t *);
		spy_old_globals.nalloc = n;
		spy_old_globals.pool = spy_temp_pool;

		spy_cleaner_event.handler = spy_clean_old_global;
		spy_cleaner_event.log = global->log;
		spy_cleaner_event.data = &dumb;
		dumb.fd = (spy_socket_t) -1;
	}

	spy_temp_pool->log = global->log;

	old = spy_array_push(&spy_old_cycles);
	if (old == NULL) {
		exit(1);
	}
	*old = old_global;

	if (!spy_cleaner_event.timer_set) {
		spy_add_timer(&spy_cleaner_event, 30000);
		spy_cleaner_event.timer_set = 1;
	}

	return global;

	failed:

	/* 回滚新配置 */
	file = part->elts;

	for (i = 0; /* void */; i++) {

		if (i >= part->nelts) {
			if (part->next == NULL) {
				break;
			}
			part = part->next;
			file = part->elts;
			i = 0;
		}

		if (file[i].fd == SPY_INVALID_FILE || file[i].fd == spy_stderr) {
			continue;
		}

		if (spy_close_file(file[i].fd) == SPY_FILE_ERROR) {
			spy_log_error(SPY_LOG_EMERG, log, spy_errno,
					spy_close_file_n " \"%s\" failed", file[i].name.data);
		}
	}

	ls = global->listening.elts;
	for (i = 0; i < global->listening.nelts; i++) {
		if (ls[i].fd == -1 || !ls[i].open) {
			continue;
		}

		if (spy_close_socket(ls[i].fd) == -1) {
			spy_log_error(SPY_LOG_EMERG, log, spy_socket_errno,
					spy_close_socket_n " %S failed", &ls[i].addr_text);
		}
	}

	spy_destroy_pool(pool);

	return NULL;
}

#if 0
static void spy_destroy_cycle_pools(spy_conf_t *conf) {
	spy_destroy_pool(conf->temp_pool);
	spy_destroy_pool(conf->pool);
}
#endif

static spy_int_t spy_cmp_sockaddr(struct sockaddr *sa1, struct sockaddr *sa2) {
	struct sockaddr_in *sin1, *sin2;

	if (sa1->sa_family != sa2->sa_family) {
		return SPY_DECLINED;
	}

	switch (sa1->sa_family) {

	default: /* AF_INET */

		sin1 = (struct sockaddr_in *) sa1;
		sin2 = (struct sockaddr_in *) sa2;

		if (sin1->sin_port != sin2->sin_port) {
			return SPY_DECLINED;
		}

		if (sin1->sin_addr.s_addr != sin2->sin_addr.s_addr) {
			return SPY_DECLINED;
		}

		break;
	}

	return SPY_OK;
}

static void spy_clean_old_global(spy_event_t *ev) {
	spy_uint_t i, n, found, live;
	spy_log_t *log;
	spy_global_t **global;

	log = spy_global->log;
	spy_temp_pool->log = log;

	spy_log_debug(SPY_LOG_DEBUG_CORE, log, 0, "%s", "clean old globals");

	live = 0;

	/* 遍历所有旧global */
	global = spy_old_globals.elts;
	for (i = 0; i < spy_old_globals.nelts; i++) {

		if (global[i] == NULL) {
			continue;
		}

		found = 0;

		for (n = 0; n < global[i]->connection_n; n++) {
			/* 發現還未處理完的連接 */
			if (global[i]->connections[n].fd != (spy_socket_t) -1) {
				found = 1;

				spy_log_debug(SPY_LOG_DEBUG_CORE, log, 0, "live fd:%d", n);

				break;
			}
		}

		if (found) {
			live = 1;
			continue;
		}

		spy_log_debug(SPY_LOG_DEBUG_CORE, log, 0, "clean old global: %d", i);

		spy_destroy_pool(global[i]->pool);
		global[i] = NULL;
	}

	spy_log_debug(SPY_LOG_DEBUG_CORE, log, 0, "old global status: %d", live);

	if (live) {
		/* 设置过期事件超时 */
		spy_add_timer(ev, 30000);

	} else {
		spy_destroy_pool(spy_temp_pool);
		spy_temp_pool = NULL;
		spy_old_globals.nelts = 0;
	}
}
