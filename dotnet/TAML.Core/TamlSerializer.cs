using System.Text;
using System.Reflection;
using System.Collections;

namespace TAML.Core;

public class TamlSerializer
{
    private const char Tab = '\t';
    private const char NewLine = '\n';
    
    /// <summary>
    /// Serializes an object to TAML format and returns a string
    /// </summary>
    public static string Serialize(object obj)
    {
        if (obj == null)
            return "null";
        
        var sb = new StringBuilder();
        SerializeObject(obj, sb, 0);
        return sb.ToString();
    }
    
    /// <summary>
    /// Serializes an object to TAML format and writes to a stream
    /// </summary>
    public static void Serialize(object obj, Stream stream)
    {
        var tamlString = Serialize(obj);
        using var writer = new StreamWriter(stream, Encoding.UTF8, leaveOpen: true);
        writer.Write(tamlString);
        writer.Flush();
    }
    
    /// <summary>
    /// Serializes an object to TAML format and returns a MemoryStream
    /// </summary>
    public static Stream SerializeToStream(object obj)
    {
        var stream = new MemoryStream();
        Serialize(obj, stream);
        stream.Position = 0;
        return stream;
    }
    
    private static void SerializeObject(object obj, StringBuilder sb, int indentLevel)
    {
        if (obj == null)
        {
            return;
        }
        
        var type = obj.GetType();
        
        // Handle primitive types and strings
        if (IsPrimitiveType(type))
        {
            sb.Append(FormatValue(obj));
            return;
        }
        
        // Handle dictionaries (before IEnumerable check, since dictionaries implement IEnumerable)
        if (obj is IDictionary<string, object?> dict)
        {
            SerializeDictionary(dict, sb, indentLevel);
            return;
        }
        
        // Handle collections (arrays, lists, etc.)
        if (obj is IEnumerable enumerable and not string)
        {
            SerializeCollection(enumerable, sb, indentLevel);
            return;
        }
        
        // Handle complex objects
        SerializeComplexObject(obj, sb, indentLevel);
    }
    
    private static void SerializeComplexObject(object obj, StringBuilder sb, int indentLevel)
    {
        var type = obj.GetType();
        var properties = type.GetProperties(BindingFlags.Public | BindingFlags.Instance)
            .Where(p => p.CanRead);
        
        var fields = type.GetFields(BindingFlags.Public | BindingFlags.Instance);
        
        foreach (var property in properties)
        {
            var value = property.GetValue(obj);
            SerializeMember(property.Name, value, sb, indentLevel);
        }
        
        foreach (var field in fields)
        {
            var value = field.GetValue(obj);
            SerializeMember(field.Name, value, sb, indentLevel);
        }
    }
    
    private static void SerializeMember(string name, object? value, StringBuilder sb, int indentLevel)
    {
        if (value == null)
        {
            WriteIndent(sb, indentLevel);
            sb.Append(name);
            sb.Append(Tab);
            sb.Append("~");
            sb.Append(NewLine);
            return;
        }
        
        var type = value.GetType();
        
        // If it's a primitive type or string, write as key-value pair
        if (IsPrimitiveType(type))
        {
            WriteIndent(sb, indentLevel);
            sb.Append(name);
            sb.Append(Tab);
            sb.Append(FormatValue(value));
            sb.Append(NewLine);
        }
        // If it's a dictionary, write the key then the dictionary items
        else if (value is IDictionary<string, object?> dict)
        {
            WriteIndent(sb, indentLevel);
            sb.Append(name);
            sb.Append(NewLine);
            SerializeDictionary(dict, sb, indentLevel + 1);
        }
        // If it's a collection, write the key then the items
        else if (value is IEnumerable enumerable and not string)
        {
            WriteIndent(sb, indentLevel);
            sb.Append(name);
            sb.Append(NewLine);
            SerializeCollection(enumerable, sb, indentLevel + 1);
        }
        // If it's a complex object, write the key then its properties
        else
        {
            WriteIndent(sb, indentLevel);
            sb.Append(name);
            sb.Append(NewLine);
            SerializeComplexObject(value, sb, indentLevel + 1);
        }
    }
    
