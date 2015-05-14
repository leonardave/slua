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

using System;
using LuaInterface;
using UnityEngine;

namespace SLua
{
	class LuaValueType
	{
		static string code = @"
	local args={...}
	local Infinity = 1.0 / 0.0
	local Vector3 = UnityEngine.Vector3
	local Time = UnityEngine.Time

	local function Class(cls,base,ctor,static,instance)
		local rawget=rawget
		local rawset=rawset
		local cls=cls or {}
        local setmetatable=setmetatable

		for k,v in pairs(instance) do
			rawset(base,k,v)
		end

		local fields={}

		local function isField(k)
			local c=k:sub(1,1)
			if c>='a' and c<='z' then
				return true
			end
			return false
		end

		for k,v in pairs(static) do
			if isField(k) then
				rawset(fields,k,v)
			else
				rawset(cls,k,v)
			end
		end

		setmetatable(cls,{
			__call=function(t,...)
				local o = ctor(...)
				setmetatable(o,base)
				return o
			end,
			__index=function(t,k)
				if isField(k) then 
					local f=rawget(fields,k)
					if f then return f() end
				end
			end,
		})
		return cls
	end

	Class( UnityEngine.Vector3, args[2],
		
        function(x,y,z)
			local r={x,y,z}
			return r
		end,

		{
			one=function() return Vector3(1,1,1)  end;
			zero=function() return Vector3(0,0,0)  end;
			left=function() return Vector3(-1,0,0)  end;
			right=function() return Vector3(1,0,0)  end;
			up=function() return Vector3(0,1,0)  end;
			down=function() return Vector3(0,-1,0)  end;
			forward=function() return Vector3(0,0,1)  end;
			back=function() return Vector3(0,0,-1)  end;

			SmoothDamp=function(current,target,currentVelocity,smoothTime,maxSpeed,deltaTime)
				maxSpeed = 	maxSpeed or Infinity
				deltaTime = deltaTime or Time.deltaTime
				return Vector3.SmoothDampInner(current,target,currentVelocity,smoothTime,maxSpeed,deltaTime)
			end;
		},

		{
			Set=function(self,x,y,z) self[1],self[2],self[3]=x,y,z end;
		}
	)

";
		static internal void init(IntPtr l)
		{
			int err = LuaObject.pushTry(l);

			

			if (LuaDLL.luaL_loadstring(l,code) != 0)
			{
				string errstr = LuaDLL.lua_tostring(l, -1);
				throw new Exception(errstr);			
			}

			LuaDLL.luaL_getmetatable(l, "Slua_Vector3_Meta");
			LuaDLL.luaL_getmetatable(l, "Slua_Vector3_Meta");
			LuaDLL.luaL_getmetatable(l, "Slua_Vector3_Meta");
			LuaDLL.luaL_getmetatable(l, "Slua_Vector3_Meta");
			LuaDLL.luaL_getmetatable(l, "Slua_Vector3_Meta");
			
			LuaDLL.lua_pcall(l, 5, 0, err);
			LuaDLL.lua_settop(l, 0);
		}
	}
}