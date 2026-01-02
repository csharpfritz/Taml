#include <iostream>
#include "Taml.h"

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

    // Note: Parse and Serialize are placeholders and will throw exceptions
    std::cout << "\n=== Note on Parse/Serialize ===" << std::endl;
    std::cout << "Parse and Serialize are not implemented yet and will throw exceptions if called." << std::endl;

    std::cout << "\nAll tests completed." << std::endl;
        
    return 0;
}
