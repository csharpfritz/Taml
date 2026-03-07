//! TAML Serializer - Serialize Value objects into TAML formatted text

use crate::constants::{EMPTY_STRING, NULL_VALUE, TAB};
use crate::value::Value;

/// Options for serializing TAML
#[derive(Debug, Clone, Default)]
pub struct SerializeOptions {
    /// Starting indentation level
    pub indent_level: usize,
}

/// Serialize a Value to TAML format
pub fn stringify(value: &Value) -> String {
    stringify_with_options(value, SerializeOptions::default())
}

/// Serialize a Value to TAML format with custom options
pub fn stringify_with_options(value: &Value, options: SerializeOptions) -> String {
    let mut lines = Vec::new();
    serialize_value(value, &mut lines, options.indent_level);
    lines.join("\n")
}

fn serialize_value(value: &Value, lines: &mut Vec<String>, level: usize) {
    match value {
        Value::Object(obj) => {
            serialize_object(obj, lines, level);
        }
        Value::Array(arr) => {
            serialize_array(arr, lines, level);
        }
        _ => {
            // Leaf values should not be serialized directly at root
            lines.push(value_to_string(value));
        }
    }
}

fn serialize_object(
    obj: &std::collections::HashMap<String, Value>,
    lines: &mut Vec<String>,
    level: usize,
) {
    let indent = TAB.to_string().repeat(level);

    for (key, value) in obj {
        match value {
            Value::Object(nested_obj) => {
                lines.push(format!("{}{}", indent, key));
                serialize_object(nested_obj, lines, level + 1);
            }
            Value::Array(arr) => {
                lines.push(format!("{}{}", indent, key));
                serialize_array(arr, lines, level + 1);
            }
            _ => {
                let val_str = value_to_string(value);
                lines.push(format!("{}{}\t{}", indent, key, val_str));
            }
        }
    }
}

fn serialize_array(arr: &[Value], lines: &mut Vec<String>, level: usize) {
    let indent = TAB.to_string().repeat(level);

    for item in arr {
        match item {
            Value::Object(obj) => {
                serialize_object(obj, lines, level);
            }
            Value::Array(nested_arr) => {
                serialize_array(nested_arr, lines, level);
            }
            _ => {
                let val_str = value_to_string(item);
                lines.push(format!("{}{}", indent, val_str));
            }
        }
    }
}

fn value_to_string(value: &Value) -> String {
    match value {
        Value::Null => NULL_VALUE.to_string(),
        Value::String(s) if s.is_empty() => EMPTY_STRING.to_string(),
        Value::String(s) => s.clone(),
        Value::Integer(i) => i.to_string(),
        Value::Float(f) => f.to_string(),
        Value::Boolean(b) => b.to_string(),
        Value::Array(_) | Value::Object(_) => {
            // These should not be converted to string directly
            String::new()
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashMap;

    #[test]
    fn test_stringify_simple() {
        let mut obj = HashMap::new();
        obj.insert("key".to_string(), Value::String("value".to_string()));
        
        let result = stringify(&Value::Object(obj));
        assert_eq!(result, "key\tvalue");
    }

    #[test]
    fn test_stringify_null() {
        let mut obj = HashMap::new();
        obj.insert("key".to_string(), Value::Null);
        
        let result = stringify(&Value::Object(obj));
        assert_eq!(result, "key\t~");
    }

    #[test]
    fn test_stringify_empty_string() {
        let mut obj = HashMap::new();
        obj.insert("key".to_string(), Value::String(String::new()));
        
        let result = stringify(&Value::Object(obj));
        assert_eq!(result, "key\t\"\"");
    }

    #[test]
    fn test_stringify_boolean() {
        let mut obj = HashMap::new();
        obj.insert("enabled".to_string(), Value::Boolean(true));
        obj.insert("disabled".to_string(), Value::Boolean(false));
        
        let result = stringify(&Value::Object(obj));
        assert!(result.contains("true") || result.contains("false"));
    }

    #[test]
    fn test_stringify_numbers() {
        let mut obj = HashMap::new();
        obj.insert("integer".to_string(), Value::Integer(42));
        obj.insert("float".to_string(), Value::Float(3.14));
        
        let result = stringify(&Value::Object(obj));
        assert!(result.contains("42") || result.contains("3.14"));
    }

    #[test]
    fn test_stringify_nested_object() {
        let mut server = HashMap::new();
        server.insert("host".to_string(), Value::String("localhost".to_string()));
        server.insert("port".to_string(), Value::Integer(8080));
        
        let mut obj = HashMap::new();
        obj.insert("server".to_string(), Value::Object(server));
        
        let result = stringify(&Value::Object(obj));
        assert!(result.contains("server"));
        assert!(result.contains("host\tlocalhost") || result.contains("localhost"));
    }

    #[test]
    fn test_stringify_array() {
        let items = vec![
            Value::String("first".to_string()),
            Value::String("second".to_string()),
            Value::String("third".to_string()),
        ];
        
        let mut obj = HashMap::new();
        obj.insert("items".to_string(), Value::Array(items));
        
        let result = stringify(&Value::Object(obj));
        assert!(result.contains("items"));
        assert!(result.contains("first"));
        assert!(result.contains("second"));
        assert!(result.contains("third"));
    }
}
