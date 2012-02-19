#include <spy_config.h>
#include <spy_core.h>

static spy_int_t spy_parse_core_conf(spy_conf_t *cf);
static spy_lua_state *conf_state;

static spy_int_t spy_conf_test_full_name(spy_str_t *name);

static spy_conf_handler_pt spy_conf_module_handlers[] = { spy_parse_core_conf };

static char* spy_conf_module_names[] = { "core" };

static spy_int_t spy_parse_core_conf(spy_conf_t *cf) {

	u_char *p;
	spy_core_conf_t *data;
	spy_lua_state *L = conf_state;

	data = (spy_core_conf_t *) spy_palloc(cf->pool, sizeof(spy_core_conf_t));

	spy_lua_getfield(L, -1, cf->name);

	if (!spy_lua_istable(L, -1)) {
		return SPY_ERROR;
	}

#if 0
	// get port key
	if (spy_lua_getkey_field_uint(L, "port", &(data->port)) != SPY_OK)
	return SPY_ERROR;
#endif

	/* get listen addr */
	if (spy_lua_getkey_field_str(L, "listen_addr", p) != SPY_OK)
		return SPY_ERROR;

	/* get work_connections key */
	if (spy_lua_getkey_field_uint(L, "work_connections",
			&(data->worker_connections)) != SPY_OK)
		return SPY_ERROR;

	/* get error log path */
	if (spy_lua_getkey_field_str(L, "error_log", p) != SPY_OK)
		return SPY_ERROR;

	spy_str_set(&data->error_log, p);

	/* get error level */
	if (spy_lua_getkey_field_str(L, "error_level", p) != SPY_OK)
		return SPY_ERROR;

	spy_str_set(&data->error_level, p);

	/* get debug level */
	if (spy_lua_getkey_field_str(L, "debug_level", p) != SPY_OK)
		return SPY_ERROR;

#if 0
	// get listen table
	spy_lua_getfield(L, -1, "listen");

	if (!spy_lua_istable(L, -1)) {
		return SPY_ERROR;
	}

	// get listen table length
	l = spy_lua_objlen(L, -1);
	data->listen_n = l;

	data->listen = spy_palloc(cf->pool, l * SPY_INET_ADDRSTRLEN);

	for (i = 1; i <= l; i++) {
		spy_lua_getindex_field_str(L, i, (u_char *) data->listen[i - 1]);
	}
#endif

	spy_lua_pop(conf_state, 1);

	cf->data = data;

	return SPY_OK;
}

spy_int_t spy_conf_init(spy_global_t *global) {

	size_t i;
	spy_conf_t *conf;

	if (spy_lua_loadfile((const char *) global->conf_file.data, &conf_state)
			!= SPY_OK)
		return SPY_ERROR;

	// get spy table, index : -1
	spy_lua_getglobal(conf_state, "spy");

	if (!spy_lua_istable(conf_state, -1))
		return SPY_ERROR;

	conf = spy_palloc(global->pool, sizeof(spy_conf_t) * SPY_CONF_MAX_INDEX);

	for (i = SPY_CONF_MIN_INDEX; i < SPY_CONF_MAX_INDEX; i++) {
		conf[i].log = global->log;
		conf[i].handler = spy_conf_module_handlers[i];
		conf[i].name = spy_conf_module_names[i];
		conf[i].pool = global->pool;

		if (spy_conf_module_handlers[i](&conf[i]) != SPY_OK)
			return SPY_ERROR;
	}

	global->conf = conf;
	global->endian = spy_get_endian();

	return SPY_OK;
}

spy_open_file_t *
spy_conf_open_file(spy_global_t *global, spy_str_t *name) {
	spy_str_t full;
	spy_uint_t i;
	spy_list_part_t *part;
	spy_open_file_t *file;

	if (name->len) {
		full = *name;

		if (spy_conf_full_name(global, &full, 0) != SPY_OK) {
			return NULL;
		}

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

			if (full.len != file[i].name.len) {
				continue;
			}

			if (spy_strcmp(full.data, file[i].name.data) == 0) {
				return &file[i];
			}
		}
	}

	file = spy_list_push(&global->open_files);
	if (file == NULL) {
		return NULL;
	}

	if (name->len) {
		file->fd = SPY_INVALID_FILE;
		file->name = full;

	} else {
		file->fd = spy_stderr;
		file->name = *name;
	}

	file->buffer = NULL;

	return file;
}

// 合并出完整名
spy_int_t spy_conf_full_name(spy_global_t *global, spy_str_t *name,
		spy_uint_t conf_prefix) {
	size_t len;
	u_char *p, *n, *prefix;
	spy_int_t rc;

	rc = spy_conf_test_full_name(name);

	if (rc == SPY_OK) {
		return rc;
	}

	if (conf_prefix) {
		len = global->conf_prefix.len;
		prefix = global->conf_prefix.data;

	} else { /* 依据prefix设置 */
		len = global->prefix.len;
		prefix = global->prefix.data;
	}

	n = spy_pnalloc(global->pool, len + name->len + 1);
	if (n == NULL) {
		return SPY_ERROR;
	}

	p = spy_cpymem(n, prefix, len);
	spy_cpystrn(p, name->data, name->len + 1);

	name->len += len;
	name->data = n;

	return SPY_OK;
}

// 判斷是否是全路徑
static spy_int_t spy_conf_test_full_name(spy_str_t *name) {

	if (name->data[0] == '/') {
		return SPY_OK;
	}

	return SPY_DECLINED;
}

static void spy_conf_push_split_str(spy_array_t *arr, u_char *str, u_char sep) {
	size_t l;
	u_char *p;

	p = str;
	while (*p != sep)
		l++;
}
