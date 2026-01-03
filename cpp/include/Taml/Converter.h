#pragma once

#include <string>
#include <any>
#include <unordered_map>
#include <vector>

namespace Taml
{
    /// <summary>
    /// Provides methods to convert from various formats (JSON, XML, YAML) to TAML format
    /// </summary>
    class Converter
    {
    public:
        /// <summary>
        /// Parses JSON string and converts it to TAML format
        /// </summary>
        /// <param name="json">The JSON string to parse</param>
        /// <returns>TAML formatted string</returns>
        static std::string ParseFromJson(const std::string& json);

        /// <summary>
        /// Parses XML string and converts it to TAML format
        /// </summary>
        /// <param name="xml">The XML string to parse</param>
        /// <returns>TAML formatted string</returns>
        static std::string ParseFromXml(const std::string& xml);

        /// <summary>
        /// Parses YAML string and converts it to TAML format
        /// </summary>
        /// <param name="yaml">The YAML string to parse</param>
        /// <returns>TAML formatted string</returns>
        static std::string ParseFromYaml(const std::string& yaml);

    private:
        static std::any ConvertJsonElement(const std::any& element);
        static std::any ConvertYamlObject(const std::any& yamlObj);
        static std::any ConvertXElement(const std::any& element);
    };
}
