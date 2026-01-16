//! TAML Parser - Parse TAML formatted text into Value objects

use crate::constants::{EMPTY_STRING, NULL_VALUE, TAB};
use crate::error::{TAMLError, TAMLResult};
use crate::value::Value;
use std::collections::HashMap;

/// Options for parsing TAML
#[derive(Debug, Clone)]
pub struct ParseOptions {
    /// Enable strict parsing that raises errors on invalid TAML
    pub strict: bool,
    /// Automatically convert strings to native types (int, float, bool)
    pub type_conversion: bool,
}

impl Default for ParseOptions {
    fn default() -> Self {
        Self {
            strict: false,
            type_conversion: true,
        }
    }
}

/// Parse a TAML string into a Value
pub fn parse(text: &str) -> TAMLResult<Value> {
    parse_with_options(text, ParseOptions::default())
}

/// Parse a TAML string with strict validation
pub fn parse_strict(text: &str) -> TAMLResult<Value> {
    parse_with_options(
        text,
        ParseOptions {
            strict: true,
            ..Default::default()
        },
    )
}

/// Parse a TAML string with custom options
pub fn parse_with_options(text: &str, options: ParseOptions) -> TAMLResult<Value> {
    let lines: Vec<&str> = text.lines().collect();
    let mut entries = Vec::new();
    
    // First pass: Parse lines into entries
    for (i, line) in lines.iter().enumerate() {
        let line_num = i + 1;

        // Skip empty lines and comments
        if line.trim().is_empty() || line.trim_start().starts_with('#') {
            continue;
        }

        // Check for space indentation
        if line.starts_with(' ') {
            if options.strict {
                return Err(TAMLError::with_line(
                    "Indentation must use tabs, not spaces",
                    line_num,
                ));
            }
            continue;
        }

        // Count leading tabs
        let level = line.chars().take_while(|&c| c == TAB).count();
        let content = &line[level..];

        if content.trim().is_empty() {
            continue;
        }

        // Find key-value separator
        let tab_index = content.find(TAB);
        let (key, raw_value) = if let Some(idx) = tab_index {
            let key = &content[..idx];
            let value_start = idx + content[idx..].chars().take_while(|&c| c == TAB).count();
            let raw = if value_start < content.len() {
                content[value_start..].trim_end()
            } else {
                ""
            };
            (key, Some(raw))
        } else {
            (content.trim_end(), None)
        };

        if key.is_empty() {
            if options.strict {
                return Err(TAMLError::with_line("Line has no key", line_num));
            }
            continue;
        }

        // Check for tabs in value
        if let Some(val) = raw_value {
            if val.contains(TAB) {
                if options.strict {
                    return Err(TAMLError::with_line(
                        "Value contains invalid tab character",
                        line_num,
                    ));
                }
                continue;
            }
        }

        entries.push(Entry {
            level,
            key: key.to_string(),
            raw_value: raw_value.map(|s| s.to_string()),
        });
    }

    // Second pass: Build tree
    if entries.is_empty() {
        return Ok(Value::Object(HashMap::new()));
    }

    let (value, _) = build_tree(&entries, 0, entries.len(), 0, options.type_conversion)?;
    Ok(value)
}

struct Entry {
    level: usize,
    key: String,
    raw_value: Option<String>,
}

