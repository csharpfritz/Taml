using System.Text;
using TAML.Core;

namespace TAML.Tests;

public class TamlConverterTests
{
    #region JSON Conversion Tests

    [Fact]
    public void ParseFromJson_WithSimpleObject_ConvertsCorrectly()
    {
        // Given
        var json = """
        {
            "name": "John",
            "age": 30,
            "active": true
        }
        """;

        // When
        var taml = TamlConverter.ParseFromJson(json);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("name\t", taml);
        Assert.Contains("John", taml);
        Assert.Contains("age\t", taml);
        Assert.Contains("30", taml);
        Assert.Contains("active\t", taml);
        Assert.Contains("true", taml);
    }

    [Fact]
    public void ParseFromJson_WithNestedObject_ConvertsCorrectly()
    {
        // Given
        var json = """
        {
            "person": {
                "name": "Jane",
                "age": 25
            }
        }
        """;

        // When
        var taml = TamlConverter.ParseFromJson(json);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("person", taml);
        Assert.Contains("name\t", taml);
        Assert.Contains("Jane", taml);
        Assert.Contains("age\t", taml);
        Assert.Contains("25", taml);
    }

    [Fact]
    public void ParseFromJson_WithArray_ConvertsCorrectly()
    {
        // Given
        var json = """
        {
            "items": ["apple", "banana", "cherry"]
        }
        """;

        // When
        var taml = TamlConverter.ParseFromJson(json);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("items", taml);
        Assert.Contains("apple", taml);
        Assert.Contains("banana", taml);
        Assert.Contains("cherry", taml);
    }

    [Fact]
    public void ParseFromJson_WithNull_ConvertsToTilde()
    {
        // Given
        var json = """
        {
            "name": "Test",
            "value": null
        }
        """;

        // When
        var taml = TamlConverter.ParseFromJson(json);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("value\t~", taml);
    }

    [Fact]
    public void ParseFromJson_WithEmptyString_ConvertsCorrectly()
    {
        // Given
        var json = """
        {
            "name": "",
            "value": "test"
        }
        """;

        // When
        var taml = TamlConverter.ParseFromJson(json);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("name\t\"\"", taml);
    }

    [Fact]
    public void ParseFromJson_WithNumbers_ConvertsCorrectly()
    {
        // Given
        var json = """
        {
            "integer": 42,
            "decimal": 3.14,
            "negative": -10
        }
        """;

        // When
        var taml = TamlConverter.ParseFromJson(json);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("integer\t42", taml);
        Assert.Contains("decimal\t3.14", taml);
        Assert.Contains("negative\t-10", taml);
    }

    [Fact]
    public void ParseFromJson_WithBooleans_ConvertsCorrectly()
    {
        // Given
        var json = """
        {
            "enabled": true,
            "disabled": false
        }
        """;

        // When
        var taml = TamlConverter.ParseFromJson(json);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("enabled\ttrue", taml);
        Assert.Contains("disabled\tfalse", taml);
    }

    [Fact]
    public void ParseFromJson_WithEmptyString_ReturnsEmpty()
    {
        // Given
        var json = "";

        // When
        var taml = TamlConverter.ParseFromJson(json);

        // Then
        Assert.Equal(string.Empty, taml);
    }

    [Fact]
    public void ParseFromJson_WithComplexNestedStructure_ConvertsCorrectly()
    {
        // Given
        var json = """
        {
            "config": {
                "database": {
                    "host": "localhost",
                    "port": 5432
                },
                "features": ["auth", "logging"]
            }
        }
        """;

        // When
        var taml = TamlConverter.ParseFromJson(json);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("config", taml);
        Assert.Contains("database", taml);
        Assert.Contains("host\tlocalhost", taml);
        Assert.Contains("port\t5432", taml);
        Assert.Contains("features", taml);
        Assert.Contains("auth", taml);
        Assert.Contains("logging", taml);
    }

    #endregion

    #region XML Conversion Tests

    [Fact]
    public void ParseFromXml_WithSimpleElement_ConvertsCorrectly()
    {
        // Given
        var xml = """
        <person>
            <name>John</name>
            <age>30</age>
        </person>
        """;

        // When
        var taml = TamlConverter.ParseFromXml(xml);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("name\tJohn", taml);
        Assert.Contains("age\t30", taml);
    }

    [Fact]
    public void ParseFromXml_WithNestedElements_ConvertsCorrectly()
    {
        // Given
        var xml = """
        <config>
            <database>
                <host>localhost</host>
                <port>5432</port>
            </database>
        </config>
        """;

        // When
        var taml = TamlConverter.ParseFromXml(xml);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("database", taml);
        Assert.Contains("host\tlocalhost", taml);
        Assert.Contains("port\t5432", taml);
    }

