
#include "luautils.h"

const char* lua_stringexpr( lua_State* L, const char* expr,
                            const char* def ) {
  const char* r = def ;
  char buf[256] = "" ;
  /* Assign the Lua expression to a Lua global variable. */
  sprintf( buf, "evalExpr=%s", expr );
  if ( !luaL_dostring( L, buf ) ) {
    /* Get the value of the global varibable */
    lua_getglobal( L, "evalExpr" );
    if ( lua_isstring( L, -1 ) ) {
      r = lua_tostring( L, -1 );
    }
    /* remove lua_getglobal value */
    lua_pop( L, 1 );
  }
  return r ;
}

int lua_numberexpr( lua_State* L, const char* expr, double* out ) {
  int ok = 0 ;
  char buf[256] = "" ;
  /* Assign the Lua expression to a Lua global variable. */
  sprintf( buf, "evalExpr=%s", expr );
  if ( !luaL_dostring( L, buf ) ) {
    /* Get the value of the global varibable */
    lua_getglobal( L, "evalExpr" );
    if ( lua_isnumber( L, -1 ) ) {
      *out = lua_tonumber( L, -1 );
      ok = 1;
    }
    /* remove lua_getglobal value */
    lua_pop( L, 1 );
  }
  return ok ;
}

int lua_intexpr( lua_State* L, const char* expr, int* out ) {
  double d ;
  int ok = lua_numberexpr( L, expr, &d );
  if ( ok ) {
    *out = (int) d ;
  }
  return ok ;
}

int lua_boolexpr( lua_State* L, const char* expr ) {
  int ok = 0 ;
  char buf[256] = "" ;
  /* Assign the Lua expression to a Lua global variable. */
  sprintf( buf, "evalExpr=%s", expr );
  if ( !luaL_dostring( L, buf ) ) {
    /* Get the value of the global varibable */
    lua_getglobal( L, "evalExpr" );
    if ( lua_isboolean( L, -1 ) ) {
      ok = lua_toboolean( L, -1 );
    }
    /* remove lua_getglobal value */
    lua_pop( L, 1 );
  }
  return ok ;
}
