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

#ifndef SLUA_MATH_HEADER_
#define SLUA_MATH_HEADER_

#define kEpsilon 0.00001f
#define kPI 3.14159265358979323846264338327950288419716939937510f
#define ToAngle 57.29578f
#define ToRadius 0.01745329f

namespace SMath {


	inline float Clamp(float value){
		if (value < 0.f)
		{
			return 0.f;
		}
		if (value > 1.f)
		{
			return 1.f;
		}
		return value;
	}

	inline float Clamp(float value, float min, float max)
	{
		if (value < min)
		{
			value = min;
			return value;
		}
		if (value > max)
		{
			value = max;
		}
		return value;
	}

	inline float Lerp(float from, float to, float t)
	{
		return to * t + from * (1.0f - t);
	}
}


#endif