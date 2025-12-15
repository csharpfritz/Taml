using System.Text;
using System.Text.Json;
using System.Xml.Linq;
using System.Dynamic;
using System.Collections;
using YamlDotNet.Serialization;

namespace TAML.Core;

/// <summary>
/// Provides methods to convert from various formats (JSON, XML, YAML) to TAML format
/// </summary>
public static class TamlConverter
{
    /// <summary>
    /// Parses JSON string and converts it to TAML format
    /// </summary>
    /// <param name="json">The JSON string to parse</param>
    /// <returns>TAML formatted string</returns>
    public static string ParseFromJson(string json)
    {
        if (string.IsNullOrWhiteSpace(json))
            return string.Empty;

        // Parse JSON to a generic object structure
        var jsonElement = JsonSerializer.Deserialize<JsonElement>(json);
        
        // Convert to an intermediate object structure that can be serialized
        var obj = ConvertJsonElement(jsonElement);
        
        // Serialize to TAML
        return TamlSerializer.Serialize(obj ?? new object());
    }

    /// <summary>
    /// Parses XML string and converts it to TAML format
    /// </summary>
    /// <param name="xml">The XML string to parse</param>
    /// <returns>TAML formatted string</returns>
    public static string ParseFromXml(string xml)
    {
        if (string.IsNullOrWhiteSpace(xml))
            return string.Empty;

        // Parse XML
        var doc = XDocument.Parse(xml);
        
        if (doc.Root == null)
            return string.Empty;
        
        // Convert to an intermediate object structure
        var obj = ConvertXElement(doc.Root);
        
        // Serialize to TAML
        return TamlSerializer.Serialize(obj);
    }

    /// <summary>
    /// Parses YAML string and converts it to TAML format
    /// </summary>
    /// <param name="yaml">The YAML string to parse</param>
    /// <returns>TAML formatted string</returns>
    public static string ParseFromYaml(string yaml)
    {
        if (string.IsNullOrWhiteSpace(yaml))
            return string.Empty;

        // Parse YAML using YamlDotNet
        var deserializer = new DeserializerBuilder().Build();
        var yamlObj = deserializer.Deserialize(new StringReader(yaml));
        
        // Convert YAML object to a format compatible with TAML serializer
        var obj = ConvertYamlObject(yamlObj);
        
        // Serialize to TAML
        return TamlSerializer.Serialize(obj ?? new object());
    }

    private static object? ConvertJsonElement(JsonElement element)
    {
        switch (element.ValueKind)
        {
            case JsonValueKind.Object:
                dynamic obj = new ExpandoObject();
                var objDict = (IDictionary<string, object?>)obj;
                foreach (var property in element.EnumerateObject())
                {
                    objDict[property.Name] = ConvertJsonElement(property.Value);
                }
                return obj;

            case JsonValueKind.Array:
                var list = new List<object?>();
                foreach (var item in element.EnumerateArray())
                {
                    list.Add(ConvertJsonElement(item));
                }
                return list;

            case JsonValueKind.String:
                return element.GetString();

            case JsonValueKind.Number:
                if (element.TryGetInt32(out var intValue))
                    return intValue;
                if (element.TryGetInt64(out var longValue))
                    return longValue;
                if (element.TryGetDecimal(out var decimalValue))
                    return decimalValue;
                return element.GetDouble();

            case JsonValueKind.True:
                return true;

            case JsonValueKind.False:
                return false;

            case JsonValueKind.Null:
                return null;

            default:
                return null;
        }
    }

    private static object? ConvertYamlObject(object? yamlObj)
    {
        if (yamlObj == null)
            return null;

        // Handle dictionaries (YamlDotNet returns Dictionary<object, object>)
        if (yamlObj is IDictionary dict)
        {
            dynamic obj = new ExpandoObject();
            var objDict = (IDictionary<string, object?>)obj;
            
            foreach (var key in dict.Keys)
            {
                if (key == null) continue;
                var keyStr = key.ToString() ?? string.Empty;
                objDict[keyStr] = ConvertYamlObject(dict[key]);
            }
            
            return obj;
        }

        // Handle lists
        if (yamlObj is IList list)
        {
            var result = new List<object?>();
            foreach (var item in list)
            {
                result.Add(ConvertYamlObject(item));
            }
            return result;
        }

        // Handle primitive types - return as-is
        return yamlObj;
    }

    private static object ConvertXElement(XElement element)
    {
        dynamic obj = new ExpandoObject();
        var objDict = (IDictionary<string, object?>)obj;

        // Add element name as a property if it has no children
        if (!element.HasElements)
        {
            var value = element.Value;
            if (string.IsNullOrEmpty(value))
                return obj;
            
            // If it's just a simple value element, return the value directly
            if (!element.HasAttributes)
                return value;
                
            objDict["_value"] = value;
        }

        // Add attributes
        foreach (var attr in element.Attributes())
        {
            objDict[$"@{attr.Name.LocalName}"] = attr.Value;
        }

        // Add child elements
        var childGroups = element.Elements().GroupBy(e => e.Name.LocalName);
        foreach (var group in childGroups)
        {
            var elements = group.ToList();
            if (elements.Count == 1)
            {
                objDict[elements[0].Name.LocalName] = ConvertXElement(elements[0]);
            }
            else
            {
                // Multiple elements with the same name - create a list
                var list = elements.Select(ConvertXElement).ToList();
                objDict[group.Key] = list;
            }
        }

        return obj;
    }
}