    private static void SerializeDictionary(IDictionary<string, object?> dict, StringBuilder sb, int indentLevel)
    {
        foreach (var kvp in dict)
        {
            SerializeMember(kvp.Key, kvp.Value, sb, indentLevel);
        }
    }
    
    private static void SerializeCollection(IEnumerable collection, StringBuilder sb, int indentLevel)
    {
        foreach (var item in collection)
        {
            if (item == null)
            {
                WriteIndent(sb, indentLevel);
                sb.Append("~");
                sb.Append(NewLine);
                continue;
            }
            
            var type = item.GetType();
            
            if (IsPrimitiveType(type))
            {
                WriteIndent(sb, indentLevel);
                sb.Append(FormatValue(item));
                sb.Append(NewLine);
            }
            else if (item is IEnumerable enumerable and not string)
            {
                SerializeCollection(enumerable, sb, indentLevel);
            }
            else
            {
                // For complex objects in a list, serialize their properties
                SerializeComplexObject(item, sb, indentLevel);
            }
        }
    }
    
    private static bool IsPrimitiveType(Type type)
    {
        return type.IsPrimitive 
            || type.IsEnum 
            || type == typeof(string) 
            || type == typeof(decimal) 
            || type == typeof(DateTime) 
            || type == typeof(DateTimeOffset)
            || type == typeof(TimeSpan)
            || type == typeof(Guid);
    }
    
    private static string FormatValue(object value)
    {
        return value switch
        {
            string s when s == "" => "\"\"",  // Empty string becomes ""
            bool b => b ? "true" : "false",
            DateTime dt => dt.ToString("o"), // ISO 8601 format
            DateTimeOffset dto => dto.ToString("o"),
            _ => value.ToString() ?? string.Empty
        };
    }
    
    private static void WriteIndent(StringBuilder sb, int indentLevel)
    {
        for (int i = 0; i < indentLevel; i++)
        {
            sb.Append(Tab);
        }
    }
    
    #region Deserialization Methods
    
    /// <summary>
    /// Deserializes a TAML string to the specified type
    /// </summary>
    public static T? Deserialize<T>(string taml)
    {
        if (string.IsNullOrWhiteSpace(taml))
            return default;
        
        if (taml.Trim() == "null")
            return default;
        
        var lines = ParseLines(taml);
        return (T?)DeserializeFromLines(typeof(T), lines, 0, out _);
    }
    
    /// <summary>
    /// Deserializes a TAML string to the specified type
    /// </summary>
    public static object? Deserialize(string taml, Type targetType)
    {
        if (string.IsNullOrWhiteSpace(taml))
            return null;
        
        if (taml.Trim() == "null")
            return null;
        
        var lines = ParseLines(taml);
        return DeserializeFromLines(targetType, lines, 0, out _);
    }
    
    /// <summary>
    /// Deserializes a TAML stream to the specified type
    /// </summary>
    public static T? Deserialize<T>(Stream stream)
    {
        using var reader = new StreamReader(stream, Encoding.UTF8, leaveOpen: true);
        var taml = reader.ReadToEnd();
        return Deserialize<T>(taml);
    }
    
    /// <summary>
    /// Deserializes a TAML stream to the specified type
    /// </summary>
    public static object? Deserialize(Stream stream, Type targetType)
    {
        using var reader = new StreamReader(stream, Encoding.UTF8, leaveOpen: true);
        var taml = reader.ReadToEnd();
        return Deserialize(taml, targetType);
    }
    
