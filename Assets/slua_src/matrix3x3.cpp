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

/*
* A function for creating a rotation matrix that rotates a vector called
* "from" into another vector called "to".
* Input : from[3], to[3] which both must be *normalized* non-zero vectors
* Output: mtx[3][3] -- a 3x3 matrix in colum-major form
* Author: Tomas Muler, 1999
*/
void fromToRotation(const Vector3& from, const Vector3& to, float mtx[3][3])
{
	float e, h;
	Vector3 v = Vector3::Cross(from, to);
	e = Vector3::Dot(from, to);
	if (e > 1.0 - kEpsilon)     /* "from" almost or equal to "to"-vector? */
	{
		/* return identity */
		mtx[0][0] = 1.0; mtx[0][1] = 0.0; mtx[0][2] = 0.0;
		mtx[1][0] = 0.0; mtx[1][1] = 1.0; mtx[1][2] = 0.0;
		mtx[2][0] = 0.0; mtx[2][1] = 0.0; mtx[2][2] = 1.0;
	}
	else if (e < -1.0 + kEpsilon) /* "from" almost or equal to negated "to"? */
	{
		Vector3 up, left;
		float invlen;
		float fxx, fyy, fzz, fxy, fxz, fyz;
		float uxx, uyy, uzz, uxy, uxz, uyz;
		float lxx, lyy, lzz, lxy, lxz, lyz;
		/* left=CROSS(from, (1,0,0)) */
		left[0] = 0.0; left[1] = from[2]; left[2] = -from[1];
		if (Vector3::Dot(left, left) < kEpsilon) /* was left=CROSS(from,(1,0,0)) a good choice? */
		{
			/* here we now that left = CROSS(from, (1,0,0)) will be a good choice */
			left[0] = -from[2]; left[1] = 0.0; left[2] = from[0];
		}
		/* normalize "left" */
		invlen = 1.0f / sqrt(Vector3::Dot(left, left));
		left[0] *= invlen;
		left[1] *= invlen;
		left[2] *= invlen;
		up = Vector3::Cross(left, from);
		/* now we have a coordinate system, i.e., a basis;    */
		/* M=(from, up, left), and we want to rotate to:      */
		/* N=(-from, up, -left). This is done with the matrix:*/
		/* N*M^T where M^T is the transpose of M              */
		fxx = -from[0] * from[0]; fyy = -from[1] * from[1]; fzz = -from[2] * from[2];
		fxy = -from[0] * from[1]; fxz = -from[0] * from[2]; fyz = -from[1] * from[2];

		uxx = up[0] * up[0]; uyy = up[1] * up[1]; uzz = up[2] * up[2];
		uxy = up[0] * up[1]; uxz = up[0] * up[2]; uyz = up[1] * up[2];

		lxx = -left[0] * left[0]; lyy = -left[1] * left[1]; lzz = -left[2] * left[2];
		lxy = -left[0] * left[1]; lxz = -left[0] * left[2]; lyz = -left[1] * left[2];
		/* symmetric matrix */
		mtx[0][0] = fxx + uxx + lxx; mtx[0][1] = fxy + uxy + lxy; mtx[0][2] = fxz + uxz + lxz;
		mtx[1][0] = mtx[0][1];   mtx[1][1] = fyy + uyy + lyy; mtx[1][2] = fyz + uyz + lyz;
		mtx[2][0] = mtx[0][2];   mtx[2][1] = mtx[1][2];   mtx[2][2] = fzz + uzz + lzz;
	}
	else  /* the most common case, unless "from"="to", or "from"=-"to" */
	{
		/* ...otherwise use this hand optimized version (9 mults less) */
		float hvx, hvz, hvxy, hvxz, hvyz;
		h = (1.0f - e) / Vector3::Dot(v, v);
		hvx = h*v[0];
		hvz = h*v[2];
		hvxy = hvx*v[1];
		hvxz = hvx*v[2];
		hvyz = hvz*v[1];
		mtx[0][0] = e + hvx*v[0]; mtx[0][1] = hvxy - v[2];     mtx[0][2] = hvxz + v[1];
		mtx[1][0] = hvxy + v[2];  mtx[1][1] = e + h*v[1] * v[1]; mtx[1][2] = hvyz - v[0];
		mtx[2][0] = hvxz - v[1];  mtx[2][1] = hvyz + v[0];     mtx[2][2] = e + hvz*v[2];
	}
}

