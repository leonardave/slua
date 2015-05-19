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
	local Quaternion = UnityEngine.Quaternion
	local Time = UnityEngine.Time
	local Color = UnityEngine.Color

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
					local f=rawget(fields,'get_'..k)
					if f then return f(t) end
				end
			end,
			__newindex=function(t,k,v)
				if isField(k) then 
					local f=rawget(fields,'set_'..k)
					if f then return f(t,v) end
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
			get_x=function(self) return self[1] end;
			get_y=function(self) return self[2] end;
			get_z=function(self) return self[3] end;
			set_x=function(self,v) self[1]=v end;
			set_y=function(self,v) self[2]=v end;
			set_z=function(self,v) self[3]=v end;
		}
	)


	Class( UnityEngine.Quaternion, args[4],

        function(x,y,z,w)
			local r={x,y,z,w}
			return r
		end,

		{
			get_identity=function() return Quaternion(0,0,0,1)  end;

			LookRotation=function(forward,up)
				up = up or Vector3.up
				return Quaternion.LookRotationInner(forward,up)
			end;
		},

		{
			Set=function(self,x,y,z,w) self[1],self[2],self[3],self[4]=x,y,z,w end;
			SetLookRotation=function(self,forward,up)
				up=up or Vector3.up
				self:SetLookRotationInner(forward,up)
			end;
			get_x=function(self) return self[1] end;
			get_y=function(self) return self[2] end;
			get_z=function(self) return self[3] end;
			get_w=function(self) return self[4] end;
			set_x=function(self,v) self[1]=v end;
			set_y=function(self,v) self[2]=v end;
			set_z=function(self,v) self[3]=v end;
			set_w=function(self,v) self[4]=v end;
		}
	)

	Class( UnityEngine.Color, args[5],

        function(r,g,b,a)
			local r={r,g,b,a}
			return r
		end,

		{
			get_black=function() return Color(0,0,0,1)  end;
			get_blue=function() return Color(0,0,1,1)  end;
			get_clear=function() return Color(0,0,0,0)  end;
			get_cyan=function() return Color(0,1,1,1)  end;
			get_gray=function() return Color(0.5,0.5,0.5,1)  end;
			get_grey=function() return Color(0.5,0.5,0.5,1)  end;
			get_green=function() return Color(0,1,0,1)  end;
			get_magenta=function() return Color(1,0,1,1)  end;
			get_red=function() return Color(1,0,0,1)  end;
			get_white=function() return Color(1,1,1,1)  end;
			get_yellow=function() return Color(1,0.92,0.016,1)  end;
		},

		{
			Set=function(self,r,g,b,a) self[1],self[2],self[3],self[4]=r,g,b,a end;
			get_r=function(self) return self[1] end;
			get_g=function(self) return self[2] end;
			get_b=function(self) return self[3] end;
			get_a=function(self) return self[4] end;
			set_r=function(self,v) self[1]=v end;
			set_g=function(self,v) self[2]=v end;
			set_b=function(self,v) self[3]=v end;
			set_a=function(self,v) self[4]=v end;
			get_grayscale=function(self) return (((0.299 * self.r) + (0.587 * self.g)) + (0.114 * self.b)) end
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

			LuaDLL.luaL_getmetatable(l, "Slua_Vector2_Meta");
			LuaDLL.luaL_getmetatable(l, "Slua_Vector3_Meta");
			LuaDLL.luaL_getmetatable(l, "Slua_Vector4_Meta");
			LuaDLL.luaL_getmetatable(l, "Slua_Quaternion_Meta");
			LuaDLL.luaL_getmetatable(l, "Slua_Color_Meta");
			
			LuaDLL.lua_pcall(l, 5, 0, err);
			LuaDLL.lua_settop(l, 0);
		}
	}
}