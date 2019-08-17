using System;

namespace LoaderWatch.Library
{
	public sealed class LdrGraphEdge : RemoteStruct<LdrGraphEdge.LDRP_CSLIST_ENTRY>
	{
		public SingleListEntry DependenciesLink => Context.As<SingleListEntry>(Native.DependenciesLink.Next, this);
		public LdrGraphNode DependencyNode => Context.As<LdrGraphNode>(Native.DependencyNode, this);
		public SingleListEntry IncomingDependenciesLink => Context.As<SingleListEntry>(Native.IncomingDependenciesLink.Next, this);
		public LdrGraphNode ParentNode => Context.As<LdrGraphNode>(Native.ParentNode, this);

		public struct LDRP_CSLIST_ENTRY : IEquatable<LDRP_CSLIST_ENTRY>
		{
			public SingleListEntry.SINGLE_LIST_ENTRY DependenciesLink;
			public IntPtr DependencyNode;
			public SingleListEntry.SINGLE_LIST_ENTRY IncomingDependenciesLink;
			public IntPtr ParentNode;

			public bool Equals(LDRP_CSLIST_ENTRY other)
			{
				return DependenciesLink.Equals(other.DependenciesLink)
				       && DependencyNode.Equals(other.DependencyNode)
				       && IncomingDependenciesLink.Equals(other.IncomingDependenciesLink)
				       && ParentNode.Equals(other.ParentNode);
			}

			public override bool Equals(object obj)
			{
				return obj is LDRP_CSLIST_ENTRY other && Equals(other);
			}

			public override int GetHashCode()
			{
				return HashCode.Combine(DependenciesLink, DependencyNode, IncomingDependenciesLink, ParentNode);
			}

			public static bool operator ==(LDRP_CSLIST_ENTRY left, LDRP_CSLIST_ENTRY right)
			{
				return left.Equals(right);
			}

			public static bool operator !=(LDRP_CSLIST_ENTRY left, LDRP_CSLIST_ENTRY right)
			{
				return !left.Equals(right);
			}
		}
	}
}
