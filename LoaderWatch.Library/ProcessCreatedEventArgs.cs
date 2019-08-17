using System;

namespace LoaderWatch.Library
{
	public sealed class ProcessCreatedEventArgs : ProcessEventArgs
	{
		public ProcessCreatedEventArgs(ProcessEventArgs args) : base(args)
		{
		}

		public ProcessCreatedEventArgs(ProcessContext context, IntPtr processHandle, uint processId, uint threadId) : base(context, processHandle, processId, threadId)
		{
		}
	}
}