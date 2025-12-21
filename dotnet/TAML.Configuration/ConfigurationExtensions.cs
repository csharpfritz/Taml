using Microsoft.Extensions.Configuration;

namespace TAML.Configuration;

public static class ConfigurationExtensions
{

	/// <summary>
	/// Adds TAML configuration provider to the configuration builder for the standard "appsettings.taml" file.
	/// </summary>
	/// <param name="builder">The configuration builder</param>
	/// <returns></returns>
	public static IConfigurationBuilder AddTamlConfiguration(this IConfigurationBuilder builder)
	{
		return builder.Add(new TamlConfigurationSource("appsettings.taml"));
	}

	public static IConfigurationBuilder AddTamlConfiguration(this IConfigurationBuilder builder, string path)
	{
		return builder.Add(new TamlConfigurationSource(path));
	}

}

internal class TamlConfigurationSource(string path) : IConfigurationSource
{
	private readonly string _path = !string.IsNullOrWhiteSpace(path) 
		? path 
		: throw new ArgumentException("Path cannot be null, empty, or whitespace.", nameof(path));

	public IConfigurationProvider Build(IConfigurationBuilder builder)
	{
		return new TamlConfigurationProvider(_path);
	}

}
