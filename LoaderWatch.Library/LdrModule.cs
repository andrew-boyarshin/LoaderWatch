using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace LoaderWatch.Library
{
	public sealed class LdrModule : RemoteStruct<LdrModule.LDR_DATA_TABLE_ENTRY>
	{
		public ListEntry InLoadOrderLinks => Context.As<ListEntry>(Address + GetFieldOffset(nameof(InLoadOrderLinks)), this);
		public ListEntry InMemoryOrderLinks => Context.As<ListEntry>(Address + GetFieldOffset(nameof(InMemoryOrderLinks)), this);
		public ListEntry InInitializationOrderLinks => Context.As<ListEntry>(Address + GetFieldOffset(nameof(InInitializationOrderLinks)), this);
		public ListEntry HashLinks => Context.As<ListEntry>(Address + GetFieldOffset(nameof(HashLinks)), this);
		public string BaseDllName
		{
			get
			{
				if (Dead)
				{
					if (BaseDllNameHistory.Count == 0)
						return "<UNKNOWN DEAD>";

					return $"{BaseDllNameHistory[^1]} (DEAD)";
				}

				var name = Context.AsString(Native.BaseDllName, this);

				if (BaseDllNameHistory.Count == 0 || !string.Equals(BaseDllNameHistory[^1], name, StringComparison.Ordinal))
				{
					BaseDllNameHistory.Add(name);
				}

				return name;
			}
		}

		public string FullDllName => Context.AsString(Native.FullDllName, this);
		public LdrGraphNode DdagNode => Context.As<LdrGraphNode>(Native.DdagNode, this);
		public ListEntry NodeModuleLink => Context.As<ListEntry>(Address + GetFieldOffset(nameof(NodeModuleLink)), this);
		public LdrLoadContext LoadContext => Context.As<LdrLoadContext>(Native.LoadContext, this);

		public readonly IList<string> BaseDllNameHistory = new List<string>();

		[StructLayout(LayoutKind.Explicit)]
		public struct LDR_DATA_TABLE_ENTRY : IEquatable<LDR_DATA_TABLE_ENTRY>
		{
			[FieldOffset(0)] public ListEntry.LIST_ENTRY InLoadOrderLinks;
			[FieldOffset(0x10)] public ListEntry.LIST_ENTRY InMemoryOrderLinks;
			[FieldOffset(0x20)] public ListEntry.LIST_ENTRY InInitializationOrderLinks;
			[FieldOffset(0x30)] public IntPtr DllBase;
			[FieldOffset(0x38)] public IntPtr EntryPoint;
			[FieldOffset(0x40)] public uint SizeOfImage;
			[FieldOffset(0x48)] public UNICODE_STRING FullDllName;
			[FieldOffset(0x58)] public UNICODE_STRING BaseDllName;
			[FieldOffset(0x68)] public LDR_DATA_TABLE_ENTRY_FLAGS Flags;
			[FieldOffset(0x6c)] public ushort ObsoleteLoadCount;
			[FieldOffset(0x6e)] public ushort TlsIndex;
			[FieldOffset(0x70)] public ListEntry.LIST_ENTRY HashLinks;
			[FieldOffset(0x80)] public uint TimeDateStamp;
			[FieldOffset(0x88)] public IntPtr EntryPointActivationContext;
			[FieldOffset(0x90)] public IntPtr Lock;
			[FieldOffset(0x98)] public IntPtr DdagNode;
			[FieldOffset(0xa0)] public ListEntry.LIST_ENTRY NodeModuleLink; // LDR_DDAG_NODE.Modules
			[FieldOffset(0xb0)] public IntPtr LoadContext;
			[FieldOffset(0xb8)] public IntPtr ParentDllBase;
			[FieldOffset(0xc0)] public IntPtr SwitchBackContext;

			[FieldOffset(0xf8)] public IntPtr OriginalBase;

			//LARGE_INTEGER LoadTime;
			[FieldOffset(0x108)] public uint BaseNameHashValue;

			[FieldOffset(0x10c)] public LDR_DLL_LOAD_REASON LoadReason;

			//LDRP_PATH_SEARCH_OPTIONS ImplicitPathOptions;
			[FieldOffset(0x114)] public uint ReferenceCount;
			[FieldOffset(0x118)] public uint DependentLoadFlags;

			public bool Equals(LDR_DATA_TABLE_ENTRY other)
			{
				return InLoadOrderLinks.Equals(other.InLoadOrderLinks)
				       && InMemoryOrderLinks.Equals(other.InMemoryOrderLinks)
				       && InInitializationOrderLinks.Equals(other.InInitializationOrderLinks)
				       && DllBase.Equals(other.DllBase)
				       && EntryPoint.Equals(other.EntryPoint)
				       && SizeOfImage == other.SizeOfImage
				       && FullDllName.Equals(other.FullDllName)
				       && BaseDllName.Equals(other.BaseDllName)
				       && Flags == other.Flags
				       && ObsoleteLoadCount == other.ObsoleteLoadCount
				       && TlsIndex == other.TlsIndex
				       && HashLinks.Equals(other.HashLinks)
				       && TimeDateStamp == other.TimeDateStamp
				       && EntryPointActivationContext.Equals(other.EntryPointActivationContext)
				       && Lock.Equals(other.Lock)
				       && DdagNode.Equals(other.DdagNode)
				       && NodeModuleLink.Equals(other.NodeModuleLink)
				       && LoadContext.Equals(other.LoadContext)
				       && ParentDllBase.Equals(other.ParentDllBase)
				       && SwitchBackContext.Equals(other.SwitchBackContext)
				       && OriginalBase.Equals(other.OriginalBase)
				       && BaseNameHashValue == other.BaseNameHashValue
				       && LoadReason == other.LoadReason
				       && ReferenceCount == other.ReferenceCount
				       && DependentLoadFlags == other.DependentLoadFlags;
			}

			public override bool Equals(object obj)
			{
				return obj is LDR_DATA_TABLE_ENTRY other && Equals(other);
			}

			public override int GetHashCode()
			{
				var hashCode = new HashCode();
				hashCode.Add(InLoadOrderLinks);
				hashCode.Add(InMemoryOrderLinks);
				hashCode.Add(InInitializationOrderLinks);
				hashCode.Add(DllBase);
				hashCode.Add(EntryPoint);
				hashCode.Add(SizeOfImage);
				hashCode.Add(FullDllName);
				hashCode.Add(BaseDllName);
				hashCode.Add(Flags);
				hashCode.Add(ObsoleteLoadCount);
				hashCode.Add(TlsIndex);
				hashCode.Add(HashLinks);
				hashCode.Add(TimeDateStamp);
				hashCode.Add(EntryPointActivationContext);
				hashCode.Add(Lock);
				hashCode.Add(DdagNode);
				hashCode.Add(NodeModuleLink);
				hashCode.Add(LoadContext);
				hashCode.Add(ParentDllBase);
				hashCode.Add(SwitchBackContext);
				hashCode.Add(OriginalBase);
				hashCode.Add(BaseNameHashValue);
				hashCode.Add(LoadReason);
				hashCode.Add(ReferenceCount);
				hashCode.Add(DependentLoadFlags);
				return hashCode.ToHashCode();
			}

			public static bool operator ==(LDR_DATA_TABLE_ENTRY left, LDR_DATA_TABLE_ENTRY right)
			{
				return left.Equals(right);
			}

			public static bool operator !=(LDR_DATA_TABLE_ENTRY left, LDR_DATA_TABLE_ENTRY right)
			{
				return !left.Equals(right);
			}
		}
	}
}