#include <spy_config.h>
#include <spy_core.h>

spy_int_t spy_lua_loadfile(const char *filename, spy_lua_state *L) {
	L = lua_open();
	luaL_openlibs(L);
	if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0)) {
		//fprintf(stderr, "%s", lua_tostring(L, -1));
		lua_pop(L, 1);/* pop error message from the stack */
		return SPY_ERROR;
	}


	spy_lua_getglobal(L, "i");
	int test = spy_lua_tonumber(L, -1);
	printf("%d\n", test);
	return SPY_OK;
}

void spy_lua_close(spy_lua_state *L) {
	lua_close(L);
}

#ifdef _SPY_LUA_UNIT_TEST_

#if 0
int main() {
	spy_int_t res;
	spy_lua_state *L;

	res = spy_lua_loadfile("/home/terry/Documents/lua/luatest.lua", L);
	if (res == SPY_ERROR)
		return 1;

	spy_lua_close(L);

	return 0;
}
#endif
#endif
