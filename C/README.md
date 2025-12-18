# TAML C Library

A complete C implementation of the TAML (Tab Annotated Markup Language) parser, serializer, and validator with format conversion utilities.

## Features

- ✅ **Full TAML Parser** - Parse TAML text into C data structures
- ✅ **TAML Serializer** - Convert C structures back to TAML format
- ✅ **Strict Validator** - Validate TAML documents with detailed error messages
- ✅ **Type Conversion** - Automatic conversion between strings and native types (int, float, bool)
- ✅ **Format Converters** - Convert between TAML, JSON, XML, and YAML formats
- ✅ **Memory Safe** - Comprehensive memory management with cleanup functions
- ✅ **Zero Dependencies** - Pure C99 implementation with no external dependencies
- ✅ **Easy to Use** - Simple, intuitive API

## Installation

### Build from Source

```bash
cd C
make
```

This will create:
- `lib/libtaml.a` - Static library
- `lib/libtaml.so` - Shared library

### System-wide Installation

```bash
sudo make install
```

This installs the library to `/usr/local/lib` and the header to `/usr/local/include`.

### Uninstall

```bash
sudo make uninstall
```

## Quick Start

### Basic Example

```c
#include <stdio.h>
#include "taml.h"

int main() {
    // Parse TAML text
    const char *taml_text = 
        "application\tMyApp\n"
        "version\t1.0.0\n"
        "server\n"
        "\thost\tlocalhost\n"
        "\tport\t8080\n";
    
    taml_document_t *doc = taml_parse(taml_text, NULL);
    
    if (doc && doc->root) {
        // Access values
        taml_value_t *app = taml_object_get(doc->root, "application");
        if (app && app->type == TAML_TYPE_STRING) {
            printf("Application: %s\n", app->data.string_val);
        }
        
        // Access nested values
        taml_value_t *server = taml_object_get(doc->root, "server");
        if (server && server->type == TAML_TYPE_OBJECT) {
            taml_value_t *port = taml_object_get(server, "port");
            if (port && port->type == TAML_TYPE_INT) {
                printf("Port: %lld\n", port->data.int_val);
            }
        }
        
        taml_free_document(doc);
    }
    
    return 0;
}
```

### Compile and Run

```bash
gcc -o example example.c -Iinclude -Llib -ltaml
./example
```

## API Documentation

### Core Types

```c
taml_document_t   // Represents a parsed TAML document
taml_value_t      // Represents a TAML value (object, array, string, etc.)
taml_error_t      // Error codes
```

### Value Types

- `TAML_TYPE_NULL` - Null value (`~`)
- `TAML_TYPE_BOOL` - Boolean (`true` or `false`)
- `TAML_TYPE_INT` - Integer number
- `TAML_TYPE_FLOAT` - Floating-point number
- `TAML_TYPE_STRING` - String value
- `TAML_TYPE_OBJECT` - Object (key-value pairs)
- `TAML_TYPE_ARRAY` - Array of values

### Parser Functions

#### Parse from String

```c
taml_document_t* taml_parse(const char *text, const taml_parser_options_t *options);
```

Parse TAML text into a document structure.

**Options:**
- `strict` - Enable strict parsing mode with detailed error checking
- `type_conversion` - Automatically convert strings to native types

**Example:**

```c
taml_parser_options_t options = {
    .strict = true,
    .type_conversion = true
};

taml_document_t *doc = taml_parse(text, &options);
if (doc) {
    if (doc->error_message) {
        printf("Error: %s (line %d)\n", doc->error_message, doc->error_line);
    }
    taml_free_document(doc);
}
```

#### Parse from File

```c
taml_document_t* taml_parse_file(const char *filename, const taml_parser_options_t *options);
```

### Serializer Functions

#### Stringify to String

```c
char* taml_stringify(const taml_document_t *doc, const taml_serializer_options_t *options);
```

Convert a TAML document to a string. Remember to free the returned string.

**Example:**

```c
char *taml_str = taml_stringify(doc, NULL);
if (taml_str) {
    printf("%s\n", taml_str);
    taml_free_string(taml_str);
}
```

#### Write to File

```c
taml_error_t taml_write_file(const taml_document_t *doc, const char *filename, 
                              const taml_serializer_options_t *options);
```

### Validator Functions

#### Validate String

```c
taml_error_t taml_validate(const char *text, char *error_msg, 
                           size_t error_msg_size, int *error_line);
```

Validate TAML text without parsing it.

**Example:**

```c
char error_msg[256];
int error_line;

taml_error_t err = taml_validate(text, error_msg, sizeof(error_msg), &error_line);
if (err != TAML_OK) {
    printf("Validation failed: %s (line %d)\n", error_msg, error_line);
}
```

#### Validate File

```c
taml_error_t taml_validate_file(const char *filename, char *error_msg,
                                 size_t error_msg_size, int *error_line);
```

### Value Creation Functions

```c
taml_value_t* taml_create_object(void);
taml_value_t* taml_create_array(void);
taml_value_t* taml_create_string(const char *str);
taml_value_t* taml_create_int(long long num);
taml_value_t* taml_create_float(double num);
taml_value_t* taml_create_bool(bool val);
taml_value_t* taml_create_null(void);
```

### Object Operations

```c
// Get value by key
taml_value_t* taml_object_get(const taml_value_t *obj, const char *key);

// Set value by key
taml_error_t taml_object_set(taml_value_t *obj, const char *key, taml_value_t *value);
```

**Example:**

