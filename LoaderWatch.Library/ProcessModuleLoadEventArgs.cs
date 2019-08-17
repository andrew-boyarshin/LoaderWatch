using System;

namespace LoaderWatch.Library
{
	public sealed class ProcessModuleLoadEventArgs : ProcessEventArgs
	{
		public ProcessModuleLoadEventArgs(ProcessEventArgs args) : base(args)
		{
		}

		public string Name { get; set; }

		public ProcessModuleLoadEventArgs(ProcessContext context, IntPtr processHandle, uint processId, uint threadId) : base(context, processHandle, processId, threadId)
		{
		}
	}
}