using Microsoft.Extensions.Configuration;
using TAML.Configuration;
using TAML.Core;

namespace TAML.Tests;

public class TamlConfigurationTests
{
	#region ConfigurationExtensions Tests
	
	[Fact]
	public void GivenConfigurationBuilder_WhenAddingTamlConfigurationWithDefaultPath_ThenConfigurationSourceIsAdded()
	{
		// Given
		var builder = new ConfigurationBuilder();
		
		// When
		builder.AddTamlConfiguration();
		
		// Then
		var sources = builder.Sources;
		Assert.Single(sources);
		Assert.IsAssignableFrom<IConfigurationSource>(sources[0]);
	}
	
	[Fact]
	public void GivenConfigurationBuilder_WhenAddingTamlConfigurationWithCustomPath_ThenConfigurationSourceIsAdded()
	{
		// Given
		var builder = new ConfigurationBuilder();
		var customPath = "custom.taml";
		
		// When
		builder.AddTamlConfiguration(customPath);
		
		// Then
		var sources = builder.Sources;
		Assert.Single(sources);
		Assert.IsAssignableFrom<IConfigurationSource>(sources[0]);
	}
	
	#endregion
	
	#region TamlConfigurationProvider - Path Validation Tests
	
	[Fact]
	public void GivenNullPath_WhenAddingTamlConfiguration_ThenThrowsArgumentException()
	{
		// Given
		var builder = new ConfigurationBuilder();
		string? nullPath = null;
		
		// When/Then
		var exception = Assert.Throws<ArgumentException>(() => builder.AddTamlConfiguration(nullPath!));
		Assert.Contains("Path cannot be null, empty, or whitespace", exception.Message);
		Assert.Equal("path", exception.ParamName);
	}
	
	[Fact]
	public void GivenEmptyPath_WhenAddingTamlConfiguration_ThenThrowsArgumentException()
	{
		// Given
		var builder = new ConfigurationBuilder();
		var emptyPath = string.Empty;
		
		// When/Then
		var exception = Assert.Throws<ArgumentException>(() => builder.AddTamlConfiguration(emptyPath));
		Assert.Contains("Path cannot be null, empty, or whitespace", exception.Message);
		Assert.Equal("path", exception.ParamName);
	}
	
	[Fact]
	public void GivenWhitespacePath_WhenAddingTamlConfiguration_ThenThrowsArgumentException()
	{
		// Given
		var builder = new ConfigurationBuilder();
		var whitespacePath = "   ";
		
		// When/Then
		var exception = Assert.Throws<ArgumentException>(() => builder.AddTamlConfiguration(whitespacePath));
		Assert.Contains("Path cannot be null, empty, or whitespace", exception.Message);
		Assert.Equal("path", exception.ParamName);
	}
	
	#endregion
	
	#region TamlConfigurationProvider - Basic Configuration Tests
	
	[Fact]
	public void GivenSimpleTamlFile_WhenLoadingConfiguration_ThenValuesAreAccessible()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		var tamlContent = "AppName\tMyApplication\nVersion\t1.0.0\nDebug\ttrue\nMaxConnections\t100";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			
			// Then
			Assert.Equal("MyApplication", config["AppName"]);
			Assert.Equal("1.0.0", config["Version"]);
			Assert.Equal("true", config["Debug"]);
			Assert.Equal("100", config["MaxConnections"]);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	[Fact]
	public void GivenNestedTamlFile_WhenLoadingConfiguration_ThenNestedValuesAreAccessibleWithColonSeparator()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		var tamlContent = "Database\n\tHost\tlocalhost\n\tPort\t5432\n\tConnectionString\tServer=localhost;Database=mydb\n\tCredentials\n\t\tUsername\tadmin\n\t\tPassword\tsecret123";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			
			// Then
			Assert.Equal("localhost", config["Database:Host"]);
			Assert.Equal("5432", config["Database:Port"]);
			Assert.Equal("Server=localhost;Database=mydb", config["Database:ConnectionString"]);
			Assert.Equal("admin", config["Database:Credentials:Username"]);
			Assert.Equal("secret123", config["Database:Credentials:Password"]);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	[Fact]
	public void GivenTamlFileWithArrays_WhenLoadingConfiguration_ThenArrayValuesAreAccessibleByIndex()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		var tamlContent = "AllowedHosts\n\tlocalhost\n\texample.com\n\tapi.example.com";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			
			// Then
			Assert.Equal("localhost", config["AllowedHosts:0"]);
			Assert.Equal("example.com", config["AllowedHosts:1"]);
			Assert.Equal("api.example.com", config["AllowedHosts:2"]);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	[Fact]
	public void GivenTamlFileWithComplexStructure_WhenLoadingConfiguration_ThenAllValuesAreAccessible()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		// Create a realistic app configuration structure in TAML
		var tamlContent = "App\n\tName\tWebAPI\n\tEnvironment\tProduction\nLogging\n\tLogLevel\n\t\tDefault\tInformation\n\t\tMicrosoft\tWarning\n\t\tSystem\tError\nConnectionStrings\n\tDefaultConnection\tServer=localhost;Database=prod";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			
			// Then
			Assert.Equal("WebAPI", config["App:Name"]);
			Assert.Equal("Production", config["App:Environment"]);
			Assert.Equal("Information", config["Logging:LogLevel:Default"]);
			Assert.Equal("Warning", config["Logging:LogLevel:Microsoft"]);
			Assert.Equal("Error", config["Logging:LogLevel:System"]);
			Assert.Equal("Server=localhost;Database=prod", config["ConnectionStrings:DefaultConnection"]);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	#endregion
	
