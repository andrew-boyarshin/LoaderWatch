using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace LoaderWatch.Library
{
	public sealed class SingleListEntry : RemoteStruct<SingleListEntry.SINGLE_LIST_ENTRY>, IEnumerable<SingleListEntry>, IListEntry
	{
		public SingleListEntry Next => Context.As<SingleListEntry>(Native.Next, this);

		IEnumerator IEnumerable.GetEnumerator()
		{
			return GetEnumerator();
		}

		public IEnumerator<SingleListEntry> GetEnumerator()
		{
			return new ListEnumerator(this);
		}

		private class ListEnumerator : IEnumerator<SingleListEntry>
		{
			private readonly SingleListEntry _start;

			public ListEnumerator(SingleListEntry start)
			{
				_start = start ?? throw new ArgumentNullException(nameof(start));
			}

			public bool MoveNext()
			{
				if (Current == null)
					Current = _start;

				Current = Current.Next;
				return Current != null;
			}

			public void Reset()
			{
				Current = null;
			}

			public void Dispose()
			{
			}

			object IEnumerator.Current => Current;

			public SingleListEntry Current { get; private set; }
		}

		[StructLayout(LayoutKind.Sequential, Pack = 0)]
		public struct SINGLE_LIST_ENTRY : IEquatable<SINGLE_LIST_ENTRY>
		{
			public IntPtr Next;

			public bool Equals(SINGLE_LIST_ENTRY other)
			{
				return Next.Equals(other.Next);
			}

			public override bool Equals(object obj)
			{
				return obj is SINGLE_LIST_ENTRY other && Equals(other);
			}

			public override int GetHashCode()
			{
				return Next.GetHashCode();
			}

			public static bool operator ==(SINGLE_LIST_ENTRY left, SINGLE_LIST_ENTRY right)
			{
				return left.Equals(right);
			}

			public static bool operator !=(SINGLE_LIST_ENTRY left, SINGLE_LIST_ENTRY right)
			{
				return !left.Equals(right);
			}
		}
	}
}
