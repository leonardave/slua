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

Quaternion Quaternion::Slerp(const Quaternion& from, const Quaternion& to, float t)
{

}

std::string Quaternion::ToString()
{
	char str[128];
#ifdef _WINDOWS
	_snprintf(str, 128, "Quaternion(%f,%f,%f,&f)", x, y, z, w);
#else
	snprintf(str, 128, "Quaternion(%f,%f,%f,&f)", x, y, z, w);
#endif
	return str;
}

extern "C" void luaopen_quaternion(lua_State *L) {
// 	class_def<Quaternion>(L, "Quaternion")
// 		.method("__index", value_type_index)
// 		.method("__tostring", &Quaternion::ToString)
// 		.method("__mul", &__mul<Quaternion>)
// 		.method("__eq", &__eq<Quaternion>)
// 		.method("__unm", &__unm<Quaternion>)
// 
// // 		.method("Normalize", &Vector3::Normalize)
// // 		.method("ToString", &Vector3::ToString)
// // 
// // 		.method("magnitude", &Vector3::magnitude)
// // 		.method("normalized", &Vector3::normalized)
// // 		.method("sqrMagnitude", &Vector3::sqrMagnitude)
// // 
// // 		.func("Angle", &Vector3::Angle)
// // 		.func("ClampMagnitude", &Vector3::ClampMagnitude)
// // 		.func("Cross", &Vector3::Cross)
// // 		.func("Distance", &Vector3::Distance)
// 		.func("Dot", &Quaternion::Dot)
// 		.func("Lerp", &Quaternion::Lerp)
// // 		.func("Max", &Vector3::Max)
// // 		.func("Min", &Vector3::Min)
// // 		.func("MoveTowards", &Vector3::MoveTowards)
// // 		.func("Normalize", Normalize)
// // 		.func("OrthoNormalize", &Vector3::OrthoNormalize)
// // 		.func("Project", &Vector3::Project)
// // 		.func("ProjectOnPlane", &Vector3::ProjectOnPlane)
// // 		.func("Reflect", &Vector3::Reflect)
// 		.func("RotateTowards", &Quaternion::RotateTowards)
// 		.func("Slerp", &Quaternion::Slerp)
// 		.func("SmoothDampInner", &Vector3::SmoothDamp)
/*		.end();*/
}

