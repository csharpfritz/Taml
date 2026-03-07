#include "Taml/Serializer.h"
#include "Taml/Exception.h"
#include <sstream>
#include <algorithm>
#include <typeinfo>
#include <list>
#include <memory>
#include <type_traits>
#include <optional>

namespace Taml
{
    std::string Serializer::Serialize(const std::any& obj) {
        if (!obj.has_value()) {
            return "null";
        }
        
        std::stringstream sb;
        SerializeObject(obj, sb, 0);
        return sb.str();
    }

    void Serializer::Serialize(const std::any& obj, std::ostream& stream) {
        std::string tamlString = Serialize(obj);
        stream << tamlString;
        stream.flush();
    }

    std::unique_ptr<std::stringstream> Serializer::SerializeToStream(const std::any& obj) {
        auto stream = std::make_unique<std::stringstream>();
        Serialize(obj, *stream);
        return stream;
    }

    void Serializer::SerializeObject(const std::any& obj, std::stringstream& sb, int depth) {
        if (!obj.has_value()) {
            sb << "null";
            return;
        }
        
        const std::type_info& type = obj.type();
        
        if (IsPrimitiveType(type)) {
            sb << FormatValue(obj);
        } else if (type == typeid(std::unordered_map<std::string, std::any>)) {
            SerializeDictionary(std::any_cast<std::unordered_map<std::string, std::any>>(obj), sb, depth);
        } else if (type == typeid(std::vector<std::any>)) {
            SerializeCollection(std::any_cast<std::vector<std::any>>(obj), sb, depth);
        } else {
            // Complex object
            SerializeComplexObject(obj, sb, depth);
        }
    }

    void Serializer::SerializeComplexObject(const std::any& obj, std::stringstream& sb, int indentLevel) {
        // Fallback: Placeholder message for complex objects
        WriteIndent(sb, indentLevel);
        sb << "# TODO: Complex object serialization - type not supported" << NewLine;
    }

    void Serializer::SerializeMember(const std::string& name, const std::any& value, std::stringstream& sb, int indentLevel) {
        if (!value.has_value()) {
            WriteIndent(sb, indentLevel);
            sb << name << Tab << "~" << NewLine;
            return;
        }
        
        const std::type_info& type = value.type();
        
        if (IsPrimitiveType(type)) {
            WriteIndent(sb, indentLevel);
            sb << name << Tab << FormatValue(value) << NewLine;
        } else if (type == typeid(std::unordered_map<std::string, std::any>)) {
            WriteIndent(sb, indentLevel);
            sb << name << NewLine;
            SerializeDictionary(std::any_cast<std::unordered_map<std::string, std::any>>(value), sb, indentLevel + 1);
        } else if (type == typeid(std::vector<std::any>)) {
            WriteIndent(sb, indentLevel);
            sb << name << NewLine;
            SerializeCollection(std::any_cast<std::vector<std::any>>(value), sb, indentLevel + 1);
        } else {
            // Complex object
            WriteIndent(sb, indentLevel);
            sb << name << NewLine;
            SerializeComplexObject(value, sb, indentLevel + 1);
        }
    }

    void Serializer::WriteIndent(std::stringstream& sb, int indentLevel) {
        for (int i = 0; i < indentLevel; ++i) {
            sb << Tab;
        }
    }

    bool Serializer::IsPrimitiveType(const std::type_info& type) {
        return type == typeid(int) || type == typeid(double) || type == typeid(std::string) || type == typeid(bool);
    }

