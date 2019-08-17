namespace LoaderWatch.Library
{
	public static class Extensions
	{
		public static T ContainingRecord<T>(this IListEntry self, int offset) where T : class, IRemoteStruct, new()
		{
			self.AssertRefreshed();

			return self.Context.As<T>(self.Address - offset);
		}
	}
}
