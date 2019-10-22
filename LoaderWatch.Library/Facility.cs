using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using LoaderWatch.Library.NativeDebug;

namespace LoaderWatch.Library
{
	public sealed class Facility
	{
		public delegate void DebugExceptionEventHandler(object sender, DebugExceptionEventArgs e);
		public delegate void ProcessCreatedEventHandler(object sender, ProcessCreatedEventArgs e);
		public delegate void ProcessModuleLoadEventHandler(object sender, ProcessModuleLoadEventArgs e);
		public delegate void ProcessDebugOutputEventHandler(object sender, ProcessDebugOutputEventArgs e);
		public delegate void ProcessBreakpointEventHandler(object sender, ProcessBreakpointEventArgs e);

		public event DebugExceptionEventHandler DebugException;
		public event ProcessCreatedEventHandler ProcessCreated;
		public event ProcessModuleLoadEventHandler ProcessModuleLoad;
		public event ProcessDebugOutputEventHandler ProcessDebugOutput;
		public event ProcessBreakpointEventHandler ProcessBreakpoint;

		public Facility()
		{
		}

		public static IntPtr Offset(string name)
		{
			var res = NativeFacility.LookupSymbolOffset($@"*!{name}");
			if (res == null)
				throw new ApplicationException($"\"{name}\" lookup failed");

			if (res.Count == 0)
				throw new ApplicationException($"\"{name}\" not found");

			if (res.Count == 1)
			{
				var va = res[0].Data.Address - res[0].Data.ModBase;
				if (va < 0)
					throw new ApplicationException($"\"{name}\" VA is negative");

				return new IntPtr(checked((long)va));
			}

			throw new ApplicationException($"\"{name}\" is an ambiguous symbol.");
		}

