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

#ifndef SLUA_MATRIX3X3_HEADER_
#define SLUA_MATRIX3X3_HEADER_

#include "slua.hpp"

struct Matrix3x3
{
	union
	{
		float data[9];
		float data3x3[3][3];
	};
	

	Matrix3x3& SetAxisAngle(const Vector3& rotationAxis, float radians);
	Vector3 Multiply(const Vector3& inV) const;

	float& Get(int row, int column) 				{ return data[row + (column * 3)]; }
	const float& Get(int row, int column)const 	{ return data[row + (column * 3)]; }

	Matrix3x3& Matrix3x3::SetFromToRotation(const Vector3& from, const Vector3& to);
	void SetIdentity();
	void SetOrthoNormal(const Vector3& inX, const Vector3& inY, const Vector3& inZ);

	static bool MatrixToEuler(const Matrix3x3& matrix, Vector3& v);

};

#endif