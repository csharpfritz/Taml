# TAML Standards Test Suite

This directory contains standard test documents for validating TAML parser implementations against the official TAML specification.

## Purpose

The standards folder provides a collection of reference TAML documents that:

- **Validate parser compliance**: Test that TAML libraries correctly implement the specification
- **Ensure consistency**: Verify that all implementations handle TAML documents the same way
- **Document expected behavior**: Serve as examples of valid TAML syntax and features
- **Support testing**: Provide ready-to-use test cases for parser development

## Structure

Each subdirectory corresponds to a version of the TAML specification:

- `0.1/` - Test documents for TAML Specification v0.1

## Current Specification

The current TAML specification is available here: [TAML-SPEC.md](../TAML-SPEC.md)

**Current Version: v0.1**

## Using These Standards

### For Parser Developers

1. Navigate to the version folder matching your target specification (e.g., `0.1/`)
2. Use the test documents to validate your parser implementation
3. Ensure your parser can correctly parse all valid examples
4. Verify your parser produces the expected output for each document

### For Library Users

These documents demonstrate correct TAML syntax and can be used as:
- Reference examples when writing TAML documents
- Test cases for validating TAML documents
- Templates for common use cases

## Test Document Types

Each version folder contains:

- **Basic feature tests**: Simple documents testing individual features (key-value pairs, nesting, lists, etc.)
- **Complex examples**: Real-world scenarios combining multiple features
- **Edge cases**: Documents testing boundary conditions and special values (null, empty strings, etc.)

## Contributing

When adding new test documents:

1. Place them in the appropriate version folder
2. Ensure they are valid according to that version's specification
3. Include comments explaining what features are being tested
4. Use descriptive file names (e.g., `nested-structures.taml`, `list-examples.taml`)

## Version History

- **v0.1** - Initial specification with core features (tabs for hierarchy, key-value pairs, lists, null/empty values, comments)
