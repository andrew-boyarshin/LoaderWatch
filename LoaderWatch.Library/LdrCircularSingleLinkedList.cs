using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;

namespace LoaderWatch.Library
{
	public sealed class LdrCircularSingleLinkedList : RemoteStruct<LdrCircularSingleLinkedList.LDRP_CSLIST>, IEnumerable<SingleListEntry>
	{
		public SingleListEntry Tail => Context.As<SingleListEntry>(Native.Tail, this);

		IEnumerator IEnumerable.GetEnumerator()
		{
			return GetEnumerator();
		}

		public IEnumerator<SingleListEntry> GetEnumerator()
		{
			return new ListEnumerator(Tail);
		}

		private class ListEnumerator : IEnumerator<SingleListEntry>
		{
			private readonly SingleListEntry _start;

			public ListEnumerator(SingleListEntry start)
			{
				_start = start;
			}

			public bool MoveNext()
			{
				if (_start == null)
					return false;

				if (Current == null)
					Current = _start;
				else if (Current == _start)
					return false;

				Current = Current.Next;

				return true;
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

		public struct LDRP_CSLIST : IEquatable<LDRP_CSLIST>
		{
			public IntPtr Tail;

			public override bool Equals(object obj)
			{
				return obj is LDRP_CSLIST List && Equals(List);
			}

			public bool Equals([AllowNull] LDRP_CSLIST other)
			{
				return Tail.Equals(other.Tail);
			}

			public override int GetHashCode()
			{
				return HashCode.Combine(Tail);
			}

			public static bool operator ==(LDRP_CSLIST left, LDRP_CSLIST right)
			{
				return left.Equals(right);
			}

			public static bool operator !=(LDRP_CSLIST left, LDRP_CSLIST right)
			{
				return !(left == right);
			}
		}
	}
}
