# Copilot Instructions for TAML Project

## Project Overview

TAML (Tab Annotated Markup Language) is a minimalist hierarchical data serialization format that uses only tabs and newlines for structure. The project includes:

- **Parser implementations** in Python, JavaScript, and C#/.NET
- **VSCode extension** for syntax highlighting and language support
- **Language Server Protocol (LSP)** implementation for real-time validation
- **Comprehensive specification** defined in TAML-SPEC.md

The core philosophy: **simplicity over expressiveness**. TAML uses only tabs and newlines—no brackets, braces, colons, quotes (except for empty strings `""`), or hyphens.

## TAML-Specific Rules

### Critical TAML Format Rules
- **Tabs only**: ALWAYS use tab characters (`\t`) for indentation, NEVER spaces
- **One tab per level**: Each nesting level increases indentation by exactly one tab
- **Key-value separator**: One or more tabs separate keys from values
- **No tabs in content**: Keys and values cannot contain tab characters
- **Special values**: `~` represents null, `""` represents empty string
- **Comments**: Lines starting with `#` are comments (mid-line `#` is literal)
- **Parent keys**: Keys with children have no value on the same line

### Validation Philosophy
- Strict validation ensures consistency and prevents ambiguity
- Clear error messages with line numbers for validation failures
- Parsers should support both strict and lenient modes

## Repository Structure

```
/
├── python/              # Python implementation
├── javascript/          # JavaScript/Node.js implementation
├── dotnet/             # C#/.NET implementation
├── tools/              # Editor tools (VSCode extension, LSP)
├── examples/           # Real-world TAML examples
├── standards/          # Standards and conventions
├── TAML-SPEC.md       # Complete language specification
└── README.md          # Main documentation
```

## Coding Standards

### Python (`python/`)

**Style Guide**: Follow PEP 8 with these specifics:
- Use type hints for all function parameters and return values
- Prefer `"""docstrings"""` with Args/Returns/Raises format
- Import typing modules: `from typing import Any, Dict, List, Optional, Union`
- Use descriptive variable names with snake_case
- Exception handling: Use custom `TAMLError` exception with line numbers

**Testing**:
- Use pytest for testing
- Test files: `test_*.py` or `*_test.py`
- Run tests: `cd python && pytest`
- Aim for comprehensive test coverage

**Example**:
```python
def parse(text: str, strict: bool = False, type_conversion: bool = True) -> Dict[str, Any]:
    """
    Parse a TAML string into a Python dictionary
    
    Args:
        text: TAML formatted text
        strict: Enable strict parsing (default: False)
        type_conversion: Convert string values to native types (default: True)
    
    Returns:
        Parsed Python dictionary
    
    Raises:
        TAMLError: If parsing fails in strict mode
    """
```

### JavaScript (`javascript/`)

**Style Guide**:
- Use ES modules (`import`/`export`)
- Add JSDoc comments for all exported functions
- Use camelCase for variables and functions
- Use PascalCase for classes
- Prefer `const` over `let`, avoid `var`
- Use arrow functions for callbacks
- No semicolons (ASI style as shown in existing code)

**Testing**:
- Use Node.js native test runner
- Run tests: `cd javascript && npm test`
- Test file: `test.js`

**Example**:
```javascript
/**
 * Parse a TAML string into a JavaScript object
 * @param {string} text - TAML formatted text
 * @param {Object} options - Parsing options
 * @param {boolean} options.strict - Enable strict parsing (default: false)
 * @returns {Object} Parsed JavaScript object
 */
export function parse(text, options = {}) {
  // implementation
}
```

### C#/.NET (`dotnet/`)

**Style Guide**:
- Follow standard .NET conventions
- Use tabs for indentation (consistent with TAML philosophy)
- Use XML documentation comments (`///`) for public APIs
- PascalCase for classes, methods, and properties
- camelCase for local variables and private fields
- Nullable reference types enabled
- Use `namespace TAML.Core;` (file-scoped namespaces)

**Projects**:
- `TAML.Core`: Core parsing and serialization logic
- `TAML.Tests`: Unit tests using xUnit or NUnit
- `TAML.Configuration`: Configuration integration

**Building & Testing**:
- Build: `dotnet build dotnet/dotnet.sln`
- Test: `dotnet test dotnet/TAML.Tests/TAML.Tests.csproj`
- Run from repo root

