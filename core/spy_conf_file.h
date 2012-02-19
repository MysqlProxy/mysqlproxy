#ifndef _SPY_CONF_FILE_INCLUDE_H_
#define _SPY_CONF_FILE_INCLUDE_H_

#include <spy_config.h>
#include <spy_core.h>

#define SPY_CORE_CONF	0

#define SPY_CONF_MIN_INDEX 0
#define SPY_CONF_MAX_INDEX 1

typedef struct spy_core_conf_s spy_core_conf_t;
typedef spy_int_t (*spy_conf_handler_pt)(spy_conf_t *cf);

struct spy_conf_s {

	void *data;

	const char *name;

	spy_conf_handler_pt handler;

	spy_pool_t *pool;
	spy_log_t *log;
};

struct spy_core_conf_s {
	spy_array_t listen_addr;
	spy_array_t debug_level;
	spy_str_t error_log;
	spy_str_t error_level;
	spy_uint_t worker_connections;
};

struct spy_open_file_s {
	spy_fd_t fd;
	spy_str_t name;

	u_char *buffer;
	u_char *pos;
	u_char *last;
};

spy_int_t spy_conf_init(spy_global_t *global);
spy_open_file_t *
spy_conf_open_file(spy_global_t *global, spy_str_t *name);
spy_int_t spy_conf_full_name(spy_global_t *global, spy_str_t *name,
		spy_uint_t conf_prefix);

#define spy_get_core_conf(global)	global->conf[SPY_CORE_CONF].data

#endif
