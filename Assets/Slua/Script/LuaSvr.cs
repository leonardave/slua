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


namespace SLua
{
	using System;
	using System.Collections;
	using System.Collections.Generic;

	using UnityEngine;
	using LuaInterface;
	using System.Reflection;
	using System.Diagnostics;
	using Debug = UnityEngine.Debug;

	public class LuaSvr
	{
		public LuaState luaState;
		static LuaSvrGameObject lgo;
		int errorReported = 0;


		public LuaSvr()
			: this(null)
		{

		}

		public LuaSvr(string main)
		{
			luaState = new LuaState();

			GameObject go = new GameObject("LuaSvrProxy");
			lgo = go.AddComponent<LuaSvrGameObject>();
			GameObject.DontDestroyOnLoad(go);
			lgo.state = luaState;
			lgo.onUpdate = this.tick;

			int err = LuaObject.pushTry(luaState.L);
			LuaDLL.lua_pushcfunction(luaState.L, init);
			LuaDLL.pcall(luaState.L, 0, 0, err);

			start(main);

			if (LuaDLL.lua_gettop(luaState.L) != errorReported)
			{
                errorReported = LuaDLL.lua_gettop(luaState.L);
                Debug.LogError(string.Format("Some function not remove temp value({0}) from lua stack. You should fix it.", LuaDLL.luaL_typename(luaState.L, errorReported)));
            }
		}

		[MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
		static int init(IntPtr L)
		{
			LuaObject.init(L);
			BindAll(L);
			LuaTimer.reg(L);
			LuaCoroutine.reg(L, lgo);
			Helper.reg(L);
			LuaDLL.luaS_openextlibs(L);
			LuaValueType.init(L);
			return 0;
		}

		public object start(string main)
		{
			if (main != null)
			{
				luaState.doFile(main);
				LuaFunction func = (LuaFunction)luaState["main"];
				if(func!=null)
					return func.call();
			}
			return null;
		}

		void tick()
		{
			if (LuaDLL.lua_gettop(luaState.L) != errorReported)
			{
				errorReported = LuaDLL.lua_gettop(luaState.L);
				Debug.LogError(string.Format("Some function not remove temp value({0}) from lua stack. You should fix it.",LuaDLL.luaL_typename(luaState.L,errorReported)));
			}

			luaState.checkRef();
			LuaTimer.tick(Time.deltaTime);
		}


		static void BindAll(IntPtr l)
		{
#if !RELEASE_MARCO // some RELEASE_MARCO used for release version 
			Assembly[] ams = AppDomain.CurrentDomain.GetAssemblies();

			List<Type> bindlist = new List<Type>();
			foreach(Assembly a in ams) 
			{
				Type[] ts=a.GetExportedTypes();
				foreach (Type t in ts)
				{
					if (t.GetCustomAttributes(typeof(LuaBinderAttribute),false).Length > 0)
					{
						bindlist.Add(t);
					}
				}
			}

			bindlist.Sort( new System.Comparison<Type>((Type a,Type b) =>
			{
				LuaBinderAttribute la = (LuaBinderAttribute)a.GetCustomAttributes(typeof(LuaBinderAttribute),false)[0];
				LuaBinderAttribute lb = (LuaBinderAttribute)b.GetCustomAttributes(typeof(LuaBinderAttribute),false)[0];

				return la.order.CompareTo(lb.order);
			})
			);

			foreach (Type t in bindlist)
			{
				t.GetMethod("Bind").Invoke(null, new object[] { l });
			}
#else 
			// 如果是发布版可以用下面4行替换上面的代码, bind的速度更快, 保证游戏启动速度, 注意bind顺序
			// If is release version, you can use below 4 lines code avoid reflect search what to bind, make sure game will run full speed on start, keep bind order.
			BindUnity.Bind(l);
			// BindUnityUI.Bind(l); // if exists
			// BindDll.Bind(l); // if exists
			BindCustom.Bind(l);
#endif
		}
	}
}
