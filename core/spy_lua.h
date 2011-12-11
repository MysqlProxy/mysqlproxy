#ifndef _SPY_LUA_H_INCLUDED_
#define _SPY_LUA_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

typedef lua_State spy_lua_state;


spy_int_t
spy_lua_loadfile(const char *filename, spy_lua_state **L);

void
spy_lua_close(spy_lua_state *L);

#define spy_lua_getglobal(L, name)		lua_getglobal(L, name)
#define spy_lua_isnumber(L, index)		lua_isnumber(L, index)
#define spy_lua_tonumber(L, index)		lua_tonumber(L, index)

#endif
