# TAML Rust

Tab Accessible Markup Language (TAML) parser, serializer, and validator for Rust.

## Installation

Add this to your `Cargo.toml`:

```toml
[dependencies]
taml = "0.1.0"
```

## Usage

### Parsing TAML

```rust
use taml::parse;

let taml_text = r#"
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
"#;

let data = parse(taml_text).unwrap();
```

### Serializing to TAML

```rust
use taml::{Value, stringify};
use std::collections::HashMap;

let mut data = HashMap::new();
data.insert("application".to_string(), Value::String("MyApp".to_string()));
data.insert("version".to_string(), Value::String("1.0.0".to_string()));

let mut server = HashMap::new();
server.insert("host".to_string(), Value::String("localhost".to_string()));
server.insert("port".to_string(), Value::Integer(8080));
server.insert("ssl".to_string(), Value::Boolean(true));

data.insert("server".to_string(), Value::Object(server));

let taml_text = stringify(&Value::Object(data));
println!("{}", taml_text);
```

### Validating TAML

```rust
use taml::validate;

let taml_text = "key\tvalue\nserver\n\thost\tlocalhost";
let errors = validate(taml_text);

if errors.is_empty() {
    println!("TAML is valid!");
} else {
    for error in errors {
        eprintln!("Validation error: {}", error);
    }
}
```

### Parsing Options

```rust
use taml::{parse_with_options, ParseOptions};

// Strict parsing - raises errors on invalid TAML
let options = ParseOptions {
    strict: true,
    type_conversion: true,
};

match parse_with_options(taml_text, options) {
    Ok(data) => println!("Parsed successfully"),
    Err(e) => eprintln!("Parse error: {}", e),
}

// Disable type conversion - all values remain strings
let options = ParseOptions {
    strict: false,
    type_conversion: false,
};

let data = parse_with_options(taml_text, options).unwrap();
```

## Features

- **Simple API**: Just `parse()`, `stringify()`, and `validate()`
- **Type Conversion**: Automatic conversion of numbers and booleans
- **Null Support**: Use `~` for null values
- **Empty Strings**: Use `""` for empty strings
- **Error Handling**: Clear error messages with line numbers in strict mode
- **Lists and Objects**: Full support for nested structures
- **Validation**: Comprehensive validation according to TAML spec

## Value Type

The `Value` enum represents TAML values:

```rust
pub enum Value {
    String(String),
    Integer(i64),
    Float(f64),
    Boolean(bool),
    Null,
    Array(Vec<Value>),
    Object(HashMap<String, Value>),
}
```

## Null and Empty Values

TAML distinguishes between null values and empty strings:

```rust
use taml::parse;

let taml_text = r#"
username	alice
password	~
nickname	""
"#;

let data = parse(taml_text).unwrap();
let obj = data.as_object().unwrap();

assert_eq!(obj.get("username").unwrap().as_string(), Some("alice"));
assert!(obj.get("password").unwrap().is_null());  // null value
assert_eq!(obj.get("nickname").unwrap().as_string(), Some(""));  // empty string
```

## Validation

The validator checks TAML structure according to the spec:

- Only tabs for indentation (no spaces)
- No tabs in keys or values
- Proper indentation levels
- Valid parent-child relationships

```rust
use taml::{validate, ErrorType};

let invalid_taml = "server\n    host\tlocalhost";  // spaces instead of tabs
let errors = validate(invalid_taml);

assert_eq!(errors.len(), 1);
assert_eq!(errors[0].error_type, ErrorType::SpaceIndentation);
```

## Error Types

Validation errors are categorized:

- `SpaceIndentation`: Spaces used instead of tabs
- `TabInKey`: Tab character found in key
- `TabInValue`: Tab character found in value
- `InconsistentIndent`: Indentation level skips levels
- `OrphanedLine`: Indented line with no parent
- `EmptyKey`: Line has no key
- `MixedIndent`: Mixed spaces and tabs

## Examples

See the `examples/` directory in the repository for more examples.

## License

MIT
