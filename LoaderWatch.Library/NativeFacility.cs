using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace LoaderWatch.Library
{
	public static class NativeFacility
	{
		public struct PH_THREAD_STACK_FRAME
		{
			public IntPtr PcAddress;
			public IntPtr ReturnAddress;
			public IntPtr FrameAddress;
			public IntPtr StackAddress;
			public IntPtr BStoreAddress;
			public IntPtr Parameter1;
			public IntPtr Parameter2;
			public IntPtr Parameter3;
			public IntPtr Parameter4;
			public uint Flags;
		}

		public unsafe struct PH_STRING
		{
			public IntPtr Length;
			public char* Buffer;
		}

		public unsafe struct THREAD_STACK_ITEM
		{
			public PH_THREAD_STACK_FRAME StackFrame;
			public uint Index;
			public PH_STRING* Symbol;
		}

		public unsafe delegate void PhpWalkThreadStackCallback_CALLBACK(THREAD_STACK_ITEM* item);

		public struct PH_THREAD_STACK_CONTEXT
		{
			public IntPtr ProcessId;
			public IntPtr ThreadId;
			public IntPtr ThreadHandle;

			public bool StopWalk;

			public uint Count;

			public int WalkStatus;

			public IntPtr Callback;
		}

		public struct ThreadStackItem
		{
			public PH_THREAD_STACK_FRAME StackFrame;
			public uint Index;
			public string Symbol;
		}

		public struct SymbolInfoSafe
		{
			public uint TypeIndex;     // Type Index of symbol
			public uint Index;
			public uint Size;
			public ulong ModBase;      // Base Address of module comtaining this symbol
			public uint Flags;
			public ulong Value;        // Value of symbol, ValuePresent should be 1
			public ulong Address;      // Address of symbol including base address of module
			public uint Register;      // register holding value or pointer to value
			public uint Scope;         // scope of the symbol
			public uint Tag;           // pdb classification
		}

		public struct PH_SYMBOL_INFO
		{
			public PH_STRING Name;
			public SymbolInfoSafe Data;
		}

		public struct SymbolInfo
		{
			public string Name;
			public SymbolInfoSafe Data;
		}

		public unsafe delegate void LookupSymbolOffsetCallback_CALLBACK(PH_SYMBOL_INFO* item);

		[DllImport("LoaderWatch.Library.Native.dll", CallingConvention = CallingConvention.StdCall,
			ExactSpelling = true)]
		private static extern unsafe int PhpRefreshThreadStackThreadStart(
			[In, Out] ref PH_THREAD_STACK_CONTEXT threadStackContext);

		[DllImport("LoaderWatch.Library.Native.dll", CallingConvention = CallingConvention.StdCall,
			ExactSpelling = true)]
		public static extern int InitializePhLib();

		[DllImport("LoaderWatch.Library.Native.dll", CallingConvention = CallingConvention.StdCall,
			ExactSpelling = true)]
		public static extern void PhCreateThreadProvider([In] IntPtr processId);

		[DllImport("LoaderWatch.Library.Native.dll", CallingConvention = CallingConvention.StdCall,
			ExactSpelling = true)]
		private static extern unsafe bool PhLoadModuleDefaultSymbolProvider(
			[In] LdrModule.LDR_DATA_TABLE_ENTRY* module,
			[MarshalAs(UnmanagedType.LPWStr)] string fullName);

		[DllImport("LoaderWatch.Library.Native.dll", CallingConvention = CallingConvention.StdCall,
			ExactSpelling = true)]
		public static extern void SetSearchPathSymbolProvider([MarshalAs(UnmanagedType.LPWStr)] string path);

		[DllImport("LoaderWatch.Library.Native.dll", CallingConvention = CallingConvention.StdCall,
			ExactSpelling = true)]
		public static extern int LoadDllProcess(
			IntPtr processId,
			IntPtr threadId,
			[MarshalAs(UnmanagedType.LPWStr)] string path);

		[DllImport("LoaderWatch.Library.Native.dll", CallingConvention = CallingConvention.StdCall,
			ExactSpelling = true)]
		public static extern void InitCreateUserThreadEx([In] IntPtr pointer);

		[DllImport("LoaderWatch.Library.Native.dll", CallingConvention = CallingConvention.StdCall,
			ExactSpelling = true)]
		private static extern unsafe bool LookupSymbolOffset(
			[MarshalAs(UnmanagedType.LPWStr)] string mask,
			IntPtr callback);

		public static bool PhLoadModuleDefaultSymbolProvider(LdrModule module)
		{
			unsafe
			{
				fixed (LdrModule.LDR_DATA_TABLE_ENTRY* ptr = &module.Native)
					return PhLoadModuleDefaultSymbolProvider(ptr, module.FullDllName);
			}
		}

		private static readonly List<ThreadStackItem> ThreadStackItemResult = new List<ThreadStackItem>();

		internal static unsafe void ThreadStackCallback(THREAD_STACK_ITEM* item)
		{
			ThreadStackItemResult.Add(new ThreadStackItem
			{
				StackFrame = item->StackFrame,
				Index = item->Index,
				Symbol = new string(item->Symbol->Buffer, 0, item->Symbol->Length.ToInt32() / 2)
			});
		}

		public static List<ThreadStackItem> ThreadStack(IntPtr processId, IntPtr threadId, IntPtr threadHandle)
		{
			Monitor.Enter(ThreadStackItemResult);
			try
			{
				ThreadStackItemResult.Clear();
				unsafe
				{
					var context = new PH_THREAD_STACK_CONTEXT
					{
						ProcessId = processId,
						ThreadId = threadId,
						ThreadHandle = threadHandle,
						Callback =
							Marshal.GetFunctionPointerForDelegate<PhpWalkThreadStackCallback_CALLBACK>(
								ThreadStackCallback)
					};

					PhpRefreshThreadStackThreadStart(ref context);

					if (context.StopWalk || context.WalkStatus < 0)
						return null;

					return new List<ThreadStackItem>(ThreadStackItemResult);
				}
			}
			finally
			{
				Monitor.Exit(ThreadStackItemResult);
			}
		}

		private static readonly List<SymbolInfo> LookupSymbolOffsetResult = new List<SymbolInfo>();

		internal static unsafe void LookupSymbolOffsetCallback(PH_SYMBOL_INFO* item)
		{
			LookupSymbolOffsetResult.Add(new SymbolInfo
			{
				Name = new string(item->Name.Buffer, 0, item->Name.Length.ToInt32() / 2),
				Data = item->Data,
			});
		}

		public static List<SymbolInfo> LookupSymbolOffset(string mask)
		{
			Monitor.Enter(LookupSymbolOffsetResult);
			try
			{
				LookupSymbolOffsetResult.Clear();
				unsafe
				{
					var callback = Marshal.GetFunctionPointerForDelegate<LookupSymbolOffsetCallback_CALLBACK>(LookupSymbolOffsetCallback);

					var result = LookupSymbolOffset(mask, callback);

					if (!result)
						return null;

					return new List<SymbolInfo>(LookupSymbolOffsetResult);
				}
			}
			finally
			{
				Monitor.Exit(LookupSymbolOffsetResult);
			}
		}

		public static string Status(int status)
		{
			return status == 0 ? "OK" : $"0x{status:X8}";
		}


		[DllImport("KERNEL32.DLL", SetLastError = true, CharSet = CharSet.Unicode, ExactSpelling = true)]
		public static extern IntPtr AddDllDirectory(string path);

		public const uint LOAD_LIBRARY_SEARCH_DEFAULT_DIRS = 0x00001000;

		[DllImport("KERNEL32.DLL", SetLastError = true, ExactSpelling = true)]
		public static extern bool SetDefaultDllDirectories(uint flags);
	}
}