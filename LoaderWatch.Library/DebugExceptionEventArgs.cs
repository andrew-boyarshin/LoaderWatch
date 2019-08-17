using System;

namespace LoaderWatch.Library
{
	public sealed class DebugExceptionEventArgs : ProcessEventArgs
	{
		public Exception Exception { get; set; }

		public DebugExceptionEventArgs(ProcessEventArgs args) : base(args)
		{
		}

		public DebugExceptionEventArgs(ProcessContext context, IntPtr processHandle, uint processId, uint threadId) : base(context, processHandle, processId, threadId)
		{
		}
	}
}