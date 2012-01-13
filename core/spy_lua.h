#ifndef _SPY_LUA_H_INCLUDED_
#define _SPY_LUA_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

typedef lua_State spy_lua_state;

#define spy_lua_getglobal(L, name)		lua_getglobal(L, name)
#define spy_lua_isnumber(L, index)		lua_isnumber(L, index)
#define spy_lua_tonumber(L, index)		lua_tonumber(L, index)
#define spy_lua_istable(L, index)		lua_istable(L, index)
#define spy_lua_gettable(L, index)		lua_gettable(L, index)
#define spy_lua_pushstring(L, s)		lua_pushstring(L, s)
#define spy_lua_getfield(L, index, k)	lua_getfield(L, index, k)
#define spy_lua_pop(L, n)				lua_pop(L, n)
#define spy_lua_objlen(L, index)		lua_objlen(L, index)
#define spy_lua_tostring(L, indexs)		lua_tostring(L, index)

spy_int_t
spy_lua_loadfile(const char *filename, spy_lua_state **L);

void
spy_lua_close(spy_lua_state *L);

spy_int_t
spy_lua_get_field_int(spy_lua_state *L, const char *key, int *val);

#endif
