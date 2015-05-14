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
}

#include "matrix3x3.hpp"


void GetRotMatrixNormVec(float* out, const float* inVec, float radians)
{
	float s, c;
	float vx, vy, vz, xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

	s = std::sin(radians);
	c = std::cos(radians);

	vx = inVec[0];
	vy = inVec[1];
	vz = inVec[2];

#define M(row,col)  out[row*3 + col]


	xx = vx * vx;
	yy = vy * vy;
	zz = vz * vz;
	xy = vx * vy;
	yz = vy * vz;
	zx = vz * vx;
	xs = vx * s;
	ys = vy * s;
	zs = vz * s;
	one_c = 1.0F - c;

	M(0, 0) = (one_c * xx) + c;
	M(1, 0) = (one_c * xy) - zs;
	M(2, 0) = (one_c * zx) + ys;

	M(0, 1) = (one_c * xy) + zs;
	M(1, 1) = (one_c * yy) + c;
	M(2, 1) = (one_c * yz) - xs;

	M(0, 2) = (one_c * zx) - ys;
	M(1, 2) = (one_c * yz) + xs;
	M(2, 2) = (one_c * zz) + c;

#undef M
}

Matrix3x3& Matrix3x3::SetAxisAngle(const Vector3& rotationAxis, float radians)
{
	GetRotMatrixNormVec(data, rotationAxis.data, radians);
	return *this;
}

Vector3 Matrix3x3::Multiply(const Vector3& v) const
{
	Vector3 res;
	res.x = data[0] * v.x + data[3] * v.y + data[6] * v.z;
	res.y = data[1] * v.x + data[4] * v.y + data[7] * v.z;
	res.z = data[2] * v.x + data[5] * v.y + data[8] * v.z;
	return res;
}
