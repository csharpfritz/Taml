using System.Text;
using TAML.Core;

namespace TAML.Tests;

public class TamlSerializerTests
{
    #region Primitive Types Tests
    
    [Fact]
    public void GivenNullObject_WhenSerializing_ThenReturnsNull()
    {
        // Given
        object? obj = null;
        
        // When
        var result = TamlSerializer.Serialize(obj!);
        
        // Then
        Assert.Equal("null", result);
    }
    
    [Fact]
    public void GivenSimpleString_WhenSerializing_ThenReturnsStringValue()
    {
        // Given
        var obj = "Hello World";
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Equal("Hello World", result);
    }
    
    [Fact]
    public void GivenInteger_WhenSerializing_ThenReturnsIntegerAsString()
    {
        // Given
        var obj = 42;
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Equal("42", result);
    }
    
    [Fact]
    public void GivenBoolean_WhenSerializing_ThenReturnsLowercaseTrueOrFalse()
    {
        // Given
        var trueValue = true;
        var falseValue = false;
        
        // When
        var trueResult = TamlSerializer.Serialize(trueValue);
        var falseResult = TamlSerializer.Serialize(falseValue);
        
        // Then
        Assert.Equal("true", trueResult);
        Assert.Equal("false", falseResult);
    }
    
    [Fact]
    public void GivenDecimal_WhenSerializing_ThenReturnsDecimalAsString()
    {
        // Given
        var obj = 3.14m;
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Equal("3.14", result);
    }
    
    #endregion
    
    #region Multi-Tab Separator Tests
    
    [Fact]
    public void GivenTamlWithSingleTab_WhenDeserializing_ThenParsesCorrectly()
    {
        // Given
        var taml = "name\tJohn\nage\t30";
        
        // When
        var result = TamlSerializer.Deserialize<SimpleObject>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.Equal("John", result.Name);
        Assert.Equal(30, result.Age);
    }
    
    [Fact]
    public void GivenTamlWithMultipleTabs_WhenDeserializing_ThenParsesCorrectly()
    {
        // Given
        var taml = "name\t\t\tJohn\nage\t\t30";
        
        // When
        var result = TamlSerializer.Deserialize<SimpleObject>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.Equal("John", result.Name);
        Assert.Equal(30, result.Age);
    }
    
    [Fact]
    public void GivenTamlWithMixedTabCounts_WhenDeserializing_ThenAllParsedCorrectly()
    {
        // Given
        var taml = "name\tAlice\nage\t\t\t25\nisActive\t\ttrue";
        
        // When
        var result = TamlSerializer.Deserialize<SimpleObject>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.Equal("Alice", result.Name);
        Assert.Equal(25, result.Age);
        Assert.True(result.IsActive);
    }
    
    [Fact]
    public void GivenSerializedObjectWithMultipleTabs_WhenRoundTripping_ThenDeserializesCorrectly()
    {
        // Given - Serialize an object first
        var original = new SimpleObject
        {
            Name = "TestName",
            Age = 42,
            IsActive = true
        };
        
        var serialized = TamlSerializer.Serialize(original);
        
        // Add extra tabs to the serialized output
        var withMultipleTabs = serialized
            .Replace("Name\tTestName", "Name\t\t\t\tTestName")
            .Replace("Age\t42", "Age\t\t\t42")
            .Replace("IsActive\ttrue", "IsActive\t\ttrue");
        
        // When - Deserialize both versions
        var result1 = TamlSerializer.Deserialize<SimpleObject>(serialized);
        var result2 = TamlSerializer.Deserialize<SimpleObject>(withMultipleTabs);
        
        // Then - Both should produce identical results
        Assert.NotNull(result1);
        Assert.NotNull(result2);
        Assert.Equal(result1.Name, result2.Name);
        Assert.Equal(result1.Age, result2.Age);
        Assert.Equal(result1.IsActive, result2.IsActive);
        Assert.Equal("TestName", result2.Name);
        Assert.Equal(42, result2.Age);
        Assert.True(result2.IsActive);
    }
    
    [Fact]
    public void GivenVisuallyAlignedTaml_WhenDeserializing_ThenParsesAsExpected()
    {
        // Given - simulating aligned columns for readability
        var taml = "short\t\t\tvalue1\n" +
                   "very_long_key\tvalue2\n" +
                   "medium\t\t\tvalue3";
        
        // When - Test with a simple object
        var result = TamlSerializer.Deserialize<AlignedObject>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.Equal("value1", result.Short);
        Assert.Equal("value2", result.Very_long_key);
        Assert.Equal("value3", result.Medium);
    }
    
    #endregion
    
    #region Simple Object Tests
    
