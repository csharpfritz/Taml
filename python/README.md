# TAML Python

Tab Accessible Markup Language (TAML) parser and serializer for Python.

## Installation

```bash
pip install -e .
```

## Usage

### Parsing TAML

```python
import taml

taml_text = """
application	MyApp
version	1.0.0

server
	host	localhost
	port	8080
	ssl	true

features
	authentication
	logging
	caching
"""

data = taml.parse(taml_text)
print(data)
# {
#     'application': 'MyApp',
#     'version': '1.0.0',
#     'server': {
#         'host': 'localhost',
#         'port': 8080,
#         'ssl': True
#     },
#     'features': ['authentication', 'logging', 'caching']
# }
```

### Serializing to TAML

```python
import taml

data = {
    'application': 'MyApp',
    'version': '1.0.0',
    'server': {
        'host': 'localhost',
        'port': 8080,
        'ssl': True
    },
    'features': ['authentication', 'logging', 'caching']
}

taml_text = taml.stringify(data)
print(taml_text)
```

### Options

#### Parsing Options

- `strict` (bool, default: `False`): Enable strict parsing that raises errors on invalid TAML
- `type_conversion` (bool, default: `True`): Automatically convert strings to native Python types (int, float, bool)

```python
# Strict parsing
try:
    data = taml.parse(text, strict=True)
except taml.TAMLError as e:
    print(f"Error: {e}")

# Disable type conversion (all values remain strings)
data = taml.parse(text, type_conversion=False)
```

#### Serialization Options

- `indent_level` (int, default: `0`): Starting indentation level
- `type_conversion` (bool, default: `True`): Convert native types to string representations

### Handling Null and Empty Values

TAML distinguishes between null values and empty strings:

```python
import taml

taml_text = """
username	alice
password	~
nickname	""
"""

data = taml.parse(taml_text)
# {
#     'username': 'alice',
#     'password': None,  # null value
#     'nickname': ''     # empty string
# }
```

## Features

- **Simple API**: Just `parse()` and `stringify()`
- **Type Conversion**: Automatic conversion of numbers and booleans
- **Null Support**: Use `~` for null values
- **Empty Strings**: Use `""` for empty strings
- **Error Handling**: Clear error messages with line numbers in strict mode
- **Lists and Dictionaries**: Full support for nested structures

## Validation

The parser validates TAML structure according to the spec:

- Only tabs for indentation (no spaces)
- No tabs in keys or values
- Proper indentation levels
- Valid parent-child relationships

Use `strict=True` to enforce validation and get detailed error messages.

## License

MIT
