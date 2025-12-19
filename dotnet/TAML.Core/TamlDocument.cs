namespace TAML.Core;

/// <summary>
/// Represents a generic TAML document that can hold any TAML structure.
/// The document is represented as a dictionary of key-value pairs where values can be:
/// - Primitive types (string, int, bool, etc.)
/// - Nested TamlDocument objects (for nested sections)
/// - Lists of values or TamlDocument objects
/// </summary>
public class TamlDocument
{
	/// <summary>
	/// The underlying data structure holding the document content
	/// </summary>
	public Dictionary<string, object?> Data { get; private set; }

	/// <summary>
	/// Creates an empty TAML document
	/// </summary>
	public TamlDocument()
	{
		Data = new Dictionary<string, object?>();
	}

	/// <summary>
	/// Creates a TAML document from existing data
	/// </summary>
	public TamlDocument(Dictionary<string, object?> data)
	{
		Data = data ?? new Dictionary<string, object?>();
	}

	/// <summary>
	/// Indexer to access document values by key
	/// </summary>
	public object? this[string key]
	{
		get => Data.TryGetValue(key, out var value) ? value : null;
		set => Data[key] = value;
	}

	/// <summary>
	/// Tries to get a value from the document
	/// </summary>
	public bool TryGetValue(string key, out object? value)
	{
		return Data.TryGetValue(key, out value);
	}

	/// <summary>
	/// Gets a value as a specific type, or returns default if not found or cannot convert
	/// </summary>
	public T? GetValue<T>(string key)
	{
		if (!Data.TryGetValue(key, out var value))
			return default;

		if (value is T typedValue)
			return typedValue;

		try
		{
			return (T?)Convert.ChangeType(value, typeof(T));
		}
		catch
		{
			return default;
		}
	}

	/// <summary>
	/// Sets a value in the document
	/// </summary>
	public void SetValue(string key, object? value)
	{
		Data[key] = value;
	}

	/// <summary>
	/// Gets all keys in the document
	/// </summary>
	public IEnumerable<string> GetKeys()
	{
		return Data.Keys;
	}

	/// <summary>
	/// Checks if the document contains a specific key
	/// </summary>
	public bool ContainsKey(string key)
	{
		return Data.ContainsKey(key);
	}

	/// <summary>
	/// Gets a nested section as a TamlDocument
	/// </summary>
	public TamlDocument? GetSection(string key)
	{
		if (!Data.TryGetValue(key, out var value))
			return null;

		if (value is Dictionary<string, object?> dict)
			return new TamlDocument(dict);

		return null;
	}

	/// <summary>
	/// Flattens the document into a flat dictionary with colon-separated keys (e.g., "Section:Key")
	/// This is useful for configuration providers
	/// </summary>
	public Dictionary<string, string?> Flatten(string prefix = "")
	{
		var result = new Dictionary<string, string?>(StringComparer.OrdinalIgnoreCase);
		FlattenInternal(Data, prefix, result);
		return result;
	}

	private static void FlattenInternal(Dictionary<string, object?> data, string prefix, Dictionary<string, string?> result)
	{
		foreach (var kvp in data)
		{
			var key = string.IsNullOrEmpty(prefix) ? kvp.Key : $"{prefix}:{kvp.Key}";

			if (kvp.Value == null)
			{
				result[key] = null;
			}
			else if (kvp.Value is Dictionary<string, object?> nestedDict)
			{
				FlattenInternal(nestedDict, key, result);
			}
			else if (kvp.Value is System.Collections.IEnumerable enumerable and not string)
			{
				int index = 0;
				foreach (var item in enumerable)
				{
					var arrayKey = $"{key}:{index}";
					if (item is Dictionary<string, object?> itemDict)
					{
						FlattenInternal(itemDict, arrayKey, result);
					}
					else
					{
						result[arrayKey] = item?.ToString();
					}
					index++;
				}
			}
			else
			{
				result[key] = kvp.Value.ToString();
			}
		}
	}

	/// <summary>
	/// Loads a TAML document from a file
	/// </summary>
	public static TamlDocument LoadFromFile(string path)
	{
		if (!File.Exists(path))
			throw new FileNotFoundException($"TAML file not found: {path}");

		var tamlContent = File.ReadAllText(path);
		return Parse(tamlContent);
	}

	/// <summary>
	/// Loads a TAML document from a file asynchronously
	/// </summary>
	public static async Task<TamlDocument> LoadFromFileAsync(string path)
	{
		if (!File.Exists(path))
			throw new FileNotFoundException($"TAML file not found: {path}");

		var tamlContent = await File.ReadAllTextAsync(path);
		return Parse(tamlContent);
	}

	/// <summary>
	/// Parses a TAML string into a TamlDocument
	/// </summary>
	public static TamlDocument Parse(string tamlContent)
	{
		if (string.IsNullOrWhiteSpace(tamlContent))
			return new TamlDocument();

		// Deserialize to Dictionary<string, object?>
		var data = TamlSerializer.Deserialize<Dictionary<string, object?>>(tamlContent);
		return new TamlDocument(data ?? new Dictionary<string, object?>());
	}

	/// <summary>
	/// Saves the document to a file
	/// </summary>
	public void SaveToFile(string path)
	{
		var tamlContent = TamlSerializer.Serialize(Data);
		File.WriteAllText(path, tamlContent);
	}

	/// <summary>
	/// Saves the document to a file asynchronously
	/// </summary>
	public async Task SaveToFileAsync(string path)
	{
		var tamlContent = TamlSerializer.Serialize(Data);
		await File.WriteAllTextAsync(path, tamlContent);
	}

	/// <summary>
	/// Converts the document to a TAML string
	/// </summary>
	public override string ToString()
	{
		return TamlSerializer.Serialize(Data);
	}
}