    [Fact]
    public void GivenSimpleObject_WhenSerializing_ThenReturnsKeyValuePairs()
    {
        // Given
        var obj = new SimpleObject
        {
            Name = "Test",
            Age = 25,
            IsActive = true
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        var lines = result.Split('\n', StringSplitOptions.RemoveEmptyEntries);
        Assert.Contains("Name\tTest", lines);
        Assert.Contains("Age\t25", lines);
        Assert.Contains("IsActive\ttrue", lines);
    }
    
    [Fact]
    public void GivenObjectWithNullProperty_WhenSerializing_ThenReturnsTildeValue()
    {
        // Given
        var obj = new SimpleObject
        {
            Name = null,
            Age = 25,
            IsActive = true
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Contains("Name\t~", result);
    }
        [Fact]
    public void GivenTamlWithTildeValue_WhenDeserializing_ThenReturnsNull()
    {
        // Given
        var taml = "Name\t~\nAge\t25\nIsActive\ttrue";
        
        // When
        var result = TamlSerializer.Deserialize<SimpleObject>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.Null(result.Name);
        Assert.Equal(25, result.Age);
        Assert.True(result.IsActive);
    }
    
    [Fact]
    public void GivenMultipleNullProperties_WhenSerializing_ThenAllSerializeAsTilde()
    {
        // Given
        var obj = new MultiNullObject
        {
            Field1 = null,
            Field2 = "value",
            Field3 = null,
            Field4 = null
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Contains("Field1\t~", result);
        Assert.Contains("Field2\tvalue", result);
        Assert.Contains("Field3\t~", result);
        Assert.Contains("Field4\t~", result);
    }
    
    [Fact]
    public void GivenTamlWithMultipleTildeValues_WhenDeserializing_ThenAllDeserializeAsNull()
    {
        // Given
        var taml = "Field1\t~\nField2\tvalue\nField3\t~\nField4\t~";
        
        // When
        var result = TamlSerializer.Deserialize<MultiNullObject>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.Null(result.Field1);
        Assert.Equal("value", result.Field2);
        Assert.Null(result.Field3);
        Assert.Null(result.Field4);
    }
    
    [Fact]
    public void GivenListWithNullItems_WhenSerializing_ThenNullItemsSerializeAsTilde()
    {
        // Given
        var obj = new NullableListContainer
        {
            Items = new List<string?> { "first", null, "third", null }
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Contains("Items\n", result);
        Assert.Contains("\tfirst\n", result);
        Assert.Contains("\t~\n", result);
        Assert.Contains("\tthird\n", result);
    }
    
    [Fact]
    public void GivenTamlListWithTildeItems_WhenDeserializing_ThenDeserializesWithNulls()
    {
        // Given
        var taml = "Items\n\tfirst\n\t~\n\tthird\n\t~";
        
        // When
        var result = TamlSerializer.Deserialize<NullableListContainer>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.NotNull(result.Items);
        Assert.Equal(4, result.Items.Count);
        Assert.Equal("first", result.Items[0]);
        Assert.Null(result.Items[1]);
        Assert.Equal("third", result.Items[2]);
        Assert.Null(result.Items[3]);
    }
    
    [Fact]
    public void GivenNestedObjectWithNullValue_WhenSerializing_ThenSerializesTilde()
    {
        // Given
        var obj = new ParentObject
        {
            Name = "Parent",
            Child = null
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Contains("Name\tParent\n", result);
        Assert.Contains("Child\t~\n", result);
    }
    
    [Fact]
    public void GivenTamlWithTildeForNestedObject_WhenDeserializing_ThenDeserializesAsNull()
    {
        // Given
        var taml = "Name\tParent\nChild\t~";
        
        // When
        var result = TamlSerializer.Deserialize<ParentObject>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.Equal("Parent", result.Name);
        Assert.Null(result.Child);
    }
    
    [Fact]
    public void GivenTildeValue_WhenRoundTripping_ThenPreservesNull()
    {
        // Given
        var original = new SimpleObject
        {
            Name = null,
            Age = 42,
            IsActive = true
        };
        
        // When
        var serialized = TamlSerializer.Serialize(original);
        var deserialized = TamlSerializer.Deserialize<SimpleObject>(serialized);
        
        // Then
        Assert.NotNull(deserialized);
        Assert.Null(deserialized.Name);
        Assert.Equal(42, deserialized.Age);
        Assert.True(deserialized.IsActive);
        Assert.Contains("Name\t~", serialized);
    }
    
    [Fact]
    public void GivenEmptyString_WhenSerializing_ThenSerializesAsDoubleQuotes()
    {
        // Given
        var obj = new SimpleObject
        {
            Name = "",
            Age = 42,
            IsActive = true
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        // Empty string should be: "Name\t\"\"\n" (tab followed by "" then newline)
        Assert.Contains("Name\t\"\"\n", result);
        Assert.DoesNotContain("Name\t~", result);
        Assert.Contains("Age\t42", result);
    }
    
    [Fact]
    public void GivenEmptyStringVsNull_WhenSerializing_ThenDifferentOutput()
    {
        // Given
        var emptyObj = new SimpleObject { Name = "", Age = 1, IsActive = true };
        var nullObj = new SimpleObject { Name = null, Age = 1, IsActive = true };
        
        // When
        var emptyResult = TamlSerializer.Serialize(emptyObj);
        var nullResult = TamlSerializer.Serialize(nullObj);
        
        // Then
        Assert.Contains("Name\t\"\"\n", emptyResult); // Empty string with ""
        Assert.Contains("Name\t~", nullResult);        // Null value with tilde
        Assert.NotEqual(emptyResult, nullResult);
    }
    
    [Fact]
    public void GivenTamlWithDoubleQuotes_WhenDeserializing_ThenDeserializesAsEmptyString()
    {
        // Given - "" represents empty string
        var taml = "Name\t\"\"\nAge\t42\nIsActive\ttrue";
        
        // When
        var result = TamlSerializer.Deserialize<SimpleObject>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.Equal("", result.Name); // Should be empty string, not null
        Assert.Equal(42, result.Age);
        Assert.True(result.IsActive);
    }
    
    [Fact]
    public void GivenEmptyString_WhenRoundTripping_ThenPreservesEmptyString()
    {
        // Given
        var original = new SimpleObject
        {
            Name = "",
            Age = 42,
            IsActive = true
        };
        
        // When
        var serialized = TamlSerializer.Serialize(original);
        var deserialized = TamlSerializer.Deserialize<SimpleObject>(serialized);
        
        // Then
        Assert.NotNull(deserialized);
        Assert.Equal("", deserialized.Name);
        Assert.Equal(42, deserialized.Age);
        Assert.True(deserialized.IsActive);
        Assert.Contains("Name\t\"\"", serialized);
    }
    
    [Fact]
    public void GivenThreeStateValues_WhenRoundTripping_ThenPreservesAllStates()
    {
        // Given - null, empty string, and non-empty string
        var original = new MultiNullObject
        {
            Field1 = null,
            Field2 = "",
            Field3 = "value",
            Field4 = null
        };
        
        // When
        var serialized = TamlSerializer.Serialize(original);
        var deserialized = TamlSerializer.Deserialize<MultiNullObject>(serialized);
        
        // Then - all three states preserved
        Assert.NotNull(deserialized);
        Assert.Null(deserialized.Field1);           // null preserved
        Assert.Equal("", deserialized.Field2);      // empty string preserved
        Assert.Equal("value", deserialized.Field3); // value preserved
        Assert.Null(deserialized.Field4);           // null preserved
        
        // Verify serialized format
        Assert.Contains("Field1\t~", serialized);
        Assert.Contains("Field2\t\"\"", serialized);
        Assert.Contains("Field3\tvalue", serialized);
        Assert.Contains("Field4\t~", serialized);
    }
        #endregion
    
    #region Collection Tests
    
    [Fact]
    public void GivenListOfStrings_WhenSerializing_ThenReturnsIndentedItems()
    {
        // Given
        var obj = new ListContainer
        {
            Items = new List<string> { "first", "second", "third" }
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Contains("Items\n", result);
        Assert.Contains("\tfirst\n", result);
        Assert.Contains("\tsecond\n", result);
        Assert.Contains("\tthird\n", result);
    }
    
    [Fact]
    public void GivenListOfIntegers_WhenSerializing_ThenReturnsIndentedNumbers()
    {
        // Given
        var obj = new NumberListContainer
        {
            Numbers = new List<int> { 1, 2, 3, 4, 5 }
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Contains("Numbers\n", result);
        Assert.Contains("\t1\n", result);
        Assert.Contains("\t2\n", result);
        Assert.Contains("\t5\n", result);
    }
    
    [Fact]
    public void GivenArray_WhenSerializing_ThenReturnsIndentedItems()
    {
        // Given
        var obj = new string[] { "alpha", "beta", "gamma" };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Contains("alpha\n", result);
        Assert.Contains("beta\n", result);
        Assert.Contains("gamma\n", result);
    }
    
    #endregion
    
    #region Nested Object Tests
    
    [Fact]
    public void GivenNestedObject_WhenSerializing_ThenReturnsProperIndentation()
    {
        // Given
        var obj = new ParentObject
        {
            Name = "Parent",
            Child = new ChildObject
            {
                Name = "Child",
                Value = 100
            }
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Contains("Name\tParent\n", result);
        Assert.Contains("Child\n", result);
        Assert.Contains("\tName\tChild\n", result);
        Assert.Contains("\tValue\t100\n", result);
    }
    
    [Fact]
    public void GivenDeeplyNestedObject_WhenSerializing_ThenReturnsMultipleLevelsOfIndentation()
    {
        // Given
        var obj = new Level1
        {
            Name = "L1",
            Level2 = new Level2
            {
                Name = "L2",
                Level3 = new Level3
                {
                    Name = "L3",
                    Value = "deep"
                }
            }
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Contains("Name\tL1\n", result);
        Assert.Contains("Level2\n", result);
        Assert.Contains("\tName\tL2\n", result);
        Assert.Contains("\tLevel3\n", result);
        Assert.Contains("\t\tName\tL3\n", result);
        Assert.Contains("\t\tValue\tdeep\n", result);
    }
    
    #endregion
    
    #region Complex Scenarios
    
    [Fact]
    public void GivenObjectWithListOfComplexObjects_WhenSerializing_ThenReturnsProperStructure()
    {
        // Given
        var obj = new TeamContainer
        {
            TeamName = "Development",
            Members = new List<Person>
            {
                new Person { Name = "Alice", Age = 30 },
                new Person { Name = "Bob", Age = 25 }
            }
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Contains("TeamName\tDevelopment\n", result);
        Assert.Contains("Members\n", result);
        Assert.Contains("\tName\tAlice\n", result);
        Assert.Contains("\tAge\t30\n", result);
        Assert.Contains("\tName\tBob\n", result);
        Assert.Contains("\tAge\t25\n", result);
    }
    
    [Fact]
    public void GivenComplexConfiguration_WhenSerializing_ThenMatchesSpecExample()
    {
        // Given
        var obj = new AppConfig
        {
            Application = "MyApp",
            Version = "1.0.0",
            Server = new ServerConfig
            {
                Host = "0.0.0.0",
                Port = 8080,
                Ssl = true
            }
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Contains("Application\tMyApp\n", result);
        Assert.Contains("Version\t1.0.0\n", result);
        Assert.Contains("Server\n", result);
        Assert.Contains("\tHost\t0.0.0.0\n", result);
        Assert.Contains("\tPort\t8080\n", result);
        Assert.Contains("\tSsl\ttrue\n", result);
    }
    
    #endregion
    
    #region Stream Tests
    
    [Fact]
    public void GivenObject_WhenSerializingToStream_ThenStreamContainsCorrectData()
    {
        // Given
        var obj = new SimpleObject { Name = "Test", Age = 30, IsActive = true };
        using var stream = new MemoryStream();
        
        // When
        TamlSerializer.Serialize(obj, stream);
        
        // Then
        stream.Position = 0;
        using var reader = new StreamReader(stream);
        var result = reader.ReadToEnd();
        
        Assert.Contains("Name\tTest", result);
        Assert.Contains("Age\t30", result);
        Assert.Contains("IsActive\ttrue", result);
    }
    
    [Fact]
    public void GivenObject_WhenSerializingToStreamMethod_ThenReturnsReadableStream()
    {
        // Given
        var obj = new SimpleObject { Name = "StreamTest", Age = 42, IsActive = false };
        
        // When
        using var stream = TamlSerializer.SerializeToStream(obj);
        
        // Then
        using var reader = new StreamReader(stream);
        var result = reader.ReadToEnd();
        
        Assert.Contains("Name\tStreamTest", result);
        Assert.Contains("Age\t42", result);
        Assert.Contains("IsActive\tfalse", result);
    }
    
    [Fact]
    public void GivenStreamSerialization_WhenReadingPosition_ThenStreamIsAtBeginning()
    {
        // Given
        var obj = new { Value = "test" };
        
        // When
        using var stream = TamlSerializer.SerializeToStream(obj);
        
        // Then
        Assert.Equal(0, stream.Position);
    }
    
    #endregion
    
    #region Comment Tests
    
    [Fact]
    public void GivenTamlWithLeadingHashComment_WhenDeserializing_ThenCommentIsIgnored()
    {
        // Given - line starting with # is a comment
        var taml = "# This is a comment\nName\tJohn\nAge\t30";
        
        // When
        var result = TamlSerializer.Deserialize<SimpleObject>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.Equal("John", result.Name);
        Assert.Equal(30, result.Age);
    }
    
    [Fact]
    public void GivenTamlWithMidLineHash_WhenDeserializing_ThenHashIsTreatedAsLiteral()
    {
        // Given - # in the middle of a value is NOT a comment
        var taml = "Name\tJohn # Smith\nAge\t30";
        
        // When
        var result = TamlSerializer.Deserialize<SimpleObject>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.Equal("John # Smith", result.Name); // Hash should be part of the value
        Assert.Equal(30, result.Age);
    }
    
    [Fact]
    public void GivenObjectWithHashInValue_WhenSerializing_ThenHashIsPreserved()
    {
        // Given
        var obj = new SimpleObject
        {
            Name = "User#123",
            Age = 25,
            IsActive = true
        };
        
        // When
        var result = TamlSerializer.Serialize(obj);
        
        // Then
        Assert.Contains("Name\tUser#123\n", result);
    }
    
    [Fact]
    public void GivenObjectWithHashInValue_WhenRoundTripping_ThenHashIsPreserved()
    {
        // Given
        var original = new SimpleObject
        {
            Name = "Channel#456",
            Age = 42,
            IsActive = true
        };
        
        // When
        var serialized = TamlSerializer.Serialize(original);
        var deserialized = TamlSerializer.Deserialize<SimpleObject>(serialized);
        
        // Then
        Assert.NotNull(deserialized);
        Assert.Equal("Channel#456", deserialized.Name);
        Assert.Equal(42, deserialized.Age);
    }
    
    [Fact]
    public void GivenTamlWithMultipleCommentLines_WhenDeserializing_ThenAllCommentsIgnored()
    {
        // Given
        var taml = @"# Comment 1
Name	Alice
# Comment 2
Age	30
# Comment 3
IsActive	true
# Final comment";
        
        // When
        var result = TamlSerializer.Deserialize<SimpleObject>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.Equal("Alice", result.Name);
        Assert.Equal(30, result.Age);
        Assert.True(result.IsActive);
    }
    
    [Fact]
    public void GivenTamlWithIndentedComment_WhenDeserializing_ThenCommentIsIgnored()
    {
        // Given - Comments at any indentation level (after TrimStart) should be ignored
        var taml = "# Comment at start\nName\tAlice\nAge\t30\n# Comment in middle\nIsActive\ttrue";
        
        // When
        var result = TamlSerializer.Deserialize<SimpleObject>(taml);
        
        // Then
        Assert.NotNull(result);
        Assert.Equal("Alice", result.Name);
        Assert.Equal(30, result.Age);
        Assert.True(result.IsActive);
    }
    
    [Fact]
    public void GivenValueStartingWithHash_WhenRoundTripping_ThenValueIsPreserved()
    {
        // Given - value that starts with # but has other content before it on the line
        var original = new SimpleObject
        {
            Name = "#hashtag",
            Age = 99,
            IsActive = false
        };
        
        // When
        var serialized = TamlSerializer.Serialize(original);
        var deserialized = TamlSerializer.Deserialize<SimpleObject>(serialized);
        
        // Then - the value should be preserved (not treated as comment because key comes first)
        Assert.NotNull(deserialized);
        Assert.Equal("#hashtag", deserialized.Name);
        Assert.Equal(99, deserialized.Age);
        Assert.False(deserialized.IsActive);
    }
    
    #endregion
    
    #region Test Helper Classes
    
    private class SimpleObject
    {
        public string? Name { get; set; }
        public int Age { get; set; }
        public bool IsActive { get; set; }
    }
    
    private class ListContainer
    {
        public List<string>? Items { get; set; }
    }
    
    private class NumberListContainer
    {
        public List<int>? Numbers { get; set; }
    }
    
    private class ParentObject
    {
        public string? Name { get; set; }
        public ChildObject? Child { get; set; }
    }
    
    private class ChildObject
    {
        public string? Name { get; set; }
        public int Value { get; set; }
    }
    
    private class Level1
    {
        public string? Name { get; set; }
        public Level2? Level2 { get; set; }
    }
    
    private class Level2
    {
        public string? Name { get; set; }
        public Level3? Level3 { get; set; }
    }
    
    private class Level3
    {
        public string? Name { get; set; }
        public string? Value { get; set; }
    }
    
    private class Person
    {
        public string? Name { get; set; }
        public int Age { get; set; }
    }
    
    private class TeamContainer
    {
        public string? TeamName { get; set; }
        public List<Person>? Members { get; set; }
    }
    
    private class AppConfig
    {
        public string? Application { get; set; }
        public string? Version { get; set; }
        public ServerConfig? Server { get; set; }
    }
    
    private class ServerConfig
    {
        public string? Host { get; set; }
        public int Port { get; set; }
        public bool Ssl { get; set; }
    }
    
    private class AlignedObject
    {
        public string? Short { get; set; }
        public string? Very_long_key { get; set; }
        public string? Medium { get; set; }
    }
    
    private class MultiNullObject
    {
        public string? Field1 { get; set; }
        public string? Field2 { get; set; }
        public string? Field3 { get; set; }
        public string? Field4 { get; set; }
    }
    
    private class NullableListContainer
    {
        public List<string?>? Items { get; set; }
    }
    
    private class BooleanObject
    {
        public bool Enabled { get; set; }
        public bool Disabled { get; set; }
        public bool Toggle { get; set; }
    }
    
    private class RawTextObject
    {
        public string? Content { get; set; }
    }
    
    private class DateObject
    {
        public DateTime Created { get; set; }
        public DateTime Updated { get; set; }
    }
    
    #endregion
    
    #region Feature 1: Extended Booleans Tests
    
    [Theory]
    [InlineData("true")]
    [InlineData("True")]
    [InlineData("TRUE")]
    [InlineData("yes")]
    [InlineData("Yes")]
    [InlineData("YES")]
    [InlineData("on")]
    [InlineData("On")]
    [InlineData("ON")]
    public void GivenTruthyBooleanValue_WhenDeserializing_ThenReturnsTrueTyped(string truthyValue)
    {
        var taml = $"Enabled\t{truthyValue}\nDisabled\tfalse\nToggle\tfalse";
        var result = TamlSerializer.Deserialize<BooleanObject>(taml);
        Assert.NotNull(result);
        Assert.True(result.Enabled);
    }
    
    [Theory]
    [InlineData("false")]
    [InlineData("False")]
    [InlineData("FALSE")]
    [InlineData("no")]
    [InlineData("No")]
    [InlineData("NO")]
    [InlineData("off")]
    [InlineData("Off")]
    [InlineData("OFF")]
    public void GivenFalsyBooleanValue_WhenDeserializing_ThenReturnsFalseTyped(string falsyValue)
    {
        var taml = $"Enabled\ttrue\nDisabled\t{falsyValue}\nToggle\ttrue";
        var result = TamlSerializer.Deserialize<BooleanObject>(taml);
        Assert.NotNull(result);
        Assert.False(result.Disabled);
    }
    
    [Theory]
    [InlineData("true", true)]
    [InlineData("yes", true)]
    [InlineData("on", true)]
    [InlineData("YES", true)]
    [InlineData("ON", true)]
    [InlineData("false", false)]
    [InlineData("no", false)]
    [InlineData("off", false)]
    [InlineData("NO", false)]
    [InlineData("OFF", false)]
    public void GivenExtendedBooleans_WhenDeserializingToDict_ThenAutoDetectsBoolean(string boolValue, bool expected)
    {
        var taml = $"flag\t{boolValue}";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.IsType<bool>(result["flag"]);
        Assert.Equal(expected, (bool)result["flag"]!);
    }
    
    [Fact]
    public void GivenIntegerOneAndZero_WhenDeserializingToDict_ThenStaysAsInteger()
    {
        var taml = "one\t1\nzero\t0";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.IsType<int>(result["one"]);
        Assert.Equal(1, (int)result["one"]!);
        Assert.IsType<int>(result["zero"]);
        Assert.Equal(0, (int)result["zero"]!);
    }
    
    [Fact]
    public void GivenBooleanTrue_WhenSerializing_ThenOutputsLowercaseTrue()
    {
        var obj = new BooleanObject { Enabled = true, Disabled = false, Toggle = true };
        var result = TamlSerializer.Serialize(obj);
        Assert.Contains("Enabled\ttrue", result);
        Assert.Contains("Disabled\tfalse", result);
    }
    
    [Fact]
    public void GivenYesBooleanValue_WhenRoundTripping_ThenSerializesAsTrue()
    {
        var taml = "Enabled\tyes\nDisabled\tno\nToggle\ton";
        var obj = TamlSerializer.Deserialize<BooleanObject>(taml);
        Assert.NotNull(obj);
        Assert.True(obj.Enabled);
        Assert.False(obj.Disabled);
        Assert.True(obj.Toggle);
        
        var serialized = TamlSerializer.Serialize(obj);
        Assert.Contains("Enabled\ttrue", serialized);
        Assert.Contains("Disabled\tfalse", serialized);
        Assert.Contains("Toggle\ttrue", serialized);
    }
    
    #endregion
    
    #region Feature 2: Raw Text Blocks Tests
    
    [Fact]
    public void GivenRawTextBlock_WhenDeserializing_ThenPreservesContent()
    {
        var taml = "Content\t...\n\tHello World";
        var result = TamlSerializer.Deserialize<RawTextObject>(taml);
        Assert.NotNull(result);
        Assert.Equal("Hello World", result.Content);
    }
    
    [Fact]
    public void GivenRawTextWithTabs_WhenDeserializing_ThenPreservesTabs()
    {
        var taml = "Content\t...\n\tHello\tWorld\twith\ttabs";
        var result = TamlSerializer.Deserialize<RawTextObject>(taml);
        Assert.NotNull(result);
        Assert.Equal("Hello\tWorld\twith\ttabs", result.Content);
    }
    
    [Fact]
    public void GivenMultilineRawText_WhenDeserializing_ThenPreservesNewlines()
    {
        var taml = "Content\t...\n\tline one\n\tline two\n\tline three";
        var result = TamlSerializer.Deserialize<RawTextObject>(taml);
        Assert.NotNull(result);
        Assert.Equal("line one\nline two\nline three", result.Content);
    }
    
    [Fact]
    public void GivenRawTextWithBlankLines_WhenDeserializing_ThenPreservesBlankLines()
    {
        var taml = "Content\t...\n\tline one\n\n\tline three";
        var result = TamlSerializer.Deserialize<RawTextObject>(taml);
        Assert.NotNull(result);
        Assert.Equal("line one\n\nline three", result.Content);
    }
    
    [Fact]
    public void GivenRawTextWithExtraTabs_WhenDeserializing_ThenPreservesContentTabs()
    {
        // Two tabs: one structural + one content tab
        var taml = "Content\t...\n\t\tindented line\n\tnormal line";
        var result = TamlSerializer.Deserialize<RawTextObject>(taml);
        Assert.NotNull(result);
        Assert.Equal("\tindented line\nnormal line", result.Content);
    }
    
    [Fact]
    public void GivenRawTextFollowedByNormalKey_WhenDeserializing_ThenBothParsed()
    {
        var taml = "Content\t...\n\traw text here\nName\tAlice\nAge\t30\nIsActive\ttrue";
        var result = TamlSerializer.Deserialize<SimpleObject>(taml);
        Assert.NotNull(result);
        Assert.Equal("Alice", result.Name);
        Assert.Equal(30, result.Age);
    }
    
    [Fact]
    public void GivenRawTextAsStringProperty_WhenDeserializingToDict_ThenParsesAsString()
    {
        var taml = "script\t...\n\tif [ $1 -eq 1 ]; then\n\t\techo \"Tab indented\"\n\tfi";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.IsType<string>(result["script"]);
        var script = (string)result["script"]!;
        Assert.Contains("if [ $1 -eq 1 ]; then", script);
        Assert.Contains("\techo \"Tab indented\"", script);
        Assert.Contains("fi", script);
    }
    
    [Fact]
    public void GivenEmptyRawText_WhenDeserializing_ThenReturnsEmptyString()
    {
        var taml = "Content\t...\nName\tAlice\nAge\t30\nIsActive\ttrue";
        var result = TamlSerializer.Deserialize<SimpleObject>(taml);
        Assert.NotNull(result);
        Assert.Equal("Alice", result.Name);
    }
    
    [Fact]
    public void GivenStringWithNewlines_WhenSerializing_ThenUsesRawTextBlock()
    {
        var obj = new RawTextObject { Content = "line one\nline two\nline three" };
        var result = TamlSerializer.Serialize(obj);
        Assert.Contains("Content\t...", result);
        Assert.Contains("\tline one", result);
        Assert.Contains("\tline two", result);
        Assert.Contains("\tline three", result);
    }
    
    [Fact]
    public void GivenRawTextBlock_WhenRoundTripping_ThenContentPreserved()
    {
        var original = "line one\nline two\nline three";
        var obj = new RawTextObject { Content = original };
        var serialized = TamlSerializer.Serialize(obj);
        var deserialized = TamlSerializer.Deserialize<RawTextObject>(serialized);
        Assert.NotNull(deserialized);
        Assert.Equal(original, deserialized.Content);
    }
    
    #endregion
    
    #region Feature 3: ISO 8601 Date Detection Tests
    
    [Fact]
    public void GivenIso8601DateOnly_WhenDeserializingToDict_ThenDetectsAsDateTime()
    {
        var taml = "date\t2024-01-15";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.IsType<DateTime>(result["date"]);
        var dt = (DateTime)result["date"]!;
        Assert.Equal(2024, dt.Year);
        Assert.Equal(1, dt.Month);
        Assert.Equal(15, dt.Day);
    }
    
    [Fact]
    public void GivenIso8601DateTimeNoTz_WhenDeserializingToDict_ThenDetectsAsDateTime()
    {
        var taml = "timestamp\t2024-01-15T14:30:00";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.IsType<DateTime>(result["timestamp"]);
        var dt = (DateTime)result["timestamp"]!;
        Assert.Equal(2024, dt.Year);
        Assert.Equal(14, dt.Hour);
        Assert.Equal(30, dt.Minute);
    }
    
    [Fact]
    public void GivenIso8601DateTimeWithZ_WhenDeserializingToDict_ThenDetectsAsDateTime()
    {
        var taml = "timestamp\t2024-01-15T14:30:00Z";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.IsType<DateTime>(result["timestamp"]);
        var dt = (DateTime)result["timestamp"]!;
        Assert.Equal(DateTimeKind.Utc, dt.Kind);
    }
    
    [Fact]
    public void GivenIso8601DateTimeWithOffset_WhenDeserializingToDict_ThenDetectsAsDateTime()
    {
        var taml = "timestamp\t2024-01-15T14:30:00+05:30";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.IsType<DateTime>(result["timestamp"]);
    }
    
    [Fact]
    public void GivenIso8601DateTimeWithFractionalSeconds_WhenDeserializingToDict_ThenDetectsAsDateTime()
    {
        var taml = "timestamp\t2024-01-15T14:30:00.123Z";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.IsType<DateTime>(result["timestamp"]);
    }
    
    [Fact]
    public void GivenIso8601YearMonth_WhenDeserializingToDict_ThenDetectsAsDateTime()
    {
        var taml = "month\t2024-01";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.IsType<DateTime>(result["month"]);
        var dt = (DateTime)result["month"]!;
        Assert.Equal(2024, dt.Year);
        Assert.Equal(1, dt.Month);
    }
    
    [Fact]
    public void GivenBareYear_WhenDeserializingToDict_ThenStaysAsInteger()
    {
        var taml = "year\t2024";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.IsType<int>(result["year"]);
        Assert.Equal(2024, (int)result["year"]!);
    }
    
    [Fact]
    public void GivenTypedDateTime_WhenDeserializing_ThenParsesCorrectly()
    {
        var taml = "Created\t2024-01-15T14:30:00Z\nUpdated\t2024-06-20T10:00:00";
        var result = TamlSerializer.Deserialize<DateObject>(taml);
        Assert.NotNull(result);
        Assert.Equal(2024, result.Created.Year);
        Assert.Equal(2024, result.Updated.Year);
        Assert.Equal(6, result.Updated.Month);
    }
    
    [Fact]
    public void GivenIso8601NegativeOffset_WhenDeserializingToDict_ThenDetectsAsDateTime()
    {
        var taml = "timestamp\t2024-07-04T09:00:00-04:00";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.IsType<DateTime>(result["timestamp"]);
    }
    
    #endregion
    
    #region Feature 4: Duplicate Bare Keys → Collection Tests
    
    [Fact]
    public void GivenDuplicateBareKeys_WhenDeserializingToDict_ThenCreatesListOfDicts()
    {
        var taml = "server\n\thost\tlocalhost\n\tport\t8080\nserver\n\thost\texample.com\n\tport\t443";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.IsType<List<Dictionary<string, object?>>>(result["server"]);
        var servers = (List<Dictionary<string, object?>>)result["server"]!;
        Assert.Equal(2, servers.Count);
        Assert.Equal("localhost", servers[0]["host"]);
        Assert.Equal(8080, servers[0]["port"]);
        Assert.Equal("example.com", servers[1]["host"]);
        Assert.Equal(443, servers[1]["port"]);
    }
    
    [Fact]
    public void GivenThreeDuplicateBareKeys_WhenDeserializingToDict_ThenCreatesListOfThree()
    {
        var taml = "user\n\tname\tAlice\nuser\n\tname\tBob\nuser\n\tname\tCharlie";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        var users = (List<Dictionary<string, object?>>)result["user"]!;
        Assert.Equal(3, users.Count);
        Assert.Equal("Alice", users[0]["name"]);
        Assert.Equal("Bob", users[1]["name"]);
        Assert.Equal("Charlie", users[2]["name"]);
    }
    
    [Fact]
    public void GivenMixedUniqueAndDuplicateKeys_WhenDeserializingToDict_ThenHandlesCorrectly()
    {
        var taml = "name\tMyApp\nserver\n\thost\tlocalhost\n\tport\t8080\nserver\n\thost\texample.com\n\tport\t443\nversion\t1.0.0";
        var result = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(result);
        Assert.Equal("MyApp", result["name"]);
        Assert.Equal("1.0.0", result["version"]);
        Assert.IsType<List<Dictionary<string, object?>>>(result["server"]);
        var servers = (List<Dictionary<string, object?>>)result["server"]!;
        Assert.Equal(2, servers.Count);
    }
    
    [Fact]
    public void GivenListOfDicts_WhenSerializing_ThenOutputsDuplicateBareKeys()
    {
        var dict = new Dictionary<string, object?>
        {
            ["server"] = new List<Dictionary<string, object?>>
            {
                new() { ["host"] = "localhost", ["port"] = 8080 },
                new() { ["host"] = "example.com", ["port"] = 443 }
            }
        };
        var result = TamlSerializer.Serialize(dict);
        // Should contain "server" appearing twice as a parent key
        var serverCount = result.Split('\n').Count(l => l.TrimStart('\t') == "server");
        Assert.Equal(2, serverCount);
        Assert.Contains("host\tlocalhost", result);
        Assert.Contains("host\texample.com", result);
    }
    
    [Fact]
    public void GivenDuplicateBareKeys_WhenRoundTripping_ThenStructurePreserved()
    {
        var taml = "server\n\thost\tlocalhost\n\tport\t8080\nserver\n\thost\texample.com\n\tport\t443";
        var parsed = TamlSerializer.Deserialize<Dictionary<string, object?>>(taml);
        Assert.NotNull(parsed);
        
        var serialized = TamlSerializer.Serialize(parsed);
        var reparsed = TamlSerializer.Deserialize<Dictionary<string, object?>>(serialized);
        Assert.NotNull(reparsed);
        
        var servers = (List<Dictionary<string, object?>>)reparsed["server"]!;
        Assert.Equal(2, servers.Count);
        Assert.Equal("localhost", servers[0]["host"]);
        Assert.Equal("example.com", servers[1]["host"]);
    }
    
    #endregion

}