#include <iostream>
#include "Taml.h"
#include "Taml/Exception.h"
#include "Taml/Validator.h"

#ifndef TAML_VERSION_MAJOR
#error "TAML_VERSION_MAJOR not defined"
#endif

int main() {
    static_assert(TAML_VERSION_MAJOR == 1, "Unexpected major version");
    std::cout << "TAML version: " << Taml::version() << std::endl;

    // Test Konstruktoren
    std::cout << "\n=== Testing Constructors ===" << std::endl;
    Taml::Document emptyDoc;  // Default constructor
    std::cout << "Empty document created." << std::endl;

    std::unordered_map<std::string, std::any> initialData;
    initialData["initial"] = std::string("value");
    Taml::Document dataDoc(initialData);  // Constructor with data
    std::cout << "Document with initial data created." << std::endl;

    // Test Data() getter
    std::cout << "\n=== Testing Data() ===" << std::endl;
    const auto& data = emptyDoc.Data();
    std::cout << "Data map size: " << data.size() << std::endl;

    // Test operator[]
    std::cout << "\n=== Testing operator[] ===" << std::endl;
    emptyDoc["name"] = std::string("Test");
    std::cout << "Set name via operator[]: " << std::any_cast<std::string>(emptyDoc["name"]) << std::endl;

    // Test TryGetValue
    std::cout << "\n=== Testing TryGetValue ===" << std::endl;
    std::any value;
    if (emptyDoc.TryGetValue("name", value)) {
        std::cout << "TryGetValue succeeded: " << std::any_cast<std::string>(value) << std::endl;
    } else {
        std::cout << "TryGetValue failed" << std::endl;
    }

    // Test GetValue<T>
    std::cout << "\n=== Testing GetValue<T> ===" << std::endl;
    emptyDoc["age"] = std::string("25");
    auto ageOpt = emptyDoc.GetValue<int>("age");
    if (ageOpt) {
        std::cout << "GetValue<int> succeeded: " << *ageOpt << std::endl;
    } else {
        std::cout << "GetValue<int> failed" << std::endl;
    }

    // Test SetValue
    std::cout << "\n=== Testing SetValue ===" << std::endl;
    emptyDoc.SetValue("city", std::string("Berlin"));
    std::cout << "SetValue called for city." << std::endl;

    // Test GetKeys
    std::cout << "\n=== Testing GetKeys ===" << std::endl;
    auto keys = emptyDoc.GetKeys();
    std::cout << "Keys: ";
    for (const auto& key : keys) {
        std::cout << key << " ";
    }
    std::cout << std::endl;

    // Test ContainsKey
    std::cout << "\n=== Testing ContainsKey ===" << std::endl;
    std::cout << "Contains 'name': " << (emptyDoc.ContainsKey("name") ? "true" : "false") << std::endl;
    std::cout << "Contains 'nonexistent': " << (emptyDoc.ContainsKey("nonexistent") ? "true" : "false") << std::endl;

    // Test GetSection
    std::cout << "\n=== Testing GetSection ===" << std::endl;
    std::unordered_map<std::string, std::any> nestedData;
    nestedData["subkey"] = std::string("subvalue");
    emptyDoc.SetValue("nested", nestedData);
    auto sectionOpt = emptyDoc.GetSection("nested");
    if (sectionOpt) {
        std::cout << "GetSection succeeded: " << std::any_cast<std::string>((*sectionOpt)["subkey"]) << std::endl;
    } else {
        std::cout << "GetSection failed" << std::endl;
    }

    // Test Flatten
    std::cout << "\n=== Testing Flatten ===" << std::endl;
    auto flat = emptyDoc.Flatten();
    std::cout << "Flattened keys: ";
    for (const auto& [key, val] : flat) {
        std::cout << key << " ";
    }
    std::cout << std::endl;

    // Test ToString
    std::cout << "\n=== Testing ToString ===" << std::endl;
    std::cout << "Document as string:\n" << emptyDoc.ToString() << std::endl;

    // Test LoadFromFile and SaveToFile (synchronous)
    std::cout << "\n=== Testing LoadFromFile and SaveToFile ===" << std::endl;
    try {
        emptyDoc.SaveToFile("test_output.taml");
        std::cout << "SaveToFile succeeded." << std::endl;

        // Note: LoadFromFile would require a valid TAML file and implemented Parse
        // For now, just test that SaveToFile doesn't crash
    } catch (const std::exception& e) {
        std::cout << "SaveToFile failed: " << e.what() << std::endl;
    }

    // Test async methods (LoadFromFileAsync and SaveToFileAsync)
    std::cout << "\n=== Testing Async Methods ===" << std::endl;
    try {
        auto saveFuture = emptyDoc.SaveToFileAsync("test_output_async.taml");
        saveFuture.get();  // Wait for completion
        std::cout << "SaveToFileAsync succeeded." << std::endl;

        // LoadFromFileAsync would require a valid file and implemented Parse
    } catch (const std::exception& e) {
        std::cout << "Async operation failed: " << e.what() << std::endl;
    }

    // Test Serializer directly
    std::cout << "\n=== Testing Serializer ===" << std::endl;
    
    // Test Serializer::Serialize with a simple map
    std::unordered_map<std::string, std::any> testData;
    testData["key1"] = std::string("value1");
    testData["key2"] = 42;
    std::string serialized = Taml::Serializer::Serialize(std::any(testData));
    std::cout << "Serializer::Serialize result:\n" << serialized << std::endl;
    
    // Test Serializer::SerializeToStream
    auto streamPtr = Taml::Serializer::SerializeToStream(std::any(testData));
    std::cout << "Serializer::SerializeToStream result:\n" << streamPtr->str() << std::endl;
    
    // Test Serializer with Document data
    std::cout << "\n=== Testing Serializer with Document Data ===" << std::endl;
    std::cout << "Document data via Serializer:\n" << Taml::Serializer::Serialize(std::any(emptyDoc.Data())) << std::endl;
    
    // Verify that Document::ToString() uses Serializer
    std::cout << "Document::ToString() (should match above):\n" << emptyDoc.ToString() << std::endl;

    // Test Exception class
    std::cout << "\n=== Testing Exception ===" << std::endl;
    
    // Test simple exception
    try {
        throw Taml::Exception("Simple error message");
    } catch (const Taml::Exception& ex) {
        std::cout << "Simple exception caught: " << ex.what() << std::endl;
        std::cout << "Has Line: " << (ex.Line.has_value() ? "true" : "false") << std::endl;
        std::cout << "Has LineText: " << (ex.LineText.has_value() ? "true" : "false") << std::endl;
    }
    
    // Test exception with line number
    try {
        throw Taml::Exception("Error with line", 10);
    } catch (const Taml::Exception& ex) {
        std::cout << "Exception with line caught: " << ex.what() << std::endl;
        std::cout << "Line: " << (ex.Line.has_value() ? std::to_string(*ex.Line) : "none") << std::endl;
    }
    
    // Test exception with line number and text
    try {
        throw Taml::Exception("Error with line and text", 15, "invalid\tline");
    } catch (const Taml::Exception& ex) {
        std::cout << "Exception with line and text caught: " << ex.what() << std::endl;
        std::cout << "Line: " << (ex.Line.has_value() ? std::to_string(*ex.Line) : "none") << std::endl;
        std::cout << "LineText: " << (ex.LineText.has_value() ? *ex.LineText : "none") << std::endl;
    }
    
    // Test exception with inner exception
    try {
        try {
            throw std::runtime_error("Inner error");
        } catch (const std::exception& inner) {
            throw Taml::Exception("Outer error", inner);
        }
    } catch (const Taml::Exception& ex) {
        std::cout << "Exception with inner caught: " << ex.what() << std::endl;
    }

    // Test Validator
    std::cout << "\n=== Testing Validator ===" << std::endl;
    
    // Test with valid TAML
    std::string validTaml = "name\tJohn\nage\t25";
    auto validResult = Taml::Validator::Validate(validTaml);
    std::cout << "Valid TAML validation: " << (validResult.IsValid ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Number of errors: " << validResult.Errors.size() << std::endl;
    
    // Test with invalid TAML (contains spaces for indentation)
    std::string invalidTaml = "name\tJohn\n  age\t25";  // Space instead of tab for indentation
    auto invalidResult = Taml::Validator::Validate(invalidTaml);
    std::cout << "Invalid TAML validation: " << (invalidResult.IsValid ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Number of errors: " << invalidResult.Errors.size() << std::endl;

    // Test Converter
    std::cout << "\n=== Testing Converter ===" << std::endl;
    
    // Test JSON conversion (placeholder)
    std::string jsonInput = "{\"name\": \"John\", \"age\": 25}";
    std::string jsonResult = Taml::Converter::ParseFromJson(jsonInput);
    std::cout << "JSON conversion result:\n" << jsonResult << std::endl;
    
    // Test XML conversion (placeholder)
    std::string xmlInput = "<person><name>John</name><age>25</age></person>";
    std::string xmlResult = Taml::Converter::ParseFromXml(xmlInput);
    std::cout << "XML conversion result:\n" << xmlResult << std::endl;
    
    // Test YAML conversion (placeholder)
    std::string yamlInput = "name: John\nage: 25";
    std::string yamlResult = Taml::Converter::ParseFromYaml(yamlInput);
    std::cout << "YAML conversion result:\n" << yamlResult << std::endl;

    std::cout << "\nAll tests completed." << std::endl;
        
    return 0;
}
