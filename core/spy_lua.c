#include <spy_config.h>
#include <spy_core.h>

#if 0
static int spy_lua_atpanic(spy_lua_state *L) {
	//fprintf(stderr, "%s", lua_tostring(L, -1));
	lua_pop(*L, 1);/* pop error message from the stack */
	return SPY_ERROR;
}
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
		//fprintf(stderr, "%s", lua_tostring(L, -1));
		lua_pop(*L, 1);/* pop error message from the stack */
		return SPY_ERROR;
	}

	return SPY_OK;
}

void spy_lua_close(spy_lua_state *L) {
	lua_close(L);
}

#ifdef _SPY_LUA_UNIT_TEST_

int main() {
	spy_int_t res;
	spy_lua_state *L = NULL;

	res = spy_lua_loadfile("/home/terry/Documents/lua/luatest.lua", &L);
	if (res == SPY_ERROR)
		return 1;

	spy_lua_getglobal(L, "test");
	int test = spy_lua_tonumber(L, -1);
	printf("%d\n", test);

	spy_lua_close(L);

	return 0;
}

#endif
