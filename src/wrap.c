#include <iso646.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "wrap.h"


typedef struct {
	void *wrapped;
} wrapper_t;


void table_setnumber(lua_State *L, const char *key, LUA_NUMBER value) {
	lua_pushstring(L, key);
	lua_pushnumber(L, value);
	lua_settable(L, -3);
}


void table_setstring(lua_State *L, const char *key, const char *value) {
	lua_pushstring(L, key);
	lua_pushstring(L, value);
	lua_settable(L, -3);
}


void wrap_push(lua_State *L, void *wrapped, const char *classname) {
	wrapper_t *envelop = lua_newuserdata(L, sizeof(wrapper_t));
	envelop->wrapped = wrapped;
	luaL_getmetatable(L, classname);
	lua_setmetatable(L, -2);
}


void *wrap_pull(lua_State *L, int index, const char *classname) {
	wrapper_t *envelop = luaL_checkudata(L, index, classname);
	if (envelop)
		return envelop->wrapped;
	return NULL;
}
