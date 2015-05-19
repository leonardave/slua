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

#ifndef SLUA_COLOR_HEADER_
#define SLUA_COLOR_HEADER_

struct Color {

	union {
		struct {
			float r, g, b, a;
		};
		float data[4];
	};

	Color() = default;

	Color(float r, float g, float b, float a) {
		Set(r, g, b, a);
	}

	inline void Set(float r, float g, float b, float a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	std::string ToString();
	
	static Color ToGamma(const Color& c);
	static Color ToLinear(const Color& c);
	static Color Lerp(const Color& a, const Color& b, float t);


	friend Color operator + (const Color& lhs, const Color& rhs) {
		return Color(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b, lhs.a + rhs.a);
	}

	friend Color operator - (const Color& lhs, const Color& rhs) {
		return Color(lhs.r - rhs.r, lhs.g - rhs.g, lhs.b - rhs.b, lhs.a - rhs.a);
	}

	friend Color operator - (const Color& rhs) {
		return Color(- rhs.r, - rhs.g, - rhs.b, - rhs.a);
	}

	friend Color operator / (const Color& lhs, float q) {
		return Color(lhs.r/q, lhs.g/q, lhs.b/q, lhs.a/q);
	}

	friend Color operator * (const Color& lhs, float q) {
		return Color(lhs.r * q, lhs.g * q, lhs.b * q, lhs.a * q);
	}

	friend Color operator * (const Color& lhs, const Color& rhs) {
		return Color(lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b, lhs.a * rhs.a);
	}

	friend bool operator == (const Color& lhs, const Color& rhs) {
		return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
	}

	static const char* meta_name;
	static int meta_ref;
};


#endif