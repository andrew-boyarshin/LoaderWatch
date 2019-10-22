using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Threading;
using LoaderWatch.Library;
using LoaderWatch.Library.MemorySharp;
using Microsoft.Win32;

namespace LoaderWatch.CLI
{
	internal static class Program
	{
		private const string InjectModuleName = "LoaderWatch.Inject";
		private const string InjectDllName = InjectModuleName + ".dll";
		private const RegexOptions RegexCommonOptions = RegexOptions.Compiled | RegexOptions.Singleline | RegexOptions.CultureInvariant;
		private const string DefaultExecutable = @"C:\Windows\System32\notepad.exe";
		private static readonly Facility DebugFacility = new Facility();
		private static Thread _thread;
		private static string[] _rootDirectories = null;
		private static string[] _platformDirectories = null;
		private static string TargetExecutable = DefaultExecutable;

		private delegate void ProcessOutputLineMatchDelegate(ProcessContext context, Match match);

		private static readonly IList<(Regex, ProcessOutputLineMatchDelegate)> InjectOutputLineMatchers = new List<(Regex, ProcessOutputLineMatchDelegate)>
		{
			(new Regex(@"\[inj\]\|=(.+)", RegexCommonOptions), ProcessTargetOffsetsLine),
			(new Regex(@"\[inj\]: HookLdrpAllocateModuleEntry\((.+)\) -> (\S+)", RegexCommonOptions), ProcessAllocateModuleEntryLine),
			(new Regex(@"\[inj\]: HookLdrpDestroyNode\((\S+)\)", RegexCommonOptions), ProcessDestroyNodeLine),
			(new Regex(@"\[inj\]: HookLdrpMapDllWithSectionHandle\((\S+), \.\.\.\)", RegexCommonOptions), ProcessMapDllWithSectionHandleLine),
			(new Regex(@"\[inj\]: HookLdrpFreeLoadContext\((\S+)\)", RegexCommonOptions), ProcessFreeLoadContextLine),
			(new Regex(@"\[inj\]: LdrpMergeNodes EXIT", RegexCommonOptions), ProcessMergeNodesExitLine),
			(new Regex(@"\[inj\]: HookLdrpUnmapModule\((\S+)\)", RegexCommonOptions), ProcessUnmapModuleLine),
			(new Regex(@"\[inj\]: HookLdrpCondenseGraphRecurse\((\S+), (\S+), (\S+)\) ENTER", RegexCommonOptions), ProcessCondenseGraphRecurseEnterLine),
			(new Regex(@"\[inj\]: HookLdrpCondenseGraphRecurse\((\S+), (\S+), (\S+)\) EXIT", RegexCommonOptions), ProcessCondenseGraphRecurseExitLine),
			(new Regex(@"\[inj\]: HookLdrpPreprocessDllName\((.+), (.+), (.+), (\S+)\) ENTER", RegexCommonOptions), ProcessPreprocessDllNameEnterLine),
			(new Regex(@"\[inj\]: HookLdrpPreprocessDllName\((.+), (.+), (.+), (\S+)\) EXIT \[(\S+)\]", RegexCommonOptions), ProcessPreprocessDllNameExitLine),
			(new Regex(@"\[inj\]: HookLdrpApplyFileNameRedirection\((.+), (.+), (.+), (.+), (\S+)\) ENTER", RegexCommonOptions), ProcessApplyFileNameRedirectionEnterLine),
			(new Regex(@"\[inj\]: HookLdrpApplyFileNameRedirection\((.+), (.+), (.+), (.+), (\S+)\) EXIT \[(\S+)\]", RegexCommonOptions), ProcessApplyFileNameRedirectionExitLine),
		};

		private static readonly string[] HookTargets =
		{
			"LdrpAllocateModuleEntry",
			"LdrpMergeNodes",
			"LdrpDestroyNode",
			"LdrpMapDllWithSectionHandle",
			"LdrpFreeLoadContext",
			"LdrpUnmapModule",
			"LdrpCondenseGraphRecurse",
			"LdrpPreprocessDllName",
			"LdrpApplyFileNameRedirection",
		};

		private static void ProcessTargetOffsetsLine(ProcessContext context, Match match)
		{
			var pointer = new IntPtr(long.Parse(match.Groups[1].Value));
			int offset = 0;
			var memory = context.Memory;

			foreach (var hookTarget in HookTargets)
			{
				memory[pointer + offset, false].Write(Facility.Offset(hookTarget));
				offset += IntPtr.Size;
			}
		}

