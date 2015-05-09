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

#ifndef SLUA_VECTOR3_HEADER_
#define SLUA_VECTOR3_HEADER_


struct Vector3
{

	union {
		struct {
			float x, y, z;
		};
		float data[3];
	};

	Vector3() : x(0.f), y(0.f), z(0.f) {}
	Vector3(float inX, float inY, float inZ)	{ x = inX; y = inY; z = inZ; }
	explicit Vector3(const float* array)	{ x = array[0]; y = array[1]; z = array[2]; }
	void Set(float inX, float inY, float inZ)	{ x = inX; y = inY; z = inZ; }
	void Set(const float array[3])	{ x = array[0]; y = array[1]; z = array[2]; }

	float& operator[] (int i)						{ return data[i]; }
	const float& operator[] (int i)const			{ return data[i]; }

	bool operator == (const Vector3& v)const		{ return x == v.x && y == v.y && z == v.z; }
	bool operator != (const Vector3& v)const		{ return x != v.x || y != v.y || z != v.z; }

	inline Vector3& operator += (const Vector3& inV)		{ x += inV.x; y += inV.y; z += inV.z; return *this; }
	inline Vector3& operator -= (const Vector3& inV)		{ x -= inV.x; y -= inV.y; z -= inV.z; return *this; }
	inline Vector3& operator *= (float s)					{ x *= s; y *= s; z *= s; return *this; }
	inline Vector3& operator /= (float s)					{ x /= s; y /= s; z /= s; return *this; }
	inline Vector3 operator - () const					{ return Vector3(-x, -y, -z); }

	// methold
	void Normalize();


	// field
	float magnitude();

	static const float	epsilon;
	static const float	infinity;
	static const Vector3	zero;
	static const Vector3	one;
	static const Vector3	up;
	static const Vector3	down;
	static const Vector3	left;
	static const Vector3	right;
	static const Vector3	back;
	static const Vector3	forward;
	static const char* meta_name;
	static int meta_ref;
};

#endif