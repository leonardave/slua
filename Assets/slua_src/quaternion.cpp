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
#include "ctype.h"
}

#include "slua.hpp"
#include <limits>
#include <strstream>

#include "vector3.hpp"
#include "matrix3x3.hpp"
#include "quaternion.hpp"

Quaternion Quaternion::identity;
const char* Quaternion::meta_name = "Slua_Quaternion_Meta";
int Quaternion::meta_ref = LUA_NOREF;


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
		tmpQuat.Set(-q2.x,-q2.y,-q2.z,-q2.w);
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
	snprintf(str, 128, "Quaternion(%f,%f,%f,%f)", x, y, z, w);
	return str;
}

void Quaternion::SetFromToRotation(const Vector3& fromDirection, const Vector3& toDirection)
{
	*this=FromToRotation(fromDirection, toDirection);
}

void Quaternion::SetLookRotationInner(const Vector3& view, const Vector3& up)
{
	*this=LookRotationInner(view, up);
}

inline void QuaternionToAxisAngle(const Quaternion& q, Vector3& axis, float& targetAngle)
{
	targetAngle = 2.0f* std::acos(q.w);
	if (std::abs(targetAngle-0.0f)<kEpsilon)
	{
		axis = Vector3(1, 0, 0);
		return;
	}

	float div = 1.0f / std::sqrt(1.0f - q.w*q.w);
	axis.Set(q.x*div, q.y*div, q.z*div);
}

int Quaternion::ToAngleAxis(lua_State *L)
{
	Vector3 axis;
	float targetAngle;
	QuaternionToAxisAngle(*this, axis, targetAngle);
	push_value(L, axis);
	push_value(L, targetAngle*ToAngle);
	return 2;
}

static Quaternion EulerToQuaternion(const Vector3& eulerAngles)
{
	float cX(std::cos(eulerAngles.x / 2.0f));
	float sX(std::sin(eulerAngles.x / 2.0f));

	float cY(std::cos(eulerAngles.y / 2.0f));
	float sY(std::sin(eulerAngles.y / 2.0f));

	float cZ(std::cos(eulerAngles.z / 2.0f));
	float sZ(std::sin(eulerAngles.z / 2.0f));

	Quaternion qX(sX, 0.0F, 0.0F, cX);
	Quaternion qY(0.0F, sY, 0.0F, cY);
	Quaternion qZ(0.0F, 0.0F, sZ, cZ);

	Quaternion q = (qY * qX) * qZ;
	return q;
}

int Quaternion::Euler(lua_State *L)
{
	int top = lua_gettop(L);
	if (top == 3)
	{
		float x = check_type<float>(L, 1);
		float y = check_type<float>(L, 2);
		float z = check_type<float>(L, 3);
		return push_value(L, EulerToQuaternion(Vector3(x, y, z)*ToRadius));
	}
	else if (top == 1) {
		Vector3 v = check_type<Vector3>(L, 1);
		return push_value(L, EulerToQuaternion(v*ToRadius));
	}
	else
		luaL_error(L, "invalid parameters passed in");
	return 0;
}

Quaternion AxisAngleToQuaternion(const Vector3& axis, float angle)
{
	Quaternion q;
	float mag = axis.magnitude();
	if (mag > 0.000001F)
	{
		float halfAngle = angle * 0.5F;

		q.w = cos(halfAngle);

		float s = sin(halfAngle) / mag;
		q.x = s * axis.x;
		q.y = s * axis.y;
		q.z = s * axis.z;
		return q;
	}
	else
	{
		return Quaternion::identity;
	}
}

Quaternion Quaternion::AngleAxis(float angle, const Vector3& axis)
{
	return AxisAngleToQuaternion(axis, angle*ToRadius);
}


static void MatrixToQuaternion(const Matrix3x3& kRot, Quaternion& q)
{
	float fTrace = kRot.Get(0, 0) + kRot.Get(1, 1) + kRot.Get(2, 2);
	float fRoot;

	if (fTrace > 0.0f)
	{
		// |w| > 1/2, may as well choose w > 1/2
		fRoot = sqrt(fTrace + 1.0f);  // 2w
		q.w = 0.5f*fRoot;
		fRoot = 0.5f / fRoot;  // 1/(4w)
		q.x = (kRot.Get(2, 1) - kRot.Get(1, 2))*fRoot;
		q.y = (kRot.Get(0, 2) - kRot.Get(2, 0))*fRoot;
		q.z = (kRot.Get(1, 0) - kRot.Get(0, 1))*fRoot;
	}
	else
	{
		// |w| <= 1/2
		int s_iNext[3] = { 1, 2, 0 };
		int i = 0;
		if (kRot.Get(1, 1) > kRot.Get(0, 0))
			i = 1;
		if (kRot.Get(2, 2) > kRot.Get(i, i))
			i = 2;
		int j = s_iNext[i];
		int k = s_iNext[j];

		fRoot = sqrt(kRot.Get(i, i) - kRot.Get(j, j) - kRot.Get(k, k) + 1.0f);
		float* apkQuat[3] = { &q.x, &q.y, &q.z };
		*apkQuat[i] = 0.5f*fRoot;
		fRoot = 0.5f / fRoot;
		q.w = (kRot.Get(k, j) - kRot.Get(j, k)) * fRoot;
		*apkQuat[j] = (kRot.Get(j, i) + kRot.Get(i, j))*fRoot;
		*apkQuat[k] = (kRot.Get(k, i) + kRot.Get(i, k))*fRoot;
	}
	q = Quaternion::Normalize(q);
}

