//! Error types for TAML parsing and serialization

use std::fmt;

/// Result type for TAML operations
pub type TAMLResult<T> = Result<T, TAMLError>;

/// Error type for TAML operations
#[derive(Debug, Clone, PartialEq)]
pub struct TAMLError {
    pub message: String,
    pub line: Option<usize>,
}

impl TAMLError {
    /// Create a new TAML error
    pub fn new(message: impl Into<String>) -> Self {
        Self {
            message: message.into(),
            line: None,
        }
    }

    /// Create a new TAML error with line number
    pub fn with_line(message: impl Into<String>, line: usize) -> Self {
        Self {
            message: message.into(),
            line: Some(line),
        }
    }
}

impl fmt::Display for TAMLError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if let Some(line) = self.line {
            write!(f, "Line {}: {}", line, self.message)
        } else {
            write!(f, "{}", self.message)
        }
    }
}

impl std::error::Error for TAMLError {}
