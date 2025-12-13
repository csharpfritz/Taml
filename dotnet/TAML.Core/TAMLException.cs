namespace TAML.Core;

/// <summary>
/// Exception thrown when TAML parsing or serialization fails
/// </summary>
public class TAMLException : Exception
{
    /// <summary>
    /// Gets the line number where the error occurred (1-based, null if not applicable)
    /// </summary>
    public int? Line { get; }
    
    /// <summary>
    /// Gets the line text where the error occurred (null if not available)
    /// </summary>
    public string? LineText { get; }
    
    /// <summary>
    /// Creates a new TAMLException with the specified message
    /// </summary>
    public TAMLException(string message) : base(message)
    {
    }
    
    /// <summary>
    /// Creates a new TAMLException with the specified message and line number
    /// </summary>
    public TAMLException(string message, int line) : base(FormatMessage(message, line, null))
    {
        Line = line;
    }
    
    /// <summary>
    /// Creates a new TAMLException with the specified message, line number, and line text
    /// </summary>
    public TAMLException(string message, int line, string lineText) : base(FormatMessage(message, line, lineText))
    {
        Line = line;
        LineText = lineText;
    }
    
    /// <summary>
    /// Creates a new TAMLException with the specified message and inner exception
    /// </summary>
    public TAMLException(string message, Exception innerException) : base(message, innerException)
    {
    }
    
    private static string FormatMessage(string message, int line, string? lineText)
    {
        if (lineText != null)
        {
            // Truncate long lines for readability
            var displayText = lineText.Length > 50 ? lineText.Substring(0, 50) + "..." : lineText;
            return $"Line {line}: {message}\n  {displayText}";
        }
        
        return $"Line {line}: {message}";
    }
}
