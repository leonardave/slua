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


Vector3 Normalize(const Vector3& v) {
	return v.normalized();
}

void Vector3::Normalize() {
	float mag = magnitude();
	if (mag > kEpsilon)
		*this /= mag;
	else
		this->Set(0, 0, 0);
}

Vector3 Vector3::Lerp(const Vector3& from, const Vector3& to, float t)
{
	t = SMath::Clamp(t);
	return Vector3(from.x + ((to.x - from.x) * t), from.y + ((to.y - from.y) * t), from.z + ((to.z - from.z) * t));
}
Vector3 Vector3::normalized() const
{
	Vector3 v(*this);
	v.Normalize();
	return v;
}

std::string Vector3::ToString()
{
	char str[64];
	snprintf(str, 64, "Vector3(%f,%f,%f)", x,y,z);
	return str;
}

float Vector3::Angle(const Vector3& from, const Vector3& to)
{
	return acos(SMath::Clamp(Dot(from.normalized(), to.normalized()), -1.f, 1.f)*ToAngle);
}

Vector3 Vector3::ClampMagnitude(const Vector3& v, float maxLength)
{
	if (v.sqrMagnitude() > maxLength * maxLength)
	{
		return Vector3(v.normalized() * maxLength);
	}
	return v;

}


Vector3 OrthoNormalVectorFast(const Vector3& n)
{
	Vector3 res;
	if (std::abs(n.z) > kEpsilon)
	{
		float a = n.y*n.y + n.z*n.z;
		float k = 1.0f / std::sqrtf(a);
		res.x = 0;
		res.y = -n.z*k;
		res.z = n.y*k;
	}
	else
	{
		float a = n.x*n.x + n.y*n.y;
		float k = 1.0f / std::sqrtf(a);
		res.x = -n.y*k;
		res.y = n.x*k;
		res.z = 0;
	}
	return res;
}

void OrthoNormalize(Vector3& inU, Vector3& inV)
{
	float mag = inU.magnitude();
	if (mag > kEpsilon)
		inU /= mag;
	else
		inU = Vector3(1.0f, 0.0f, 0.0f);

	float dot0 = Vector3::Dot(inU, inV);
	inV -= inU * dot0;
	mag = inV.magnitude();
	if (mag < kEpsilon)
		inV = OrthoNormalVectorFast(inU);
	else
		inV /= mag;
}

void OrthoNormalize(Vector3& inU, Vector3& inV, Vector3& inW)
{
	float mag = inU.magnitude();
	if (mag > kEpsilon)
		inU /= mag;
	else
		inU = Vector3(1.0F, 0.0F, 0.0F);

	float dot0 = Vector3::Dot(inU, inV);
	inV -= inU * dot0;
	mag = inV.magnitude();
	if (mag > kEpsilon)
		inV /= mag;
	else
		inV = OrthoNormalVectorFast(inU);

	float dot1 = Vector3::Dot(inV, inW);
	dot0 = Vector3::Dot(inU, inW);
	inW -= inU * dot0 + inV*dot1;
	mag = inW.magnitude();
	if (mag > kEpsilon)
		inW /= mag;
	else
		inW = Vector3::Cross(inU, inV);
}

int Vector3::OrthoNormalize(lua_State *L)
{
	int top = lua_gettop(L);
	if (top == 2) {
		Vector3 normal = check_type<Vector3>(L, 1);
		Vector3 tangent = check_type<Vector3>(L, 2);

		::OrthoNormalize(normal, tangent);
		set_back(L, 1, normal);
		set_back(L, 2, tangent);
	}
	else if (top == 3) {
		Vector3 normal = check_type<Vector3>(L, 1);
		Vector3 tangent = check_type<Vector3>(L, 2);
		Vector3 binormal = check_type<Vector3>(L, 3);

		::OrthoNormalize(normal, tangent, binormal);
		set_back(L, 1, normal);
		set_back(L, 2, tangent);
		set_back(L, 3, binormal);
	}
	else
		luaL_error(L, "invalid parameter count");
	return 0;
}

Vector3 Vector3::Reflect(const Vector3& inDirection, const Vector3& inNormal)
{
	return ((Vector3((-2.f * Dot(inNormal, inDirection)) * inNormal)) + inDirection);
}

static inline float ClampedMove(float lhs, float rhs, float clampedDelta)
{
	float delta = rhs - lhs;
	if (delta > 0.0F)
		return lhs + min(delta, clampedDelta);
	else
		return lhs - min(-delta, clampedDelta);
}

Vector3 Vector3::RotateTowards(const Vector3& lhs, const Vector3& rhs, float angleMove, float magnitudeMove)
{
	float lhsMag = lhs.magnitude();
	float rhsMag = rhs.magnitude();

	if (lhsMag > kEpsilon && rhsMag > kEpsilon)
	{
		Vector3 lhsNorm = lhs / lhsMag;
		Vector3 rhsNorm = rhs / rhsMag;

		float dot = Vector3::Dot(lhsNorm, rhsNorm);
		if (dot > 1.0f - kEpsilon)
		{
			return Vector3::MoveTowards(lhs, rhs, magnitudeMove);
		}
		else if (dot < -1.0F + kEpsilon)
		{
			Vector3 axis = OrthoNormalVectorFast(lhsNorm);
			Matrix3x3 m;
			m.SetAxisAngle(axis, angleMove);
			Vector3 rotated = m.Multiply(lhsNorm);
			rotated *= ClampedMove(lhsMag, rhsMag, magnitudeMove);
			return rotated;
		}
		else
		{
			float angle = std::acos(dot);
			Vector3 axis = Vector3::Cross(lhsNorm, rhsNorm).normalized();
			Matrix3x3 m;
			m.SetAxisAngle(axis, min(angleMove, angle));
			Vector3 rotated = m.Multiply(lhsNorm);
			rotated *= ClampedMove(lhsMag, rhsMag, magnitudeMove);
			return rotated;
		}
	}
	else
	{
		return Vector3::MoveTowards(lhs, rhs, magnitudeMove);
	}
}