		private static void ProcessAllocateModuleEntryLine(ProcessContext context, Match match)
		{
			var pointer = new IntPtr(Convert.ToInt64(match.Groups[2].Value, 16));
			var module = context.As<LdrModule>(pointer);

			if (module.Dead)
				module.Dead = false;

			var node = module.DdagNode;

			if (node.Dead)
				node.Dead = false;

			var loadContext = module.LoadContext;

			if (loadContext.Dead)
			{
				loadContext.Dead = false;
				Console.WriteLine($"LOAD_CONTEXT \"{loadContext.DllName}\" is resurrected.");
			}
		}

		private static void ProcessDestroyNodeLine(ProcessContext context, Match match)
		{
			var pointer = new IntPtr(Convert.ToInt64(match.Groups[1].Value, 16));
			var node = context.As<LdrGraphNode>(pointer);
			Console.WriteLine($"NODE \"{node.RootModuleName}\" is dying.");
			node.Dead = true;
		}

		private static void ProcessMapDllWithSectionHandleLine(ProcessContext context, Match match)
		{
			var pointer = new IntPtr(Convert.ToInt64(match.Groups[1].Value, 16));
			var node = context.As<LdrLoadContext>(pointer);
			Console.WriteLine($"\"{node.DllName}\" is mapping...");
			node.Entry.DdagNode.RefreshRootModuleName(10);
		}

		private static void ProcessFreeLoadContextLine(ProcessContext context, Match match)
		{
			var pointer = new IntPtr(Convert.ToInt64(match.Groups[1].Value, 16));
			var node = context.As<LdrLoadContext>(pointer);
			Console.WriteLine($"LOAD_CONTEXT \"{node.DllName}\" is dying.");
			node.Dead = true;
		}

		private static void ProcessMergeNodesExitLine(ProcessContext context, Match match)
		{
			Console.WriteLine(EqEqEq);
			Console.WriteLine(EqEqEq);

			DumpGraph(context);

			Console.WriteLine(EqEqEq);
			Console.WriteLine(EqEqEq);
		}

		private static void ProcessUnmapModuleLine(ProcessContext context, Match match)
		{
			var pointer = new IntPtr(Convert.ToInt64(match.Groups[1].Value, 16));
			var node = context.As<LdrModule>(pointer);
			Console.WriteLine($"MODULE \"{node.BaseDllName}\" is dying.");
			node.DdagNode.RefreshRootModuleName(0, node);
			node.Dead = true;
		}

		private static void ProcessCondenseGraphRecurseEnterLine(ProcessContext context, Match match)
		{
			const string marker = "IN";
			ProcessCondenseGraphRecurseAnyLine(context, match, marker);
		}

		private static void ProcessCondenseGraphRecurseExitLine(ProcessContext context, Match match)
		{
			const string marker = "OUT";
			ProcessCondenseGraphRecurseAnyLine(context, match, marker);
		}

		private static void ProcessCondenseGraphRecurseAnyLine(ProcessContext context, Match match, string marker)
		{
			var pointer = new IntPtr(Convert.ToInt64(match.Groups[1].Value, 16));
			var node = context.As<LdrGraphNode>(pointer);
			var listPointer = Convert.ToInt64(match.Groups[3].Value, 16);
			var list = context.As<SingleListEntry>(new IntPtr(listPointer));
			var preorderNumber = uint.Parse(match.Groups[2].Value);

			var modules = list
				?.TakeWhile(x => x.Address.ToInt64() > 100)
				.Select(x => x.ContainingRecord<LdrGraphNode>(LdrGraphNode.CondenseLinkOffset).RootModuleName)
				.ToArray();
			var modulesText = modules.Length > 0 ? string.Join(", ", modules) : "<INITIAL>";

			Console.WriteLine($"CONDENSE {marker} {node.RootModuleName} ({preorderNumber}): {modulesText}");
		}

		private static void ProcessPreprocessDllNameEnterLine(ProcessContext context, Match match)
		{
			Console.WriteLine($"PreprocessDllName IN {match.Groups[1].Value} {match.Groups[2].Value} {match.Groups[3].Value} {match.Groups[4].Value}");
		}

		private static void ProcessPreprocessDllNameExitLine(ProcessContext context, Match match)
		{
			Console.WriteLine($"PreprocessDllName OUT {match.Groups[1].Value} {match.Groups[2].Value} {match.Groups[3].Value} {match.Groups[4].Value} -> 0x{match.Groups[5].Value}");
		}

