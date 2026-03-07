#pragma once

#include <string>
#include <vector>
#include <optional>

namespace Taml
{
    /// <summary>
    /// Represents a validation error with line information
    /// </summary>
    struct ValidationError
    {
        std::string Message;
        int Line;
        std::optional<std::string> LineText;
    };

    /// <summary>
    /// Represents the result of TAML validation
    /// </summary>
    struct ValidationResult
    {
        bool IsValid;
        std::vector<ValidationError> Errors;
    };

    /// <summary>
    /// Information about a parsed line
    /// </summary>
    struct LineInfo
    {
        int IndentLevel;
        bool IsParent;
    };

    /// <summary>
    /// Validates TAML documents and provides detailed error reporting
    /// </summary>
    class Validator
    {
    public:
        /// <summary>
        /// Validates a TAML string and returns any validation errors
        /// </summary>
        static ValidationResult Validate(const std::string& taml);

    private:
        static LineInfo ValidateLine(const std::string& line, int lineNumber, const LineInfo& previousLineInfo, std::vector<ValidationError>& errors);
    };
}