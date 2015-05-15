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
#include <vector>
#include <cmath>
#include <utility>
#include "math.hpp"
#include "vector3.hpp"


template<class R>
inline R check_type(lua_State *L, int p);

template<>
inline float check_type(lua_State *L, int p) {
	return (float)luaL_checknumber(L, p);
}

template<>
inline Vector3 check_type(lua_State *L, int p) {
	luaL_checktype(L, p, LUA_TTABLE);
	lua_rawgeti(L, p, 1);
	Vector3 v;
	v.x = (float)lua_tonumber(L, -1);
	lua_rawgeti(L, p, 2);
	v.y = (float)lua_tonumber(L, -1);
	lua_rawgeti(L, p, 3);
	v.z = (float)lua_tonumber(L, -1);
	lua_pop(L, 3);
	return v;
}

template<>
inline int check_type(lua_State *L, int p) {
	int v = luaL_checkint(L, p);
	return v;
}

inline int push_value(lua_State *L, const float& v) {
	lua_pushnumber(L, v);
	return 1;
}

inline int push_value(lua_State *L, std::string&& str) {
	lua_pushlstring(L, str.c_str(), str.size());
	return 1;
}

inline int push_value(lua_State *L, const std::string& str) {
	lua_pushlstring(L, str.c_str(), str.size());
	return 1;
}

inline int push_value(lua_State *L, const bool& b) {
	lua_pushboolean(L, b);
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
	lua_getref(L, T::meta_ref);
	lua_setmetatable(L, -2);
	return 1;
}



template<class T>
void add_field(lua_State *L, const char* key, const T& v) {
	push_value(L, v);
	lua_setfield(L, -2, key);
}

inline void set_back(lua_State *L, int p,const Vector3& v) {
	lua_pushnumber(L, v.x);
	lua_rawseti(L, 1, 1);
	lua_pushnumber(L, v.y);
	lua_rawseti(L, 1, 2);
	lua_pushnumber(L, v.z);
	lua_rawseti(L, p, 3);
}

template <int... N>
struct index_tuple
{};

template <int N, int I, class Indices>
struct make_index_tuple_aux;

template <int N, int I, int... Indices>
struct make_index_tuple_aux<N, I, index_tuple<Indices...> >
	: make_index_tuple_aux < N - 1, I + 1, index_tuple<Indices..., I> >
{};

template <int I, int... Indices>
struct make_index_tuple_aux < 0, I, index_tuple<Indices...> >
{
	typedef index_tuple<Indices...> type;
};

template <int N>
struct make_index_tuple
	: make_index_tuple_aux < N, 0, index_tuple<> >
{};

struct void_result
{};

struct value_result
{};

struct true_type {};
struct false_type {};

template<class R>
struct make_result_push {
	typedef value_result type;
};

template<>
struct make_result_push<void> {
	typedef void_result type;
};



template <class F, class RC, class... Args>
inline int invoke_function(
	lua_State* L, F const& f, false_type, RC 
	, Args&&... args)
{
	push_value(L, f(std::forward<Args>(args)...));
	return 1;
}

template <class F, class... Args>
inline int invoke_function(
	lua_State* L, F const& f, false_type, void_result, Args&&... args)
{
	f(std::forward<Args>(args)...);
	return 0;
}

template <class F, class RC, class This, class... Args>
inline int invoke_function(
	lua_State* L, F const& f, true_type, RC 
	, This&& this_, Args&&... args)
{
	push_value(L, (this_.*f)(std::forward<Args>(args)...));
	return 1;
}

template <class F, class This, class... Args>
inline int invoke_function(
	lua_State* L, F const& f, true_type, void_result, This&& this_, Args&&... args)
{
	(this_.*f)(std::forward<Args>(args)...);
	return 0;
}



template < class T > 
struct deduce_type
{
	typedef T type;
};

template < class T >
struct deduce_type<const T&>
{
	typedef T type;
};

template <class R, class C, class ...A, int ...I>
int invoke_aux(lua_State *L, R(C::*f)(A...), index_tuple<I...>) {
	
	int indices[sizeof...(A)+1] = { I... };

	C self = check_type<typename deduce_type<C>::type>(L, 1);

	int r = invoke_function(L, f, true_type(), 
		typename make_result_push<R>::type(), self, 
		check_type<typename deduce_type<A>::type>(L, indices[I])...);
	set_back(L,1, self);
	return r;
}

