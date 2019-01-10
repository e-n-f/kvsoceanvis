#ifndef WINDREALM_LUA_UTILS_H
#define WINDREALM_LUA_UTILS_H

#include <lua5.1/lua.hpp>

/**
  Evaluates a Lua expression and returns the string result.
  If an error occurs or the result is not string, def is returned.
*/
const char* lua_stringexpr( lua_State* L, const char* expr, const char* def );


/**
  Evaluates a Lua expression that results in a number.
  The number is stored in out.
  Returns 0 if there's an error, 1 otherwise.
*/
int lua_numberexpr( lua_State* L, const char* expr, double* out );


/**
  Evaluates a Lua expression that results in a number and cast to an int.
  Returns 0 if there's an error, 1 otherwise.
*/
int lua_intexpr( lua_State* L, const char* expr, int* out );


/**
  Evaluates a Lua expression that results in a number.
  Returns 0 if there's an error, 1 otherwise.
*/
int lua_boolexpr( lua_State* L, const char* expr );
#endif
