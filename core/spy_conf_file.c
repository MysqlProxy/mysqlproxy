#include <spy_config.h>
#include <spy_core.h>

static spy_int_t spy_parse_core_conf(spy_conf_t *cf);
static spy_lua_state *conf_state;

static spy_conf_handler_pt spy_conf_module_handlers[] = { spy_parse_core_conf };

static char* spy_conf_module_names[] = { "core" };

static spy_int_t spy_parse_core_conf(spy_conf_t *cf) {

	spy_uint_t l;
	spy_core_conf_t *data;
	spy_lua_state *L = conf_state;

	data = (spy_core_conf_t *) malloc(sizeof(spy_core_conf_t));

	spy_lua_getfield(L, -1, cf->name);

	if (!spy_lua_istable(L, -1)) {
		return SPY_ERROR;
	}

	// get timeout key
	if (!spy_lua_get_field_uint(L, "timeout", &(data->timeout)))
		return SPY_ERROR;

	// get port key
	if (!spy_lua_get_field_uint(L, "port", &(data->port)))
		return SPY_ERROR;

	// get listen table
	spy_lua_getfield(L, -1, "listen");

	if (!spy_lua_istable(L, -1)) {
		return SPY_ERROR;
	}

	// get listen table length
	l = spy_lua_objlen(L, -2);
	data->listen_n = l;

	// pop cf->core->listen
	spy_lua_pop(conf_state, 1);

	// pop cf->core
	spy_lua_pop(conf_state, 1);

	cf->data = data;

	return SPY_OK;
}

spy_int_t spy_conf_init(spy_global_t *proxy) {

	int i;

	spy_conf_t *conf;

	if (!spy_lua_loadfile(proxy->conf_path, &conf_state))
		return SPY_ERROR;

	// get spy table, index : -1
	spy_lua_getglobal(conf_state, "spy");

	if (!spy_lua_istable(conf_state, -1))
		return SPY_ERROR;

	conf = malloc(sizeof(spy_conf_t) * SPY_CONF_MAX_INDEX);

	for (i = SPY_CONF_MIN_INDEX; i < SPY_CONF_MAX_INDEX; i++) {
		conf[i].handler = spy_conf_module_handlers[i];
		conf[i].name = spy_conf_module_names[i];

		if (!spy_conf_module_handlers[i](&conf[i]))
			return SPY_ERROR;
	}

	proxy->conf = conf;

	return SPY_OK;
}

