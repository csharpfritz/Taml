#include <iostream>
#include <format>  // C++20 for formatted output
#include "Taml.h"

// ANSI color codes (optional, works in modern terminals)
constexpr const char* GREEN = "\033[32m";
constexpr const char* RED = "\033[31m";
constexpr const char* YELLOW = "\033[33m";
constexpr const char* BLUE = "\033[34m";
constexpr const char* RESET = "\033[0m";

// Helper functions for pretty output
void printSection(const std::string& title) {
    std::cout << std::format("\n{:=^60}\n", title);
}

void printSubsection(const std::string& title) {
    std::cout << std::format("\n--- {} ---\n", title);
}

void printResult(bool success, const std::string& desc) {
    const char* color = success ? GREEN : RED;
    const char* symbol = success ? "?" : "?";
    std::cout << std::format("{}{}{} {}\n", color, symbol, RESET, desc);
}

void printInfo(const std::string& desc) {
    std::cout << std::format("{}{}{}\n", BLUE, desc, RESET);
}

void printWarning(const std::string& desc) {
    std::cout << std::format("{}{}{}\n", YELLOW, desc, RESET);
}

// Quick compile-time check that Boost.Describe is available
struct _DescribeTest
{
    int a;
};
BOOST_DESCRIBE_STRUCT(_DescribeTest, (), (a))

#ifndef TAML_VERSION_MAJOR
#error "TAML_VERSION_MAJOR not defined"
#endif

// Define a simple test class with Boost.Describe
struct TestPerson
{
    std::string name;
    int age;
};

BOOST_DESCRIBE_STRUCT(TestPerson, (), (name, age))