```c
taml_value_t *obj = taml_create_object();
taml_object_set(obj, "name", taml_create_string("John"));
taml_object_set(obj, "age", taml_create_int(30));

taml_value_t *name = taml_object_get(obj, "name");
```

### Array Operations

```c
// Get array size
size_t taml_array_size(const taml_value_t *arr);

// Get value by index
taml_value_t* taml_array_get(const taml_value_t *arr, size_t index);

// Append value
taml_error_t taml_array_append(taml_value_t *arr, taml_value_t *value);
```

**Example:**

```c
taml_value_t *arr = taml_create_array();
taml_array_append(arr, taml_create_string("item1"));
taml_array_append(arr, taml_create_string("item2"));

size_t size = taml_array_size(arr);
taml_value_t *first = taml_array_get(arr, 0);
```

### Format Conversion Functions

#### JSON Conversion

```c
// JSON to TAML
taml_document_t* taml_from_json(const char *json_text);

// TAML to JSON
char* taml_to_json(const taml_document_t *doc);
```

**Example:**

```c
const char *json = "{\"name\": \"John\", \"age\": 30}";
taml_document_t *doc = taml_from_json(json);

if (doc) {
    char *taml_str = taml_stringify(doc, NULL);
    printf("TAML:\n%s\n", taml_str);
    taml_free_string(taml_str);
    
    char *json_out = taml_to_json(doc);
    printf("JSON:\n%s\n", json_out);
    taml_free_string(json_out);
    
    taml_free_document(doc);
}
```

#### XML and YAML Conversion

```c
// XML conversion
taml_document_t* taml_from_xml(const char *xml_text);
char* taml_to_xml(const taml_document_t *doc);

// YAML conversion
taml_document_t* taml_from_yaml(const char *yaml_text);
char* taml_to_yaml(const taml_document_t *doc);
```

**Note:** XML and YAML converters are stubs in the current implementation and would require full XML/YAML parser libraries.

### Memory Management

```c
// Free document and all contents
void taml_free_document(taml_document_t *doc);

// Free a value and all contents
void taml_free_value(taml_value_t *value);

// Free a string returned by TAML functions
void taml_free_string(char *str);
```

**Important:** Always free documents, values, and strings to prevent memory leaks.

### Utility Functions

```c
// Get error message for error code
const char* taml_error_string(taml_error_t error);

// Get library version
const char* taml_version(void);
```

## Building and Testing

### Build Library

```bash
make
```

### Build and Run Tests

```bash
make tests
make test
```

### Build Examples

```bash
make examples
./examples/example_basic
```

### Clean Build

```bash
make clean
```

## Examples

See the `examples/` directory for complete examples:

- `example_basic.c` - Basic parsing, serialization, validation, and conversion examples

Run the example:

```bash
make examples
./examples/example_basic
```

## Error Handling

All functions return appropriate error codes or NULL on failure. Check return values and handle errors appropriately:

```c
taml_document_t *doc = taml_parse(text, NULL);
if (!doc) {
    fprintf(stderr, "Failed to parse TAML\n");
    return 1;
}

if (doc->error_message) {
    fprintf(stderr, "Parse error: %s (line %d)\n", 
            doc->error_message, doc->error_line);
    taml_free_document(doc);
    return 1;
}

// Use document...

taml_free_document(doc);
```

## Error Codes

- `TAML_OK` - Success
- `TAML_ERROR_NULL_INPUT` - Null input provided
- `TAML_ERROR_MEMORY` - Memory allocation failed
- `TAML_ERROR_INVALID_INDENTATION` - Invalid indentation (spaces used)
- `TAML_ERROR_INVALID_TAB_IN_KEY` - Tab character in key
- `TAML_ERROR_INVALID_TAB_IN_VALUE` - Tab character in value
- `TAML_ERROR_INCONSISTENT_INDENT` - Inconsistent indentation level
- `TAML_ERROR_ORPHANED_LINE` - Indented line has no parent
- `TAML_ERROR_PARENT_WITH_VALUE` - Parent key has value on same line
- `TAML_ERROR_EMPTY_KEY` - Empty key
- `TAML_ERROR_MIXED_INDENT` - Mixed spaces and tabs
- `TAML_ERROR_PARSE_FAILED` - General parse failure

## Validation Rules

The validator enforces these TAML rules:

1. ✅ **Tabs only** - Indentation must use tabs, not spaces
2. ✅ **No tabs in content** - Keys and values cannot contain tab characters
3. ✅ **Consistent indentation** - Each level increases by exactly one tab
4. ✅ **Valid parent-child** - Indented lines must have a parent
5. ✅ **No mixed indentation** - Cannot mix spaces and tabs

## Limitations

- **XML/YAML Converters**: The XML and YAML conversion functions are currently stubs. Full implementation would require integrating external XML/YAML parser libraries.
- **Unicode**: The library handles ASCII and UTF-8 strings but does not perform Unicode normalization.
- **Platform**: Tested on Linux. Should work on macOS and Windows with minor adjustments.

## Contributing

Contributions are welcome! Please ensure:

1. Code follows C99 standard
2. All tests pass (`make test`)
3. Memory is properly managed (no leaks)
4. Documentation is updated

## License

MIT License - See repository root for details.

## See Also

- [TAML Specification](../TAML-SPEC.md)
- [Python Implementation](../python/)
- [JavaScript Implementation](../javascript/)
- [.NET Implementation](../dotnet/)
- [Examples](../examples/)

---

**TAML C Library** - Because sometimes less markup is more. 🚀
