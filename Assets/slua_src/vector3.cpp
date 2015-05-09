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

#define LUA_LIB

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "slua.h"
}

#include "slua.hpp"
#include <limits>

#include "vector3.hpp"

using namespace std;

const Vector3	Vector3::zero = Vector3(0, 0, 0);
const Vector3	Vector3::one = Vector3(1.0f, 1.0f, 1.0f);
const Vector3	Vector3::back = Vector3(0, 0, -1.f);
const Vector3	Vector3::forward = Vector3(0, 0, 1.f);
const Vector3	Vector3::down = Vector3(0, -1.f, 0);
const Vector3	Vector3::up = Vector3(0, 1.f, 0);
const Vector3	Vector3::left = Vector3(-1.f, 0, 0);
const Vector3	Vector3::right = Vector3(1.f, 0, 0);
const char*		Vector3::meta_name = "Slua_Vector3_Meta";
int				Vector3::meta_ref = LUA_NOREF;

void Vector3::Normalize() {
	float mag = magnitude();
	if (mag > kEpsilon)
		*this /= mag;
	else
		this->Set(0, 0, 0);
}

float Vector3::magnitude()
{
	return sqrtf(x*x + y*y + z*z);
}


// static struct luaL_Reg instancereg[] = {
// 		{ "Normalize", vec3_normalize },
// 		{ "magnitude", vec3_magnitude },
// 		{ "__index", value_type_index, },
// 		{ NULL, NULL }
// };

extern "C" {

	int value_type_index(lua_State *L) {
		luaL_checktype(L, 1, LUA_TTABLE);
		luaL_checktype(L, 2, LUA_TSTRING);

		lua_getmetatable(L, 1);

		// get member function
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
		if (!lua_isnil(L, -1)) {
			lua_remove(L, -2);
			return 1;
		}
		// get property / field
		lua_pop(L, 1); // pop last nil

		lua_getfield(L, -1, "__property");
		if (lua_isnil(L, -1)) {
			luaL_error(L, "can't find %s", lua_isstring(L, 2));
			return 0;
		}
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
		if (!lua_isfunction(L, -1)) {
			luaL_error(L, "can't find %s", lua_isstring(L, 2));
			return 0;
		}

		lua_pushvalue(L, 1);
		lua_call(L, 1, 1);
		lua_remove(L, -2); // remove __property
		lua_remove(L, -3); // remove metatable
		return 1;
		
	}

	struct Foo {
		Foo(int num) : num_(num) {}
		void print_add(int i) const { 
		}
		int num_;
	};

	void luaopen_vector3(lua_State *L) {

		//luaL_newmetatable(L,Vector3::meta_name);
		//luaL_register(L, NULL, instancereg);
		class_def<Vector3>(L, "Vector3")
			.method("Normalize", &Vector3::Normalize);

		using std::placeholders::_1;
		Foo foo(1);
		std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
		//f_add_display(foo, 1);

		lua_pop(L, 1);

	}
}
