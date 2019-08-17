using System;
using System.Diagnostics;

namespace LoaderWatch.Library
{
	public sealed class PebLdr : RemoteStruct<PebLdr.PEB_LDR_DATA>
	{
		[DebuggerDisplay("{Native.InLoadOrderModuleList}")]
		public ListEntry InLoadOrderModuleList => Context.As<ListEntry>(Address + GetFieldOffset(nameof(InLoadOrderModuleList)));
		[DebuggerDisplay("{Native.InMemoryOrderModuleList}")]
		public ListEntry InMemoryOrderModuleList => Context.As<ListEntry>(Address + GetFieldOffset(nameof(InMemoryOrderModuleList)));
		[DebuggerDisplay("{Native.InInitializationOrderModuleList}")]
		public ListEntry InInitializationOrderModuleList => Context.As<ListEntry>(Address + GetFieldOffset(nameof(InInitializationOrderModuleList)));

		public struct PEB_LDR_DATA : IEquatable<PEB_LDR_DATA>
		{
			public uint Length;
			public bool Initialized;
			public IntPtr SsHandle;
			public ListEntry.LIST_ENTRY InLoadOrderModuleList;
			public ListEntry.LIST_ENTRY InMemoryOrderModuleList;
			public ListEntry.LIST_ENTRY InInitializationOrderModuleList;
			public IntPtr EntryInProgress;
			public bool ShutdownInProgress;
			public IntPtr ShutdownThreadId;

			public bool Equals(PEB_LDR_DATA other)
			{
				return Length == other.Length
				       && Initialized == other.Initialized 
				       && SsHandle.Equals(other.SsHandle)
				       && InLoadOrderModuleList.Equals(other.InLoadOrderModuleList)
				       && InMemoryOrderModuleList.Equals(other.InMemoryOrderModuleList) 
				       && InInitializationOrderModuleList.Equals(other.InInitializationOrderModuleList) 
				       && EntryInProgress.Equals(other.EntryInProgress) 
				       && ShutdownInProgress == other.ShutdownInProgress
				       && ShutdownThreadId.Equals(other.ShutdownThreadId);
			}

			public override bool Equals(object obj)
			{
				return obj is PEB_LDR_DATA other && Equals(other);
			}

			public override int GetHashCode()
			{
				var hashCode = new HashCode();
				hashCode.Add(Length);
				hashCode.Add(Initialized);
				hashCode.Add(SsHandle);
				hashCode.Add(InLoadOrderModuleList);
				hashCode.Add(InMemoryOrderModuleList);
				hashCode.Add(InInitializationOrderModuleList);
				hashCode.Add(EntryInProgress);
				hashCode.Add(ShutdownInProgress);
				hashCode.Add(ShutdownThreadId);
				return hashCode.ToHashCode();
			}

			public static bool operator ==(PEB_LDR_DATA left, PEB_LDR_DATA right)
			{
				return left.Equals(right);
			}

			public static bool operator !=(PEB_LDR_DATA left, PEB_LDR_DATA right)
			{
				return !left.Equals(right);
			}
		}
	}
}
