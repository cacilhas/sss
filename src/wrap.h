#ifndef _WRAP_H
#define _WRAP_H

/* copyright ©2014, Rodrigo Cacilhας <batalema@cacilhas.info> */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void table_setnumber(lua_State *, const char *, LUA_NUMBER);
void table_setstring(lua_State *, const char *, const char *);

void wrap_push(lua_State *, void *, const char *);
void *wrap_pull(lua_State *, int, const char *);

#ifdef __cplusplus
}
#endif

#endif /* _WRAP_H */
