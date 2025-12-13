# TAML Python Implementation

## Overview

This is the Python implementation of TAML (Tab Accessible Markup Language), following the TAML-SPEC.md specification. It provides a clean, Pythonic API for parsing and serializing TAML documents.

## Architecture

The implementation is organized into three main modules:

### 1. `taml/__init__.py`
- Package entry point
- Exports public API: `parse()`, `stringify()`, and `TAMLError`

### 2. `taml/parser.py`
- **`parse(text, strict=False, type_conversion=True)`**: Main parsing function
- **`TAMLError`**: Custom exception class for parsing errors
- **Helper functions**:
  - `_convert_type()`: Converts string values to native Python types
  - `_is_array_parent()`: Determines if a node should be a list or dict
  - `_has_children()`: Checks if a line has child elements

### 3. `taml/serializer.py`
- **`stringify(obj, indent_level=0, type_conversion=True)`**: Main serialization function
- **Helper functions**:
  - `_serialize_value()`: Serializes individual values
  - `_serialize_object()`: Serializes dictionary objects

## Key Features

### 1. Pythonic API
- Simple, intuitive function names (`parse`, `stringify`)
- Type hints for better IDE support
- Follows Python conventions (snake_case, docstrings)

### 2. Type Conversion
- Automatic conversion of strings to int, float, bool
- Configurable via `type_conversion` parameter
- Preserves null (`~`) and empty string (`""`) semantics

### 3. Error Handling
- Custom `TAMLError` exception with line numbers
- Two modes:
  - **Lenient mode** (default): Skips invalid lines
  - **Strict mode**: Raises errors on first invalid line

### 4. Standards Compliance
- Follows TAML-SPEC.md exactly
- Validates:
  - Tab-only indentation
  - No tabs in keys or values
  - Proper indentation levels
  - Parent-child relationships

### 5. Data Structure Detection
- Automatically determines if nodes are lists or dicts
- Lists: Children with no tab separators and no grandchildren
- Dicts: Children with tab separators (key-value pairs)

## Implementation Details

### Parsing Algorithm

1. **Line Processing**:
   - Split input by newlines
   - Skip empty lines and comments
   - Validate indentation (tabs only)

2. **Indentation Tracking**:
   - Stack-based approach to track hierarchy
   - Each stack entry has level and node reference
   - Pop stack when moving to lower indentation level

3. **Value Detection**:
   - Check for tab separator to distinguish keys from values
   - Parse special values (`~`, `""`)
   - Apply type conversion if enabled

4. **Structure Detection**:
   - Look ahead to determine if parent is list or dict
   - Check immediate children for key-value pairs
   - Lists have simple values, dicts have key-value pairs

### Serialization Algorithm

1. **Type Detection**:
   - Handle None → `~`
   - Handle empty string → `""`
   - Handle primitives → string conversion
   - Handle collections → recursive serialization

2. **Indentation**:
   - Track current indentation level
   - Multiply tab character by level for each line

3. **Object Serialization**:
   - Iterate over dict items
   - Write key on one line, children on following lines
   - Leaf values on same line as key

4. **List Serialization**:
   - Write list items with proper indentation
   - No keys, just indented values

## Comparison with Other Implementations

### JavaScript (taml-js)
- Similar API design
- Uses CommonJS/ESM modules
- Async not needed (synchronous parsing)

### .NET (TAML.Core)
- More verbose API (TamlSerializer class)
- Strong typing with generics
- Reflection-based serialization

### Python (this implementation)
- Most concise API
- Dynamic typing
- Dict-based data structures
- No reflection needed

## Testing

Three test modules cover all functionality:

1. **`test_parser.py`**: Parsing edge cases and validation
2. **`test_serializer.py`**: Serialization correctness
3. **`test_roundtrip.py`**: Parse → stringify → parse identity

Run tests with:
```bash
python -m unittest discover tests
```

Or with pytest:
```bash
pytest tests/
```

## Dependencies

**Runtime**: None - pure Python standard library

**Development** (optional):
- pytest: For running tests
- pytest-cov: For coverage reports

## Python Version Support

- Python 3.7+
- Type hints compatible with 3.7+
- No use of walrus operator or other 3.8+ features

## Future Enhancements

Potential additions (not required by spec):

1. **Validation mode**: Validate without parsing
2. **Custom type converters**: Register custom type handlers
3. **Streaming parser**: For large files
4. **Pretty printing**: Format TAML with alignment
5. **Schema validation**: Validate against a schema

## Files

```
python/
├── taml/
│   ├── __init__.py      # Package entry point
│   ├── parser.py        # Parsing implementation
│   └── serializer.py    # Serialization implementation
├── tests/
│   ├── __init__.py
│   ├── test_parser.py
│   ├── test_serializer.py
│   └── test_roundtrip.py
├── README.md            # User documentation
├── USAGE.md             # Detailed usage guide
├── IMPLEMENTATION.md    # This file
├── LICENSE              # MIT license
├── setup.py             # setuptools configuration
├── pyproject.toml       # Modern Python packaging
├── requirements.txt     # Dependencies
├── MANIFEST.in          # Package manifest
├── .gitignore          # Git ignore rules
└── example.py          # Usage examples
```

## Design Decisions

### 1. Dictionary as Default Return Type
Python dicts are the natural choice for TAML objects, providing:
- Easy key access
- Native JSON compatibility
- Built-in iteration

### 2. Type Hints
Added type hints for better IDE support and documentation, while maintaining compatibility with Python 3.7+.

### 3. Two-Function API
Minimal API surface: just `parse()` and `stringify()` for most use cases.

### 4. Exception Design
Custom `TAMLError` includes line numbers for debugging, similar to JSON parsing errors.

### 5. Lenient by Default
Following the robustness principle: be strict in what you send, lenient in what you accept.

## Conformance

This implementation fully conforms to TAML-SPEC.md v0.1:

- ✅ Tab-based indentation
- ✅ Tab-separated key-value pairs
- ✅ Multiple tabs for visual alignment
- ✅ Null values (`~`)
- ✅ Empty strings (`""`)
- ✅ Comments (`#` prefix)
- ✅ Lists (indented values)
- ✅ Nested structures
- ✅ Type conversion (optional)
- ✅ Strict validation mode
- ✅ All error types from spec

## License

MIT License - See LICENSE file
