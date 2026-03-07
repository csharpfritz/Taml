#include "Taml/Exception.h"

namespace Taml
{
    Exception::Exception(const std::string& message)
        : std::runtime_error(message)
    {
    }
    
    Exception::Exception(const std::string& message, int line)
        : std::runtime_error(FormatMessage(message, line, nullptr))
    {
        Line = line;
    }
    
    Exception::Exception(const std::string& message, int line, const std::string& lineText)
        : std::runtime_error(FormatMessage(message, line, &lineText))
    {
        Line = line;
        LineText = lineText;
    }
    
    Exception::Exception(const std::string& message, const std::exception& innerException)
        : std::runtime_error(message + " (Inner exception: " + innerException.what() + ")")
    {
    }
    
    std::string Exception::FormatMessage(const std::string& message, int line, const std::string* lineText)
    {
        if (lineText != nullptr)
        {
            // Truncate long lines for readability
            std::string displayText = lineText->length() > 50 ? lineText->substr(0, 50) + "..." : *lineText;
            return "Line " + std::to_string(line) + ": " + message + "\n  " + displayText;
        }
        
        return "Line " + std::to_string(line) + ": " + message;
    }
}
