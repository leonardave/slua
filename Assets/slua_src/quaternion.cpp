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

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "slua.h"
#include "ctype.h"
}

#include "slua.hpp"
#include <limits>
#include <strstream>

#include "vector3.hpp"
#include "matrix3x3.hpp"
#include "quaternion.hpp"

Quaternion Quaternion::identity;


inline Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	Quaternion tmpQuat;
	if (Dot(q1, q2) < 0.f)
	{
		tmpQuat.Set(q1.x + t * (-q2.x - q1.x),
			q1.y + t * (-q2.y - q1.y),
			q1.z + t * (-q2.z - q1.z),
			q1.w + t * (-q2.w - q1.w));
	}
	else
	{
		tmpQuat.Set(q1.x + t * (q2.x - q1.x),
			q1.y + t * (q2.y - q1.y),
			q1.z + t * (q2.z - q1.z),
			q1.w + t * (q2.w - q1.w));
	}
	return Normalize(tmpQuat);
}

Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	float dot = Dot(q1, q2);

	Quaternion tmpQuat;
	if (dot < 0.0f)
	{
		dot = -dot;
		tmpQuat.Set(-q2.x,
			-q2.y,
			-q2.z,
			-q2.w);
	}
	else
		tmpQuat = q2;


	if (dot < 0.95f)
	{
		float angle = std::acos(dot);
		float sinadiv, sinat, sinaomt;
		sinadiv = 1.0f / std::sin(angle);
		sinat = std::sin(angle*t);
		sinaomt = std::sin(angle*(1.0f - t));
		tmpQuat.Set((q1.x*sinaomt + tmpQuat.x*sinat)*sinadiv,
			(q1.y*sinaomt + tmpQuat.y*sinat)*sinadiv,
			(q1.z*sinaomt + tmpQuat.z*sinat)*sinadiv,
			(q1.w*sinaomt + tmpQuat.w*sinat)*sinadiv);
		return tmpQuat;

	}
	else
	{
		return Lerp(q1, tmpQuat, t);
	}
}

std::string Quaternion::ToString()
{
	char str[128];
	snprintf(str, 128, "Quaternion(%f,%f,%f,&f)", x, y, z, w);
	return str;
}



extern "C" void luaopen_quaternion(lua_State *L) {
	class_def<Quaternion>(L, "Quaternion")
		.method("__index", value_type_index)
		.method("__tostring", &Quaternion::ToString)
		.method("__mul", &__mul<Quaternion>)
		.method("__eq", &__eq<Quaternion>)
		.method("__unm", &__unm<Quaternion>)
		.func("Dot", &Quaternion::Dot)
		.func("Lerp", &Quaternion::Lerp)
		.func("RotateTowards", &Quaternion::RotateTowards)
		.func("Slerp", &Quaternion::Slerp)
		.end();
}

