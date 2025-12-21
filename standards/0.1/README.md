# TAML v0.1 Standard Test Documents

This directory contains test documents for validating TAML parsers against the v0.1 specification.

## Specification Reference

These documents are based on [TAML Specification v0.1](../../TAML-SPEC.md)

## Test Documents

### Basic Features

#### 01-basic-key-value.taml
Tests basic key-value pair functionality:
- Single tab separators
- Multiple tab separators (for alignment)
- Various value representations (strings, numbers, booleans)
- Special values (null `~`, empty string `""`)

#### 02-nested-structures.taml
Tests hierarchical nesting:
- Simple parent-child relationships
- Multi-level nesting (3+ levels deep)
- Multiple sibling sections
- Mixed keys with and without values

#### 03-lists.taml
Tests list structures:
- Simple lists of values
- Lists with various item lengths
- Nested lists (lists within structures)
- Multiple lists in the same document

#### 04-null-and-empty.taml
Tests the distinction between null and empty values:
- Null values using `~`
- Empty strings using `""`
- Regular string values
- Null and empty in nested structures

#### 05-comments.taml
Tests comment handling:
- Top-level comments
- Comments between entries
- Comments in nested structures
- Comments with special characters

### Complex Features

#### 06-mixed-structures.taml
Tests combinations of features:
- Key-value pairs with nested structures
- Lists within nested structures
- Multiple nesting levels with lists
- Real-world configuration scenario

#### 07-complex-example.taml
Tests comprehensive real-world usage:
- Deep nesting (4+ levels)
- Multiple sections with different structures
- Combination of all TAML features
- E-commerce platform configuration example

## Expected Parser Behavior

All parsers implementing TAML v0.1 should:

1. **Parse all documents successfully** without errors
2. **Distinguish between null and empty** - `~` and `""` are different values
3. **Ignore comments** - lines starting with `#` should not appear in parsed output
4. **Handle arbitrary tab spacing** - multiple tabs between key and value are equivalent to one
5. **Maintain hierarchy** - nested structures should be represented correctly in the output format

## Testing Your Parser

To validate your TAML parser implementation:

```bash
# Example: Test parsing each document
for file in *.taml; do
    echo "Testing $file"
    your-parser-command "$file"
done
```

### Validation Checklist

- [ ] All 7 test documents parse without errors
- [ ] Nested structures are correctly represented
- [ ] Lists are properly identified and structured
- [ ] Null (`~`) and empty string (`""`) are distinguished
- [ ] Comments are ignored
- [ ] Key-value pairs are correctly extracted

## Common Issues

### Tabs vs Spaces
All indentation must be tabs. If your editor converts tabs to spaces, disable that feature.

### Tab in Values
Values cannot contain tab characters. The parser should reject or error on such input.

### Parent Key Values
A key with children cannot have a value on the same line. Either have a value (no children) or have children (no value).

## Contributing

When adding new test documents:
1. Follow the naming convention: `0#-descriptive-name.taml`
2. Include comments explaining what is being tested
3. Update this README with the new test description
4. Ensure the document is valid according to TAML v0.1 spec