    private static List<TamlLine> ParseLines(string taml)
    {
        var lines = new List<TamlLine>();
        var rawLines = taml.Split(new[] { '\n', '\r' }, StringSplitOptions.RemoveEmptyEntries);
        
        int lineNumber = 0;
        foreach (var rawLine in rawLines)
        {
            lineNumber++;
            
            // Skip comments
            if (rawLine.TrimStart().StartsWith("#"))
                continue;
            
            // Check for space indentation
            if (rawLine.Length > 0 && rawLine[0] == ' ')
            {
                throw new TAMLException("Indentation must use tabs, not spaces", lineNumber, rawLine);
            }
            
            // Count leading tabs for indentation level
            int indentLevel = 0;
            for (int i = 0; i < rawLine.Length; i++)
            {
                if (rawLine[i] == Tab)
                    indentLevel++;
                else if (rawLine[i] == ' ')
                {
                    throw new TAMLException("Mixed spaces and tabs in indentation", lineNumber, rawLine);
                }
                else
                    break;
            }
            
            var content = rawLine.Substring(indentLevel);
            
            if (string.IsNullOrWhiteSpace(content))
            {
                throw new TAMLException("Line has no content after indentation", lineNumber, rawLine);
            }
            
            // Check if it's a key-value pair (contains tab separator)
            var tabIndex = content.IndexOf(Tab);
            if (tabIndex > 0)
            {
                var key = content.Substring(0, tabIndex);
                
                // Skip all separator tabs (one or more)
                int valueStart = tabIndex;
                while (valueStart < content.Length && content[valueStart] == Tab)
                {
                    valueStart++;
                }
                
                var value = valueStart < content.Length ? content.Substring(valueStart) : string.Empty;
                
                // Check for tabs in value
                if (value.Contains(Tab))
                {
                    throw new TAMLException("Value contains invalid tab character", lineNumber, rawLine);
                }
                
                lines.Add(new TamlLine(indentLevel, key, value, true));
            }
            else if (tabIndex == 0)
            {
                throw new TAMLException("Key is empty (line starts with tab)", lineNumber, rawLine);
            }
            else
            {
                // Just a key (parent) or list item value
                lines.Add(new TamlLine(indentLevel, content, null, false));
            }
        }
        
        return lines;
    }
    
    private static object? DeserializeFromLines(Type targetType, List<TamlLine> lines, int startIndex, out int nextIndex)
    {
        if (startIndex >= lines.Count)
        {
            nextIndex = startIndex;
            return null;
        }
        
        var firstLine = lines[startIndex];
        
        // Handle primitive types
        if (IsPrimitiveType(targetType))
        {
            var valueStr = firstLine.HasValue ? firstLine.Value : firstLine.Key;
            nextIndex = startIndex + 1;
            return ConvertValue(valueStr, targetType);
        }
        
        // Handle collections
        if (IsCollectionType(targetType, out var elementType))
        {
            return DeserializeCollection(targetType, elementType!, lines, startIndex, out nextIndex);
        }
        
        // Handle complex objects
        return DeserializeComplexObject(targetType, lines, startIndex, out nextIndex);
    }
    
    private static object? DeserializeComplexObject(Type targetType, List<TamlLine> lines, int startIndex, out int nextIndex)
    {
        var instance = Activator.CreateInstance(targetType);
        if (instance == null)
        {
            nextIndex = startIndex;
            return null;
        }
        
        var currentIndent = startIndex > 0 ? lines[startIndex].IndentLevel : -1;
        nextIndex = startIndex;
        
        while (nextIndex < lines.Count)
        {
            var line = lines[nextIndex];
            
            // Stop if we're back to same or lower indent level (except for the first iteration)
            if (nextIndex > startIndex && line.IndentLevel <= currentIndent)
                break;
            
            // Skip if not at the expected child level
            if (line.IndentLevel != currentIndent + 1)
            {
                nextIndex++;
                continue;
            }
            
            var memberName = line.Key;
            
            // Try to find property or field
            var property = targetType.GetProperty(memberName, BindingFlags.Public | BindingFlags.Instance | BindingFlags.IgnoreCase);
            var field = targetType.GetField(memberName, BindingFlags.Public | BindingFlags.Instance);
            
            if (property != null && property.CanWrite)
            {
                if (line.HasValue)
                {
                    // Simple value
                    var value = ConvertValue(line.Value!, property.PropertyType);
                    property.SetValue(instance, value);
                    nextIndex++;
                }
                else
                {
                    // Complex object or collection
                    nextIndex++;
                    var value = DeserializeFromLines(property.PropertyType, lines, nextIndex, out nextIndex);
                    property.SetValue(instance, value);
                }
            }
            else if (field != null)
            {
                if (line.HasValue)
                {
                    // Simple value
                    var value = ConvertValue(line.Value!, field.FieldType);
                    field.SetValue(instance, value);
                    nextIndex++;
                }
                else
                {
                    // Complex object or collection
                    nextIndex++;
                    var value = DeserializeFromLines(field.FieldType, lines, nextIndex, out nextIndex);
                    field.SetValue(instance, value);
                }
            }
            else
            {
                // Unknown property/field, skip it
                nextIndex++;
            }
        }
        