template <class R, class C, class ...A, int ...I>
int invoke_aux(lua_State *L, R(C::*f)(A...) const, index_tuple<I...>) {

	int indices[sizeof...(A)+1] = { I... };

	C self = check_type<typename deduce_type<C>::type>(L, 1);

	int r = invoke_function(L, f, true_type(), 
		typename make_result_push<R>::type(), self,
	 	check_type<typename deduce_type<A>::type>(L, indices[I])...);
	set_back(L,1, self);
	return r;
}

template <class R, class ...A, int ...I>
int invoke_aux(lua_State *L, R(*f)(A...), index_tuple<I...>) {

	int indices[sizeof...(A)+1] = { I... };
	return invoke_function(L, f, false_type(), typename make_result_push<R>::type(), 
		check_type<typename deduce_type<A>::type>(L, indices[I]+1)...);
}


template <class R,class C, class ...A>
int invoke(lua_State *L,R(C::*f)(A...))
{
	return invoke_aux(L, f, typename make_index_tuple<sizeof...(A)>::type());
}

template <class R, class C, class ...A>
int invoke(lua_State *L, R(C::*f)(A...) const)
{
	return invoke_aux(L, f, typename make_index_tuple<sizeof...(A)>::type());
}

template <class R, class ...A>
int invoke(lua_State *L, R(*f)(A...))
{
	return invoke_aux(L, f, typename make_index_tuple<sizeof...(A)>::type());
}

template<class F>
struct LuaFunction
{
	static std::vector<F> func;
	static int wrap(lua_State *L) {
		int n = lua_tointeger(L, lua_upvalueindex(1));
		F f = func[n];
		return invoke(L, f);
	}
};

template<class F> 
std::vector<F> LuaFunction<F>::func;


template<class T>
struct ValueType {

	lua_State *L;
	ValueType(lua_State *L,const char* name):L(L) {
		lua_getglobal(L, "UnityEngine");
		if (lua_istable(L, -1)) {
			lua_pushstring(L, name);
			lua_rawget(L, -2);
			luaL_newmetatable(L, T::meta_name);
			lua_pushvalue(L, -1);
			T::meta_ref = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		lua_remove(L, 1);
	}

	template<class F>
	ValueType<T>& method(const char* name, F func) {
		int n = (int) LuaFunction<F>::func.size();
		LuaFunction<F>::func.push_back(func);
		lua_pushstring(L, name);
		lua_pushinteger(L, n);
		lua_pushcclosure(L, LuaFunction<F>::wrap, 1);
		lua_rawset(L, -3);
		return *this;
	}

	ValueType<T>& method(const char* name, lua_CFunction func) {
		lua_pushstring(L, name);
		lua_pushcfunction(L, func);
		lua_rawset(L, -3);
		return *this;
	}

	template<class F>
	ValueType<T>& func(const char* name, F func) {
		int n = (int)LuaFunction<F>::func.size();
		LuaFunction<F>::func.push_back(func);
		lua_pushstring(L, name);
		lua_pushinteger(L, n);
		lua_pushcclosure(L, LuaFunction<F>::wrap, 1);
		lua_rawset(L, -4);
		return *this;
	}

	ValueType<T>& func(const char* name, lua_CFunction func) {
		lua_pushstring(L, name);
		lua_pushcfunction(L, func);
		lua_rawset(L, -4);
		return *this;
	}

	void end() {
		lua_pop(L, 2);
	}
};


template<class T>
ValueType<T> class_def(lua_State *L,const char* name) {
	ValueType<T> vt(L,name);
	return vt;
}


int value_type_index(lua_State *L);


template<class T>
inline bool __eq(const T& a, const T& b) {
	return a == b;
}

template<class T>
inline Vector3 __add(const T& a, const T& b) {
	return a + b;
}

template<class T>
inline Vector3 __sub(const T& a, const T& b) {
	return a - b;
}

template<class T>
inline Vector3 __mul(const T& a, float b) {
	return a * b;
}

template<class T>
inline Vector3 __div(const T& a, float b) {
	return a / b;
}


template<class T>
inline Vector3 __unm(const T& a) {
	return -a;
}


#endif