**Example**:
```csharp
/// <summary>
/// Represents a generic TAML document that can hold any TAML structure.
/// </summary>
public class TamlDocument
{
	/// <summary>
	/// The underlying data structure holding the document content
	/// </summary>
	public Dictionary<string, object?> Data { get; private set; }
}
```

## Testing Philosophy

### Test Coverage
- Write tests for all parsing edge cases
- Test validation rules thoroughly (tabs vs spaces, indentation levels)
- Include tests for special values (`~`, `""`)
- Test error handling and error messages
- Cover both valid and invalid TAML documents

### Test Organization
- Group tests by feature area
- Use descriptive test names that explain what's being tested
- Include inline comments for complex test scenarios
- Reference TAML-SPEC.md sections when testing validation rules

## Security Practices

- **Never hard-code secrets** in example files or tests
- **Validate input** strictly to prevent injection attacks
- **Handle tabs carefully**: Ensure tab characters in values are rejected
- **Error messages**: Don't expose internal implementation details
- **File operations**: Validate file paths and handle I/O errors gracefully

## Documentation Standards

- **README files**: Each subdirectory has its own README
- **Code examples**: Show both parsing and serialization
- **Specification**: TAML-SPEC.md is the authoritative source
- **Comments**: Focus on "why" rather than "what" the code does
- **API docs**: Document all public APIs with usage examples

## Build and Test Commands

### Python
```bash
cd python
pip install -e .              # Install in development mode
pytest                        # Run tests
python example.py            # Run example
```

### JavaScript
```bash
cd javascript
npm install                  # Install dependencies (if any added)
npm test                     # Run tests
node example.js             # Run example
```

### .NET
```bash
cd dotnet
dotnet restore dotnet.sln                    # Restore dependencies
dotnet build dotnet.sln --configuration Release   # Build solution
dotnet test TAML.Tests/TAML.Tests.csproj    # Run tests
```

### Tools (VSCode Extension)
```bash
cd tools/vscode-taml
npm install
npm run compile
```

### Tools (Language Server)
```bash
cd tools/taml-language-server
npm install
npm run build
```

## Common Tasks and Patterns

### Adding a New Parser Feature
1. Update TAML-SPEC.md if it affects the specification
2. Implement in all three languages (Python, JavaScript, C#)
3. Add tests for each implementation
4. Update relevant README files with usage examples
5. Consider impact on editor tools (LSP validation, syntax highlighting)

### Fixing a Bug
1. Add a failing test that reproduces the bug
2. Fix the bug in the affected implementation(s)
3. Ensure the test passes
4. Check if the bug exists in other language implementations
5. Update documentation if behavior was unclear

### Adding Examples
- Place in `/examples` directory
- Use `.taml` file extension
- Include comments explaining the structure
- Reference from main README.md
- Show real-world use cases (configs, APIs, data structures)

## Error Handling Patterns

### Python
```python
raise TAMLError("Indentation must use tabs, not spaces", line_num)
```

### JavaScript
```javascript
throw new TAMLError('Indentation must use tabs, not spaces', lineNum)
```

### C#
```csharp
throw new TAMLException("Indentation must use tabs, not spaces", lineNum);
```

## Contributing Guidelines

- **Consistency across implementations**: All three parsers should behave identically
- **Specification first**: Update TAML-SPEC.md before implementing new features
- **Test-driven**: Write tests before or alongside implementation
- **Documentation**: Update relevant docs with any changes
- **Examples**: Add practical examples to demonstrate new features
- **Editor config**: Respect `.editorconfig` settings (tabs for `.taml` files)

## Resources

- **Main Spec**: [TAML-SPEC.md](../TAML-SPEC.md)
- **Python Docs**: [python/README.md](../python/README.md)
- **JavaScript Docs**: [javascript/README.md](../javascript/README.md)
- **Tools Docs**: [tools/README.md](../tools/README.md)
- **Examples**: [examples/](../examples/)

## Philosophy and Design Decisions

- **Simplicity is paramount**: Resist feature creep
- **Tabs do the work**: No additional markup needed
- **Visual clarity**: Structure should be immediately obvious
- **Keyboard accessible**: Tab-based navigation for accessibility
- **Strict by design**: Clear rules prevent ambiguity
- **Human-first**: Optimized for human reading and writing, not machine efficiency

---

**Remember**: When working on TAML code, always use tabs for indentation, and ensure all three implementations (Python, JavaScript, C#) remain consistent with the specification.
