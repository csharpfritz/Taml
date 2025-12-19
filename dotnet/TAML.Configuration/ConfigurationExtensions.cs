using Microsoft.Extensions.Configuration;

namespace TAML.Configuration;

public static class ConfigurationExtensions
{

	/// <summary>
	/// Adds TAML configuration provider to the configuration builder for the standard "appsettings.taml" file.
	/// </summary>
	/// <param name="builder">The configuration builder</param>
	/// <returns></returns>
	public static IConfigurationBuilder AddTamlConfiguration(this ConfigurationBuilder builder)
	{
		return builder.Add(new TamlConfigurationSource("appsettings.taml"));
	}

	public static IConfigurationBuilder AddTamlConfiguration(this ConfigurationBuilder builder, string path)
	{
		return builder.Add(new TamlConfigurationSource(path));
	}

}

internal class TamlConfigurationSource(string path) : IConfigurationSource
{

	public IConfigurationProvider Build(IConfigurationBuilder builder)
	{
		return new TamlConfigurationProvider(path);
	}

}