Matrix3x3& Matrix3x3::SetFromToRotation(const Vector3& from, const Vector3& to)
{
	float mtx[3][3];
	fromToRotation(from, to, mtx);
	// transpose
	Get(0, 0) = mtx[0][0];	Get(0, 1) = mtx[0][1];	Get(0, 2) = mtx[0][2];
	Get(1, 0) = mtx[1][0];	Get(1, 1) = mtx[1][1];	Get(1, 2) = mtx[1][2];
	Get(2, 0) = mtx[2][0];	Get(2, 1) = mtx[2][1];	Get(2, 2) = mtx[2][2];
	return *this;
}

void Matrix3x3::SetIdentity()
{
	Get(0, 0) = 1.0f;	Get(0, 1) = 0.0f;	Get(0, 2) = 0.0f;
	Get(1, 0) = 0.0f;	Get(1, 1) = 1.0f;	Get(1, 2) = 0.0f;
	Get(2, 0) = 0.0f;	Get(2, 1) = 0.0f;	Get(2, 2) = 1.0f;
}

void Matrix3x3::SetOrthoNormal(const Vector3& inX, const Vector3& inY, const Vector3& inZ)
{
	Get(0, 0) = inX[0];	Get(0, 1) = inY[0];	Get(0, 2) = inZ[0];
	Get(1, 0) = inX[1];	Get(1, 1) = inY[1];	Get(1, 2) = inZ[1];
	Get(2, 0) = inX[2];	Get(2, 1) = inY[2];	Get(2, 2) = inZ[2];
}

void MakePositive(Vector3& euler)
{
	const float negativeFlip = -0.0001F;
	const float positiveFlip = (kPI * 2.0F) - 0.0001F;

	if (euler.x < negativeFlip)
		euler.x += 2.0 * kPI;
	else if (euler.x > positiveFlip)
		euler.x -= 2.0 * kPI;

	if (euler.y < negativeFlip)
		euler.y += 2.0 * kPI;
	else if (euler.y > positiveFlip)
		euler.y -= 2.0 * kPI;

	if (euler.z < negativeFlip)
		euler.z += 2.0 * kPI;
	else if (euler.z > positiveFlip)
		euler.z -= 2.0 * kPI;
}

bool Matrix3x3::MatrixToEuler(const Matrix3x3& matrix, Vector3& v)
{
	// from http://www.geometrictools.com/Documentation/EulerAngles.pdf
	// YXZ order
	if (matrix.Get(1, 2) < 0.999F) // some fudge for imprecision
	{
		if (matrix.Get(1, 2) > -0.999F) // some fudge for imprecision
		{
			v.x = asin(-matrix.Get(1, 2));
			v.y = atan2(matrix.Get(0, 2), matrix.Get(2, 2));
			v.z = atan2(matrix.Get(1, 0), matrix.Get(1, 1));
			MakePositive(v);
			return true;
		}
		else
		{
			// WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
			v.x = kPI * 0.5F;
			v.y = atan2(matrix.Get(0, 1), matrix.Get(0, 0));
			v.z = 0.0F;
			MakePositive(v);

			return false;
		}
	}
	else
	{
		// WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
		v.x = -kPI * 0.5F;
		v.y = atan2(-matrix.Get(0, 1), matrix.Get(0, 0));
		v.z = 0.0F;
		MakePositive(v);
		return false;
	}
}