fn build_tree(
    entries: &[Entry],
    start: usize,
    end: usize,
    expected_level: usize,
    type_conversion: bool,
) -> TAMLResult<(Value, usize)> {
    let mut result = HashMap::new();
    let mut i = start;

    while i < end && entries[i].level >= expected_level {
        let entry = &entries[i];
        
        if entry.level > expected_level {
            i += 1;
            continue;
        }

        if let Some(ref raw) = entry.raw_value {
            // Leaf node with value
            let value = convert_value(raw, type_conversion);
            result.insert(entry.key.clone(), value);
            i += 1;
        } else {
            // Parent node - find its children
            let child_level = expected_level + 1;
            let mut j = i + 1;
            
            // Find all direct children
            let children_start = j;
            let mut children_end = j;
            let mut has_key_value = false;
            let mut all_values_only = true;
            
            while j < end && entries[j].level > expected_level {
                if entries[j].level == child_level {
                    children_end = j + 1;
                    if entries[j].raw_value.is_some() {
                        has_key_value = true;
                    }
                    // Check if this child has its own children
                    if j + 1 < end && entries[j + 1].level > child_level {
                        all_values_only = false;
                    }
                }
                j += 1;
            }
            
            // Determine if children form an array or object
            if !has_key_value && all_values_only && children_start < children_end {
                // It's an array
                let mut arr = Vec::new();
                for entry in entries.iter().take(children_end).skip(children_start) {
                    if entry.level == child_level {
                        arr.push(Value::String(entry.key.clone()));
                    }
                }
                result.insert(entry.key.clone(), Value::Array(arr));
            } else {
                // It's an object - recursively build
                let (child_obj, _) = build_tree(entries, children_start, j, child_level, type_conversion)?;
                result.insert(entry.key.clone(), child_obj);
            }
            
            i = j;
        }
    }

    Ok((Value::Object(result), i))
}

/// Convert a string value to appropriate type
fn convert_value(raw: &str, type_conversion: bool) -> Value {
    if raw == NULL_VALUE {
        return Value::Null;
    }

    if raw == EMPTY_STRING {
        return Value::String(String::new());
    }

    if !type_conversion {
        return Value::String(raw.to_string());
    }

    // Try boolean
    if raw == "true" {
        return Value::Boolean(true);
    }
    if raw == "false" {
        return Value::Boolean(false);
    }

    // Try integer
    if let Ok(i) = raw.parse::<i64>() {
        return Value::Integer(i);
    }

    // Try float
    if let Ok(f) = raw.parse::<f64>() {
        return Value::Float(f);
    }

    // Default to string
    Value::String(raw.to_string())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_simple_key_value() {
        let taml = "key\tvalue";
        let result = parse(taml).unwrap();
        
        let obj = result.as_object().unwrap();
        assert_eq!(obj.get("key").unwrap().as_string(), Some("value"));
    }

    #[test]
    fn test_parse_null_value() {
        let taml = "key\t~";
        let result = parse(taml).unwrap();
        
        let obj = result.as_object().unwrap();
        assert!(obj.get("key").unwrap().is_null());
    }

    #[test]
    fn test_parse_empty_string() {
        let taml = "key\t\"\"";
        let result = parse(taml).unwrap();
        
        let obj = result.as_object().unwrap();
        assert_eq!(obj.get("key").unwrap().as_string(), Some(""));
    }

    #[test]
    fn test_parse_boolean() {
        let taml = "enabled\ttrue\ndisabled\tfalse";
        let result = parse(taml).unwrap();
        
        let obj = result.as_object().unwrap();
        assert_eq!(obj.get("enabled").unwrap().as_boolean(), Some(true));
        assert_eq!(obj.get("disabled").unwrap().as_boolean(), Some(false));
    }

    #[test]
    fn test_parse_numbers() {
        let taml = "integer\t42\nfloat\t3.14";
        let result = parse(taml).unwrap();
        
        let obj = result.as_object().unwrap();
        assert_eq!(obj.get("integer").unwrap().as_integer(), Some(42));
        assert_eq!(obj.get("float").unwrap().as_float(), Some(3.14));
    }

    #[test]
    fn test_parse_nested_object() {
        let taml = "server\n\thost\tlocalhost\n\tport\t8080";
        let result = parse(taml).unwrap();
        
        let obj = result.as_object().unwrap();
        let server = obj.get("server").unwrap().as_object().unwrap();
        assert_eq!(server.get("host").unwrap().as_string(), Some("localhost"));
        assert_eq!(server.get("port").unwrap().as_integer(), Some(8080));
    }

    #[test]
    fn test_parse_array() {
        let taml = "items\n\tfirst\n\tsecond\n\tthird";
        let result = parse(taml).unwrap();
        
        let obj = result.as_object().unwrap();
        let items = obj.get("items").unwrap().as_array().unwrap();
        assert_eq!(items.len(), 3);
        assert_eq!(items[0].as_string(), Some("first"));
        assert_eq!(items[1].as_string(), Some("second"));
        assert_eq!(items[2].as_string(), Some("third"));
    }
}
