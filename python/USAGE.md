# TAML Python - Usage Guide

## Installation

### From source
```bash
cd python
pip install -e .
```

### For development
```bash
cd python
pip install -e .
python -m unittest discover tests
```

## Quick Start

```python
import taml

# Parse TAML
data = taml.parse("key\tvalue")

# Serialize to TAML
taml_string = taml.stringify({'key': 'value'})
```

## API Reference

### `parse(text, strict=False, type_conversion=True)`

Parse a TAML string into a Python dictionary.

**Parameters:**
- `text` (str): TAML formatted text
- `strict` (bool): Enable strict parsing that raises errors on invalid TAML
- `type_conversion` (bool): Automatically convert strings to native Python types

**Returns:**
- `dict`: Parsed Python dictionary

**Raises:**
- `TAMLError`: If parsing fails in strict mode

**Example:**
```python
import taml

text = """
server
	host	localhost
	port	8080
"""

data = taml.parse(text)
# {'server': {'host': 'localhost', 'port': 8080}}
```

### `stringify(obj, indent_level=0, type_conversion=True)`

Serialize a Python object to TAML format.

**Parameters:**
- `obj` (Any): Python object to serialize
- `indent_level` (int): Starting indentation level
- `type_conversion` (bool): Convert native types to string representations

**Returns:**
- `str`: TAML formatted string

**Example:**
```python
import taml

data = {
    'server': {
        'host': 'localhost',
        'port': 8080
    }
}

text = taml.stringify(data)
```

### `TAMLError`

Exception class for TAML parsing errors.

**Attributes:**
- `line` (int or None): Line number where error occurred
- `message` (str): Error message

**Example:**
```python
import taml

try:
    data = taml.parse("invalid", strict=True)
except taml.TAMLError as e:
    print(f"Error on line {e.line}: {e}")
```

## Type Conversion

### Automatic Type Conversion (default)

When `type_conversion=True`, the parser automatically converts:

- `"true"` → `True`
- `"false"` → `False`
- `"42"` → `42` (int)
- `"3.14"` → `3.14` (float)
- `"~"` → `None`
- `'""'` → `""` (empty string)

```python
text = """
string	hello
number	42
float	3.14
bool	true
null	~
empty	""
"""

data = taml.parse(text)
# {
#     'string': 'hello',
#     'number': 42,        # int
#     'float': 3.14,       # float
#     'bool': True,        # bool
#     'null': None,        # None
#     'empty': ''          # empty string
# }
```

### Disable Type Conversion

```python
data = taml.parse(text, type_conversion=False)
# All values remain strings except null (~) and empty ("")
```

## Special Values

### Null Values

Use `~` to represent null/None:

```python
text = "password\t~"
data = taml.parse(text)
# {'password': None}

# Serialize null
taml.stringify({'password': None})
# "password\t~"
```

### Empty Strings

Use `""` to represent empty strings:

```python
text = 'nickname\t""'
data = taml.parse(text)
# {'nickname': ''}

# Serialize empty string
taml.stringify({'nickname': ''})
# 'nickname\t""'
```

## Working with Lists

Lists are represented as indented values without keys:

```python
text = """
items
	first
	second
	third
"""

data = taml.parse(text)
# {'items': ['first', 'second', 'third']}

# Serialize list
data = {'items': ['a', 'b', 'c']}
taml.stringify(data)
# "items\n\ta\n\tb\n\tc"
```

## Working with Nested Objects

Objects can be nested to any depth:

```python
text = """
config
	database
		host	localhost
		port	5432
	cache
		enabled	true
"""

data = taml.parse(text)
# {
#     'config': {
#         'database': {
#             'host': 'localhost',
#             'port': 5432
#         },
#         'cache': {
#             'enabled': True
#         }
#     }
# }
```

## Strict Mode

Enable strict mode to validate TAML structure:

```python
import taml

# This will raise TAMLError
text = "parent\n    child\tvalue"  # spaces instead of tabs

try:
    data = taml.parse(text, strict=True)
except taml.TAMLError as e:
    print(e)
    # "Line 2: Indentation must use tabs, not spaces"
```

## Comments

Lines starting with `#` are treated as comments:

```python
text = """
# Configuration file
application	MyApp
# Server settings
server
	host	localhost
"""

data = taml.parse(text)
# {'application': 'MyApp', 'server': {'host': 'localhost'}}
```

## Error Handling

### Lenient Mode (default)

By default, the parser skips invalid lines and continues:

```python
text = """
valid\tdata
    invalid line with spaces
another\tvalid
"""

data = taml.parse(text)  # strict=False
# {'valid': 'data', 'another': 'valid'}
# Invalid line is silently skipped
```

### Strict Mode

Strict mode raises errors on the first invalid line:

```python
try:
    data = taml.parse(text, strict=True)
except taml.TAMLError as e:
    print(f"Error: {e}")
    print(f"Line number: {e.line}")
```

## Comparison with JavaScript and .NET

### JavaScript (Node.js)
```javascript
const taml = require('taml-js');
const data = taml.parse(text);
const tamlString = taml.stringify(data);
```

### Python
```python
import taml
data = taml.parse(text)
taml_string = taml.stringify(data)
```

### .NET (C#)
```csharp
using TAML.Core;
var data = TamlSerializer.Deserialize<Dictionary<string, object>>(text);
var tamlString = TamlSerializer.Serialize(data);
```

All three implementations follow the same TAML specification and are interoperable.

## Best Practices

1. **Use strict mode for validation**: When reading configuration files, enable strict mode to catch errors early.

2. **Type conversion**: Enable type conversion for configuration files, disable for data that should remain strings.

3. **Null vs empty**: Use `~` for absent/unknown values, `""` for explicitly empty strings.

4. **Comments**: Add comments to explain configuration sections.

5. **Indentation**: Always use tabs, never spaces.

## Examples

See `example.py` for comprehensive examples of:
- Parsing TAML
- Serializing Python objects
- Round-trip conversion
- Handling special values
- Type conversion
