using System;

namespace LoaderWatch.Library
{
	public sealed class ProcessAnyEventArgs : ProcessEventArgs
	{
		public ProcessAnyEventArgs(ProcessEventArgs args) : base(args)
		{
		}

		public ProcessAnyEventArgs(ProcessContext context, IntPtr processHandle, uint processId, uint threadId) : base(context, processHandle, processId, threadId)
		{
		}
	}
}