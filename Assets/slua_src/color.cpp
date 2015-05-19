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

#include "slua.hpp"
#include "color.hpp"


const char* Color::meta_name = "Slua_Color_Meta";
int Color::meta_ref = LUA_NOREF;




inline float ToGammaSpace(float value)
{
	if (value <= 0.f)
		return 0.f;
	else if (value <= 0.0031308f)
		return 12.92f * value;
	else if (value <= 1.f)
		return 1.055f * std::pow(value, 0.41666f) - 0.055f;
	else
		return std::pow(value, 0.41666f);
}

inline float ToLinearSpace(float value)
{
	if (value <= 0.04045f)
		return value / 12.92f;
	else if (value < 1.0f)
		return std::pow((value + 0.055f) / 1.055f, 2.4f);
	else
		return std::pow(value, 2.4f);
}

Color Color::ToGamma(const Color& c)
{
	return Color(ToGammaSpace(c.r),
		ToGammaSpace(c.g), ToGammaSpace(c.b), c.a);
}


Color Color::ToLinear(const Color& c)
{
	return Color(ToLinearSpace(c.r),
		ToLinearSpace(c.g), ToLinearSpace(c.b), c.a);
}

Color Color::Lerp(const Color& a, const Color& b, float t)
{
	t = SMath::Clamp(t, 0, 1.f);
	return  a*(1.0f - t) + b*t;
}

std::string Color::ToString()
{
	char buf[128];
	snprintf(buf, 128, "Color(%f,%f,%f,%f)", r, g, b, a);
	return buf;
}

static int __mul(lua_State *L) {
	if (lua_isnumber(L, 2)) {
		Color c = check_type<Color>(L, 1);
		float q = check_type<float>(L, 2);
		return push_value(L, c*q);
	}
	else {
		Color c1 = check_type<Color>(L, 1);
		Color c2 = check_type<Color>(L, 2);
		return push_value(L, c1*c2);
	}
}



extern "C" void luaopen_color(lua_State *L) {
	class_def<Color>(L, "Color")
		.method("__index", value_type_index)
		.method("__add", &__add<Color>)
		.method("__sub", &__sub<Color>)
		.method("__mul", &__mul)
		.method("__div", &__div<Color>)
		.method("__eq", &__eq<Color>)
		.method("__unm", &__unm<Color>)
		.method("__tostring", &Color::ToString)
		.method("get_gamma", &Color::ToGamma)
		.method("get_linear", &Color::ToGamma)
		.method("ToString", &Color::ToString)
		.func("Lerp", &Color::Lerp)
		.end();
}