		private static void ProcessApplyFileNameRedirectionEnterLine(ProcessContext context, Match match)
		{
			Console.WriteLine($"ApplyFileNameRedirection IN {match.Groups[1].Value} {match.Groups[2].Value} {match.Groups[3].Value} {match.Groups[4].Value}");
		}

		private static void ProcessApplyFileNameRedirectionExitLine(ProcessContext context, Match match)
		{
			Console.WriteLine($"ApplyFileNameRedirection OUT {match.Groups[1].Value} {match.Groups[2].Value} {match.Groups[3].Value} {match.Groups[4].Value} SxS:{match.Groups[5].Value} -> 0x{match.Groups[6].Value}");
		}

		// Based on https://stackoverflow.com/a/11672569
		public static string WhereSearch(string filename)
		{
			if (File.Exists(filename))
				return Path.GetFullPath(filename);

			var extensions = new[] { string.Empty }.Concat(
				Environment.GetEnvironmentVariable("PATHEXT")
					.Split(';', StringSplitOptions.RemoveEmptyEntries)
					.Where(e => e.StartsWith('.'))
				);

			const string appPaths = @"Software\Microsoft\Windows\CurrentVersion\App Paths";

			using var registryOverride = extensions.Select(e => Path.Combine(appPaths, filename + e))
				.SelectMany(path => new[] { Registry.CurrentUser.OpenSubKey(path), Registry.LocalMachine.OpenSubKey(path) })
				.FirstOrDefault(x => x != null);

			if (registryOverride != null)
			{
				var path = registryOverride.GetValue(null);
				if (path != null)
					return path as string;
			}

			var paths = new[] { Environment.CurrentDirectory }.Concat(
				Environment.GetEnvironmentVariable("PATH").Split(';', StringSplitOptions.RemoveEmptyEntries)
				);

			var combinations = paths.SelectMany(
				x => extensions,
				(path, extension) => Path.Combine(path, filename + extension)
				);

			return combinations.FirstOrDefault(File.Exists);
		}