Vector3 Vector3::Project(const Vector3& vector, const Vector3& onNormal)
{
	float num = Dot(onNormal, onNormal);
	if (num < kEpsilon)
	{
		return zero;
	}
	return Vector3((onNormal * Dot(vector, onNormal)) / num);
}

Vector3 Vector3::Slerp(const Vector3& lhs, const Vector3& rhs, float t)
{
	float lhsMag = lhs.magnitude();
	float rhsMag = rhs.magnitude();

	if (lhsMag < kEpsilon || rhsMag < kEpsilon)
		return Lerp(lhs, rhs, t);

	float lerpedMagnitude = SMath::Lerp(lhsMag, rhsMag, t);

	float dot = Dot(lhs, rhs) / (lhsMag * rhsMag);
	if (dot > 1.0F - kEpsilon)
	{
		return Lerp(lhs, rhs, t);
	}
	else if (dot < -1.0f + kEpsilon)
	{
		Vector3 lhsNorm = lhs / lhsMag;
		Vector3 axis = OrthoNormalVectorFast(lhsNorm);
		Matrix3x3 m;
		m.SetAxisAngle(axis, kPI * t);
		Vector3 slerped = m.Multiply(lhsNorm);
		slerped *= lerpedMagnitude;
		return slerped;
	}
	else
	{
		Vector3 axis = Cross(lhs, rhs);
		Vector3 lhsNorm = lhs / lhsMag;
		axis = axis.normalized();
		float angle = acos(dot) * t;

		Matrix3x3 m;
		m.SetAxisAngle(axis, angle);
		Vector3 slerped = m.Multiply(lhsNorm);
		slerped *= lerpedMagnitude;
		return slerped;
	}
}

int Vector3::SmoothDamp(lua_State *L)
{
	Vector3 current = check_type<Vector3>(L, 1);
	Vector3 target = check_type<Vector3>(L, 1); 
	Vector3 currentVelocity = check_type<Vector3>(L, 3);
	float smoothTime = check_type<float>(L, 4);
	float maxSpeed = check_type<float>(L, 5);
	float deltaTime = check_type<float>(L, 6);

	smoothTime = std::max(0.0001f, smoothTime);
	float num = 2.f / smoothTime;
	float num2 = num * deltaTime;
	float num3 = 1.f / (((1.f + num2) + ((0.48f * num2) * num2)) + (((0.235f * num2) * num2) * num2));
	Vector3 vector = current - target;
	Vector3 vector2 = target;
	float maxLength = maxSpeed * smoothTime;
	vector = ClampMagnitude(vector, maxLength);
	target = current - vector;
	Vector3 vector3 = (Vector3)((currentVelocity + (num * vector)) * deltaTime);
	currentVelocity = (Vector3)((currentVelocity - (num * vector3)) * num3);
	Vector3 vector4 = target + ((Vector3)((vector + vector3) * num3));
	if (Dot(vector2 - current, vector4 - vector2) > 0.f)
	{
		vector4 = vector2;
		currentVelocity = Vector3((vector4 - vector2) / deltaTime);
	}

	push_value(L, vector4);
	push_value(L, currentVelocity);
	return 2;
}

extern "C" {

	void luaopen_vector3(lua_State *L) {

		class_def<Vector3>(L, "Vector3")
			.method("__index", value_type_index)
			.method("__add", &__add<Vector3>)
			.method("__sub", &__sub<Vector3>)
			.method("__mul", &__mul<Vector3>)
			.method("__div", &__div<Vector3>)
			.method("__eq", &__eq<Vector3>)
			.method("__unm", &__unm<Vector3>)
			.method("__tostring", &Vector3::ToString)
			.method("Normalize", &Vector3::Normalize)
			.method("ToString", &Vector3::ToString)
			.method("get_magnitude", &Vector3::magnitude)
			.method("get_normalized", &Vector3::normalized)
			.method("get_sqrMagnitude", &Vector3::sqrMagnitude)

			.func("Angle", &Vector3::Angle)
			.func("ClampMagnitude", &Vector3::ClampMagnitude)
			.func("Cross", &Vector3::Cross)
			.func("Distance", &Vector3::Distance)
			.func("Dot", &Vector3::Dot)
			.func("Lerp", &Vector3::Lerp)
			.func("Max", &Vector3::Max)
			.func("Min", &Vector3::Min)
			.func("MoveTowards", &Vector3::MoveTowards)
			.func("Normalize", Normalize)
			.func("OrthoNormalize", &Vector3::OrthoNormalize)
			.func("Project", &Vector3::Project)
			.func("ProjectOnPlane", &Vector3::ProjectOnPlane)
			.func("Reflect", &Vector3::Reflect)
			.func("RotateTowards", &Vector3::RotateTowards)
			.func("Scale", &Vector3::Scale)
			.func("Slerp", &Vector3::Slerp)
			.func("SmoothDampInner", &Vector3::SmoothDamp)
			.end();
	}
}
