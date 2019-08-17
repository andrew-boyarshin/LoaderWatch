using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace LoaderWatch.Library
{
	public interface IRemoteStruct : IEquatable<IRemoteStruct>
	{
		ProcessContext Context { get; }
		IntPtr Address { get; }
		Type ContainerType { get; }
		bool Dead { get; }

		void Assign(ProcessContext context, IntPtr address);

		bool Refresh();
		void AssertRefreshed();
	}

	public abstract class RemoteStruct<T> : IRemoteStruct where T : struct
	{
		public ProcessContext Context { get; private set; }
		public IntPtr Address { get; private set; }
		public Type ContainerType => typeof(T);

		private T _native;
		public bool Dead
		{
			get => dead;

			set
			{
				var death = !dead && value;
				var resurrection = dead && !value;

				if (death)
					OnDeath();

				dead = value;

				if (resurrection)
					Refresh();
			}
		}

		public ref T Native
		{
			get
			{
				AssertRefreshed();
				return ref _native;
			}
		}

		protected bool Assigned;
		protected bool RefreshedAtLeastOnce;
		private bool dead;

		public void Assign(ProcessContext context, IntPtr address)
		{
			Context = context ?? throw new ArgumentNullException(nameof(context));
			Address = address;
			Assigned = true;
		}

		protected virtual void OnDeath()
		{
		}

		public void AssertRefreshed()
		{
			Refresh();
		}

		private void AssertNotDead()
		{
			AssertAssigned();

			if (!Dead)
				return;

			if (Debugger.IsAttached)
				Debugger.Break();

			throw new InvalidOperationException($"{GetType().Name} at {Address:X} is dead.");
		}

		protected TK CheckRefreshed<TK>() where TK : class
		{
			AssertRefreshed();
			return null;
		}

		public bool Refresh()
		{
			AssertNotDead();

			Debugger.NotifyOfCrossThreadDependency();

			RefreshedAtLeastOnce = true;
			var entry = Context.Memory[Address, false].Read<T>();
			var different = !Equals(_native, entry);
			_native = entry;

			return different;
		}

		private void AssertAssigned()
		{
			if (Assigned && Address != IntPtr.Zero)
				return;

			if (Debugger.IsAttached)
				Debugger.Break();

			throw new NullReferenceException($"{GetType().Name} not assigned.");
		}

		public static int GetFieldOffset(string name) => Marshal.OffsetOf<T>(name).ToInt32();

		public bool Equals(IRemoteStruct other)
		{
			if (other is null) return false;
			return ReferenceEquals(this, other) || Address.Equals(other.Address);
		}

		public override bool Equals(object obj)
		{
			if (obj is null) return false;
			if (ReferenceEquals(this, obj)) return true;
			return obj.GetType() == GetType() && Equals((IRemoteStruct)obj);
		}

		public override int GetHashCode()
		{
			return Address.GetHashCode();
		}

		public static bool operator ==(RemoteStruct<T> left, RemoteStruct<T> right)
		{
			return Equals(left, right);
		}

		public static bool operator !=(RemoteStruct<T> left, RemoteStruct<T> right)
		{
			return !Equals(left, right);
		}
	}
}