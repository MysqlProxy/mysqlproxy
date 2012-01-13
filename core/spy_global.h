#ifndef _SPY_GLOBAL_H_INCLUDED_
#define _SPY_GLOBAL_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

struct spy_global_s {

	//spy_listening_t *listen;
	const char* conf_path;

	spy_conf_t *conf;

	spy_log_t *log;

	spy_connection_t *connections;
	spy_uint_t connection_n;

	spy_connection_t *free_connections;
	spy_uint_t free_connection_n;

	spy_event_t *read_events;
	spy_event_t *write_events;

	spy_uint_t listening_n;
	spy_listening_t *listening;
};

extern volatile spy_global_t *spy_global;

#endif
