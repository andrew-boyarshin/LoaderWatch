using System;

namespace LoaderWatch.Library
{
	public abstract class ProcessEventArgs : EventArgs
	{
		public ProcessContext Context { get; set; }
		public IntPtr ProcessHandle;
		public IntPtr ProcessId;
		public IntPtr ThreadId;

		protected ProcessEventArgs(ProcessContext context, IntPtr processHandle, IntPtr processId, IntPtr threadId)
		{
			Context = context ?? throw new ArgumentNullException(nameof(context));
			ProcessHandle = processHandle;
			ProcessId = processId;
			ThreadId = threadId;
		}

		protected ProcessEventArgs(ProcessContext context, IntPtr processHandle, uint processId, uint threadId)
			: this(context, processHandle, (IntPtr) processId, (IntPtr) threadId)
		{
		}

		protected ProcessEventArgs(ProcessEventArgs args)
			: this(args.Context, args.ProcessHandle, args.ProcessId, args.ThreadId)
		{
		}
	}
}