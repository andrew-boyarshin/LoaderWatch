using System;

namespace LoaderWatch.Library
{
	public sealed class ProcessBreakpointEventArgs : ProcessEventArgs
	{
		public ProcessBreakpointEventArgs(ProcessEventArgs args) : base(args)
		{
		}

		public ProcessBreakpointEventArgs(ProcessContext context, IntPtr processHandle, uint processId, uint threadId) : base(context, processHandle, processId, threadId)
		{
		}
	}
}