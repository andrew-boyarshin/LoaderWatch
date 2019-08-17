using System;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;

namespace LoaderWatch.Library
{
	[StructLayout(LayoutKind.Sequential)]
	public struct UNICODE_STRING : IEquatable<UNICODE_STRING>
	{
		public ushort Length;
		public ushort MaximumLength;
		public IntPtr buffer;

		public override bool Equals(object obj)
		{
			return obj is UNICODE_STRING String && Equals(String);
		}

		public bool Equals([AllowNull] UNICODE_STRING other)
		{
			return Length == other.Length &&
				   MaximumLength == other.MaximumLength &&
				   buffer.Equals(other.buffer);
		}

		public override int GetHashCode()
		{
			return HashCode.Combine(Length, MaximumLength, buffer);
		}

		public static bool operator ==(UNICODE_STRING left, UNICODE_STRING right)
		{
			return left.Equals(right);
		}

		public static bool operator !=(UNICODE_STRING left, UNICODE_STRING right)
		{
			return !(left == right);
		}
	}

	public enum LDR_DDAG_STATE
	{
		LdrModulesMerged = -5,
		LdrModulesInitError = -4,
		LdrModulesSnapError = -3,
		LdrModulesUnloaded = -2,
		LdrModulesUnloading = -1,
		LdrModulesPlaceHolder = 0,
		LdrModulesMapping = 1,
		LdrModulesMapped = 2,
		LdrModulesWaitingForDependencies = 3,
		LdrModulesSnapping = 4,
		LdrModulesSnapped = 5,
		LdrModulesCondensed = 6,
		LdrModulesReadyToInit = 7,
		LdrModulesInitializing = 8,
		LdrModulesReadyToRun = 9
	}

	public enum LDR_DLL_LOAD_REASON
	{
		LoadReasonStaticDependency = 0,
		LoadReasonStaticForwarderDependency = 1,
		LoadReasonDynamicForwarderDependency = 2,
		LoadReasonDelayloadDependency = 3,
		LoadReasonDynamicLoad = 4,
		LoadReasonAsImageLoad = 5,
		LoadReasonAsDataLoad = 6,
		LoadReasonEnclavePrimary = 7,
		LoadReasonEnclaveDependency = 8,
		LoadReasonUnknown = -1
	}

	[Flags]
	public enum LDR_DATA_TABLE_ENTRY_FLAGS : int
	{
		PackagedBinary = 1, // 0
		MarkedForRemoval = 1 << 1, // 1
		ImageDll = 1 << 2, // 2
		LoadNotificationsSent = 1 << 3, // 3
		TelemetryEntryProcessed = 1 << 4, // 4
		ProcessStaticImport = 1 << 5, // 5
		InLegacyLists = 1 << 6, // 6
		InIndexes = 1 << 7, // 7
		ShimDll = 1 << 8, // 8
		InExceptionTable = 1 << 9, // 9
		ReservedFlags1 = 1 << 10, // 10
		ReactOSSystemMapped = 1 << 11, // 11
		LoadInProgress = 1 << 12, // 12
		LoadConfigProcessed = 1 << 13, // 13
		EntryProcessed = 1 << 14, // 14
		ProtectDelayLoad = 1 << 15, // 15
		ReactOSLdrSymbolsLoaded = 1 << 16, // 16
		ReactOSDriverDependency = 1 << 17, // 17
		DontCallForThreads = 1 << 18, // 18
		ProcessAttachCalled = 1 << 19, // 19
		ProcessAttachFailed = 1 << 20, // 20
		CorDeferredValidate = 1 << 21, // 21
		CorImage = 1 << 22, // 22
		DontRelocate = 1 << 23, // 23
		CorILOnly = 1 << 24, // 24
		ChpeImage = 1 << 25, // 25; CHPE = Compiled Hybrid Portable Executable
		ReactOSDriverVerifying = 1 << 26, // 26
		ReactOSNativeMapped = 1 << 27, // 27
		Redirected = 1 << 28, // 28
		ReactOSShimSuppress = 1 << 29, // 29
		ReactOSKernelLoaded = 1 << 30, // 30
		CompatDatabaseProcessed = 1 << 31, // 31
	}
}
