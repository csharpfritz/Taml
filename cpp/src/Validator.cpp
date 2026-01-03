#include "Taml/Validator.h"
#include <sstream>
#include <algorithm>

namespace Taml
{
    ValidationResult Validator::Validate(const std::string& taml)
    {
        std::vector<ValidationError> errors;
        std::istringstream stream(taml);
        std::string line;
        std::vector<std::string> lines;
        
        while (std::getline(stream, line))
        {
            // Handle both \n and \r\n by removing \r if present
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }
            lines.push_back(line);
        }
        
        LineInfo previousLineInfo = { -1, false };
        
        for (size_t i = 0; i < lines.size(); ++i)
        {
            int lineNumber = static_cast<int>(i) + 1;
            const std::string& currentLine = lines[i];
            
            // Skip blank lines and comments
            std::string trimmed = currentLine;
            trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), [](unsigned char ch) { return !std::isspace(ch); }));
            if (trimmed.empty() || trimmed.starts_with("#"))
            {
                previousLineInfo = { previousLineInfo.IndentLevel, false };
                continue;
            }
            
            LineInfo lineInfo = ValidateLine(currentLine, lineNumber, previousLineInfo, errors);
            previousLineInfo = lineInfo;
        }
        
        return ValidationResult
        {
            errors.empty(),
            errors
        };
    }

    LineInfo Validator::ValidateLine(const std::string& line, int lineNumber, const LineInfo& previousLineInfo, std::vector<ValidationError>& errors)
    {
        // Count leading tabs and spaces
        int tabCount = 0;
        int spaceCount = 0;
        size_t firstNonWhitespace = std::string::npos;
        
        for (size_t i = 0; i < line.length(); ++i)
        {
            if (line[i] == '\t')
            {
                tabCount++;
            }
            else if (line[i] == ' ')
            {
                spaceCount++;
            }
            else
            {
                firstNonWhitespace = i;
                break;
            }
        }
        
        // Check for mixed indentation (tabs and spaces)
        if (tabCount > 0 && spaceCount > 0)
        {
            errors.push_back(ValidationError{
                "Mixed tabs and spaces in indentation",
                lineNumber,
                line
            });
        }
        // Check for spaces used for indentation
        else if (spaceCount > 0 && tabCount == 0)
        {
            errors.push_back(ValidationError{
                "Indentation must use tabs, not spaces",
                lineNumber,
                line
            });
        }
        
        // Check if line has content after indentation
        if (firstNonWhitespace == std::string::npos)
        {
            // Empty line after indentation - this might be valid
            return LineInfo{ tabCount, false };
        }
        
        // Check for key-value format (contains tab separator)
        size_t tabIndex = line.find('\t', firstNonWhitespace);
        if (tabIndex != std::string::npos)
        {
            // Has tab separator - check for multiple tabs
            size_t nextTab = line.find('\t', tabIndex + 1);
            if (nextTab != std::string::npos)
            {
                errors.push_back(ValidationError{
                    "Multiple tab separators in line",
                    lineNumber,
                    line
                });
            }
            
            // Check if value contains tabs (only if there's a value)
            if (tabIndex + 1 < line.length())
            {
                std::string value = line.substr(tabIndex + 1);
                if (value.find('\t') != std::string::npos)
                {
                    errors.push_back(ValidationError{
                        "Value contains invalid tab character",
                        lineNumber,
                        line
                    });
                }
            }
            
            return LineInfo{ tabCount, false }; // Key-value pair
        }
        else
        {
            // No tab separator - this is a parent key or list item
            return LineInfo{ tabCount, true }; // Parent key
        }
    }
}
