#pragma once

#include <string>
#include <any>
#include <sstream>
#include <ostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <list>
#include <istream>
#include <algorithm>
#include <cctype>
#include <typeinfo>
#include <optional>

namespace Taml
{
    /// <summary>
    /// Interface for TAML serializers responsible for serializing and deserializing TAML documents.
    /// </summary>
    class Serializer
    {
    public:
        /// <summary>
        /// Serializes an object to TAML format and returns a string
        /// </summary>
        static std::string Serialize(const std::any& obj);

        /// <summary>
        /// Serializes an object to TAML format and writes to a stream
        /// </summary>
        static void Serialize(const std::any& obj, std::ostream& stream);

        /// <summary>
        /// Serializes an object to TAML format and returns a MemoryStream
        /// </summary>
        static std::unique_ptr<std::stringstream> SerializeToStream(const std::any& obj);

        /// <summary>
        /// Deserializes a TAML string to the specified type
        /// </summary>
        template<typename T>
        static T Deserialize(const std::string& taml) {
            if (taml.empty() || std::all_of(taml.begin(), taml.end(), [](char c) { return std::isspace(c); })) {
                return T{};
            }
            
            if (taml == "null") {
                return T{};
            }
            
            auto lines = ParseLines(taml);
            size_t nextIndex;
            auto result = DeserializeFromLines(typeid(T), lines, 0, nextIndex);
            if (result.has_value() && result.type() == typeid(T)) {
                return std::any_cast<T>(result);
            }
            return T{};
        }

        /// <summary>
        /// Deserializes a TAML string to the specified type
        /// </summary>
        static std::any Deserialize(const std::string& taml, const std::type_info& targetType);

        /// <summary>
        /// Deserializes a TAML stream to the specified type
        /// </summary>
        template<typename T>
        static T Deserialize(std::istream& stream) {
            std::string taml((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
            return Deserialize<T>(taml);
        }

        /// <summary>
        /// Deserializes a TAML stream to the specified type
        /// </summary>
        static std::any Deserialize(std::istream& stream, const std::type_info& targetType);

    private:
        static constexpr char Tab = '\t';
        static constexpr char NewLine = '\n';

        struct TamlLine
        {
            int IndentLevel;
            std::string Key;
            std::optional<std::string> Value;
            bool HasValue;
        };

        static void SerializeObject(const std::any& obj, std::stringstream& sb, int depth);
        static void SerializeComplexObject(const std::any& obj, std::stringstream& sb, int indentLevel);
        static void SerializeMember(const std::string& name, const std::any& value, std::stringstream& sb, int indentLevel);
        static void WriteIndent(std::stringstream& sb, int indentLevel);
        static bool IsPrimitiveType(const std::type_info& type);
        static std::string FormatValue(const std::any& value);
        static void SerializeDictionary(const std::unordered_map<std::string, std::any>& dict, std::stringstream& sb, int indentLevel);
        static void SerializeCollection(const std::vector<std::any>& collection, std::stringstream& sb, int indentLevel);

        static std::vector<TamlLine> ParseLines(const std::string& taml);
        static std::any DeserializeFromLines(const std::type_info& targetType, const std::vector<TamlLine>& lines, size_t startIndex, size_t& nextIndex);
        static std::any DeserializeComplexObject(const std::type_info& targetType, const std::vector<TamlLine>& lines, size_t startIndex, size_t& nextIndex);
        static std::any DeserializeCollection(const std::type_info& targetType, const std::vector<TamlLine>& lines, size_t startIndex, size_t& nextIndex);
        static std::any DeserializeDictionary(const std::type_info& targetType, const std::vector<TamlLine>& lines, size_t startIndex, size_t& nextIndex);
        static std::any ConvertValue(const std::string& value, const std::type_info& targetType);
        static bool IsCollectionType(const std::type_info& type, const std::type_info*& elementType);
        static bool IsDictionaryType(const std::type_info& type, const std::type_info*& keyType, const std::type_info*& valueType);
    };
}