	#region TamlConfigurationProvider - Section Tests
	
	[Fact]
	public void GivenNestedConfiguration_WhenAccessingSection_ThenSectionValuesAreAccessible()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		var tamlContent = "Database\n\tPrimary\n\t\tHost\tdb1.example.com\n\t\tPort\t5432\n\tSecondary\n\t\tHost\tdb2.example.com\n\t\tPort\t5433";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			var primarySection = config.GetSection("Database:Primary");
			
			// Then
			Assert.Equal("db1.example.com", primarySection["Host"]);
			Assert.Equal("5432", primarySection["Port"]);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	[Fact]
	public void GivenConfiguration_WhenGettingChildKeys_ThenCorrectKeysAreReturned()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		var tamlContent = "Root\n\tChild1\tValue1\n\tChild2\tValue2\n\tNested\n\t\tGrandChild\tValue3";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			var section = config.GetSection("Root");
			var children = section.GetChildren();
			
			// Then
			var childKeys = children.Select(c => c.Key).ToList();
			Assert.Contains("Child1", childKeys);
			Assert.Contains("Child2", childKeys);
			Assert.Contains("Nested", childKeys);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	#endregion
	
	#region TamlConfigurationProvider - Binding Tests
	
	[Fact]
	public void GivenTamlConfiguration_WhenBindingToObject_ThenObjectIsPopulated()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		var tamlContent = "AppSettings\n\tApplicationName\tTestApp\n\tVersion\t2.0.1\n\tMaxUsers\t500\n\tEnableFeatureX\ttrue";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			var appSettings = new AppSettings();
			config.GetSection("AppSettings").Bind(appSettings);
			
			// Then
			Assert.Equal("TestApp", appSettings.ApplicationName);
			Assert.Equal("2.0.1", appSettings.Version);
			Assert.Equal(500, appSettings.MaxUsers);
			Assert.True(appSettings.EnableFeatureX);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	[Fact]
	public void GivenTamlConfigurationWithListOfObjects_WhenLoadingConfiguration_ThenValuesAreAccessible()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		// TAML format for arrays of objects - use nested structure
		var tamlContent = "ServerConfig\n\tServers\n\t\tHost\tserver1.example.com\n\t\tPort\t8080\n\t\tHost\tserver2.example.com\n\t\tPort\t8081";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			
			// Then - check that values are accessible through configuration
			Assert.NotNull(config["ServerConfig:Servers:0:Host"]);
			Assert.NotNull(config["ServerConfig:Servers:0:Port"]);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	#endregion
	
	#region TamlConfigurationProvider - Edge Cases
	
	[Fact]
	public void GivenNonExistentFile_WhenLoadingConfiguration_ThenConfigurationIsEmpty()
	{
		// Given
		var nonExistentFile = Path.Combine(Path.GetTempPath(), $"{Guid.NewGuid()}.taml");
		
		// When
		var builder = new ConfigurationBuilder();
		builder.AddTamlConfiguration(nonExistentFile);
		var config = builder.Build();
		
		// Then - configuration should have no values
		var allValues = config.AsEnumerable().Where(kvp => !string.IsNullOrEmpty(kvp.Value)).ToList();
		Assert.Empty(allValues);
	}
	
	[Fact]
	public void GivenEmptyTamlFile_WhenLoadingConfiguration_ThenConfigurationIsEmpty()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		File.WriteAllText(tempFile, "");
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			
			// Then - configuration should have no values
			var allValues = config.AsEnumerable().Where(kvp => !string.IsNullOrEmpty(kvp.Value)).ToList();
			Assert.Empty(allValues);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	[Fact]
	public void GivenTamlWithNullValue_WhenLoadingConfiguration_ThenNullIsAccessible()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		// TAML uses ~ for null values
		var tamlContent = "Setting1\tvalue1\nSetting2\t~\nSetting3\tvalue3";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			
			// Then
			Assert.Equal("value1", config["Setting1"]);
			Assert.Null(config["Setting2"]);
			Assert.Equal("value3", config["Setting3"]);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	[Fact]
	public void GivenConfiguration_WhenAccessingNonExistentKey_ThenReturnsNull()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		var tamlContent = "ExistingKey\tvalue";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			
			// Then
			Assert.Null(config["NonExistentKey"]);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	[Fact]
	public void GivenConfiguration_WhenKeyAccessIsCaseInsensitive_ThenValuesAreAccessible()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		var tamlContent = "AppName\tMyApp";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			
			// Then
			Assert.Equal("MyApp", config["AppName"]);
			Assert.Equal("MyApp", config["appname"]);
			Assert.Equal("MyApp", config["APPNAME"]);
			Assert.Equal("MyApp", config["aPpNaMe"]);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	#endregion
	
