using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;

namespace LoaderWatch.Library
{
	public sealed class LdrGraphNode : RemoteStruct<LdrGraphNode.LDR_DDAG_NODE>
	{
		public ListEntry Modules => Context.As<ListEntry>(Address + GetFieldOffset(nameof(Modules)), this);
		public uint LoadCount => Native.LoadCount;
		public uint LoadWhileUnloadingCount => Native.LoadWhileUnloadingCount;
		public uint LowestLink => Native.LowestLink;

		public LdrCircularSingleLinkedList DependenciesList =>
			Context.As<LdrCircularSingleLinkedList>(Address + GetFieldOffset(nameof(Dependencies)), this);

		public LdrCircularSingleLinkedList IncomingDependenciesList =>
			Context.As<LdrCircularSingleLinkedList>(Address + GetFieldOffset(nameof(IncomingDependencies)), this);

		public LDR_DDAG_STATE State => Native.State;

		public SingleListEntry CondenseLinkList =>
			Context.As<SingleListEntry>(Address + GetFieldOffset(nameof(CondenseLink)), this);

		public uint PreorderNumber => Native.PreorderNumber;

		public IEnumerable<LdrGraphNode> CondenseLink
		{
			get
			{
				return CondenseLinkList?.Select(x => x.ContainingRecord<LdrGraphNode>(CondenseLinkOffset));
			}
		}

		public IEnumerable<LdrGraphEdge> Dependencies
		{
			get
			{
				var list = DependenciesList;
				return list.Tail == null
					? null
					: list.Select(x => x.ContainingRecord<LdrGraphEdge>(DependenciesLinkOffset));
			}
		}

		public IEnumerable<LdrGraphEdge> IncomingDependencies
		{
			get
			{
				var list = IncomingDependenciesList;
				return list.Tail == null
					? null
					: list.Select(x => x.ContainingRecord<LdrGraphEdge>(IncomingDependenciesLinkOffset));
			}
		}

		private static readonly int NodeModuleLinkOffset = LdrModule.GetFieldOffset(nameof(LdrModule.NodeModuleLink));

		private static readonly int DependenciesLinkOffset =
			LdrGraphEdge.GetFieldOffset(nameof(LdrGraphEdge.DependenciesLink));

		private static readonly int IncomingDependenciesLinkOffset =
			LdrGraphEdge.GetFieldOffset(nameof(LdrGraphEdge.IncomingDependenciesLink));

		public static readonly int CondenseLinkOffset = GetFieldOffset(nameof(CondenseLink));

		public LdrModule RootModule => Modules.Next.ContainingRecord<LdrModule>(NodeModuleLinkOffset);

		private string rootModuleName;

		public string RootModuleName
		{
			get
			{
				if (string.IsNullOrEmpty(rootModuleName))
					RefreshRootModuleName(-10);

				return rootModuleName;
			}

			private set => rootModuleName = value;
		}

		public void RefreshRootModuleName(int certainty, LdrModule module = null)
		{
			if (certainty <= 0 && !string.IsNullOrEmpty(rootModuleName))
				return;

			module ??= RootModule;
			try
			{
				RootModuleName = module.BaseDllName;
			}
			catch (Win32Exception)
			{

			}
		}

		protected override void OnDeath()
		{
			base.OnDeath();
			rootModuleName = null;
		}

		[StructLayout(LayoutKind.Sequential, Pack = 8)]
		public struct LDR_DDAG_NODE : IEquatable<LDR_DDAG_NODE>
		{
			public ListEntry.LIST_ENTRY Modules;
			public IntPtr ServiceTagList;
			public uint LoadCount;
			public uint LoadWhileUnloadingCount;
			public uint LowestLink;
			public LdrCircularSingleLinkedList.LDRP_CSLIST Dependencies;
			public LdrCircularSingleLinkedList.LDRP_CSLIST IncomingDependencies;
			public LDR_DDAG_STATE State;
			public SingleListEntry.SINGLE_LIST_ENTRY CondenseLink;
			public uint PreorderNumber;

			public bool Equals(LDR_DDAG_NODE other)
			{
				return Modules.Equals(other.Modules)
					   && ServiceTagList.Equals(other.ServiceTagList)
					   && LoadCount == other.LoadCount
					   && LoadWhileUnloadingCount == other.LoadWhileUnloadingCount
					   && LowestLink == other.LowestLink
					   && Dependencies.Equals(other.Dependencies)
					   && IncomingDependencies.Equals(other.IncomingDependencies)
					   && State == other.State
					   && CondenseLink.Equals(other.CondenseLink)
					   && PreorderNumber == other.PreorderNumber;
			}

			public override bool Equals(object obj)
			{
				return obj is LDR_DDAG_NODE other && Equals(other);
			}

			public override int GetHashCode()
			{
				var hashCode = new HashCode();
				hashCode.Add(Modules);
				hashCode.Add(ServiceTagList);
				hashCode.Add(LoadCount);
				hashCode.Add(LoadWhileUnloadingCount);
				hashCode.Add(LowestLink);
				hashCode.Add(Dependencies);
				hashCode.Add(IncomingDependencies);
				hashCode.Add(State);
				hashCode.Add(CondenseLink);
				hashCode.Add(PreorderNumber);
				return hashCode.ToHashCode();
			}

			public static bool operator ==(LDR_DDAG_NODE left, LDR_DDAG_NODE right)
			{
				return left.Equals(right);
			}

			public static bool operator !=(LDR_DDAG_NODE left, LDR_DDAG_NODE right)
			{
				return !left.Equals(right);
			}
		}
	}
}