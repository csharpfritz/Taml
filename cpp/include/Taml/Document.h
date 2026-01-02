#pragma once

#include <unordered_map>
#include <string>
#include <any>
#include <optional>
#include <vector>
#include <future>

namespace Taml
{
    /// <summary>
    /// Represents a generic TAML document that can hold any TAML structure.
    /// The document is represented as a dictionary of key-value pairs where values can be:
    /// - Primitive types (string, int, bool, etc.)
    /// - Nested TamlDocument objects (for nested sections)
    /// - Lists of values or TamlDocument objects
    /// </summary>
    class Document
    {
    public:

        /// <summary>
        /// The underlying data structure holding the document content
        /// </summary>
        const std::unordered_map<std::string, std::any>& Data() const { return _data; }

        /// <summary>
        /// Creates an empty TAML document
        /// </summary>
        Document();

        /// <summary>
        /// Creates a TAML document from existing data
        /// </summary>
        Document(const std::unordered_map<std::string, std::any>& data);

        /// <summary>
        /// Indexer to access document values by key
        /// </summary>
        std::any& operator[](const std::string& key);
        const std::any& operator[](const std::string& key) const;

        /// <summary>
        /// Tries to get a value from the document
        /// </summary>
        bool TryGetValue(const std::string& key, std::any& value) const;

        /// <summary>
        /// Gets a value as a specific type, or returns default if not found or cannot convert
        /// </summary>
        template<typename T>
        std::optional<T> GetValue(const std::string& key) const;

        /// <summary>
        /// Sets a value in the document
        /// </summary>
        void SetValue(const std::string& key, const std::any& value);

        /// <summary>
        /// Gets all keys in the document
        /// </summary>
        std::vector<std::string> GetKeys() const;

        /// <summary>
        /// Checks if the document contains a specific key
        /// </summary>
        bool ContainsKey(const std::string& key) const;

        /// <summary>
        /// Gets a nested section as a TamlDocument
        /// </summary>
        std::optional<Document> GetSection(const std::string& key) const;

        /// <summary>
        /// Flattens the document into a flat dictionary with colon-separated keys (e.g., "Section:Key")
        /// This is useful for configuration providers
        /// </summary>
        std::unordered_map<std::string, std::optional<std::string>> Flatten(const std::string& prefix = "") const;

        /// <summary>
        /// Loads a TAML document from a file
        /// </summary>
        static Document LoadFromFile(const std::string& path);

        /// <summary>
        /// Loads a TAML document from a file asynchronously
        /// </summary>
        static std::future<Document> LoadFromFileAsync(const std::string& path);

        /// <summary>
        /// Saves the document to a file
        /// </summary>
        void SaveToFile(const std::string& path) const;

        /// <summary>
        /// Saves the document to a file asynchronously
        /// </summary>
        std::future<void> SaveToFileAsync(const std::string& path) const;

        /// <summary>
        /// Parses a TAML string into a TamlDocument
        /// </summary>
        static Document Parse(const std::string& content);

        /// <summary>
        /// Serializes a TAML document data structure into a TAML string
        /// </summary>
        static std::string Serialize(const std::unordered_map<std::string, std::any>& data);

    private:
        std::unordered_map<std::string, std::any> _data;

        void FlattenInternal(const std::unordered_map<std::string, std::any>& data, const std::string& currentPrefix, std::unordered_map<std::string, std::optional<std::string>>& result) const;

    };

    // Template implementation
    template<typename T>
    std::optional<T> Document::GetValue(const std::string& key) const {
        std::any value;
        if (!TryGetValue(key, value)) return std::nullopt;

        // Try direct cast
        try {
            return std::any_cast<T>(value);
        } catch (const std::bad_any_cast&) {
            // Try conversion, e.g., string to T
            if constexpr (std::is_same_v<T, int>) {
                if (auto* str = std::any_cast<std::string>(&value)) {
                    try {
                        return std::stoi(*str);
                    } catch (...) {
                        return std::nullopt;
                    }
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (auto* str = std::any_cast<std::string>(&value)) {
                    try {
                        return std::stod(*str);
                    } catch (...) {
                        return std::nullopt;
                    }
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                if (auto* str = std::any_cast<std::string>(&value)) {
                    if (*str == "true") return true;
                    if (*str == "false") return false;
                    return std::nullopt;
                }
            }
            // Add more conversions as needed
            return std::nullopt;
        }
    }

    // Flatten implementation
    inline void Document::FlattenInternal(const std::unordered_map<std::string, std::any>& data, const std::string& currentPrefix, std::unordered_map<std::string, std::optional<std::string>>& result) const {
        for (const auto& pair : data) {
            std::string key = currentPrefix.empty() ? pair.first : currentPrefix + ":" + pair.first;
            if (auto* str = std::any_cast<std::string>(&pair.second)) {
                result[key] = *str;
            } else if (auto* map = std::any_cast<std::unordered_map<std::string, std::any>>(&pair.second)) {
                FlattenInternal(*map, key, result);
            } else {
                // For other types, convert to string or set to nullopt
                // For simplicity, set to nullopt for non-string, non-map values
                result[key] = std::nullopt;
            }
        }
    }

    inline std::unordered_map<std::string, std::optional<std::string>> Document::Flatten(const std::string& prefix) const {
        std::unordered_map<std::string, std::optional<std::string>> result;
        FlattenInternal(_data, prefix, result);
        return result;
    }
}