    [Fact]
    public void ParseFromXml_WithAttributes_ConvertsWithAtPrefix()
    {
        // Given
        var xml = """
        <person id="123" type="employee">
            <name>Jane</name>
        </person>
        """;

        // When
        var taml = TamlConverter.ParseFromXml(xml);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("@id\t123", taml);
        Assert.Contains("@type\temployee", taml);
        Assert.Contains("name\tJane", taml);
    }

    [Fact]
    public void ParseFromXml_WithRepeatingElements_ConvertsToList()
    {
        // Given
        var xml = """
        <items>
            <item>apple</item>
            <item>banana</item>
            <item>cherry</item>
        </items>
        """;

        // When
        var taml = TamlConverter.ParseFromXml(xml);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("item", taml);
        Assert.Contains("apple", taml);
        Assert.Contains("banana", taml);
        Assert.Contains("cherry", taml);
    }

    [Fact]
    public void ParseFromXml_WithEmptyString_ReturnsEmpty()
    {
        // Given
        var xml = "";

        // When
        var taml = TamlConverter.ParseFromXml(xml);

        // Then
        Assert.Equal(string.Empty, taml);
    }

    [Fact]
    public void ParseFromXml_WithComplexStructure_ConvertsCorrectly()
    {
        // Given
        var xml = """
        <application>
            <name>MyApp</name>
            <version>1.0.0</version>
            <server>
                <host>0.0.0.0</host>
                <port>8080</port>
            </server>
        </application>
        """;

        // When
        var taml = TamlConverter.ParseFromXml(xml);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("name\tMyApp", taml);
        Assert.Contains("version\t1.0.0", taml);
        Assert.Contains("server", taml);
        Assert.Contains("host\t0.0.0.0", taml);
        Assert.Contains("port\t8080", taml);
    }

    #endregion

    #region YAML Conversion Tests

    [Fact]
    public void ParseFromYaml_WithSimpleKeyValues_ConvertsCorrectly()
    {
        // Given
        var yaml = """
        name: John
        age: 30
        active: true
        """;

        // When
        var taml = TamlConverter.ParseFromYaml(yaml);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("name\t", taml);
        Assert.Contains("John", taml);
        Assert.Contains("age\t", taml);
        Assert.Contains("30", taml);
        Assert.Contains("active\t", taml);
        Assert.Contains("true", taml); // Booleans are serialized as lowercase
    }

    [Fact]
    public void ParseFromYaml_WithNestedStructure_ConvertsCorrectly()
    {
        // Given
        var yaml = """
        database:
          host: localhost
          port: 5432
        """;

        // When
        var taml = TamlConverter.ParseFromYaml(yaml);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("database", taml);
        Assert.Contains("host\t", taml);
        Assert.Contains("localhost", taml);
        Assert.Contains("port\t", taml);
        Assert.Contains("5432", taml);
    }

    [Fact]
    public void ParseFromYaml_WithList_ConvertsCorrectly()
    {
        // Given
        var yaml = """
        items:
          - apple
          - banana
          - cherry
        """;

        // When
        var taml = TamlConverter.ParseFromYaml(yaml);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("items", taml);
        Assert.Contains("apple", taml);
        Assert.Contains("banana", taml);
        Assert.Contains("cherry", taml);
    }

    [Fact]
    public void ParseFromYaml_WithNull_ConvertsToTilde()
    {
        // Given
        var yaml = """
        name: Test
        value: null
        """;

        // When
        var taml = TamlConverter.ParseFromYaml(yaml);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("value\t~", taml);
    }

    [Fact]
    public void ParseFromYaml_WithEmptyString_ReturnsEmpty()
    {
        // Given
        var yaml = "";

        // When
        var taml = TamlConverter.ParseFromYaml(yaml);

        // Then
        Assert.Equal(string.Empty, taml);
    }

    [Fact]
    public void ParseFromYaml_WithComplexStructure_ConvertsCorrectly()
    {
        // Given
        var yaml = """
        application: MyApp
        version: 1.0.0
        server:
          host: 0.0.0.0
          port: 8080
          ssl: true
        features:
          - authentication
          - logging
          - caching
        """;

        // When
        var taml = TamlConverter.ParseFromYaml(yaml);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("application\tMyApp", taml);
        Assert.Contains("version\t1.0.0", taml);
        Assert.Contains("server", taml);
        Assert.Contains("host\t0.0.0.0", taml);
        Assert.Contains("port\t8080", taml);
        Assert.Contains("features", taml);
        Assert.Contains("authentication", taml);
        Assert.Contains("logging", taml);
        Assert.Contains("caching", taml);
    }

    [Fact]
    public void ParseFromYaml_WithNumbers_ConvertsCorrectly()
    {
        // Given
        var yaml = """
        integer: 42
        decimal: 3.14
        negative: -10
        """;

        // When
        var taml = TamlConverter.ParseFromYaml(yaml);

        // Then
        Assert.NotNull(taml);
        Assert.Contains("integer\t42", taml);
        Assert.Contains("decimal\t3.14", taml);
        Assert.Contains("negative\t-10", taml);
    }

    #endregion
}
