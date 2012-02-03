#include <spy_config.h>
#include <spy_core.h>

static spy_int_t spy_parse_core_conf(spy_conf_t *cf);
static spy_lua_state *conf_state;

static spy_conf_handler_pt spy_conf_module_handlers[] = { spy_parse_core_conf };

static char* spy_conf_module_names[] = { "core" };

static spy_int_t spy_parse_core_conf(spy_conf_t *cf) {

	spy_uint_t l, i;
	spy_core_conf_t *data;
	spy_lua_state *L = conf_state;

	data = (spy_core_conf_t *) spy_alloc(sizeof(spy_core_conf_t), cf->log);

	spy_lua_getfield(L, -1, cf->name);

	if (!spy_lua_istable(L, -1)) {
		return SPY_ERROR;
	}

	// get timeout key
	if (spy_lua_getkey_field_uint(L, "timeout", &(data->timeout)) != SPY_OK)
		return SPY_ERROR;

	// get port key
	if (spy_lua_getkey_field_uint(L, "port", &(data->port)) != SPY_OK)
		return SPY_ERROR;

	// get max_conn key
	if (spy_lua_getkey_field_uint(L, "max_conn", &(data->max_conn)) != SPY_OK)
		return SPY_ERROR;

	// get listen table
	spy_lua_getfield(L, -1, "listen");

	if (!spy_lua_istable(L, -1)) {
		return SPY_ERROR;
	}

	// get listen table length
	l = spy_lua_objlen(L, -1);
	data->listen_n = l;

	data->listen = malloc(l * SPY_INET_ADDRSTRLEN);

	for (i = 1; i <= l; i++) {
		spy_lua_getindex_field_str(L, i, (u_char *) data->listen[i - 1]);
	}

	// pop cf->core->listen
	// pop cf->core
	spy_lua_pop(conf_state, 2);

	cf->data = data;

	return SPY_OK;
}

spy_int_t spy_conf_init(spy_global_t *global) {

	int i;

	spy_conf_t *conf;

	if (spy_lua_loadfile(global->conf_path, &conf_state) != SPY_OK)
		return SPY_ERROR;

	// get spy table, index : -1
	spy_lua_getglobal(conf_state, "spy");

	if (!spy_lua_istable(conf_state, -1))
		return SPY_ERROR;

	conf = spy_alloc(sizeof(spy_conf_t) * SPY_CONF_MAX_INDEX, global->log);

	for (i = SPY_CONF_MIN_INDEX; i < SPY_CONF_MAX_INDEX; i++) {
		conf[i].log = global->log;
		conf[i].handler = spy_conf_module_handlers[i];
		conf[i].name = spy_conf_module_names[i];

		if (spy_conf_module_handlers[i](&conf[i]) != SPY_OK)
			return SPY_ERROR;
	}

	global->conf = conf;
	global->endian = spy_get_endian();

	return SPY_OK;
}

