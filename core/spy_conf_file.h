#ifndef _SPY_CONF_FILE_INCLUDE_H_
#define _SPY_CONF_FILE_INCLUDE_H_

#include <spy_config.h>
#include <spy_core.h>

#define SPY_CORE_CONF	1

#define SPY_CONF_MIN_INDEX 0
#define SPY_CONF_MAX_INDEX 1

typedef struct spy_core_conf_s spy_core_conf_t;
typedef spy_int_t (*spy_conf_handler_pt)(spy_conf_t *cf);

struct spy_conf_s {

	void *data;

	const char *name;

	spy_conf_handler_pt handler;
};

struct spy_core_conf_s {
	spy_uint_t port;
	spy_uint_t timeout;
	spy_uint_t listen_n;
	u_char *listen[SPY_INET_ADDRSTRLEN];
};

spy_int_t spy_conf_init(spy_global_t *proxy);

#endif
