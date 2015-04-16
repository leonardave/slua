using System;
using System.Runtime.InteropServices;
using System.Text;

namespace LuaInterface
{
#pragma warning disable 414
	public class MonoPInvokeCallbackAttribute : System.Attribute
	{
		private Type type;
		public MonoPInvokeCallbackAttribute(Type t)
		{
			type = t;
		}
	}
#pragma warning restore 414

	public enum LuaTypes : int
	{
		LUA_TNONE = -1,
		LUA_TNIL = 0,
		LUA_TBOOLEAN = 1,
		LUA_TLIGHTUSERDATA = 2,
		LUA_TNUMBER = 3,
		LUA_TSTRING = 4,
		LUA_TTABLE = 5,
		LUA_TFUNCTION = 6,
		LUA_TUSERDATA = 7,
		LUA_TTHREAD = 8,
	}

	public enum LuaGCOptions
	{
		LUA_GCSTOP = 0,
		LUA_GCRESTART = 1,
		LUA_GCCOLLECT = 2,
		LUA_GCCOUNT = 3,
		LUA_GCCOUNTB = 4,
		LUA_GCSTEP = 5,
		LUA_GCSETPAUSE = 6,
		LUA_GCSETSTEPMUL = 7,
	}

	public enum LuaThreadStatus : int
	{
		LUA_YIELD = 1,
		LUA_ERRRUN = 2,
		LUA_ERRSYNTAX = 3,
		LUA_ERRMEM = 4,
		LUA_ERRERR = 5,
	}

