#include <spy_config.h>
#include <spy_core.h>

#if 0
static int spy_lua_atpanic(spy_lua_state *L) {
	//fprintf(stderr, "%s", lua_tostring(L, -1));
	lua_pop(*L, 1);/* pop error message from the stack */
	return SPY_ERROR;
}
#endif

#if 0
static void spy_lua_stack_dump(spy_lua_state *L);
#endif

static void *
spy_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize) {
	(void) ud;
	(void) osize; /* not used */
	if (nsize == 0) {
		free(ptr);
		return NULL;
	} else
		return realloc(ptr, nsize);
}

spy_int_t spy_lua_loadfile(const char *filename, spy_lua_state **L) {
	*L = lua_newstate(spy_lua_alloc, 0);
	//lua_atpanic(*L, spy_lua_atpanic);
	luaL_openlibs(*L);
	if (luaL_loadfile(*L, filename) || lua_pcall(*L, 0, 0, 0)) {
		spy_log_stdout("%s", spy_lua_tostring(*L, -1));
		lua_pop(*L, 1);/* pop error message from the stack */
		return SPY_ERROR;
	}

	return SPY_OK;
}

void spy_lua_close(spy_lua_state *L) {
	lua_close(L);

}

spy_int_t spy_lua_getkey_field_int(spy_lua_state *L, const char *key,
		spy_int_t *val) {
	// param : timeout, index : -1 ,spy table index : -2
	spy_lua_pushstring(L, key);

	//get spy.xxx.timeout
	spy_lua_gettable(L, -2);

	if (!spy_lua_isnumber(L, -1)) {
		return SPY_ERROR;
	}

	*val = (spy_int_t) spy_lua_tonumber(L, -1);

	// remove number
	spy_lua_pop(L, 1);

	return SPY_OK;
}

spy_int_t spy_lua_getkey_field_uint(spy_lua_state *L, const char *key,
		spy_uint_t *val) {
	// param : timeout, index : -1 ,spy table index : -2
	spy_lua_pushstring(L, key);

	//get spy.xxx.timeout
	spy_lua_gettable(L, -2);

	if (!spy_lua_isnumber(L, -1)) {
		return SPY_ERROR;
	}

	*val = (spy_uint_t) spy_lua_tonumber(L, -1);

	// remove number
	spy_lua_pop(L, 1);

	return SPY_OK;
}

spy_int_t spy_lua_getkey_field_str(spy_lua_state *L, const char *key,
		u_char *val) {

	spy_lua_pushstring(L, key);

	spy_lua_gettable(L, -2);

	if (!spy_lua_istring(L, -1)) {
		return SPY_ERROR;
	}

	val = (u_char *) spy_lua_tostring(L, -1);

	spy_lua_pop(L, 1);

	return SPY_OK;
}

spy_int_t spy_lua_getindex_field_str(spy_lua_state *L, spy_int_t index,
		u_char *val) {

	size_t l;
	// param : timeout, index : -1 ,spy table index : -2
	spy_lua_pushnumber(L, index);

	//get spy.xxx.timeout spy.core.listen
	spy_lua_gettable(L, -2);

	if (!spy_lua_isstring(L, -1)) {
		return SPY_ERROR;
	}

	l = spy_lua_objlen(L, -1);

	spy_memcpy(val, spy_lua_tostring(L, -1), l);

	// remove string
	spy_lua_pop(L, 1);

	return SPY_OK;
}

#if 0
static void spy_lua_stack_dump(spy_lua_state *L) {
	spy_int_t i;
	int top = spy_lua_gettop(L);

	spy_log_stdout("lua stack dump.");
	for (i = 1; i <= top; i++) { /* repeat for each level */
		int t = spy_lua_type(L, i);
		switch (t) {
			case LUA_TSTRING: /* strings */
			spy_log_stdout("`%s'", spy_lua_tostring(L, i));
			break;
			case LUA_TBOOLEAN: /* booleans */
			spy_log_stdout(spy_lua_toboolean(L, i) ? "true" : "false");
			break;
			case LUA_TNUMBER: /* numbers */
			spy_log_stdout("%d", (spy_int_t) spy_lua_tonumber(L, i));
			break;
			default: /* other values */
			spy_log_stdout("%s", spy_lua_typename(L, t));
			break;
		}
	}
	spy_log_stdout("lua stack dump end.");
}
#endif

#ifdef _SPY_LUA_UNIT_TEST_

int main() {
	spy_int_t res;
	spy_lua_state *L = NULL;

	res = spy_lua_loadfile("/home/terry/github/spy-0.0.01/lua/spy.lua", &L);
	if (res == SPY_ERROR) {
		return 1;
	}

	// get spy table, index : -1
	spy_lua_getglobal(L, "spy");

	if (!spy_lua_istable(L, -1)) {
		return 1;
	}

	spy_lua_getfield(L, -1, "core");

	if (!spy_lua_istable(L, -1)) {
		return 1;
	}

	// param : timeout, index : -1 ,spy table index : -2
	spy_lua_pushstring(L, "timeout");

	//get spy.timeout
	spy_lua_gettable(L, -2);

	if (!spy_lua_isnumber(L, -1)) {
		return 1;
	}

	int timeout = (int) spy_lua_tonumber(L, -1);

	spy_log_stdout("%d", timeout);

	// pop timeout
	spy_lua_pop(L, 1);

	// get listen table
	spy_lua_getfield(L, -1, "listen");

	if (!spy_lua_istable(L, -1)) {
		return SPY_ERROR;
	}

	spy_int_t l, i;
	l = spy_lua_objlen(L, -1);
	spy_log_stdout("l : %d", l);

	u_char(*listen)[SPY_INET_ADDRSTRLEN] =
	(u_char(*)[SPY_INET_ADDRSTRLEN]) malloc(l * SPY_INET_ADDRSTRLEN);

	for (i = 1; i <= l; i++) {
		spy_memzero(listen[i-1], SPY_INET_ADDRSTRLEN);
		spy_lua_getindex_field_str(L, i, listen[i - 1], SPY_INET_ADDRSTRLEN);

		spy_log_stdout("%s", listen[i - 1]);
	}

	spy_lua_close(L);

	return 0;
}

#endif