		public void DebugThread(string executable)
		{
			var lpStartupInfo = new STARTUPINFO();
			var lpProcessInformation = new PROCESS_INFORMATION();
			if (!NativeMethods.CreateProcess(executable, "",
				IntPtr.Zero, IntPtr.Zero, false,
				NativeMethods.CreateProcessFlags.DEBUG_PROCESS |
				NativeMethods.CreateProcessFlags.DEBUG_ONLY_THIS_PROCESS,
				IntPtr.Zero, null, lpStartupInfo,
				lpProcessInformation))
			{
				Console.WriteLine($"FAIL with {Marshal.GetLastWin32Error()}");
			}

			var process = Process.GetProcessById(lpProcessInformation.dwProcessId);

			ProcessContext context = null;

			var loop = true;
			while (loop)
			{
				var debugEvent = new DebugEvent64();
				NativeMethods.WaitForDebugEvent64(ref debugEvent, -1);

				try
				{
				switch (debugEvent.header.dwDebugEventCode)
				{
					case NativeDebugEventCode.None:
						break;
					case NativeDebugEventCode.EXCEPTION_DEBUG_EVENT:
							switch (debugEvent.union.Exception.ExceptionRecord.ExceptionCode)
							{
								case ExceptionCode.STATUS_BREAKPOINT:
									OnProcessBreakpoint(new ProcessBreakpointEventArgs(context, lpProcessInformation.hProcess, debugEvent.header.dwProcessId, debugEvent.header.dwThreadId));
									break;
							}
							break;
					case NativeDebugEventCode.CREATE_THREAD_DEBUG_EVENT:
						break;
					case NativeDebugEventCode.CREATE_PROCESS_DEBUG_EVENT:
					{
						context = new ProcessContext(process);
						OnProcessCreated(new ProcessCreatedEventArgs(context, lpProcessInformation.hProcess, debugEvent.header.dwProcessId, debugEvent.header.dwThreadId));
						break;
					}
					case NativeDebugEventCode.EXIT_THREAD_DEBUG_EVENT:
						break;
					case NativeDebugEventCode.EXIT_PROCESS_DEBUG_EVENT:
					{
						loop = false;
						break;
					}
					case NativeDebugEventCode.LOAD_DLL_DEBUG_EVENT:
					{
						var loadDllDebugInfo = debugEvent.union.LoadDll;
						string value = null;
						if (loadDllDebugInfo.lpImageName != IntPtr.Zero)
						{
							var intPtr = context.Memory[loadDllDebugInfo.lpImageName, false].Read<IntPtr>();
							if (intPtr != IntPtr.Zero)
							{
								value = context.Memory[intPtr, false]
									.ReadString(loadDllDebugInfo.fUnicode != 0 ? Encoding.Unicode : Encoding.ASCII);
							}
						}

						OnProcessModuleLoad(new ProcessModuleLoadEventArgs(context, lpProcessInformation.hProcess, debugEvent.header.dwProcessId, debugEvent.header.dwThreadId)
						{
							Name = value
						});

						break;
					}
					case NativeDebugEventCode.UNLOAD_DLL_DEBUG_EVENT:
						break;
					case NativeDebugEventCode.OUTPUT_DEBUG_STRING_EVENT:
					{
						var outputDebugStringInfo = debugEvent.union.OutputDebugString;

						var value = context.Memory[outputDebugStringInfo.lpDebugStringData, false]
							.ReadString(outputDebugStringInfo.fUnicode != 0 ? Encoding.Unicode : Encoding.ASCII);

						OnProcessDebugOutput(new ProcessDebugOutputEventArgs(context, lpProcessInformation.hProcess, debugEvent.header.dwProcessId, debugEvent.header.dwThreadId)
						{
							Value = value
						});
						break;
					}
					case NativeDebugEventCode.RIP_EVENT:
						break;
					default:
						throw new ArgumentOutOfRangeException();
					}
				}
				catch (Exception e)
				{
					OnDebugException(new DebugExceptionEventArgs(context, lpProcessInformation.hProcess, debugEvent.header.dwProcessId, debugEvent.header.dwThreadId)
					{
						Exception = e
					});

					NativeMethods.ContinueDebugEvent(debugEvent.header.dwProcessId, debugEvent.header.dwThreadId,
						NativeMethods.ContinueStatus.DBG_EXCEPTION_NOT_HANDLED);

					continue;
				}

				NativeMethods.ContinueDebugEvent(debugEvent.header.dwProcessId, debugEvent.header.dwThreadId,
					NativeMethods.ContinueStatus.DBG_CONTINUE);
			}
		}

		private void OnProcessCreated(ProcessCreatedEventArgs e)
		{
			ProcessCreated?.Invoke(this, e);
		}

		private void OnProcessModuleLoad(ProcessModuleLoadEventArgs e)
		{
			ProcessModuleLoad?.Invoke(this, e);
		}

		private void OnProcessDebugOutput(ProcessDebugOutputEventArgs e)
		{
			ProcessDebugOutput?.Invoke(this, e);
		}

		private static readonly int LinkOffset = LdrModule.GetFieldOffset(nameof(LdrModule.InLoadOrderLinks));
		private static HashSet<string> ModulesWithSymbols = new HashSet<string>();

		public static void LoadSymbols(ProcessContext context)
		{
			foreach (var entry in context.PebLdr.InLoadOrderModuleList)
			{
				var module = entry.ContainingRecord<LdrModule>(LinkOffset);
				var name = module.BaseDllName;

				var dllName = module.FullDllName.Trim();

				if (ModulesWithSymbols.Contains(dllName))
					continue;

				Console.Write($"Loading symbols for {name}... ");

				var status = NativeFacility.PhLoadModuleDefaultSymbolProvider(module);
				Console.WriteLine(status ? "OK" : "FAIL");

				ModulesWithSymbols.Add(dllName);
			}
		}

		private void OnDebugException(DebugExceptionEventArgs e)
		{
			DebugException?.Invoke(this, e);
		}

		private void OnProcessBreakpoint(ProcessBreakpointEventArgs e)
		{
			ProcessBreakpoint?.Invoke(this, e);
		}
	}
}