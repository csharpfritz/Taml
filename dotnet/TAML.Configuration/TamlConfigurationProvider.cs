using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Primitives;
using TAML.Core;

namespace TAML.Configuration;

internal class TamlConfigurationProvider(string path) : IConfigurationProvider
{
	private Dictionary<string, string?> _data = new Dictionary<string, string?>(StringComparer.OrdinalIgnoreCase);

	public IEnumerable<string> GetChildKeys(IEnumerable<string> earlierKeys, string? parentPath)
	{
		var prefix = parentPath == null ? string.Empty : parentPath + ":";
		
		return _data
			.Where(kv => kv.Key.StartsWith(prefix, StringComparison.OrdinalIgnoreCase))
			.Select(kv => 
			{
				var key = kv.Key.Substring(prefix.Length);
				var separatorIndex = key.IndexOf(':');
				return separatorIndex >= 0 ? key.Substring(0, separatorIndex) : key;
			})
			.Concat(earlierKeys)
			.Distinct(StringComparer.OrdinalIgnoreCase);
	}

	public IChangeToken GetReloadToken()
	{
		// For now, return a token that never changes
		// In a production implementation, you could use FileSystemWatcher to detect changes
		return new ConfigurationReloadToken();
	}

	public void Load()
	{
		try
		{
			// Load the TAML file and flatten it into configuration format
			var document = TamlDocument.LoadFromFile(path);
			_data = document.Flatten();
		}
		catch (FileNotFoundException)
		{
			// If file doesn't exist, use empty data
			_data = new Dictionary<string, string?>(StringComparer.OrdinalIgnoreCase);
		}
	}

	public void Set(string key, string? value)
	{
		_data[key] = value;
	}

	public bool TryGet(string key, out string? value)
	{
		return _data.TryGetValue(key, out value);
	}
}

/// <summary>
/// Simple implementation of IChangeToken that never changes
/// </summary>
internal class ConfigurationReloadToken : IChangeToken
{
	public bool HasChanged => false;
	public bool ActiveChangeCallbacks => false;
	public IDisposable RegisterChangeCallback(Action<object?> callback, object? state) => EmptyDisposable.Instance;
	
	private class EmptyDisposable : IDisposable
	{
		public static EmptyDisposable Instance { get; } = new EmptyDisposable();
		public void Dispose() { }
	}
}