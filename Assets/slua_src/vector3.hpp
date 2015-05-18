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

	Vector3() = default;
	Vector3(float inX, float inY, float inZ)	{ x = inX; y = inY; z = inZ; }
	void Set(float inX, float inY, float inZ)	{ x = inX; y = inY; z = inZ; }

	float& operator[] (int i)						{ return data[i]; }
	const float& operator[] (int i)const			{ return data[i]; }

	bool operator == (const Vector3& v)const		{ return x == v.x && y == v.y && z == v.z; }
	bool operator != (const Vector3& v)const		{ return x != v.x || y != v.y || z != v.z; }

	inline Vector3& operator += (const Vector3& inV)		{ x += inV.x; y += inV.y; z += inV.z; return *this; }
	inline Vector3& operator -= (const Vector3& inV)		{ x -= inV.x; y -= inV.y; z -= inV.z; return *this; }
	inline Vector3& operator *= (float s)					{ x *= s; y *= s; z *= s; return *this; }
	inline Vector3& operator /= (float s)					{ x /= s; y /= s; z /= s; return *this; }
	inline Vector3 operator - () const					{ return Vector3(-x, -y, -z); }

	inline Vector3 operator * (float f) const					{ return Vector3(x*f, y*f, z*f); }
	inline Vector3 operator / (float f) const					{ return Vector3(x / f, y / f, z / f); }
	inline Vector3 operator + (const Vector3& rhs) 	const				{ return Vector3(x + rhs.x, y / rhs.y, z / rhs.z); }
	inline Vector3 operator - (const Vector3& rhs) 	const				{ return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }


	// methold
	void Normalize();
	std::string ToString();

	// field
	float magnitude() const
	{
		return std::sqrt(x*x + y*y + z*z);
	}
	Vector3 normalized() const;
	float sqrMagnitude() const{
		return x*x + y*y + z*z;
	}

	//static
	static Vector3 Lerp(const Vector3& a, const Vector3& b, float t);
	static float Angle(const Vector3& from, const Vector3& b);
	static Vector3 ClampMagnitude(const Vector3& v, float maxLength);
	inline static float Dot(const Vector3& lhs, const Vector3& rhs) {
		return (((lhs.x * rhs.x) + (lhs.y * rhs.y)) + (lhs.z * rhs.z));
	}
	inline static Vector3 Cross(const Vector3& lhs, const Vector3& rhs)
	{
		return Vector3((lhs.y * rhs.z) - (lhs.z * rhs.y), (lhs.z * rhs.x) - (lhs.x * rhs.z), (lhs.x * rhs.y) - (lhs.y * rhs.x));
	}
	inline static float Distance(const Vector3& a, const Vector3& b)
	{
		Vector3 vector(a.x - b.x, a.y - b.y, a.z - b.z);
		return vector.magnitude();
	}

	static Vector3 Max(const Vector3& lhs, const Vector3& rhs) {
		return Vector3(std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y), std::max(lhs.z, rhs.z));
	}

	static Vector3 Min(const Vector3& lhs, const Vector3& rhs) {
		return Vector3(std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y), std::min(lhs.z, rhs.z));
	}

	static Vector3 MoveTowards(const Vector3& current, const Vector3& target, float maxDistance) {
		Vector3 vector = target - current;
		float magnitude = vector.magnitude();
		if ((magnitude > maxDistance) && (magnitude != 0.f))
		{
			return (current + ((Vector3)((vector / magnitude) * maxDistance)));
		}
		return target;
	}

	static int OrthoNormalize(lua_State *L);

	static Vector3 Project(const Vector3& vector, const Vector3& onNormal);

	static Vector3 ProjectOnPlane(const Vector3& vector, const Vector3& planeNormal) {
		return vector - Project(vector, planeNormal);
	}

	static Vector3 Reflect(const Vector3& inDirection, const Vector3& inNormal);
	static Vector3 RotateTowards(const Vector3& current, const Vector3& target, float maxRadiansDelta, float maxMagnitudeDelta);

	static Vector3 Scale(const Vector3& a, const Vector3& b) {
		return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
	}

	static Vector3 Slerp(const Vector3& from, const Vector3& to, float t);

	static int SmoothDamp(lua_State *L);

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

inline static Vector3 operator * (float f, const Vector3& v)	{ return Vector3(v.x*f, v.y*f, v.z*f); }

#endif
