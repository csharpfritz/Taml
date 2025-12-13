using TAML.Core;
using Xunit;

namespace TAML.Tests;

public class TAMLExceptionTests
{
    [Fact]
    public void TAMLException_WithMessageOnly_CreatesException()
    {
        var ex = new TAMLException("Test error");
        
        Assert.Equal("Test error", ex.Message);
        Assert.Null(ex.Line);
        Assert.Null(ex.LineText);
    }
    
    [Fact]
    public void TAMLException_WithLineNumber_IncludesLineInMessage()
    {
        var ex = new TAMLException("Invalid indentation", 5);
        
        Assert.Equal("Line 5: Invalid indentation", ex.Message);
        Assert.Equal(5, ex.Line);
        Assert.Null(ex.LineText);
    }
    
    [Fact]
    public void TAMLException_WithLineNumberAndText_IncludesContextInMessage()
    {
        var ex = new TAMLException("Invalid indentation", 5, "  server\tlocalhost");
        
        Assert.Contains("Line 5: Invalid indentation", ex.Message);
        Assert.Contains("  server\tlocalhost", ex.Message);
        Assert.Equal(5, ex.Line);
        Assert.Equal("  server\tlocalhost", ex.LineText);
    }
    
    [Fact]
    public void TAMLException_WithLongLineText_TruncatesInMessage()
    {
        var longLine = new string('x', 100);
        var ex = new TAMLException("Test error", 1, longLine);
        
        Assert.Contains("Line 1: Test error", ex.Message);
        Assert.Contains("...", ex.Message);
        Assert.Equal(longLine, ex.LineText); // Full text stored in property
        Assert.DoesNotContain(longLine, ex.Message); // But truncated in message
    }
    
    [Fact]
    public void TAMLException_WithInnerException_PreservesInnerException()
    {
        var inner = new InvalidOperationException("Inner error");
        var ex = new TAMLException("Outer error", inner);
        
        Assert.Equal("Outer error", ex.Message);
        Assert.Same(inner, ex.InnerException);
    }
    
    [Fact]
    public void Deserialize_WithSpaceIndentation_ThrowsTAMLException()
    {
        var taml = "server\n  host\tlocalhost";
        
        var ex = Assert.Throws<TAMLException>(() => 
            TamlSerializer.Deserialize<object>(taml));
        
        Assert.Equal(2, ex.Line);
        Assert.Contains("spaces", ex.Message.ToLower());
        Assert.Contains("  host\tlocalhost", ex.Message);
    }
    
    [Fact]
    public void Deserialize_WithMixedIndentation_ThrowsTAMLException()
    {
        var taml = "server\n\t host\tlocalhost";
        
        var ex = Assert.Throws<TAMLException>(() => 
            TamlSerializer.Deserialize<object>(taml));
        
        Assert.Equal(2, ex.Line);
        Assert.Contains("mixed", ex.Message.ToLower());
        Assert.NotNull(ex.LineText);
    }
    
    [Fact]
    public void Deserialize_WithEmptyLine_ThrowsTAMLException()
    {
        var taml = "server\n\t";
        
        var ex = Assert.Throws<TAMLException>(() => 
            TamlSerializer.Deserialize<object>(taml));
        
        Assert.Equal(2, ex.Line);
        Assert.Contains("no content", ex.Message.ToLower());
        Assert.NotNull(ex.LineText);
    }
    
    [Fact]
    public void Deserialize_WithTabInValue_ThrowsTAMLException()
    {
        var taml = "server\tlocal\thost";
        
        var ex = Assert.Throws<TAMLException>(() => 
            TamlSerializer.Deserialize<object>(taml));
        
        Assert.Equal(1, ex.Line);
        Assert.Contains("tab", ex.Message.ToLower());
        Assert.Contains("value", ex.Message.ToLower());
        Assert.Equal("server\tlocal\thost", ex.LineText);
    }
    
    [Fact]
    public void Deserialize_WithOnlyWhitespaceAfterIndent_ThrowsTAMLException()
    {
        var taml = "server\n\t   ";
        
        var ex = Assert.Throws<TAMLException>(() => 
            TamlSerializer.Deserialize<object>(taml));
        
        Assert.Equal(2, ex.Line);
        // Will throw mixed tabs/spaces error because of tab followed by spaces
        Assert.True(ex.Message.ToLower().Contains("mixed") || ex.Message.ToLower().Contains("no content"));
    }
    
    [Fact]
    public void Deserialize_ValidTAML_DoesNotThrowTAMLException()
    {
        var taml = "server\n\thost\tlocalhost\n\tport\t8080";
        
        var result = TamlSerializer.Deserialize<object>(taml);
        
        Assert.NotNull(result);
    }
    
    [Fact]
    public void TAMLException_CanBeCaughtSpecifically()
    {
        var taml = "server\n  invalid";
        bool caughtTAMLException = false;
        
        try
        {
            TamlSerializer.Deserialize<object>(taml);
        }
        catch (TAMLException ex)
        {
            caughtTAMLException = true;
            Assert.NotNull(ex.Line);
            Assert.NotNull(ex.LineText);
        }
        catch (Exception)
        {
            // Should not catch general exception
        }
        
        Assert.True(caughtTAMLException);
    }
    
    [Fact]
    public void Deserialize_MultipleErrors_ThrowsFirstError()
    {
        var taml = "server\n  host\tlocalhost\n  port\t80\t90";
        
        var ex = Assert.Throws<TAMLException>(() => 
            TamlSerializer.Deserialize<object>(taml));
        
        // Should throw on first error (line 2 - space indentation)
        Assert.Equal(2, ex.Line);
    }
}
