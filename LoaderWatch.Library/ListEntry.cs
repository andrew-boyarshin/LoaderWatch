using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace LoaderWatch.Library
{
	public sealed class ListEntry : RemoteStruct<ListEntry.LIST_ENTRY>, IEnumerable<ListEntry>, IListEntry
	{
		public ListEntry Next => Context.As<ListEntry>(Native.Flink, this);
		public ListEntry Previous => Context.As<ListEntry>(Native.Blink, this);

		IEnumerator IEnumerable.GetEnumerator()
		{
			return GetEnumerator();
		}

		public IEnumerator<ListEntry> GetEnumerator()
		{
			return new ListEnumerator(this);
		}

		private class ListEnumerator : IEnumerator<ListEntry>
		{
			private readonly ListEntry _start;

			public ListEnumerator(ListEntry start)
			{
				_start = start ?? throw new ArgumentNullException(nameof(start));
			}

			public bool MoveNext()
			{
				if (Current == null)
					Current = _start;

				Current = Current.Next;
				return Current != _start;
			}

			public void Reset()
			{
				Current = null;
			}

			public void Dispose()
			{
			}

			object IEnumerator.Current => Current;

			public ListEntry Current { get; private set; }
		}

		[StructLayout(LayoutKind.Sequential, Pack = 0)]
		public struct LIST_ENTRY : IEquatable<LIST_ENTRY>
		{
			public IntPtr Flink;
			public IntPtr Blink;

			public bool Equals(LIST_ENTRY other)
			{
				return Flink.Equals(other.Flink) && Blink.Equals(other.Blink);
			}

			public override bool Equals(object obj)
			{
				return obj is LIST_ENTRY other && Equals(other);
			}

			public override int GetHashCode()
			{
				return HashCode.Combine(Flink, Blink);
			}

			public static bool operator ==(LIST_ENTRY left, LIST_ENTRY right)
			{
				return left.Equals(right);
			}

			public static bool operator !=(LIST_ENTRY left, LIST_ENTRY right)
			{
				return !left.Equals(right);
			}
		}
	}
}