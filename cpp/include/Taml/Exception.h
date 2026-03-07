#pragma once

#include <string>
#include <optional>
#include <stdexcept>

namespace Taml
{
    /// <summary>
    /// Represents a TAML parsing or validation error
    /// </summary>
    class Exception : public std::runtime_error
    {
    public:
        /// <summary>
        /// Gets the line number where the error occurred (1-based, null if not applicable)
        /// </summary>
        std::optional<int> Line;
        
        /// <summary>
        /// Gets the line text where the error occurred (null if not available)
        /// </summary>
        std::optional<std::string> LineText;
        
        /// <summary>
        /// Creates a new TAMLException with the specified message
        /// </summary>
        Exception(const std::string& message);
        
        /// <summary>
        /// Creates a new TAMLException with the specified message and line number
        /// </summary>
        Exception(const std::string& message, int line);
        
        /// <summary>
        /// Creates a new TAMLException with the specified message, line number, and line text
        /// </summary>
        Exception(const std::string& message, int line, const std::string& lineText);
        
        /// <summary>
        /// Creates a new TAMLException with the specified message and inner exception
        /// </summary>
        Exception(const std::string& message, const std::exception& innerException);
        
    private:
        static std::string FormatMessage(const std::string& message, int line, const std::string* lineText);
    };
}