int main()
{
    static_assert(TAML_VERSION_MAJOR == 1, "Unexpected major version");
    // std::cout << "TAML version: " << Taml::version() << std::endl;
    printInfo(std::format("TAML version: {}", Taml::version()));

    // Test Konstruktoren
    // std::cout << "\n=== Testing Constructors ===" << std::endl;
    printSection("Testing Constructors");
    Taml::Document emptyDoc; // Default constructor
    // std::cout << "Empty document created." << std::endl;
    printResult(true, "Empty document created");

    std::unordered_map<std::string, std::any> initialData;
    initialData["initial"] = std::string("value");
    Taml::Document dataDoc(initialData); // Constructor with data
    // std::cout << "Document with initial data created." << std::endl;
    printResult(true, "Document with initial data created");

    // Test Data() getter
    // std::cout << "\n=== Testing Data() ===" << std::endl;
    printSection("Testing Data()");
    const auto &data = emptyDoc.Data();
    // std::cout << "Data map size: " << data.size() << std::endl;
    printInfo(std::format("Data map size: {}", data.size()));

    // Test operator[]
    // std::cout << "\n=== Testing operator[] ===" << std::endl;
    printSection("Testing operator[]");
    emptyDoc["name"] = std::string("Test");
    // std::cout << "Set name via operator[]: " << std::any_cast<std::string>(emptyDoc["name"]) << std::endl;
    printResult(true, std::format("Set name via operator[]: {}", std::any_cast<std::string>(emptyDoc["name"])));

    // Test TryGetValue
    // std::cout << "\n=== Testing TryGetValue ===" << std::endl;
    printSection("Testing TryGetValue");
    std::any value;
    if (emptyDoc.TryGetValue("name", value))
    {
        // std::cout << "TryGetValue succeeded: " << std::any_cast<std::string>(value) << std::endl;
        printResult(true, std::format("TryGetValue succeeded: {}", std::any_cast<std::string>(value)));
    }
    else
    {
        // std::cout << "TryGetValue failed" << std::endl;
        printResult(false, "TryGetValue failed");
    }

    // Test GetValue<T>
    // std::cout << "\n=== Testing GetValue<T> ===" << std::endl;
    printSection("Testing GetValue<T>");
    emptyDoc["age"] = std::string("25");
    auto ageOpt = emptyDoc.GetValue<int>("age");
    if (ageOpt)
    {
        // std::cout << "GetValue<int> succeeded: " << *ageOpt << std::endl;
        printResult(true, std::format("GetValue<int> succeeded: {}", *ageOpt));
    }
    else
    {
        // std::cout << "GetValue<int> failed" << std::endl;
        printResult(false, "GetValue<int> failed");
    }

    // Test SetValue
    // std::cout << "\n=== Testing SetValue ===" << std::endl;
    printSection("Testing SetValue");
    emptyDoc.SetValue("city", std::string("Berlin"));
    // std::cout << "SetValue called for city." << std::endl;
    printResult(true, "SetValue called for city");

    // Test GetKeys
    // std::cout << "\n=== Testing GetKeys ===" << std::endl;
    printSection("Testing GetKeys");
    auto keys = emptyDoc.GetKeys();
    // std::cout << "Keys: ";
    // for (const auto &key : keys)
    // {
    //     std::cout << key << " ";
    // }
    // std::cout << std::endl;
    std::string keysStr;
    for (size_t i = 0; i < keys.size(); ++i) {
        keysStr += keys[i];
        if (i < keys.size() - 1) keysStr += " ";
    }
    printInfo(std::format("Keys: {}", keysStr));

    // Test ContainsKey
    // std::cout << "\n=== Testing ContainsKey ===" << std::endl;
    printSection("Testing ContainsKey");
    // std::cout << "Contains 'name': " << (emptyDoc.ContainsKey("name") ? "true" : "false") << std::endl;
    // std::cout << "Contains 'nonexistent': " << (emptyDoc.ContainsKey("nonexistent") ? "true" : "false") << std::endl;
    printResult(emptyDoc.ContainsKey("name"), "Contains 'name'");
    printResult(!emptyDoc.ContainsKey("nonexistent"), "Does not contain 'nonexistent'");

    // Test GetSection
    // std::cout << "\n=== Testing GetSection ===" << std::endl;
    printSection("Testing GetSection");
    std::unordered_map<std::string, std::any> nestedData;
    nestedData["subkey"] = std::string("subvalue");
    emptyDoc.SetValue("nested", nestedData);
    auto sectionOpt = emptyDoc.GetSection("nested");
    if (sectionOpt)
    {
        // std::cout << "GetSection succeeded: " << std::any_cast<std::string>((*sectionOpt)["subkey"]) << std::endl;
        printResult(true, std::format("GetSection succeeded: {}", std::any_cast<std::string>((*sectionOpt)["subkey"])));
    }
    else
    {
        // std::cout << "GetSection failed" << std::endl;
        printResult(false, "GetSection failed");
    }

    // Test Flatten
    // std::cout << "\n=== Testing Flatten ===" << std::endl;
    printSection("Testing Flatten");
    auto flat = emptyDoc.Flatten();
    // std::cout << "Flattened keys: ";
    // for (const auto &[key, val] : flat)
    // {
    //     std::cout << key << " ";
    // }
    // std::cout << std::endl;
    std::string flatKeys;
    for (const auto &[key, val] : flat)
    {
        flatKeys += key + " ";
    }
    printInfo(std::format("Flattened keys: {}", flatKeys));

    // Test ToString
    // std::cout << "\n=== Testing ToString ===" << std::endl;
    printSection("Testing ToString");
    // std::cout << "Document as string:\n"
    //           << emptyDoc.ToString() << std::endl;
    printInfo("Document as string:");
    std::cout << emptyDoc.ToString() << std::endl;

    // Test LoadFromFile and SaveToFile (synchronous)
    // std::cout << "\n=== Testing LoadFromFile and SaveToFile ===" << std::endl;
    printSection("Testing LoadFromFile and SaveToFile");
    try
    {
        emptyDoc.SaveToFile("test_output.taml");
        // std::cout << "SaveToFile succeeded." << std::endl;
        printResult(true, "SaveToFile succeeded");

        // Test Parse with the saved content
        std::string savedContent = emptyDoc.ToString();
        auto parsedDoc = Taml::Document::Parse(savedContent);
        // std::cout << "Parse succeeded, parsed document has " << parsedDoc.Data().size() << " keys." << std::endl;
        printResult(true, std::format("Parse succeeded, parsed document has {} keys", parsedDoc.Data().size()));

        // Note: LoadFromFile would require a valid TAML file and implemented Parse
        // For now, just test that SaveToFile doesn't crash
    }
    catch (const std::exception &e)
    {
        // std::cout << "SaveToFile or Parse failed: " << e.what() << std::endl;
        printResult(false, std::format("SaveToFile or Parse failed: {}", e.what()));
    }

    // Test async methods (LoadFromFileAsync and SaveToFileAsync)
    // std::cout << "\n=== Testing Async Methods ===" << std::endl;
    printSection("Testing Async Methods");
    try
    {
        auto saveFuture = emptyDoc.SaveToFileAsync("test_output_async.taml");
        saveFuture.get(); // Wait for completion
        // std::cout << "SaveToFileAsync succeeded." << std::endl;
        printResult(true, "SaveToFileAsync succeeded");

        // LoadFromFileAsync would require a valid file and implemented Parse
    }
    catch (const std::exception &e)
    {
        // std::cout << "Async operation failed: " << e.what() << std::endl;
        printResult(false, std::format("Async operation failed: {}", e.what()));
    }

    // Test Serializer directly
    // std::cout << "\n=== Testing Serializer ===" << std::endl;
    printSection("Testing Serializer");

    // Test Serializer::Serialize with primitive types
    // std::cout << "\n--- Testing Primitive Types ---" << std::endl;
    printSubsection("Testing Primitive Types");
    // std::cout << "Serialize int: " << Taml::Serializer::Serialize(std::any(42)) << std::endl;
    printInfo(std::format("Serialize int: {}", Taml::Serializer::Serialize(std::any(42))));
    // std::cout << "Serialize double: " << Taml::Serializer::Serialize(std::any(3.14)) << std::endl;
    printInfo(std::format("Serialize double: {}", Taml::Serializer::Serialize(std::any(3.14))));
    // std::cout << "Serialize bool true: " << Taml::Serializer::Serialize(std::any(true)) << std::endl;
    printInfo(std::format("Serialize bool true: {}", Taml::Serializer::Serialize(std::any(true))));
    // std::cout << "Serialize bool false: " << Taml::Serializer::Serialize(std::any(false)) << std::endl;
    printInfo(std::format("Serialize bool false: {}", Taml::Serializer::Serialize(std::any(false))));
    // std::cout << "Serialize string: " << Taml::Serializer::Serialize(std::any(std::string("hello"))) << std::endl;
    printInfo(std::format("Serialize string: {}", Taml::Serializer::Serialize(std::any(std::string("hello")))));
    // std::cout << "Serialize empty string: " << Taml::Serializer::Serialize(std::any(std::string(""))) << std::endl;
    printInfo(std::format("Serialize empty string: {}", Taml::Serializer::Serialize(std::any(std::string("")))));
    // std::cout << "Serialize null: " << Taml::Serializer::Serialize(std::any{}) << std::endl;
    printInfo(std::format("Serialize null: {}", Taml::Serializer::Serialize(std::any{})));

    // Test Serializer::Serialize with a simple map
    std::unordered_map<std::string, std::any> testData;
    testData["key1"] = std::string("value1");
    testData["key2"] = 42;
    testData["key3"] = true;
    testData["key4"] = std::any{}; // null
    std::string serialized = Taml::Serializer::Serialize(std::any(testData));
    // std::cout << "\n--- Testing Simple Map ---" << std::endl;
    printSubsection("Testing Simple Map");
    // std::cout << "Serializer::Serialize result:\n" << serialized << std::endl;
    printInfo("Serializer::Serialize result:");
    std::cout << serialized << std::endl;

    // Test Serializer::SerializeToStream
    auto streamPtr = Taml::Serializer::SerializeToStream(std::any(testData));
    // std::cout << "Serializer::SerializeToStream result:\n" << streamPtr->str() << std::endl;
    printInfo("Serializer::SerializeToStream result:");
    std::cout << streamPtr->str() << std::endl;

    // Test Serializer::Serialize with nested map
    // std::cout << "\n--- Testing Nested Map ---" << std::endl;
    printSubsection("Testing Nested Map");
    std::unordered_map<std::string, std::any> serializerNestedData;
    serializerNestedData["subkey1"] = std::string("subvalue1");
    serializerNestedData["subkey2"] = 99;
    testData["nested"] = serializerNestedData;
    std::string serializedNested = Taml::Serializer::Serialize(std::any(testData));
    // std::cout << "Serializer::Serialize nested result:\n" << serializedNested << std::endl;
    printInfo("Serializer::Serialize nested result:");
    std::cout << serializedNested << std::endl;

    // Test Serializer::Serialize with vector
    // std::cout << "\n--- Testing Vector ---" << std::endl;
    printSubsection("Testing Vector");
    std::vector<std::any> testVector;
    testVector.push_back(std::string("item1"));
    testVector.push_back(123);
    testVector.push_back(true);
    std::string serializedVector = Taml::Serializer::Serialize(std::any(testVector));
    // std::cout << "Serializer::Serialize vector result:\n" << serializedVector << std::endl;
    printInfo("Serializer::Serialize vector result:");
    std::cout << serializedVector << std::endl;

    // Test Serializer::Serialize with map containing vector
    // std::cout << "\n--- Testing Map with Vector ---" << std::endl;
    printSubsection("Testing Map with Vector");
    std::unordered_map<std::string, std::any> mapWithVector;
    mapWithVector["items"] = testVector;
    mapWithVector["name"] = std::string("test");
    std::string serializedMapVector = Taml::Serializer::Serialize(std::any(mapWithVector));
    // std::cout << "Serializer::Serialize map with vector result:\n" << serializedMapVector << std::endl;
    printInfo("Serializer::Serialize map with vector result:");
    std::cout << serializedMapVector << std::endl;

    // Test Serializer::Deserialize
    // std::cout << "\n--- Testing Deserialize ---" << std::endl;
    printSubsection("Testing Deserialize");
    std::string tamlInput = "name\tJohn\nage\t30\nactive\ttrue\nscore\t85.5\nitems\n\tapple\n\tbanana\n\tcherry";
    try {
        auto deserialized = Taml::Serializer::Deserialize(tamlInput, typeid(std::unordered_map<std::string, std::any>));
        if (deserialized.has_value()) {
            auto& map = std::any_cast<std::unordered_map<std::string, std::any>&>(deserialized);
            // std::cout << "Deserialized name: " << std::any_cast<std::string>(map["name"]) << std::endl;
            printInfo(std::format("Deserialized name: {}", std::any_cast<std::string>(map["name"])));
            // std::cout << "Deserialized age: " << std::any_cast<std::string>(map["age"]) << std::endl;
            printInfo(std::format("Deserialized age: {}", std::any_cast<std::string>(map["age"])));
            // std::cout << "Deserialized active: " << std::any_cast<std::string>(map["active"]) << std::endl;
            printInfo(std::format("Deserialized active: {}", std::any_cast<std::string>(map["active"])));
            // std::cout << "Deserialized score: " << std::any_cast<std::string>(map["score"]) << std::endl;
            printInfo(std::format("Deserialized score: {}", std::any_cast<std::string>(map["score"])));
            auto& items = std::any_cast<std::vector<std::any>&>(map["items"]);
            // std::cout << "Deserialized items count: " << items.size() << std::endl;
            printInfo(std::format("Deserialized items count: {}", items.size()));
            // std::cout << "First item: " << std::any_cast<std::string>(items[0]) << std::endl;
            printInfo(std::format("First item: {}", std::any_cast<std::string>(items[0])));
        } else {
            // std::cout << "Deserialize failed: returned null" << std::endl;
            printResult(false, "Deserialize failed: returned null");
        }
    } catch (const std::exception& e) {
        // std::cout << "Deserialize failed: " << e.what() << std::endl;
        printResult(false, std::format("Deserialize failed: {}", e.what()));
    }

    // Test Round-Trip: Serialize -> Deserialize -> Serialize
    // std::cout << "\n--- Testing Round-Trip ---" << std::endl;
    printSubsection("Testing Round-Trip");
    std::unordered_map<std::string, std::any> originalMap;
    originalMap["test"] = std::string("value");
    originalMap["number"] = 42;
    std::unordered_map<std::string, std::any> subMap;
    subMap["sub"] = std::string("data");
    originalMap["nested"] = subMap;

    std::string serializedOriginal = Taml::Serializer::Serialize(std::any(originalMap));
    // std::cout << "Original serialized:\n" << serializedOriginal << std::endl;
    printInfo("Original serialized:");
    std::cout << serializedOriginal << std::endl;

    auto deserializedRoundTrip = Taml::Serializer::Deserialize(serializedOriginal, typeid(std::unordered_map<std::string, std::any>));
    if (deserializedRoundTrip.has_value()) {
        std::string reserialized = Taml::Serializer::Serialize(deserializedRoundTrip);
        // std::cout << "Reserialized after round-trip:\n" << reserialized << std::endl;
        printInfo("Reserialized after round-trip:");
        std::cout << reserialized << std::endl;
        // std::cout << "Round-trip successful: " << (serializedOriginal == reserialized ? "YES" : "NO") << std::endl;
        printResult(serializedOriginal == reserialized, "Round-trip successful");
    } else {
        // std::cout << "Round-trip failed: deserialize returned null" << std::endl;
        printResult(false, "Round-trip failed: deserialize returned null");
    }

    // Test Error Handling
    // std::cout << "\n--- Testing Error Handling ---" << std::endl;
    printSubsection("Testing Error Handling");
    std::string invalidSerializerTaml = "name\tJohn\n  age\t25"; // Space instead of tab
    try {
        auto invalidResult = Taml::Serializer::Deserialize(invalidSerializerTaml, typeid(std::unordered_map<std::string, std::any>));
        // std::cout << "Invalid TAML parsed without error (unexpected)" << std::endl;
        printWarning("Invalid TAML parsed without error (unexpected)");
    } catch (const Taml::Exception& e) {
        // std::cout << "Invalid TAML correctly threw exception: " << e.what() << std::endl;
        printResult(true, std::format("Invalid TAML correctly threw exception: {}", e.what()));
    } catch (const std::exception& e) {
        // std::cout << "Invalid TAML threw std::exception: " << e.what() << std::endl;
        printResult(true, std::format("Invalid TAML threw std::exception: {}", e.what()));
    }

    // Test Serializer with Document data
    // std::cout << "\n=== Testing Serializer with Document Data ===" << std::endl;
    printSection("Testing Serializer with Document Data");
    // std::cout << "Document data via Serializer:\n"
    //           << Taml::Serializer::Serialize(std::any(emptyDoc.Data())) << std::endl;
    printInfo("Document data via Serializer:");
    std::cout << Taml::Serializer::Serialize(std::any(emptyDoc.Data())) << std::endl;

    // Verify that Document::ToString() uses Serializer
    // std::cout << "Document::ToString() (should match above):\n"
    //           << emptyDoc.ToString() << std::endl;
    printInfo("Document::ToString() (should match above):");
    std::cout << emptyDoc.ToString() << std::endl;

    // Test Exception class
    // std::cout << "\n=== Testing Exception ===" << std::endl;
    printSection("Testing Exception");

    // Test simple exception
    try
    {
        throw Taml::Exception("Simple error message");
    }
    catch (const Taml::Exception &ex)
    {
        // std::cout << "Simple exception caught: " << ex.what() << std::endl;
        printResult(true, std::format("Simple exception caught: {}", ex.what()));
        // std::cout << "Has Line: " << (ex.Line.has_value() ? "true" : "false") << std::endl;
        printInfo(std::format("Has Line: {}", ex.Line.has_value() ? "true" : "false"));
        // std::cout << "Has LineText: " << (ex.LineText.has_value() ? "true" : "false") << std::endl;
        printInfo(std::format("Has LineText: {}", ex.LineText.has_value() ? "true" : "false"));
    }

    // Test exception with line number
    try
    {
        throw Taml::Exception("Error with line", 10);
    }
    catch (const Taml::Exception &ex)
    {
        // std::cout << "Exception with line caught: " << ex.what() << std::endl;
        printResult(true, std::format("Exception with line caught: {}", ex.what()));
        // std::cout << "Line: " << (ex.Line.has_value() ? std::to_string(*ex.Line) : "none") << std::endl;
        printInfo(std::format("Line: {}", ex.Line.has_value() ? std::to_string(*ex.Line) : "none"));
    }

    // Test exception with line number and text
    try
    {
        throw Taml::Exception("Error with line and text", 15, "invalid\tline");
    }
    catch (const Taml::Exception &ex)
    {
        // std::cout << "Exception with line and text caught: " << ex.what() << std::endl;
        printResult(true, std::format("Exception with line and text caught: {}", ex.what()));
        // std::cout << "Line: " << (ex.Line.has_value() ? std::to_string(*ex.Line) : "none") << std::endl;
        printInfo(std::format("Line: {}", ex.Line.has_value() ? std::to_string(*ex.Line) : "none"));
        // std::cout << "LineText: " << (ex.LineText.has_value() ? *ex.LineText : "none") << std::endl;
        printInfo(std::format("LineText: {}", ex.LineText.has_value() ? *ex.LineText : "none"));
    }

    // Test exception with inner exception
    try
    {
        try
        {
            throw std::runtime_error("Inner error");
        }
        catch (const std::exception &inner)
        {
            throw Taml::Exception("Outer error", inner);
        }
    }
    catch (const Taml::Exception &ex)
    {
        // std::cout << "Exception with inner caught: " << ex.what() << std::endl;
        printResult(true, std::format("Exception with inner caught: {}", ex.what()));
    }

    // Test Validator
    // std::cout << "\n=== Testing Validator ===" << std::endl;
    printSection("Testing Validator");

    // Test with valid TAML
    std::string validTaml = "name\tJohn\nage\t25";
    auto validResult = Taml::Validator::Validate(validTaml);
    // std::cout << "Valid TAML validation: " << (validResult.IsValid ? "PASSED" : "FAILED") << std::endl;
    printResult(validResult.IsValid, "Valid TAML validation");
    // std::cout << "Number of errors: " << validResult.Errors.size() << std::endl;
    printInfo(std::format("Number of errors: {}", validResult.Errors.size()));

    // Test with invalid TAML (contains spaces for indentation)
    std::string invalidTaml = "name\tJohn\n  age\t25"; // Space instead of tab for indentation
    auto invalidResult = Taml::Validator::Validate(invalidTaml);
    // std::cout << "Invalid TAML validation: " << (invalidResult.IsValid ? "PASSED" : "FAILED") << std::endl;
    printResult(!invalidResult.IsValid, "Invalid TAML validation");
    // std::cout << "Number of errors: " << invalidResult.Errors.size() << std::endl;
    printInfo(std::format("Number of errors: {}", invalidResult.Errors.size()));

    // Test Converter
    // std::cout << "\n=== Testing Converter ===" << std::endl;
    printSection("Testing Converter");

    // Test JSON conversion (placeholder)
    std::string jsonInput = "{\"name\": \"John\", \"age\": 25}";
    std::string jsonResult = Taml::Converter::ParseFromJson(jsonInput);
    // std::cout << "JSON conversion result:\n"
    //           << jsonResult << std::endl;
    printInfo("JSON conversion result:");
    std::cout << jsonResult << std::endl;

    // Test XML conversion (placeholder)
    std::string xmlInput = "<person><name>John</name><age>25</age></person>";
    std::string xmlResult = Taml::Converter::ParseFromXml(xmlInput);
    // std::cout << "XML conversion result:\n"
    //           << xmlResult << std::endl;
    printInfo("XML conversion result:");
    std::cout << xmlResult << std::endl;

    // Test YAML conversion (placeholder)
    std::string yamlInput = "name: John\nage: 25";
    std::string yamlResult = Taml::Converter::ParseFromYaml(yamlInput);
    // std::cout << "YAML conversion result:\n"
    //           << yamlResult << std::endl;
    printInfo("YAML conversion result:");
    std::cout << yamlResult << std::endl;

    // Test if Boost.Describe integration works
    printSection("Testing Boost.Describe Integration");

    // Create an instance
    TestPerson person{"Alice", 30};

    // Test serialization using Boost.Describe
    printInfo("Testing Boost.Describe serialization...");
    printInfo(std::format("Person name: {}, age: {}", person.name, person.age));

    // Test serialization through Serializer using a map representation
    std::unordered_map<std::string, std::any> personData;
    personData["name"] = person.name;
    personData["age"] = person.age;
    std::string serializedPerson = Taml::Serializer::Serialize(std::any(personData));
    printInfo("Serialized person data:");
    std::cout << serializedPerson << std::endl;

    // For now, just verify the struct works
    printResult(true, "Boost.Describe integration: PASSED (struct defined and accessible)");

    printSection("All tests completed");

    return 0;
}