		internal static void Main(string[] args)
		{
			if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
			{
				Console.WriteLine("It is obvious that debugging Windows component requires Windows OS, is it not?");
				return;
			}

			NativeFacility.SetDefaultDllDirectories(NativeFacility.LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

			_rootDirectories = new[] {
				Environment.CurrentDirectory,
				Path.GetDirectoryName(typeof(Program).Assembly.Location),
			}.Where(Directory.Exists).ToArray();

			_platformDirectories = _rootDirectories
				.Select(x => Path.Combine(x, "64"))
				.Where(Directory.Exists)
				.ToArray();

			foreach (var path in _platformDirectories)
			{
				NativeFacility.AddDllDirectory(path);
			}

			NativeFacility.InitializePhLib();

			if (args.Length > 0)
				TargetExecutable = WhereSearch(args[0]);

			var imageName = Path.GetFileName(TargetExecutable);
			var keyPath = $@"SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\{imageName}";
			var key = Registry.LocalMachine.OpenSubKey(keyPath, false);
			const string GlobalFlagValueName = "GlobalFlag";
			var currentFlag = (key?.GetValue(GlobalFlagValueName) as int?) ?? 0;
			key?.Close();
			if ((currentFlag & 0x2) == 0)
			{
				try
				{
					key = Registry.LocalMachine.CreateSubKey(keyPath, true);
					key.SetValue(GlobalFlagValueName, currentFlag | 0x2, RegistryValueKind.DWord);
					key.Close();
				}
				catch (UnauthorizedAccessException)
				{
					Console.WriteLine("Couldn't set \"Image File Execution Options\" ShowSnaps flag. Restart as administrator to continue.");
					return;
				}
			}

			DebugFacility.ProcessModuleLoad += FacilityOnProcessModuleLoad;
			DebugFacility.ProcessDebugOutput += FacilityOnProcessDebugOutput;
			DebugFacility.ProcessCreated += FacilityOnProcessCreated;
			DebugFacility.DebugException += DebugFacilityOnDebugException;
			DebugFacility.ProcessBreakpoint += FacilityOnProcessBreakpoint;

			_thread = new Thread(() => DebugFacility.DebugThread(TargetExecutable));

			_thread.Start();
			_thread.Join();
		}

		private static bool ShowSnaps = false;

		private static void FacilityOnProcessModuleLoad(object sender, ProcessModuleLoadEventArgs e)
		{
			if (e.Name == null)
				return;

#if false
			if (e.Name.Contains("KERNEL32", StringComparison.OrdinalIgnoreCase))
			{
				// Earliest point in limited mode it's safe to call Facility.Offset
			}
#endif

			if (string.Equals(e.Name, @"C:\WINDOWS\System32\KERNELBASE.dll",
				StringComparison.OrdinalIgnoreCase))
			{
				var injectLib = _platformDirectories.Select(x => Path.Combine(x, InjectDllName)).First(File.Exists);
				var status = NativeFacility.LoadDllProcess(e.ProcessId, e.ThreadId, injectLib);

				Console.WriteLine($"Injection scheduling [{injectLib}] -> {NativeFacility.Status(status)}");

				if ((uint)status == 0xC000036C)
				{
					Console.WriteLine("The LoaderWatch driver didn't create the device. It is likely you haven't installed the driver or it was blocked from loading.");
					Console.WriteLine("LoaderWatch will continue in limited functionality mode.");
					Console.WriteLine("* No registry hooks");
					Console.WriteLine("* No control of module, node and load context allocations/freeing");
					Console.WriteLine("* No private NTDLL LDR function hooks");
				}

				ShowSnaps = true;
			}
		}

		private static void FacilityOnProcessBreakpoint(object sender, ProcessBreakpointEventArgs e)
		{
			DumpStack(e, out var stack);
			ShowSnaps = false;

			if (stack.Any(x => x.Symbol.Contains("LdrpDoDebuggerBreak", StringComparison.Ordinal)))
			{
				var allNodes = new HashSet<LdrGraphNode>();
				var condenseRoots = new HashSet<LdrGraphNode>();

				foreach (var entry in e.Context.PebLdr.InLoadOrderModuleList)
				{
					var module = entry.ContainingRecord<LdrModule>(LinkOffset);
					var name = module.BaseDllName;

					allNodes.Add(module.DdagNode);

					ListDependencies(module.DdagNode.Dependencies?.CheckParentGraphNode(module.DdagNode)
						.Select(x => x.DependencyNode));
					ListDependencies(module.DdagNode.IncomingDependencies?.CheckChildGraphNode(module.DdagNode)
						.Select(x => x.ParentNode));

					void ListDependencies(IEnumerable<LdrGraphNode> nodes)
					{
						if (nodes == null)
							return;

						foreach (var node in nodes)
						{
							allNodes.Add(node);

							var modules = node.Modules.ToArray();

							if (modules.Length == 1)
								continue;

							condenseRoots.Add(node);
						}
					}
				}

				var knownNodes = e.Context.Cache[typeof(LdrGraphNode)].Values.Cast<LdrGraphNode>();
				var referencedDead = allNodes.Where(x => x.Dead || x.Modules.Any(m => m.Dead)).ToArray();
				var danglingNodes = knownNodes.Except(allNodes).Where(x => !x.Dead).ToArray();

				if (referencedDead.Length > 0)
					Console.WriteLine("Referenced dead: " + string.Join(", ", referencedDead.Select(x => x.RootModuleName)));

				if (danglingNodes.Length > 0)
					Console.WriteLine("Dangling: " + string.Join(", ", danglingNodes.Select(x => x.RootModuleName)));

				if (condenseRoots.Count > 0)
				{
					var condenseRootsArray = condenseRoots.ToArray();
					for (int i = 0; i < condenseRootsArray.Length; i++)
					{
						var condenseRoot = condenseRootsArray[i];
						var modules = condenseRoot.Modules.Select(x => x.ContainingRecord<LdrModule>(NodeModuleLinkOffset).BaseDllName);
						Console.WriteLine($"CondenseRoot #{i}: {string.Join(", ", modules)}");
					}
				}
			}

			Console.ReadKey(true);
		}

		private static void DebugFacilityOnDebugException(object sender, DebugExceptionEventArgs e)
		{
			Console.WriteLine(e.Exception);
			Environment.Exit(1);
		}

		private static void FacilityOnProcessCreated(object sender, ProcessCreatedEventArgs e)
		{
			NativeFacility.PhCreateThreadProvider(e.ProcessId);

			NativeFacility.SetSearchPathSymbolProvider(@"SRV*C:\Symbols*https://msdl.microsoft.com/download/symbols");
		}

		private static readonly string[] SubstringsOfInterest =
		{
			"LdrpMergeNodes",
			"[inj]",
			"Detour",
		};

		private static readonly string[] SubstringsOfNoInterest =
		{
			" was redirected to ",
			"LdrpGetProcedureAddress",
		};

		private static readonly int LinkOffset = LdrModule.GetFieldOffset(nameof(LdrModule.InLoadOrderLinks));
		private static readonly int NodeModuleLinkOffset = LdrModule.GetFieldOffset(nameof(LdrModule.NodeModuleLink));
		private static readonly string EqEqEq = new string('=', 50);

		public static IEnumerable<LdrGraphEdge> CheckParentGraphNode(this IEnumerable<LdrGraphEdge> source,
			LdrGraphNode node)
		{
			foreach (var edge in source)
			{
				if (edge.ParentNode != node)
					throw new ApplicationException();

				yield return edge;
			}
		}

		public static IEnumerable<LdrGraphEdge> CheckChildGraphNode(this IEnumerable<LdrGraphEdge> source,
			LdrGraphNode node)
		{
			foreach (var edge in source)
			{
				if (edge.DependencyNode != node)
					throw new ApplicationException();

				yield return edge;
			}
		}

		private static void FacilityOnProcessDebugOutput(object sender, ProcessDebugOutputEventArgs e)
		{
			var context = e.Context;
			var line = e.Value;

			var matched = false;

			foreach (var (regex, action) in InjectOutputLineMatchers)
			{
				var match = regex.Match(line);
				if (!match.Success)
					continue;

				matched = true;
				action(context, match);
			}

			if (!matched)
			{
				var currentSnapsVerbosity = ShowSnaps && !SubstringsOfNoInterest.Any(x => line.Contains(x));
				if (currentSnapsVerbosity || SubstringsOfInterest.Any(x => line.Contains(x)))
				{
					Console.WriteLine(line.Trim('\n', '\r'));
				}
			}

			AssertLegacyListConsistency(context);

			Facility.LoadSymbols(context);
		}

		private static void DumpGraph(ProcessContext context)
		{
			foreach (var entry in context.PebLdr.InLoadOrderModuleList)
			{
				var module = entry.ContainingRecord<LdrModule>(LinkOffset);
				var name = module.BaseDllName;

				Console.WriteLine($"{name} ({module.DdagNode.State}):");

				ListDependencies(module.DdagNode.Dependencies?.CheckParentGraphNode(module.DdagNode)
					.Select(x => x.DependencyNode));
				ListDependencies(module.DdagNode.IncomingDependencies?.CheckChildGraphNode(module.DdagNode)
					.Select(x => x.ParentNode));

				void ListDependencies(IEnumerable<LdrGraphNode> nodes)
				{
					if (nodes != null)
					{
						Console.WriteLine(string.Join(", ", nodes.Select(FormatNode)));
					}
					else
					{
						Console.WriteLine("<NULL>");
					}

					static string FormatNode(LdrGraphNode node)
					{
						var modules = node.Modules.ToArray();

						var rootModuleName = node.RootModuleName;

						if (modules.Length == 1)
							return rootModuleName;

						return $"{rootModuleName}[{modules.Length}]";
					}
				}
			}
		}

		private static void DumpStack(ProcessEventArgs e, out IList<NativeFacility.ThreadStackItem> stack)
		{
			var context = e.Context;

			var thread = context.GetThreadById(e.ThreadId);

			stack = NativeFacility.ThreadStack(e.ProcessId, e.ThreadId, thread.Handle.DangerousGetHandle());

			if (stack != null)
			{
				foreach (var threadStackItem in stack)
				{
					Console.WriteLine(
						$"{threadStackItem.Index}: {threadStackItem.Symbol} (0x{threadStackItem.StackFrame.PcAddress.ToInt64():X})");
				}
			}
			else
			{
				Console.WriteLine("Failed to acquire stack");
			}
		}

		private static void AssertLegacyListConsistency(ProcessContext context)
		{
			var containingRecord = context.PebLdr
				.InLoadOrderModuleList
				.ContainingRecord<PebLdr>(PebLdr.GetFieldOffset(nameof(PebLdr.InLoadOrderModuleList)));
			Debug.Assert(ReferenceEquals(containingRecord, context.PebLdr));

			containingRecord = context.PebLdr
				.InMemoryOrderModuleList
				.ContainingRecord<PebLdr>(PebLdr.GetFieldOffset(nameof(PebLdr.InMemoryOrderModuleList)));
			Debug.Assert(ReferenceEquals(containingRecord, context.PebLdr));

			containingRecord = context.PebLdr
				.InInitializationOrderModuleList
				.ContainingRecord<PebLdr>(PebLdr.GetFieldOffset(nameof(PebLdr.InInitializationOrderModuleList)));
			Debug.Assert(ReferenceEquals(containingRecord, context.PebLdr));
		}
	}
}