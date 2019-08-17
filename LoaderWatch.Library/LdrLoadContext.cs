using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;

namespace LoaderWatch.Library
{
	public sealed class LdrLoadContext : RemoteStruct<LdrLoadContext.LDRP_LOAD_CONTEXT>
	{
		public string DllName => Context.AsString(Native.DllName, this);

		public int? StatusResponse
		{
			get
			{
				var statusResponse = Native.StatusResponse;
				if (statusResponse == IntPtr.Zero)
					return null;

				return Context.Memory[statusResponse, false].Read<int>();
			}
		}

		public LdrModule ParentEntry => Context.As<LdrModule>(Native.ParentEntry, this);
		public LdrModule Entry => Context.As<LdrModule>(Native.Entry, this);
		public LdrModule PendingDependencyEntry => Context.As<LdrModule>(Native.PendingDependencyEntry, this);

		[StructLayout(LayoutKind.Sequential, Pack = 8)]
		public struct LDRP_LOAD_CONTEXT : IEquatable<LDRP_LOAD_CONTEXT>
		{
			public UNICODE_STRING DllName;
			public IntPtr PathSearchContext;
			public IntPtr DllSectionHandle;
			public uint Flags;
			public IntPtr StatusResponse;
			public IntPtr ParentEntry;
			public IntPtr Entry;
			public ListEntry.LIST_ENTRY WorkQueueListEntry;
			public IntPtr PendingDependencyEntry;
			public IntPtr ImportEntries;
			public uint ImportEntriesCount;
			public uint CountOfDependenciesPendingMap;
			public IntPtr BaseAddressOfIAT;
			public IntPtr SizeOfIAT;
			public uint SnappedImportEntriesCount; // 0 <= SnappedImportEntriesCount <= ImportEntriesCount
			public IntPtr ImportDescriptor;
			public uint OriginalIATProtection;
			public IntPtr GuardCFCheckFunctionPointer;
			public IntPtr GuardCFCheckFunctionPointerThunk;
			public IntPtr DllMappedSectionSize;
			public IntPtr EnclaveContext;
			public IntPtr DllMappedFileHandle;
			public IntPtr ProcessImageSectionViewBase;
			[MarshalAs(UnmanagedType.ByValArray, SizeConst = sizeof(char))]
			public char[] DllNameStorage;

			public override bool Equals(object obj)
			{
				return obj is LDRP_LOAD_CONTEXT Context && Equals(Context);
			}

			public bool Equals([AllowNull] LDRP_LOAD_CONTEXT other)
			{
				return EqualityComparer<UNICODE_STRING>.Default.Equals(DllName, other.DllName) &&
					   PathSearchContext.Equals(other.PathSearchContext) &&
					   DllSectionHandle.Equals(other.DllSectionHandle) &&
					   Flags == other.Flags &&
					   StatusResponse.Equals(other.StatusResponse) &&
					   ParentEntry.Equals(other.ParentEntry) &&
					   Entry.Equals(other.Entry) &&
					   WorkQueueListEntry.Equals(other.WorkQueueListEntry) &&
					   PendingDependencyEntry.Equals(other.PendingDependencyEntry) &&
					   ImportEntries.Equals(other.ImportEntries) &&
					   ImportEntriesCount == other.ImportEntriesCount &&
					   CountOfDependenciesPendingMap == other.CountOfDependenciesPendingMap &&
					   BaseAddressOfIAT.Equals(other.BaseAddressOfIAT) &&
					   SizeOfIAT.Equals(other.SizeOfIAT) &&
					   SnappedImportEntriesCount == other.SnappedImportEntriesCount &&
					   ImportDescriptor.Equals(other.ImportDescriptor) &&
					   OriginalIATProtection == other.OriginalIATProtection &&
					   GuardCFCheckFunctionPointer.Equals(other.GuardCFCheckFunctionPointer) &&
					   GuardCFCheckFunctionPointerThunk.Equals(other.GuardCFCheckFunctionPointerThunk) &&
					   DllMappedSectionSize.Equals(other.DllMappedSectionSize) &&
					   EnclaveContext.Equals(other.EnclaveContext) &&
					   DllMappedFileHandle.Equals(other.DllMappedFileHandle) &&
					   ProcessImageSectionViewBase.Equals(other.ProcessImageSectionViewBase);
			}

			public override int GetHashCode()
			{
				var hash = new HashCode();
				hash.Add(DllName);
				hash.Add(PathSearchContext);
				hash.Add(DllSectionHandle);
				hash.Add(Flags);
				hash.Add(StatusResponse);
				hash.Add(ParentEntry);
				hash.Add(Entry);
				hash.Add(WorkQueueListEntry);
				hash.Add(PendingDependencyEntry);
				hash.Add(ImportEntries);
				hash.Add(ImportEntriesCount);
				hash.Add(CountOfDependenciesPendingMap);
				hash.Add(BaseAddressOfIAT);
				hash.Add(SizeOfIAT);
				hash.Add(SnappedImportEntriesCount);
				hash.Add(ImportDescriptor);
				hash.Add(OriginalIATProtection);
				hash.Add(GuardCFCheckFunctionPointer);
				hash.Add(GuardCFCheckFunctionPointerThunk);
				hash.Add(DllMappedSectionSize);
				hash.Add(EnclaveContext);
				hash.Add(DllMappedFileHandle);
				hash.Add(ProcessImageSectionViewBase);
				return hash.ToHashCode();
			}

			public static bool operator ==(LDRP_LOAD_CONTEXT left, LDRP_LOAD_CONTEXT right)
			{
				return left.Equals(right);
			}

			public static bool operator !=(LDRP_LOAD_CONTEXT left, LDRP_LOAD_CONTEXT right)
			{
				return !(left == right);
			}
		}
	}
}
