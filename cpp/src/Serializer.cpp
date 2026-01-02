#include "Taml/Serializer.h"
#include "Taml/Exception.h"

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
        // TODO: Implement complex object serialization
        // In C++, reflection is not available like in C#, so this would need
        // custom serialization logic or external libraries for reflection
        // For now, output a placeholder message
        WriteIndent(sb, indentLevel);
        sb << "# TODO: SerializeComplexObject not implemented - requires reflection or custom logic" << NewLine;
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
            return std::any_cast<std::string>(value);
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
}