	#region TamlConfigurationProvider - Integration with Multiple Sources
	
	[Fact]
	public void GivenMultipleConfigurationSources_WhenTamlIsAddedLast_ThenTamlOverridesPreviousValues()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		var tamlContent = "Setting1\tFromTaml\nSetting2\tAlsoFromTaml";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddInMemoryCollection(new Dictionary<string, string?>
			{
				{ "Setting1", "FromMemory" },
				{ "Setting3", "OnlyInMemory" }
			});
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			
			// Then
			Assert.Equal("FromTaml", config["Setting1"]); // Overridden by TAML
			Assert.Equal("AlsoFromTaml", config["Setting2"]); // From TAML
			Assert.Equal("OnlyInMemory", config["Setting3"]); // From memory, not overridden
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	[Fact]
	public void GivenMultipleConfigurationSources_WhenTamlIsAddedFirst_ThenSubsequentSourcesOverrideTaml()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		var tamlContent = "Setting1\tFromTaml\nSetting2\tAlsoFromTaml";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			builder.AddInMemoryCollection(new Dictionary<string, string?>
			{
				{ "Setting1", "FromMemory" }
			});
			var config = builder.Build();
			
			// Then
			Assert.Equal("FromMemory", config["Setting1"]); // Overridden by memory
			Assert.Equal("AlsoFromTaml", config["Setting2"]); // Still from TAML
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	#endregion
	
	#region TamlConfigurationProvider - Real-World Scenarios
	
	[Fact]
	public void GivenRealWorldAppSettings_WhenLoadingConfiguration_ThenAllSettingsAreAccessible()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		// Simulating a realistic appsettings.taml file
		var tamlContent = @"Application
	Name	MyWebApp
	Version	1.2.3
	Environment	Development
Logging
	LogLevel
		Default	Information
		Microsoft.AspNetCore	Warning
ConnectionStrings
	DefaultConnection	Server=localhost;Database=myapp;User Id=sa;Password=Pass@word1;
	RedisConnection	localhost:6379
Features
	EnableNewUI	true
	EnableBetaFeatures	false
	MaxUploadSize	10485760".Replace("\r\n", "\n").Replace("    ", "\t");
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			
			// Then
			Assert.Equal("MyWebApp", config["Application:Name"]);
			Assert.Equal("1.2.3", config["Application:Version"]);
			Assert.Equal("Development", config["Application:Environment"]);
			Assert.Equal("Information", config["Logging:LogLevel:Default"]);
			Assert.Equal("Warning", config["Logging:LogLevel:Microsoft.AspNetCore"]);
			Assert.Equal("Server=localhost;Database=myapp;User Id=sa;Password=Pass@word1;", config["ConnectionStrings:DefaultConnection"]);
			Assert.Equal("localhost:6379", config["ConnectionStrings:RedisConnection"]);
			Assert.Equal("true", config["Features:EnableNewUI"]);
			Assert.Equal("false", config["Features:EnableBetaFeatures"]);
			Assert.Equal("10485760", config["Features:MaxUploadSize"]);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	[Fact]
	public void GivenConfigurationWithArrayOfPrimitives_WhenLoadingConfiguration_ThenArrayIsAccessible()
	{
		// Given
		var tempFile = Path.GetTempFileName();
		var tamlContent = "AllowedOrigins\n\thttp://localhost:3000\n\thttp://localhost:4200\n\thttps://app.example.com";
		File.WriteAllText(tempFile, tamlContent);
		
		try
		{
			// When
			var builder = new ConfigurationBuilder();
			builder.AddTamlConfiguration(tempFile);
			var config = builder.Build();
			var origins = config.GetSection("AllowedOrigins").GetChildren().Select(c => c.Value).ToList();
			
			// Then
			Assert.Equal(3, origins.Count);
			Assert.Contains("http://localhost:3000", origins);
			Assert.Contains("http://localhost:4200", origins);
			Assert.Contains("https://app.example.com", origins);
		}
		finally
		{
			File.Delete(tempFile);
		}
	}
	
	#endregion
	
	#region Helper Classes for Binding Tests
	
	private class AppSettings
	{
		public string ApplicationName { get; set; } = string.Empty;
		public string Version { get; set; } = string.Empty;
		public int MaxUsers { get; set; }
		public bool EnableFeatureX { get; set; }
	}
	
	private class ServerConfig
	{
		public List<ServerInfo> Servers { get; set; } = new();
	}
	
	private class ServerInfo
	{
		public string Host { get; set; } = string.Empty;
		public int Port { get; set; }
	}
	
	#endregion
}
