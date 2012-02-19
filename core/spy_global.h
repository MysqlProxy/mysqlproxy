#ifndef _SPY_GLOBAL_H_INCLUDED_
#define _SPY_GLOBAL_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

struct spy_global_s {

	//spy_listening_t *listen;
	spy_str_t conf_file;
	spy_str_t conf_prefix;
	spy_str_t prefix;

	spy_conf_t *conf;
	spy_global_t *old_global;

	spy_log_t *log;
	spy_log_t new_log;
	spy_pool_t *pool;

	spy_connection_t *connections;
	spy_uint_t connection_n;

	spy_connection_t *free_connections;
	spy_uint_t free_connection_n;

	spy_event_t *read_events;
	spy_event_t *write_events;

	spy_array_t listening; // 监听数组

	spy_list_t open_files;

	spy_int_t endian;
};

extern volatile spy_global_t *spy_global;

spy_global_t *spy_global_init(spy_global_t *old_global);

#endif