    std::string Serializer::FormatValue(const std::any& value) {
        if (value.type() == typeid(int)) {
            return std::to_string(std::any_cast<int>(value));
        } else if (value.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(value));
        } else if (value.type() == typeid(std::string)) {
            auto str = std::any_cast<std::string>(value);
            if (str.empty()) return "\"\"";
            return str;
        } else if (value.type() == typeid(bool)) {
            return std::any_cast<bool>(value) ? "true" : "false";
        }
        return "unknown";
    }

    void Serializer::SerializeDictionary(const std::unordered_map<std::string, std::any>& dict, std::stringstream& sb, int indentLevel) {
        for (const auto& pair : dict) {
            SerializeMember(pair.first, pair.second, sb, indentLevel);
        }
    }

    void Serializer::SerializeCollection(const std::vector<std::any>& collection, std::stringstream& sb, int indentLevel) {
        for (const auto& item : collection) {
            WriteIndent(sb, indentLevel);
            if (item.type() == typeid(std::string)) {
                sb << std::any_cast<std::string>(item) << NewLine;
            } else {
                sb << FormatValue(item) << NewLine;
            }
        }
    }

    std::any Serializer::Deserialize(const std::string& taml, const std::type_info& targetType) {
        if (taml.empty() || std::all_of(taml.begin(), taml.end(), [](char c) { return std::isspace(c); })) {
            return std::any{};
        }
        
        if (taml == "null") {
            return std::any{};
        }
        
        auto lines = ParseLines(taml);
        size_t nextIndex;
        return DeserializeFromLines(targetType, lines, 0, nextIndex);
    }

    std::any Serializer::Deserialize(std::istream& stream, const std::type_info& targetType) {
        std::string taml((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        return Deserialize(taml, targetType);
    }

    std::vector<Serializer::TamlLine> Serializer::ParseLines(const std::string& taml) {
        std::vector<TamlLine> lines;
        std::istringstream stream(taml);
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            
            // Handle \r\n
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            
            // Skip comments
            if (!line.empty() && line[0] == '#') {
                continue;
            }
            
            // Check for space indentation
            if (!line.empty() && line[0] == ' ') {
                throw Taml::Exception("Indentation must use tabs, not spaces", lineNumber, line);
            }
            
            // Count leading tabs
            int indentLevel = 0;
            size_t i = 0;
            for (; i < line.length(); ++i) {
                if (line[i] == Tab) {
                    indentLevel++;
                } else if (line[i] == ' ') {
                    throw Taml::Exception("Mixed spaces and tabs in indentation", lineNumber, line);
                } else {
                    break;
                }
            }
            
            std::string content = line.substr(i);
            
            if (content.empty()) {
                continue;
            }
            
            // Check for key-value
            size_t tabIndex = content.find(Tab);
            if (tabIndex != std::string::npos) {
                std::string key = content.substr(0, tabIndex);
                size_t valueStart = tabIndex;
                while (valueStart < content.length() && content[valueStart] == Tab) {
                    valueStart++;
                }
                std::optional<std::string> value = valueStart < content.length() ? std::optional<std::string>(content.substr(valueStart)) : std::nullopt;
                
                if (value && value->find(Tab) != std::string::npos) {
                    throw Taml::Exception("Value contains invalid tab character", lineNumber, line);
                }
                
                lines.push_back({indentLevel, key, value, true});
            } else {
                lines.push_back({indentLevel, content, std::nullopt, false});
            }
        }
        
        return lines;
    }

    std::any Serializer::DeserializeFromLines(const std::type_info& targetType, const std::vector<TamlLine>& lines, size_t startIndex, size_t& nextIndex) {
        if (startIndex >= lines.size()) {
            nextIndex = startIndex;
            return std::any{};
        }
        
        const auto& firstLine = lines[startIndex];
        
        if (IsPrimitiveType(targetType)) {
            std::string valueStr = firstLine.HasValue && firstLine.Value ? *firstLine.Value : firstLine.Key;
            nextIndex = startIndex + 1;
            return ConvertValue(valueStr, targetType);
        }
        
        const std::type_info* keyType = nullptr;
        const std::type_info* valueType = nullptr;
        if (IsDictionaryType(targetType, keyType, valueType)) {
            return DeserializeDictionary(targetType, lines, startIndex, nextIndex);
        }
        
        const std::type_info* elementType = nullptr;
        if (IsCollectionType(targetType, elementType)) {
            return DeserializeCollection(targetType, lines, startIndex, nextIndex);
        }
        
        return DeserializeComplexObject(targetType, lines, startIndex, nextIndex);
    }

    std::any Serializer::DeserializeComplexObject(const std::type_info& targetType, const std::vector<TamlLine>& lines, size_t startIndex, size_t& nextIndex) {
        // Fallback: Complex object deserialization not implemented
        nextIndex = startIndex + 1;
        return std::any{};
    }

    std::any Serializer::DeserializeCollection(const std::type_info& targetType, const std::vector<TamlLine>& lines, size_t startIndex, size_t& nextIndex) {
        std::vector<std::any> list;
        int currentIndent = lines[startIndex].IndentLevel;
        nextIndex = startIndex;
        
        while (nextIndex < lines.size()) {
            const auto& line = lines[nextIndex];
            if (line.IndentLevel < currentIndent) break;
            if (line.IndentLevel == currentIndent) {
                std::string valueStr = line.HasValue && line.Value ? *line.Value : line.Key;
                list.push_back(ConvertValue(valueStr, typeid(std::string))); // Simplified
                nextIndex++;
            } else {
                nextIndex++;
            }
        }
        
        return list;
    }

    std::any Serializer::DeserializeDictionary(const std::type_info& targetType, const std::vector<TamlLine>& lines, size_t startIndex, size_t& nextIndex) {
        std::unordered_map<std::string, std::any> dict;
        int currentIndent = lines[startIndex].IndentLevel;
        nextIndex = startIndex;
        
        while (nextIndex < lines.size()) {
            const auto& line = lines[nextIndex];
            if (line.IndentLevel < currentIndent) break;
            if (line.IndentLevel == currentIndent) {
                if (line.HasValue) {
                    dict[line.Key] = ConvertValue(*line.Value, typeid(std::string));
                    nextIndex++;
                } else {
                    // Nested
                    nextIndex++;
                    size_t tempIndex;
                    auto nested = DeserializeFromLines(typeid(std::unordered_map<std::string, std::any>), lines, nextIndex, tempIndex);
                    dict[line.Key] = nested;
                    nextIndex = tempIndex;
                }
            } else {
                nextIndex++;
            }
        }
        
        return dict;
    }

    std::any Serializer::ConvertValue(const std::string& value, const std::type_info& targetType) {
        if (value == "null" || value == "~") {
            return std::any{};
        }
        
        if (targetType == typeid(std::string)) {
            if (value == "\"\"") return std::string("");
            return value;
        }
        
        if (targetType == typeid(int)) {
            return std::stoi(value);
        }
        
        if (targetType == typeid(double)) {
            return std::stod(value);
        }
        
        if (targetType == typeid(bool)) {
            return value == "true";
        }
        
        return std::any{};
    }

    bool Serializer::IsCollectionType(const std::type_info& type, const std::type_info*& elementType) {
        if (type == typeid(std::vector<std::any>)) {
            static const std::type_info& anyType = typeid(std::any);
            elementType = &anyType;
            return true;
        }
        elementType = nullptr;
        return false;
    }

    bool Serializer::IsDictionaryType(const std::type_info& type, const std::type_info*& keyType, const std::type_info*& valueType) {
        if (type == typeid(std::unordered_map<std::string, std::any>)) {
            static const std::type_info& stringType = typeid(std::string);
            static const std::type_info& anyType = typeid(std::any);
            keyType = &stringType;
            valueType = &anyType;
            return true;
        }
        keyType = nullptr;
        valueType = nullptr;
        return false;
    }
}
