// The MIT License (MIT)

// Copyright 2015 Siney/Pangweiwei siney@yeah.net
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef SLUA_HEADER_HPP_
#define SLUA_HEADER_HPP_

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "memory.h"
}
#include <functional>
#include "vector3.hpp"

#define kEpsilon 0.00001f

inline void check_type(lua_State *L, int p, float& v) {
	v = (float)luaL_checknumber(L, p);
}

inline void check_type(lua_State *L, int p, Vector3& v) {
	luaL_checktype(L, p, LUA_TTABLE);
	lua_rawgeti(L, p,1);
	v.x = (float)lua_tonumber(L, -1);
	lua_rawgeti(L, p, 2);
	v.y = (float)lua_tonumber(L, -1);
	lua_rawgeti(L, p, 3);
	v.z = (float)lua_tonumber(L, -1);
	lua_pop(L, 3);
}

inline void check_type(lua_State *L, int p, int& v) {
	v = luaL_checkint(L, p);
}

template<int C=0, class H, class ...T>
inline void check_type(lua_State *L, int p, H& h, T&... Args) {
	check_type(L, p + C, h);
	check_type<C+1>(L, p, Args...);
}

template<int C, class H>
inline void check_type(lua_State *L, int p, H& h) {
	check_type(L, p + C, h);
}

inline int push_value(lua_State *L, const float& v) {
	lua_pushnumber(L, v);
	return 1;
}

template<class T>
int push_value(lua_State *L, const T& v) {
	int count = sizeof(v.data) / sizeof(v.data[0]);
	lua_createtable(L, count, 0);
	for (int n = 0; n < count; ++n) {
		push_value(L, v.data[n]);
		lua_rawseti(L, -2, n + 1);
	}
	return 1;
}

template<class T>
void add_field(lua_State *L, const char* key, const T& v) {
	push_value(L, v);
	lua_setfield(L, -2, key);
}

inline void set_back(lua_State *L, const Vector3& v) {
	lua_pushnumber(L, v.x);
	lua_rawseti(L, 1, 1);
	lua_pushnumber(L, v.y);
	lua_rawseti(L, 1, 2);
	lua_pushnumber(L, v.z);
	lua_rawseti(L, 1, 3);
}

template<class F>
struct LuaFunction {

};



template <class ...Args>
struct MethodInfo {};


template<class R,class F, class ...A class ...Checker>
int invoke(lua_State *L,F f,MethodInfo<R,A....>,Checker** ...chk) 
{

}

template<class R,class ...A>
struct LuaFunction< R(A...) > 
{
	typedef R(A...) FuncType;

	static FuncType func;
	static int wrap(lua_State *L) {
		return invoke<R>(L,func,MethodInfo<R,A...>(),checker<I,A>::type()...);
	}
};

template<class R,class C,class ...A>
struct LuaFunction< R(C::*)(A...) >
{
	typedef R(C::*)(A...) FuncType;
	static FuncType func;
	static int wrap(lua_State *L) {
		return 0;
	}
};


template<class T>
struct ValueType {

	lua_State *L;
	ValueType(lua_State *L, const char* name):L(L) {
		
	}

	template<class F>
	ValueType<T>& method(const char* name, F func) {
		LuaFunction<F>::func = func;
		lua_pushcfunction(L, LuaFunction<F>::wrap);
		return *this;
	}

};


template<class T>
ValueType<T> class_def(lua_State *L, const char* name) {
	ValueType<T> vt(L, name);
	return vt;
}


#endif