        return instance;
    }
    
    private static object? DeserializeCollection(Type collectionType, Type elementType, List<TamlLine> lines, int startIndex, out int nextIndex)
    {
        var list = (IList)Activator.CreateInstance(typeof(List<>).MakeGenericType(elementType))!;
        var currentIndent = lines[startIndex].IndentLevel;
        nextIndex = startIndex;
        
        while (nextIndex < lines.Count)
        {
            var line = lines[nextIndex];
            
            // Stop if we're back to lower indent level
            if (line.IndentLevel < currentIndent)
                break;
            
            // Only process items at the current indent level (the collection items)
            if (line.IndentLevel == currentIndent)
            {
                if (IsPrimitiveType(elementType))
                {
                    var valueStr = line.HasValue ? line.Value : line.Key;
                    var value = ConvertValue(valueStr, elementType);
                    list.Add(value);
                    nextIndex++;
                }
                else
                {
                    var item = DeserializeFromLines(elementType, lines, nextIndex, out nextIndex);
                    list.Add(item);
                }
            }
            else
            {
                nextIndex++;
            }
        }
        
        // Convert to target collection type if needed
        if (collectionType.IsArray)
        {
            var array = Array.CreateInstance(elementType, list.Count);
            list.CopyTo(array, 0);
            return array;
        }
        
        if (collectionType.IsInterface || collectionType.IsAbstract)
        {
            return list;
        }
        
        // Try to create instance of the actual collection type
        try
        {
            var instance = Activator.CreateInstance(collectionType);
            if (instance is IList targetList)
            {
                foreach (var item in list)
                {
                    targetList.Add(item);
                }
                return targetList;
            }
        }
        catch
        {
            // Fall back to List<T>
        }
        
        return list;
    }
    
    private static bool IsCollectionType(Type type, out Type? elementType)
    {
        elementType = null;
        
        if (type.IsArray)
        {
            elementType = type.GetElementType();
            return true;
        }
        
        if (type.IsGenericType)
        {
            var genericDef = type.GetGenericTypeDefinition();
            if (genericDef == typeof(List<>) || 
                genericDef == typeof(IList<>) ||
                genericDef == typeof(ICollection<>) ||
                genericDef == typeof(IEnumerable<>))
            {
                elementType = type.GetGenericArguments()[0];
                return true;
            }
        }
        
        foreach (var iface in type.GetInterfaces())
        {
            if (iface.IsGenericType && iface.GetGenericTypeDefinition() == typeof(IEnumerable<>))
            {
                elementType = iface.GetGenericArguments()[0];
                return true;
            }
        }
        
        return false;
    }
    
    private static object? ConvertValue(string? value, Type targetType)
    {
        if (value == null || value == "null" || value == "~")
            return null;
        
        if (targetType == typeof(string))
        {
            // "" represents an empty string
            if (value == "\"\"")
                return "";
            return value;
        }
        
        if (targetType == typeof(bool))
            return value.ToLower() == "true";
        
        if (targetType == typeof(int))
            return int.Parse(value);
        
        if (targetType == typeof(long))
            return long.Parse(value);
        
        if (targetType == typeof(short))
            return short.Parse(value);
        
        if (targetType == typeof(byte))
            return byte.Parse(value);
        
        if (targetType == typeof(decimal))
            return decimal.Parse(value);
        
        if (targetType == typeof(double))
            return double.Parse(value);
        
        if (targetType == typeof(float))
            return float.Parse(value);
        
        if (targetType == typeof(DateTime))
            return DateTime.Parse(value);
        
        if (targetType == typeof(DateTimeOffset))
            return DateTimeOffset.Parse(value);
        
        if (targetType == typeof(TimeSpan))
            return TimeSpan.Parse(value);
        
        if (targetType == typeof(Guid))
            return Guid.Parse(value);
        
        if (targetType.IsEnum)
            return Enum.Parse(targetType, value);
        
        // Try to convert using the type's converter
        try
        {
            return Convert.ChangeType(value, targetType);
        }
        catch
        {
            return null;
        }
    }
    
    private record TamlLine(int IndentLevel, string Key, string? Value, bool HasValue);
    
    #endregion
}