static Quaternion FromToQuaternion(const Vector3& from, const Vector3& to)
{
	Matrix3x3 m;
	m.SetFromToRotation(from, to);
	Quaternion q;
	MatrixToQuaternion(m, q);
	return q;
}

Quaternion Quaternion::FromToRotation(const Vector3& fromDirection, const Vector3& toDirection)
{
	float lhsMag = fromDirection.magnitude();
	float rhsMag = toDirection.magnitude();
	if (lhsMag < kEpsilon || rhsMag < kEpsilon)
		return Quaternion::identity;
	else
		return ::FromToQuaternion(fromDirection / lhsMag, toDirection / rhsMag);
}


bool LookRotationToMatrix(const Vector3& viewVec, const Vector3& upVec, Matrix3x3* m)
{
	Vector3 z = viewVec;
	// compute u0
	float mag = z.magnitude();
	if (mag < kEpsilon)
	{
		m->SetIdentity();
		return false;
	}
	z /= mag;

	Vector3 x = Vector3::Cross(upVec, z);
	mag = x.magnitude();
	if (mag < kEpsilon)
	{
		m->SetIdentity();
		return false;
	}
	x /= mag;

	Vector3 y(Vector3::Cross(z, x));
	if (std::abs(y.sqrMagnitude()-1.f)<kEpsilon)
		return false;

	m->SetOrthoNormal(x, y, z);
	return true;
}

bool Quaternion::LookRotationToQuaternion(const Vector3& viewVec, const Vector3& upVec, Quaternion* res)
{
	Matrix3x3 m;
	if (!LookRotationToMatrix(viewVec, upVec, &m))
		return false;
	MatrixToQuaternion(m, *res);
	return true;
}

Quaternion Quaternion::LookRotationInner(const Vector3& forward, const Vector3& upwards)
{
	Quaternion q = Quaternion::identity;
	if (!LookRotationToQuaternion(forward, upwards, &q))
	{
		float mag = forward.magnitude();
		if (mag > kEpsilon)
		{
			Matrix3x3 m;
			m.SetFromToRotation(Vector3(0,0,1), forward / mag);
			MatrixToQuaternion(m, q);
		}
		else
		{
			return Quaternion::identity;
		}
	}
	return q;
}

void QuaternionToMatrix(const Quaternion& q, Matrix3x3& m)
{
	// Precalculate coordinate products
	float x = q.x * 2.0F;
	float y = q.y * 2.0F;
	float z = q.z * 2.0F;
	float xx = q.x * x;
	float yy = q.y * y;
	float zz = q.z * z;
	float xy = q.x * y;
	float xz = q.x * z;
	float yz = q.y * z;
	float wx = q.w * x;
	float wy = q.w * y;
	float wz = q.w * z;

	// Calculate 3x3 matrix from orthonormal basis
	m.data[0] = 1.0f - (yy + zz);
	m.data[1] = xy + wz;
	m.data[2] = xz - wy;

	m.data[3] = xy - wz;
	m.data[4] = 1.0f - (xx + zz);
	m.data[5] = yz + wx;

	m.data[6] = xz + wy;
	m.data[7] = yz - wx;
	m.data[8] = 1.0f - (xx + yy);
}

Vector3 Quaternion::QuaternionToEuler(const Quaternion& q)
{
	Matrix3x3 m;
	Vector3 rot;
	QuaternionToMatrix(q, m);
	Matrix3x3::MatrixToEuler(m, rot);
	return rot;
}

Vector3 Quaternion::get_eulerAngles()
{
	Quaternion outRotation = Normalize(*this);
	return QuaternionToEuler(outRotation);
}

void Quaternion::set_eulerAngles(const Vector3& v)
{
	*this = EulerToQuaternion(v*ToRadius);
}




static int __mul(lua_State *L) {
	if (is_typeof<Vector3>(L, 2)) {
		Quaternion q = check_type<Quaternion>(L, 1);
		Vector3 v = check_type<Vector3>(L, 2);
		return push_value(L, q*v);
	}
	else {
		Quaternion q1 = check_type<Quaternion>(L, 1);
		Quaternion q2 = check_type<Quaternion>(L, 2);
		return push_value(L, q1*q2);
	}
}


extern "C" void luaopen_quaternion(lua_State *L) {
	class_def<Quaternion>(L, "Quaternion")
		.method("__index", value_type_index)
		.method("__newindex", value_type_newindex)
		.method("__mul", &__mul)
		.method("__eq", &__eq<Quaternion>)
		.method("__unm", &__unm<Quaternion>)
		.method("__tostring", &Quaternion::ToString)
		.method("get_eulerAngles", &Quaternion::get_eulerAngles)
		.method("set_eulerAngles", &Quaternion::set_eulerAngles)
		.method("ToString", &Quaternion::ToString)
		.method("SetFromToRotation", &Quaternion::SetFromToRotation)
		.method("SetLookRotationInner", &Quaternion::SetLookRotationInner)
		.method("ToAngleAxis", &Quaternion::ToAngleAxis)
		.func("Angle", &Quaternion::Angle)
		.func("AngleAxis", &Quaternion::AngleAxis)
		.func("Dot", &Quaternion::Dot)
		.func("Lerp", &Quaternion::Lerp)
		.func("RotateTowards", &Quaternion::RotateTowards)
		.func("Slerp", &Quaternion::Slerp)
		.func("Euler", &Quaternion::Euler)
		.func("FromToRotation", &Quaternion::FromToRotation)
		.func("Inverse", &Quaternion::Inverse)
		.func("LookRotationInner", &Quaternion::LookRotationInner)
		.end();
}