	public sealed class LuaIndexes
	{
#if LUA_5_3
        // for lua5.3
        public static int LUA_REGISTRYINDEX = -1000000 - 1000;
#else
		// for lua5.1 or luajit
		public static int LUA_REGISTRYINDEX = -10000;
		public static int LUA_GLOBALSINDEX = -10002;
#endif
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct ReaderInfo
	{
		public String chunkData;
		public bool finished;
	}

#if UNITY_EDITOR_WIN || UNITY_STANDALONE_WIN
	[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
	public delegate int LuaCSFunction(IntPtr luaState);
#else
	public delegate int LuaCSFunction(IntPtr luaState);
#endif

	public delegate string LuaChunkReader(IntPtr luaState, ref ReaderInfo data, ref uint size);

	public delegate int LuaFunctionCallback(IntPtr luaState);
	public class LuaDLL
	{
		public static int LUA_MULTRET = -1;

		// common function 
		public static int luaL_dostring(IntPtr luaState, string chunk)
		{
			int result = LuaDLL.luaL_loadstring(luaState, chunk);
			if (result != 0)
				return result;

			return LuaDLL.lua_pcall(luaState, 0, -1, 0);
		}
		public static int lua_dostring(IntPtr luaState, string chunk)
		{
			return LuaDLL.luaL_dostring(luaState, chunk);
		}

		public static bool lua_isnil(IntPtr luaState, int index)
		{
			return (LuaDLL.lua_type(luaState, index) == LuaTypes.LUA_TNIL);
		}

		public static bool lua_isnumber(IntPtr luaState, int index)
		{
			return LuaDLLWrapper.lua_isnumber(luaState, index) > 0;
		}
		public static bool lua_isboolean(IntPtr luaState, int index)
		{
			return LuaDLL.lua_type(luaState, index) == LuaTypes.LUA_TBOOLEAN;
		}

		public static void lua_newtable(IntPtr luaState)
		{
			LuaDLL.lua_createtable(luaState, 0, 0);
		}

		public static void lua_pushcfunction(IntPtr luaState, LuaCSFunction function)
		{
			IntPtr fn = Marshal.GetFunctionPointerForDelegate(function);
			LuaDLL.lua_pushcclosure(luaState, fn, 0);
		}

		public static bool lua_isfunction(IntPtr luaState, int stackPos)
		{
			return lua_type(luaState, stackPos) == LuaTypes.LUA_TFUNCTION;
		}

		public static bool lua_islightuserdata(IntPtr luaState, int stackPos)
		{
			return lua_type(luaState, stackPos) == LuaTypes.LUA_TLIGHTUSERDATA;
		}

		public static bool lua_istable(IntPtr luaState, int stackPos)
		{
			return lua_type(luaState, stackPos) == LuaTypes.LUA_TTABLE;
		}

		public static bool lua_isthread(IntPtr luaState, int stackPos)
		{
			return lua_type(luaState, stackPos) == LuaTypes.LUA_TTHREAD;
		}

		public static void luaL_error(IntPtr luaState, string message)
		{
			LuaDLL.luaL_where(luaState, 1);
			LuaDLL.lua_pushstring(luaState, message);
			LuaDLL.lua_concat(luaState, 2);
			LuaDLL.lua_error(luaState);
		}

		public static void luaL_error(IntPtr luaState, string fmt, params object[] args)
		{
			string str = string.Format(fmt, args);
			luaL_error(luaState, str);
		}

		public static void lua_getref(IntPtr luaState, int reference)
		{
			LuaDLL.lua_rawgeti(luaState, LuaIndexes.LUA_REGISTRYINDEX, reference);
		}

		public static void lua_unref(IntPtr luaState, int reference)
		{
			LuaDLL.luaL_unref(luaState, LuaIndexes.LUA_REGISTRYINDEX, reference);
		}

		public static bool lua_isstring(IntPtr luaState, int index)
		{
			return LuaDLLWrapper.lua_isstring(luaState, index) > 0;
		}

		public static bool lua_iscfunction(IntPtr luaState, int index)
		{
			return LuaDLLWrapper.lua_iscfunction(luaState, index) > 0;
		}

		public static void luaL_getmetatable(IntPtr luaState, string meta)
		{
			LuaDLL.lua_getfield(luaState, LuaIndexes.LUA_REGISTRYINDEX, meta);
		}

		public static int lua_absindex(IntPtr luaState, int index)
		{
			return index > 0 ? index : lua_gettop(luaState) + index + 1;
		}

		public static int lua_upvalueindex(int i)
		{
#if LUA_5_3
            return LuaIndexes.LUA_REGISTRYINDEX - i;
#else
			return LuaIndexes.LUA_GLOBALSINDEX - i;
#endif
		}

		public static string lua_tostring(IntPtr luaState, int index)
		{
			int strlen;

			IntPtr str = luaS_tolstring32(luaState, index, out strlen); // fix il2cpp 64 bit

			if (str != IntPtr.Zero)
			{
				return Marshal.PtrToStringAnsi(str, strlen);
			}
			return null;
		}

		public static void luaS_pushcclosure(IntPtr l, LuaCSFunction f, int nup)
		{
			IntPtr fn = Marshal.GetFunctionPointerForDelegate(f);
			LuaDLL.lua_pushcclosure(l, fn, nup);
		}

		public static void lua_pushlstring(IntPtr luaState, byte[] str, int size)
		{
			LuaDLLWrapper.luaS_pushlstring(luaState, str, size);
		}

		// platform function

#if UNITY_WP8

		public delegate void Action<T1, T2, T3, T4, T5>(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5);
		public delegate void Action<T1, T2, T3, T4, T5, T6>(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 args6);
		public delegate void ActionOut<T1, T2, T3, T4, T5,T6>(T1 arg1, T2 arg2, out T3 arg3, out T4 arg4, out T5 arg5, out T6 arg6);
		public delegate void ActionOut<T1, T2, T3, T4,T5>(T1 arg1, T2 arg2, out T3 arg3, out T4 arg4, out T5 arg5);
		public delegate void ActionOut<T1, T2, T3,T4>(T1 arg1, T2 arg2, out T3 arg3, out T4 arg4);
		public delegate IntPtr luaS_tolstring32Delegate(IntPtr luaState, int index, out int strLen);

		static public Func<IntPtr> luaL_newstate;
		static public Func<IntPtr, string, int> luaL_loadstring;
		static public Func<IntPtr, int, int, int, int> lua_pcall;
		static public Func<IntPtr, int, int> luaL_ref;
		static public Func<IntPtr, int, LuaTypes> lua_type;
		static public Func<IntPtr, int> lua_gettop;
		static public Func<IntPtr, int, bool> lua_toboolean;
		static public Func<IntPtr, int, int> lua_tointeger;
		static public Func<IntPtr, int, double> lua_tonumber;
		static public Func<IntPtr, int, int> lua_isuserdata;
		static public Func<IntPtr, int> lua_pushthread;
		static public Func<IntPtr, int, int> lua_resume;
		static public Func<IntPtr, int, int, int> lua_call;
		static public Func<IntPtr, int, int> lua_yield;
		static public Func<IntPtr, int, int> lua_next;
		static public Func<IntPtr, int, IntPtr> lua_touserdata;
		static public Func<IntPtr, byte[], int, string, int> luaL_loadbuffer;
		static public Func<IntPtr, int, int, int> luaS_getcacheud;

		static public Func<IntPtr, int, int, int> lua_rawequal;

		static public Func<IntPtr, int,int> luaL_checkinteger;
		static public Func<IntPtr, int, int> lua_rawlen;
		static public Func<IntPtr, int, int> luaS_rawnetobj;
		static public Func<IntPtr, int, string, int> luaS_checkluatype;
		static public Func<IntPtr, int, double> luaL_checknumber;
		static public Func<IntPtr, int, string, int> luaS_subclassof;

		static public luaS_tolstring32Delegate luaS_tolstring32;

		static public Action<IntPtr> luaL_openlibs;
		static public Action<IntPtr, int, int> lua_createtable;
		static public Action<IntPtr, double> lua_pushnumber;
		static public Action<IntPtr, int> lua_pushinteger;
		static public Action<IntPtr> lua_pushnil;
		static public Action<IntPtr, int> lua_settop;
		static public Action<IntPtr, int> lua_remove;
		static public Action<IntPtr, int> lua_pop;
		static public Action<IntPtr, int> lua_pushvalue;
		static public Action<IntPtr, bool> lua_pushboolean;
		static public Action<IntPtr, int, string> lua_getfield;
		static public Action<IntPtr, int, string> lua_setfield;
		static public Action<IntPtr, int, int> lua_rawgeti;
		static public Action<IntPtr, int, int> lua_rawseti;
		static public Action<IntPtr, string> lua_pushstring;
		static public Action<IntPtr, IntPtr, int> lua_pushcclosure;
		static public Action<IntPtr, IntPtr> lua_pushlightuserdata;
		static public Action<IntPtr, string> lua_setglobal;
		static public Action<IntPtr, string> lua_getglobal;
		static public Action<IntPtr, int> lua_rawget;
		static public Action<IntPtr, int> lua_rawset;
		static public Action<IntPtr, int, int> luaL_unref;
		static public Action<IntPtr, int, LuaTypes> luaL_checktype;
		static public Action<IntPtr> lua_pushglobaltable;
		static public Action<IntPtr, int> lua_setmetatable;
		static public Action<IntPtr, int> lua_getmetatable;
		static public Action<IntPtr, int> lua_settable;
		static public Action<IntPtr, int> lua_gettable;
		static public Action<IntPtr, int> lua_replace;

		static public Action<IntPtr, float, float, float, float> luaS_pushColor;
		static public Action<IntPtr, float, float, float, float> luaS_pushQuaternion;
		static public Action<IntPtr, float, float> luaS_pushVector2;
		static public Action<IntPtr, float, float, float> luaS_pushVector3;
		static public Action<IntPtr, float, float, float, float> luaS_pushVector4;
		static public Action<IntPtr, int, float, float, float, float> luaS_setData;

		static public ActionOut<IntPtr, int, float, float, float, float> luaS_checkColor;
		static public ActionOut<IntPtr, int, float, float, float, float> luaS_checkQuaternion;
		static public ActionOut<IntPtr, int, float, float> luaS_checkVector2;
		static public ActionOut<IntPtr, int, float, float, float> luaS_checkVector3;
		static public ActionOut<IntPtr, int, float, float, float, float> luaS_checkVector4;

		static public Action<IntPtr, int, string, int, int> luaS_pushobject;

		static public Action<IntPtr, int> luaL_where;
		static public Action<IntPtr, int> lua_concat;
		static public Action<IntPtr> lua_error;
		static public Action<IntPtr,int> lua_insert;

#else // not wp8

		
#if UNITY_IPHONE && !UNITY_EDITOR
		const string LUADLL = "__Internal";
#else
		const string LUADLL = "slua";
#endif
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_tothread(IntPtr L, int index);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_xmove(IntPtr from, IntPtr to, int n);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr lua_newthread(IntPtr L);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_status(IntPtr L);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_pushthread(IntPtr L);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_gc(IntPtr luaState, LuaGCOptions what, int data);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr lua_typename(IntPtr luaState, int type);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern string luaL_gsub(IntPtr luaState, string str, string pattern, string replacement);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_isuserdata(IntPtr luaState, int stackPos);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_rawequal(IntPtr luaState, int stackPos1, int stackPos2);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_setfield(IntPtr luaState, int stackPos, string name);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int luaL_callmeta(IntPtr luaState, int stackPos, string name);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr luaL_newstate();
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_close(IntPtr luaState);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaL_openlibs(IntPtr luaState);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int luaL_loadstring(IntPtr luaState, string chunk);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_createtable(IntPtr luaState, int narr, int nrec);
#if LUA_5_3
        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void lua_getglobal(IntPtr luaState, string name);
        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void lua_setglobal(IntPtr luaState, string name);

		public static void lua_insert(IntPtr luaState, int newTop)
        {
            lua_rotate(luaState, newTop, 1);
        }

        public static void lua_pushglobaltable(IntPtr l)
        {
            lua_rawgeti(l, LuaIndexes.LUA_REGISTRYINDEX, 2); 
        }

        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int lua_rotate(IntPtr luaState, int index, int n);

        public static int lua_rawlen(IntPtr luaState, int stackPos)
		{
			return luaS_rawlen(luaState, stackPos);
		}

        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int luaL_loadbufferx(IntPtr luaState, byte[] buff, int size, string name, IntPtr x);

        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int lua_callk(IntPtr luaState, int nArgs, int nResults,int ctx,IntPtr k);

        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int lua_pcallk(IntPtr luaState, int nArgs, int nResults, int errfunc,int ctx,IntPtr k);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int luaS_pcall(IntPtr luaState, int nArgs, int nResults, int errfunc);
		
		public static int lua_call(IntPtr luaState, int nArgs, int nResults)
        {
            return lua_callk(luaState, nArgs, nResults, 0, IntPtr.Zero);
        }

        public static int lua_pcall(IntPtr luaState, int nArgs, int nResults, int errfunc)
        {
			return luaS_pcall(luaState, nArgs, nResults, errfunc);
        }

        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern double lua_tonumberx(IntPtr luaState, int index, IntPtr x);
        public static double lua_tonumber(IntPtr luaState, int index)
        {
            return lua_tonumberx(luaState, index, IntPtr.Zero);
        }        
        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int64 lua_tointegerx(IntPtr luaState, int index,IntPtr x);
        public static int lua_tointeger(IntPtr luaState, int index)
        {
            return (int)lua_tointegerx(luaState, index, IntPtr.Zero);
        }


        public static int luaL_loadbuffer(IntPtr luaState, byte[] buff, int size, string name)
        {
            return luaL_loadbufferx(luaState, buff, size, name, IntPtr.Zero);
        }

        public static void lua_remove(IntPtr l, int idx)
        {
            lua_rotate(l, (idx), -1);
            lua_pop(l, 1);
        }

        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void lua_rawgeti(IntPtr luaState, int tableIndex, Int64 index);

        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void lua_rawseti(IntPtr luaState, int tableIndex, Int64 index);

        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void lua_pushinteger(IntPtr luaState, Int64 i);

        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int64 luaL_checkinteger(IntPtr luaState, int stackPos); 

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int luaS_yield(IntPtr luaState,int nrets);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_resume(IntPtr L, IntPtr from, int narg);
#else
        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_resume(IntPtr L, int narg);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_lessthan(IntPtr luaState, int stackPos1, int stackPos2);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_getfenv(IntPtr luaState, int stackPos);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_yield(IntPtr L, int nresults);

		public static void lua_getglobal(IntPtr luaState, string name)
		{
			LuaDLL.lua_pushstring(luaState, name);
			LuaDLL.lua_gettable(luaState, LuaIndexes.LUA_GLOBALSINDEX);
		}

		public static void lua_setglobal(IntPtr luaState, string name)
		{
			LuaDLL.lua_pushstring(luaState, name);
			LuaDLL.lua_insert(luaState, -2);
			LuaDLL.lua_settable(luaState, LuaIndexes.LUA_GLOBALSINDEX);
		}

		public static void lua_pushglobaltable(IntPtr l)
		{
			LuaDLL.lua_pushvalue(l, LuaIndexes.LUA_GLOBALSINDEX);
		}

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_insert(IntPtr luaState, int newTop);

		public static int lua_rawlen(IntPtr luaState, int stackPos)
		{
            return LuaDLLWrapper.luaS_objlen(luaState, stackPos);
		}

		public static int lua_strlen(IntPtr luaState, int stackPos)
		{
			return lua_rawlen(luaState, stackPos);
		}

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_call(IntPtr luaState, int nArgs, int nResults);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_pcall(IntPtr luaState, int nArgs, int nResults, int errfunc);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern double lua_tonumber(IntPtr luaState, int index);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_tointeger(IntPtr luaState, int index);

		public static int luaL_loadbuffer(IntPtr luaState, byte[] buff, int size, string name)
        {
            return LuaDLLWrapper.luaLS_loadbuffer(luaState, buff, size, name);
        }

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_remove(IntPtr luaState, int index);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_rawgeti(IntPtr luaState, int tableIndex, int index);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_rawseti(IntPtr luaState, int tableIndex, int index);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_pushinteger(IntPtr luaState, int i);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int luaL_checkinteger(IntPtr luaState, int stackPos);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_replace(IntPtr luaState, int index);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_setfenv(IntPtr luaState, int stackPos);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_equal(IntPtr luaState, int index1, int index2);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int luaL_loadfile(IntPtr luaState, string filename);
#endif


		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_settop(IntPtr luaState, int newTop);
		public static void lua_pop(IntPtr luaState, int amount)
		{
			LuaDLL.lua_settop(luaState, -(amount) - 1);
		}

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_gettable(IntPtr luaState, int index);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_rawget(IntPtr luaState, int index);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_settable(IntPtr luaState, int index);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_rawset(IntPtr luaState, int index);

#if LUA_5_3
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_setmetatable(IntPtr luaState, int objIndex);
#else
        [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int lua_setmetatable(IntPtr luaState, int objIndex);
#endif


		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_getmetatable(IntPtr luaState, int objIndex);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_pushvalue(IntPtr luaState, int index);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_gettop(IntPtr luaState);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern LuaTypes lua_type(IntPtr luaState, int index);
		
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int luaL_ref(IntPtr luaState, int registryIndex);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaL_unref(IntPtr luaState, int registryIndex, int reference);
		
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_pushnil(IntPtr luaState);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaL_checktype(IntPtr luaState, int p, LuaTypes t);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr lua_tocfunction(IntPtr luaState, int index);


		public static bool lua_toboolean(IntPtr luaState, int index)
        {
            return LuaDLLWrapper.lua_toboolean(luaState, index) > 0;
        }


		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr luaS_tolstring32(IntPtr luaState, int index, out int strLen);

		

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr lua_atpanic(IntPtr luaState, LuaCSFunction panicf);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_pushnumber(IntPtr luaState, double number);

		public static void lua_pushboolean(IntPtr luaState, bool value)
        {
            LuaDLLWrapper.lua_pushboolean(luaState, value ? 1 : 0);
        }

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_pushstring(IntPtr luaState, string str);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int luaL_newmetatable(IntPtr luaState, string meta);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_getfield(IntPtr luaState, int stackPos, string meta);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr luaL_checkudata(IntPtr luaState, int stackPos, string meta);

		public static bool luaL_getmetafield(IntPtr luaState, int stackPos, string field)
        {
            return LuaDLLWrapper.luaL_getmetafield(luaState, stackPos, field) > 0;
        }
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_load(IntPtr luaState, LuaChunkReader chunkReader, ref ReaderInfo data, string chunkName);




		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_error(IntPtr luaState);

		public static bool lua_checkstack(IntPtr luaState, int extra)
        {
            return LuaDLLWrapper.lua_checkstack(luaState, extra) > 0;
        }
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int lua_next(IntPtr luaState, int index);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_pushlightuserdata(IntPtr luaState, IntPtr udata);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaL_where(IntPtr luaState, int level);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern double luaL_checknumber(IntPtr luaState, int stackPos);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_concat(IntPtr luaState, int n);


		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_newuserdata(IntPtr luaState, int val);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int luaS_rawnetobj(IntPtr luaState, int obj);


		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr lua_touserdata(IntPtr luaState, int index);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void lua_pushcclosure(IntPtr l, IntPtr f, int nup);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_checkVector2(IntPtr l, int p, out float x, out float y);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_checkVector3(IntPtr l, int p, out float x, out float y, out float z);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_checkVector4(IntPtr l, int p, out float x, out float y, out float z, out float w);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_checkQuaternion(IntPtr l, int p, out float x, out float y, out float z, out float w);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_checkColor(IntPtr l, int p, out float x, out float y, out float z, out float w);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_pushVector2(IntPtr l, float x, float y);


		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_pushVector3(IntPtr l, float x, float y, float z);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_pushVector4(IntPtr l, float x, float y, float z, float w);
		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_pushQuaternion(IntPtr l, float x, float y, float z, float w);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_pushColor(IntPtr l, float x, float y, float z, float w);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_setData(IntPtr l, int p, float x, float y, float z, float w);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int luaS_checkluatype(IntPtr l, int p, string t);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern void luaS_pushobject(IntPtr l, int index, string t, int gco, int cref);

		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int luaS_getcacheud(IntPtr l, int index, int cref);


		[DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
		public static extern int luaS_subclassof(IntPtr l, int index, string t);
#endif
	}
}