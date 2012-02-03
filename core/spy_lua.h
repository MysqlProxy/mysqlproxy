#ifndef _SPY_LUA_H_INCLUDED_
#define _SPY_LUA_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

typedef lua_State spy_lua_state;
typedef lua_Integer spy_lua_integer;

#define spy_lua_getglobal(L, name)		lua_getglobal(L, name)
#define spy_lua_isnumber(L, index)		lua_isnumber(L, index)
#define spy_lua_tonumber(L, index)		lua_tonumber(L, index)
#define spy_lua_istable(L, index)		lua_istable(L, index)
#define spy_lua_gettable(L, index)		lua_gettable(L, index)
#define spy_lua_pushstring(L, s)		lua_pushstring(L, s)
#define spy_lua_pushnumber(L, n)		lua_pushnumber(L, n)
#define spy_lua_getfield(L, index, k)	lua_getfield(L, index, k)
#define spy_lua_pop(L, n)				lua_pop(L, n)
#define spy_lua_objlen(L, index)		lua_objlen(L, index)
#define spy_lua_tostring(L, index)		lua_tostring(L, index)
#define spy_lua_isstring(L, index)		lua_isstring(L, index)
#define spy_lua_gettop(L)				lua_gettop(L)
#define spy_lua_type(L, i)				lua_type(L, i)
#define spy_lua_typename(L, t)			lua_typename(L, t)
#define spy_lua_toboolean(L, index)		lua_toboolean(L, index)

spy_int_t
spy_lua_loadfile(const char *filename, spy_lua_state **L);

void
spy_lua_close(spy_lua_state *L);

spy_int_t
spy_lua_getkey_field_int(spy_lua_state *L, const char *key, spy_int_t *val);

spy_int_t
spy_lua_getkey_field_uint(spy_lua_state *L, const char *key, spy_uint_t *val);

spy_int_t spy_lua_getindex_field_str(spy_lua_state *L, spy_int_t index,
		u_char *val);

#endif
