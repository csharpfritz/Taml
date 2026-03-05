using System.Globalization;
using System.Text;
using System.Text.RegularExpressions;
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
        
        // If it's a string containing newlines, use raw text block
        if (value is string strVal && strVal.Contains('\n'))
        {
            SerializeRawTextBlock(name, strVal, sb, indentLevel);
        }
        // If it's a primitive type or string, write as key-value pair
        else if (IsPrimitiveType(type))
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
        // If it's a collection of dictionaries with same implied parent key, write as duplicate bare keys
        else if (value is IEnumerable enumerable and not string)
        {
            // Check if this is a list of dictionaries (collection of objects pattern)
            if (IsListOfDictionaries(value))
            {
                SerializeDuplicateKeyCollection(name, (IEnumerable)value, sb, indentLevel);
            }
            else
            {
                WriteIndent(sb, indentLevel);
                sb.Append(name);
                sb.Append(NewLine);
                SerializeCollection(enumerable, sb, indentLevel + 1);
            }
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
    
    /// <summary>
    /// Serializes a raw text block using the ... indicator
    /// </summary>
    private static void SerializeRawTextBlock(string name, string value, StringBuilder sb, int indentLevel)
    {
        WriteIndent(sb, indentLevel);
        sb.Append(name);
        sb.Append(Tab);
        sb.Append("...");
        sb.Append(NewLine);
        
        var lines = value.Split('\n');
        for (int i = 0; i < lines.Length; i++)
        {
            WriteIndent(sb, indentLevel + 1);
            sb.Append(lines[i]);
            sb.Append(NewLine);
        }
    }
    
    /// <summary>
    /// Checks if a value is a List of Dictionary objects (collection of objects pattern)
    /// </summary>
    private static bool IsListOfDictionaries(object value)
    {
        if (value is IList list && list.Count > 0)
        {
            return list[0] is IDictionary<string, object?>;
        }
        return false;
    }
    
    /// <summary>
    /// Serializes a list of dictionaries as duplicate bare keys (collection of objects)
    /// </summary>
    private static void SerializeDuplicateKeyCollection(string name, IEnumerable collection, StringBuilder sb, int indentLevel)
    {
        foreach (var item in collection)
        {
            if (item is IDictionary<string, object?> dict)
            {
                WriteIndent(sb, indentLevel);
                sb.Append(name);
                sb.Append(NewLine);
                SerializeDictionary(dict, sb, indentLevel + 1);
            }
            else
            {
                WriteIndent(sb, indentLevel);
                sb.Append(name);
                sb.Append(NewLine);
                SerializeComplexObject(item!, sb, indentLevel + 1);
            }
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
        // Split preserving blank lines for raw text support
        var rawLines = taml.Replace("\r\n", "\n").Replace("\r", "\n").Split('\n');
        
        int lineNumber = 0;
        bool inRawText = false;
        int rawTextParentIndent = 0;
        string rawTextKey = "";
        var rawTextContent = new StringBuilder();
        bool rawTextHasContent = false;
        
        foreach (var rawLine in rawLines)
        {
            lineNumber++;
            
            // In raw text mode, collect lines until indent drops
            if (inRawText)
            {
                // Blank/empty lines in raw text are preserved
                if (string.IsNullOrEmpty(rawLine) || string.IsNullOrWhiteSpace(rawLine))
                {
                    // Check if it's truly empty (end of input context) or part of raw text
                    // Blank lines inside raw text are preserved
                    if (rawTextHasContent)
                    {
                        rawTextContent.Append(NewLine);
                    }
                    continue;
                }
                
                // Count leading tabs
                int lineIndent = 0;
                for (int i = 0; i < rawLine.Length; i++)
                {
                    if (rawLine[i] == Tab)
                        lineIndent++;
                    else
                        break;
                }
                
                // If indent is <= parent indent, raw text block ends
                if (lineIndent <= rawTextParentIndent)
                {
                    // Finish the raw text block
                    var rawValue = rawTextContent.ToString();
                    // Remove trailing newline if present
                    if (rawValue.EndsWith("\n"))
                        rawValue = rawValue.Substring(0, rawValue.Length - 1);
                    lines.Add(new TamlLine(rawTextParentIndent, rawTextKey, rawValue, true));
                    inRawText = false;
                    rawTextContent.Clear();
                    rawTextHasContent = false;
                    
                    // Now process this line normally (fall through below)
                }
                else
                {
                    // This line is part of raw text - strip the structural indent (parent + 1)
                    int structuralIndent = rawTextParentIndent + 1;
                    var content = rawLine.Substring(Math.Min(structuralIndent, rawLine.Length));
                    
                    if (rawTextHasContent)
                        rawTextContent.Append(NewLine);
                    rawTextContent.Append(content);
                    rawTextHasContent = true;
                    continue;
                }
            }
            
            // Skip empty lines in normal mode (truly empty, not whitespace-only)
            if (rawLine.Length == 0)
                continue;
            
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
            
            var lineContent = rawLine.Substring(indentLevel);
            
            if (string.IsNullOrWhiteSpace(lineContent))
            {
                throw new TAMLException("Line has no content after indentation", lineNumber, rawLine);
            }
            
            // Check if it's a key-value pair (contains tab separator)
            var tabIndex = lineContent.IndexOf(Tab);
            if (tabIndex > 0)
            {
                var key = lineContent.Substring(0, tabIndex);
                
                // Skip all separator tabs (one or more)
                int valueStart = tabIndex;
                while (valueStart < lineContent.Length && lineContent[valueStart] == Tab)
                {
                    valueStart++;
                }
                
                var value = valueStart < lineContent.Length ? lineContent.Substring(valueStart) : string.Empty;
                
                // Check for tabs in value
                if (value.Contains(Tab))
                {
                    throw new TAMLException("Value contains invalid tab character", lineNumber, rawLine);
                }
                
                // Check if this is a raw text indicator
                if (value == "...")
                {
                    inRawText = true;
                    rawTextParentIndent = indentLevel;
                    rawTextKey = key;
                    rawTextContent.Clear();
                    rawTextHasContent = false;
                    continue;
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
                lines.Add(new TamlLine(indentLevel, lineContent, null, false));
            }
        }
        
        // Handle raw text at end of input
        if (inRawText)
        {
            var rawValue = rawTextContent.ToString();
            if (rawValue.EndsWith("\n"))
                rawValue = rawValue.Substring(0, rawValue.Length - 1);
            lines.Add(new TamlLine(rawTextParentIndent, rawTextKey, rawValue, true));
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
        
        // Handle Dictionary types before collections (since Dictionary implements IEnumerable)
        if (IsDictionaryType(targetType, out var keyType, out var valueType))
        {
            return DeserializeDictionary(targetType, keyType!, valueType!, lines, startIndex, out nextIndex);
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
                // For element type of object, infer the actual type from the line structure
                if (elementType == typeof(object))
                {
                    // If line has no value and next line is at deeper indent, it's a complex object/dict
                    if (!line.HasValue && nextIndex + 1 < lines.Count && lines[nextIndex + 1].IndentLevel > line.IndentLevel)
                    {
                        var item = DeserializeFromLines(typeof(Dictionary<string, object?>), lines, nextIndex + 1, out nextIndex);
                        list.Add(item);
                    }
                    else
                    {
                        // It's a primitive value - store as string
                        var valueStr = line.HasValue ? line.Value : line.Key;
                        list.Add(valueStr);
                        nextIndex++;
                    }
                }
                else if (IsPrimitiveType(elementType))
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
    
    private static bool IsDictionaryType(Type type, out Type? keyType, out Type? valueType)
    {
        keyType = null;
        valueType = null;
        
        if (type.IsGenericType)
        {
            var genericDef = type.GetGenericTypeDefinition();
            if (genericDef == typeof(Dictionary<,>) || 
                genericDef == typeof(IDictionary<,>))
            {
                var args = type.GetGenericArguments();
                keyType = args[0];
                valueType = args[1];
                return true;
            }
        }
        
        foreach (var iface in type.GetInterfaces()
            .Where(i => i.IsGenericType && i.GetGenericTypeDefinition() == typeof(IDictionary<,>)))
        {
            var args = iface.GetGenericArguments();
            keyType = args[0];
            valueType = args[1];
            return true;
        }
        
        return false;
    }
    
    private static object? DeserializeDictionary(Type targetType, Type keyType, Type valueType, List<TamlLine> lines, int startIndex, out int nextIndex)
    {
        var dictType = typeof(Dictionary<,>).MakeGenericType(keyType, valueType);
        var dict = (System.Collections.IDictionary)Activator.CreateInstance(dictType)!;
        
        if (startIndex >= lines.Count)
        {
            nextIndex = startIndex;
            return dict;
        }
        
        var currentIndent = lines[startIndex].IndentLevel;
        nextIndex = startIndex;
        
        // Track keys that appear multiple times for duplicate bare key detection
        var duplicateKeys = new HashSet<string>();
        var seenKeys = new HashSet<string>();
        
        // Pre-scan for duplicate bare keys at the current indent level
        for (int i = startIndex; i < lines.Count; i++)
        {
            if (lines[i].IndentLevel < currentIndent)
                break;
            if (lines[i].IndentLevel == currentIndent && !lines[i].HasValue)
            {
                if (!seenKeys.Add(lines[i].Key))
                    duplicateKeys.Add(lines[i].Key);
            }
        }
        
        // Process all items at the current level
        while (nextIndex < lines.Count)
        {
            var line = lines[nextIndex];
            
            // Stop if we're back to a lower indent level
            if (line.IndentLevel < currentIndent)
                break;
            
            // Skip lines at deeper levels (they'll be handled recursively)
            if (line.IndentLevel > currentIndent)
            {
                nextIndex++;
                continue;
            }
            
            var key = ConvertValue(line.Key, keyType);
            
            if (line.HasValue)
            {
                // Simple value
                var value = ConvertValue(line.Value, valueType);
                dict[key!] = value;
                nextIndex++;
            }
            else
            {
                // Check if this is a duplicate bare key (collection of objects)
                if (duplicateKeys.Contains(line.Key))
                {
                    // Collect this entry as part of a list
                    if (!dict.Contains(key!))
                    {
                        // Create a new list for this duplicate key
                        var listForKey = new List<Dictionary<string, object?>>();
                        dict[key!] = listForKey;
                    }
                    
                    // Parse the children of this duplicate key entry
                    if (nextIndex + 1 < lines.Count && lines[nextIndex + 1].IndentLevel > line.IndentLevel)
                    {
                        nextIndex++;
                        var childDict = DeserializeFromLines(typeof(Dictionary<string, object?>), lines, nextIndex, out nextIndex);
                        if (dict[key!] is List<Dictionary<string, object?>> existingList && childDict is Dictionary<string, object?> typedDict)
                        {
                            existingList.Add(typedDict);
                        }
                    }
                    else
                    {
                        nextIndex++;
                    }
                }
                else
                {
                    // Nested structure - check if next line has deeper indent
                    if (nextIndex + 1 < lines.Count && lines[nextIndex + 1].IndentLevel > line.IndentLevel)
                    {
                        nextIndex++; // Move to first nested line
                        
                        // If valueType is object, we need to infer the actual type
                        // Check if it's a collection or a dictionary based on the structure of nested lines
                        var actualType = valueType;
                        if (valueType == typeof(object))
                        {
                            var nextLine = lines[nextIndex];
                            // If the first nested line has a key-value pair (tab-separated), it's a dictionary
                            // Otherwise it's either a list of primitives or a nested complex object
                            if (nextLine.HasValue)
                            {
                                // It's a dictionary with key-value pairs
                                actualType = typeof(Dictionary<string, object?>);
                            }
                            else
                            {
                                // Could be a list of primitives or nested objects
                                // Check if any items at this level have children (nested content)
                                // Also check for duplicate bare keys (collection of objects)
                                bool hasChildren = false;
                                var childKeySeen = new HashSet<string>();
                                int itemsChecked = 0;
                                const int maxItemsToCheck = 100;
                                
                                for (int i = nextIndex; i < lines.Count && lines[i].IndentLevel >= nextLine.IndentLevel; i++)
                                {
                                    if (lines[i].IndentLevel == nextLine.IndentLevel)
                                    {
                                        childKeySeen.Add(lines[i].Key);
                                        
                                        if(i + 1 < lines.Count && lines[i + 1].IndentLevel > lines[i].IndentLevel)
                                        {
                                            hasChildren = true;
                                        }
                                        
                                        if (++itemsChecked >= maxItemsToCheck)
                                            break;
                                    }
                                }
                                
                                // Duplicate bare keys with children → dictionary (will be handled by recursive call)
                                // Single bare keys with children → dictionary
                                // Simple values without children → list
                                actualType = hasChildren
                                    ? typeof(Dictionary<string, object?>)
                                    : typeof(List<object?>);
                            }
                        }
                        
                        var value = DeserializeFromLines(actualType, lines, nextIndex, out nextIndex);
                        dict[key!] = value;
                    }
                    else
                    {
                        // No nested content, value is null
                        dict[key!] = null;
                        nextIndex++;
                    }
                }
            }
        }
        
        return dict;
    }
    
    /// <summary>
    /// Extended boolean truthy values (case-insensitive)
    /// </summary>
    private static readonly HashSet<string> TruthyValues = new(StringComparer.OrdinalIgnoreCase)
    {
        "true", "yes", "on"
    };
    
    /// <summary>
    /// Extended boolean falsy values (case-insensitive)
    /// </summary>
    private static readonly HashSet<string> FalsyValues = new(StringComparer.OrdinalIgnoreCase)
    {
        "false", "no", "off"
    };
    
    /// <summary>
    /// Regex for detecting ISO 8601 date/time strings.
    /// Matches: YYYY-MM-DD, YYYY-MM-DDTHH:MM:SS, with optional timezone/fractional seconds.
    /// Does NOT match bare years like "2024" (those stay as integers).
    /// </summary>
    private static readonly Regex Iso8601Pattern = new(
        @"^\d{4}-\d{2}(-\d{2}(T\d{2}:\d{2}(:\d{2}(\.\d+)?)?(Z|[+-]\d{2}:\d{2})?)?)?$",
        RegexOptions.Compiled);
    
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
        {
            if (TruthyValues.Contains(value))
                return true;
            if (FalsyValues.Contains(value))
                return false;
            return false;
        }
        
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
        
        // Auto-detect type when target is object
        if (targetType == typeof(object))
        {
            return InferTypedValue(value);
        }
        
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
    
    /// <summary>
    /// Infers the typed value from a string when the target type is object.
    /// Detection order: null → empty string → booleans → dates → numbers → strings
    /// </summary>
    private static object? InferTypedValue(string value)
    {
        // Null
        if (value == "null" || value == "~")
            return null;
        
        // Empty string
        if (value == "\"\"")
            return "";
        
        // Booleans (extended, case-insensitive)
        if (TruthyValues.Contains(value))
            return true;
        if (FalsyValues.Contains(value))
            return false;
        
        // ISO 8601 dates (must contain hyphen to distinguish from plain numbers)
        if (Iso8601Pattern.IsMatch(value))
        {
            if (DateTime.TryParse(value, CultureInfo.InvariantCulture,
                DateTimeStyles.RoundtripKind, out var dateResult))
            {
                return dateResult;
            }
        }
        
        // Numbers: integers first, then decimals
        if (long.TryParse(value, NumberStyles.Integer, CultureInfo.InvariantCulture, out var longResult))
        {
            if (longResult >= int.MinValue && longResult <= int.MaxValue)
                return (int)longResult;
            return longResult;
        }
        
        if (double.TryParse(value, NumberStyles.Float, CultureInfo.InvariantCulture, out var doubleResult))
        {
            return doubleResult;
        }
        
        // Default: string
        return value;
    }
    
    private record TamlLine(int IndentLevel, string Key, string? Value, bool HasValue);
    
    #endregion
}
