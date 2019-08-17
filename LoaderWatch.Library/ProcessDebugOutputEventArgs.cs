using System;

namespace LoaderWatch.Library
{
	public sealed class ProcessDebugOutputEventArgs : ProcessEventArgs
	{
		public ProcessDebugOutputEventArgs(ProcessEventArgs args) : base(args)
		{
		}

		public string Value { get; set; }

		public ProcessDebugOutputEventArgs(ProcessContext context, IntPtr processHandle, uint processId, uint threadId) : base(context, processHandle, processId, threadId)
		{
		}
	}
}