using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using LoaderWatch.Library.MemorySharp.Helpers;
using LoaderWatch.Library.MemorySharp.Threading;

namespace LoaderWatch.Library
{
	public class ProcessContext
	{
		public MemorySharp.MemorySharp Memory { get; }

		public LdrModule LdrpImageEntry
		{
			get
			{
				if (_ldrpImageEntry == null)
				{
					var entryPtr = Memory[Memory["ntdll"].BaseAddress.Add(Facility.Offset("LdrpImageEntry")), false]
						.Read<IntPtr>();
					_ldrpImageEntry = As<LdrModule>(entryPtr);
				}

				return _ldrpImageEntry;
			}
		}

		public uint LdrInitState =>
			Memory[Memory["ntdll"].BaseAddress.Add(Facility.Offset("LdrInitState")), false]
				.Read<uint>();

		public PebLdr PebLdr => _pebLdr ??= As<PebLdr>(Memory.Peb.Ldr);

		public Process ManagedProcess => Memory.Native;

		public readonly Dictionary<Type, Dictionary<IntPtr, IRemoteStruct>> Cache = new Dictionary<Type, Dictionary<IntPtr, IRemoteStruct>>();
		private LdrModule _ldrpImageEntry;
		private PebLdr _pebLdr;

		public readonly Dictionary<IntPtr, RemoteThread> Threads = new Dictionary<IntPtr, RemoteThread>();

		public RemoteThread GetThreadById(IntPtr id)
		{
			if (Threads.TryGetValue(id, out var value))
				return value;

			value = Memory.Threads.GetThreadById(checked((int) id));
			Threads.Add(id, value);

			return value;
		}

		public ProcessContext(Process process)
		{
			Memory = new MemorySharp.MemorySharp(process);
		}

		public T As<T>(IntPtr address, IRemoteStruct assertRefreshed = null)
			where T: class, IRemoteStruct, new()
		{
			assertRefreshed?.AssertRefreshed();

			if (address == IntPtr.Zero)
				return null;

			if (!Cache.TryGetValue(typeof(T), out var cache))
			{
				cache = new Dictionary<IntPtr, IRemoteStruct>();
				Cache.Add(typeof(T), cache);
			}

			if (cache.TryGetValue(address, out var value))
				return (T) value;

			value = new T();
			value.Assign(this, address);
			cache.Add(address, value);

			return (T) value;
		}

		public string AsString(UNICODE_STRING native, IRemoteStruct assertRefreshed = null)
		{
			assertRefreshed?.AssertRefreshed();

			if (native.buffer == IntPtr.Zero)
				return null;

			if (native.Length == 0)
				return string.Empty;

			return Memory[native.buffer, false].ReadString(Encoding.Unicode, native.MaximumLength);
		}
	}
}