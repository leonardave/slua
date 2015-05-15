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

#ifndef SLUA_QUATERNION_HEADER_
#define SLUA_QUATERNION_HEADER_

struct Quaternion {

	union {
		struct {
			float x, y, z, w;
		};
		float data[4];
	};

	Quaternion() {}
	Quaternion(float inX, float inY, float inZ, float inW);

	inline void Set(float inX, float inY, float inZ, float inW) {
		x = inX; y = inY; z = inZ; w = inW;
	}
	std::string ToString();

	static Quaternion Normalize(const Quaternion& q) { 
		float mag = Magnitude(q);
		if (mag < kEpsilon)
			return Quaternion::identity;
		else
			return q / mag;
	}

	static float Magnitude(const Quaternion& q) {
		return std::sqrt(SqrMagnitude(q));
	}

	static float SqrMagnitude(const Quaternion& q) {
		return Dot(q, q);
	}

	inline static float Dot(const Quaternion& q1, const Quaternion& q2)
	{
		return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
	}


	bool operator == (const Quaternion& q)const		{ return x == q.x && y == q.y && z == q.z && w == q.w; }
	bool operator != (const Quaternion& q)const		{ return x != q.x || y != q.y || z != q.z || w != q.w; }

	Quaternion&	operator += (const Quaternion&  aQuat);
	Quaternion&	operator -= (const Quaternion&  aQuat);
	Quaternion&	operator *= (const float     	aScalar);
	Quaternion&	operator *= (const Quaternion& 	aQuat);
	Quaternion&	operator /= (const float     	aScalar);

	static Quaternion Slerp(const Quaternion& from, const Quaternion& to, float t);
	static Quaternion Lerp(const Quaternion& from, const Quaternion& to, float t);

	friend Quaternion operator + (const Quaternion& lhs, const Quaternion& rhs)
	{
		Quaternion q(lhs);
		return q += rhs;
	}

	friend Quaternion	operator - (const Quaternion& lhs, const Quaternion& rhs)
	{
		Quaternion t(lhs);
		return t -= rhs;
	}

	Quaternion operator - () const
	{
		return Quaternion(-x, -y, -z, -w);
	}

	Quaternion	operator * (const float s) const
	{
		return Quaternion(x*s, y*s, z*s, w*s);
	}

	friend Quaternion	operator * (const float s, const Quaternion& q)
	{
		Quaternion t(q);
		return t *= s;
	}

	friend Quaternion	operator / (const Quaternion& q, const float s)
	{
		Quaternion t(q);
		return t /= s;
	}

	inline friend Quaternion operator * (const Quaternion& lhs, const Quaternion& rhs)
	{
		return Quaternion(
			lhs.w*rhs.x + lhs.x*rhs.w + lhs.y*rhs.z - lhs.z*rhs.y,
			lhs.w*rhs.y + lhs.y*rhs.w + lhs.z*rhs.x - lhs.x*rhs.z,
			lhs.w*rhs.z + lhs.z*rhs.w + lhs.x*rhs.y - lhs.y*rhs.x,
			lhs.w*rhs.w - lhs.x*rhs.x - lhs.y*rhs.y - lhs.z*rhs.z);
	}

	static Quaternion identity;

};



inline Quaternion& Quaternion::operator+= (const Quaternion& aQuat)
{
	x += aQuat.x;
	y += aQuat.y;
	z += aQuat.z;
	w += aQuat.w;
	return *this;
}

inline Quaternion& Quaternion::operator-= (const Quaternion& aQuat)
{
	x -= aQuat.x;
	y -= aQuat.y;
	z -= aQuat.z;
	w -= aQuat.w;
	return *this;
}

inline Quaternion& Quaternion::operator *= (float aScalar)
{
	x *= aScalar;
	y *= aScalar;
	z *= aScalar;
	w *= aScalar;
	return *this;
}

inline Quaternion&	Quaternion::operator /= (const float aScalar)
{
	x /= aScalar;
	y /= aScalar;
	z /= aScalar;
	w /= aScalar;
	return *this;
}

inline Quaternion&	Quaternion::operator *= (const Quaternion& 	rhs)
{
	float tempx = w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y;
	float tempy = w*rhs.y + y*rhs.w + z*rhs.x - x*rhs.z;
	float tempz = w*rhs.z + z*rhs.w + x*rhs.y - y*rhs.x;
	float tempw = w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z;
	x = tempx; y = tempy; z = tempz; w = tempw;
	return *this;
}

#endif