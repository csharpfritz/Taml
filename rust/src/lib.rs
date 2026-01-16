//! TAML (Tab Accessible Markup Language) Parser and Serializer
//! 
//! TAML is a minimalist hierarchical data serialization format that uses only tabs 
//! and newlines for structure. It's designed to be simpler and more accessible than 
//! YAML while maintaining human readability.
//! 
//! # Examples
//! 
//! ## Parsing TAML
//! 
//! ```rust
//! use taml::parse;
//! 
//! let taml_text = r#"
//! application    MyApp
//! version    1.0.0
//! 
//! server
//!     host    localhost
//!     port    8080
//! "#;
//! 
//! let data = parse(taml_text).unwrap();
//! ```
//! 
//! ## Serializing to TAML
//! 
//! ```rust
//! use taml::{Value, stringify};
//! use std::collections::HashMap;
//! 
//! let mut data = HashMap::new();
//! data.insert("application".to_string(), Value::String("MyApp".to_string()));
//! data.insert("version".to_string(), Value::String("1.0.0".to_string()));
//! 
//! let taml_text = stringify(&Value::Object(data));
//! ```

mod error;
mod parser;
mod serializer;
mod validator;
mod value;

pub use error::{TAMLError, TAMLResult};
pub use parser::{parse, parse_strict, ParseOptions};
pub use serializer::{stringify, stringify_with_options, SerializeOptions};
pub use validator::{validate, ValidationError};
pub use value::Value;

/// Constants used in TAML format
pub mod constants {
    pub const TAB: char = '\t';
    pub const NULL_VALUE: &str = "~";
    pub const EMPTY_STRING: &str = "\"\"";
}
