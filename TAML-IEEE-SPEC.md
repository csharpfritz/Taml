# IEEE-Style Specification Document

**TAML Specification v0.2**  
**Tab Accessible Markup Language**

---

## Document Information

| Field | Value |
|-------|-------|
| **Document Title** | TAML (Tab Accessible Markup Language) Specification |
| **Version** | 0.2 |
| **Date** | March 5, 2026 |
| **Status** | Draft Standard |
| **Document Type** | Language Specification |
| **Classification** | Public |

---

## Abstract

This document specifies the Tab Accessible Markup Language (TAML), a minimalist hierarchical data serialization format designed for human readability and machine parseability. TAML uses exclusively tab characters and newlines for structural demarcation, eliminating the need for brackets, braces, colons, quotes (except for empty strings), or other markup symbols. This specification defines the syntax, semantics, validation rules, and conformance requirements for TAML processors.

**Keywords:** data serialization, markup language, tab-delimited, hierarchical data, configuration format

---

## Table of Contents

1. [Introduction](#1-introduction)
   - 1.1 [Scope](#11-scope)
   - 1.2 [Purpose](#12-purpose)
   - 1.3 [Design Goals](#13-design-goals)
   - 1.4 [Conformance](#14-conformance)
2. [Normative References](#2-normative-references)
3. [Terms and Definitions](#3-terms-and-definitions)
4. [Notation and Conventions](#4-notation-and-conventions)
5. [Character Set and Encoding](#5-character-set-and-encoding)
6. [Lexical Structure](#6-lexical-structure)
   - 6.1 [Whitespace](#61-whitespace)
   - 6.2 [Line Terminators](#62-line-terminators)
   - 6.3 [Comments](#63-comments)
   - 6.4 [Tokens](#64-tokens)
7. [Syntax Specification](#7-syntax-specification)
   - 7.1 [Document Structure](#71-document-structure)
   - 7.2 [Key-Value Pairs](#72-key-value-pairs)
   - 7.3 [Hierarchical Structures](#73-hierarchical-structures)
   - 7.4 [List Structures](#74-list-structures)
   - 7.5 [Special Values](#75-special-values)
   - 7.6 [Raw Text Blocks](#76-raw-text-blocks)
   - 7.7 [Data Structure Types](#77-data-structure-types)
8. [Semantic Rules](#8-semantic-rules)
   - 8.1 [Indentation Semantics](#81-indentation-semantics)
   - 8.2 [Value Interpretation](#82-value-interpretation)
   - 8.3 [Type System](#83-type-system)
9. [Validation Requirements](#9-validation-requirements)
   - 9.1 [Structural Validation](#91-structural-validation)
   - 9.2 [Lexical Validation](#92-lexical-validation)
   - 9.3 [Semantic Validation](#93-semantic-validation)
   - 9.4 [Raw Text Validation](#94-raw-text-validation)
10. [Error Handling](#10-error-handling)
    - 10.1 [Error Categories](#101-error-categories)
    - 10.2 [Error Reporting](#102-error-reporting)
11. [Conformance Requirements](#11-conformance-requirements)
    - 11.1 [Parser Conformance](#111-parser-conformance)
    - 11.2 [Serializer Conformance](#112-serializer-conformance)
12. [Security Considerations](#12-security-considerations)
13. [IANA Considerations](#13-iana-considerations)
14. [Examples](#14-examples)
15. [Appendices](#15-appendices)
    - [Appendix A: Formal Grammar](#appendix-a-formal-grammar)
    - [Appendix B: Comparison with Other Formats](#appendix-b-comparison-with-other-formats)
    - [Appendix C: Migration Guide](#appendix-c-migration-guide)

---

## 1. Introduction

### 1.1 Scope

This specification defines the Tab Accessible Markup Language (TAML), a data serialization format for representing hierarchical data structures using tab-based indentation. This document specifies:

- The complete syntax and grammar of TAML
- Semantic interpretation rules for TAML documents
- Validation requirements for TAML processors
- Conformance criteria for implementations
- Error handling and reporting mechanisms

This specification does not define:

- Application-specific schemas or validation beyond structural correctness
- Binary encoding formats
- Network transmission protocols
- Query languages or transformation mechanisms

### 1.2 Purpose

The purpose of this specification is to provide a complete and unambiguous definition of TAML suitable for:

- Implementation of parsers and serializers
- Validation of TAML documents
- Interchange of structured data between systems
- Human authoring of configuration and data files

### 1.3 Design Goals

TAML is designed with the following objectives:

1. **Simplicity**: Minimize the number of syntactic constructs and special characters
2. **Readability**: Ensure human readers can quickly understand document structure
3. **Unambiguity**: Provide clear rules with no edge cases requiring complex interpretation
4. **Accessibility**: Use common keyboard characters without requiring special input methods
5. **Error Prevention**: Design syntax to minimize common authoring errors
6. **Tool Friendliness**: Enable straightforward implementation of parsers and validators

### 1.4 Conformance

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in RFC 2119.

A conforming TAML processor MUST implement all required features specified in this document. Optional features MAY be implemented at the discretion of the implementer.

---

## 2. Normative References

The following documents are referred to in the text in such a way that some or all of their content constitutes requirements of this document:

- **RFC 2119**: Key words for use in RFCs to Indicate Requirement Levels
- **RFC 3629**: UTF-8, a transformation format of ISO 10646
- **ISO 8601**: Data elements and interchange formats — Information interchange — Representation of dates and times
- **Unicode Standard**: The Unicode Consortium. The Unicode Standard, Version 15.0.0
- **ISO/IEC 10646**: Information technology — Universal Coded Character Set (UCS)

---

## 3. Terms and Definitions

For the purposes of this document, the following terms and definitions apply:

**TAML Document**: A sequence of Unicode characters conforming to the syntax defined in this specification.

**Key**: An identifier that names a data element. Keys MUST NOT contain tab characters or line terminators. Keys MAY contain any other Unicode characters, including spaces, punctuation, hash characters, and non-ASCII characters.

**Value**: The data content associated with a key. Values MUST NOT contain tab characters.

**Node**: A single key-value pair or a key with child nodes.

**Parent Node**: A node that contains one or more child nodes.

**Child Node**: A node that is nested within another node.

**Leaf Node**: A node that has no children and contains a value.

**Map**: A collection of unique key-value pairs or unique key-object pairs. Keys serve as unique identifiers within the map.

**Collection**: An ordered sequence of values or objects where order is semantically significant and items are accessed by position/index.

**List**: A sequence of values or nodes at the same indentation level under a parent key. (See Map and Collection for semantic distinction.)

**Indentation Level**: The number of tab characters preceding content on a line.

**Key-Value Separator**: One or more tab characters separating a key from its value.

**Null Value**: A special value represented by the tilde character (`~`) indicating the absence of data.

**Empty String**: A special value represented by two double-quote characters (`""`) indicating a zero-length string.

**Raw Text Block**: A multiline value introduced by the `...` (three periods) indicator, in which tab characters and newlines are preserved as literal content.

**Raw Text Indicator**: The exact three-character sequence `...` (U+002E U+002E U+002E) used as a value to introduce a raw text block.

**Comment**: A line beginning with the hash character (`#`) that is ignored by parsers.

**Bare Key**: A key that appears on a line without a tab-separated value and without the `...` raw text indicator. A bare key either has children (parent node) or is a string value in a collection context.

**Duplicate Bare Keys**: Two or more bare keys with the same name at the same indentation level under the same parent, indicating a collection of objects.

---

## 4. Notation and Conventions

### 4.1 Syntactic Notation

This specification uses the following notational conventions:

- `\t` represents the horizontal tab character (U+0009)
- `\n` represents the line feed character (U+000A)
- `\r` represents the carriage return character (U+000D)
- `~` represents the tilde character (U+007E) used for null values
- `""` represents two consecutive double-quote characters (U+0022) for empty strings
- `#` represents the hash/number sign character (U+0023) for comments
- `...` represents three consecutive period characters (U+002E) for raw text blocks

### 4.2 Grammar Notation

The grammar in this specification uses Extended Backus-Naur Form (EBNF) with the following conventions:

- `::=` denotes a production rule
- `|` denotes alternation (OR)
- `[ ]` denotes optional elements
- `{ }` denotes zero or more repetitions
- `( )` denotes grouping
- `" "` denotes literal characters

### 4.3 Character References

Unless otherwise specified, all character codes refer to Unicode code points.

---

## 5. Character Set and Encoding

### 5.1 Character Encoding

TAML documents MUST be encoded in UTF-8 as specified in RFC 3629. Implementations MAY support other Unicode encodings (UTF-16, UTF-32) but MUST provide UTF-8 support as the baseline.

### 5.2 Character Restrictions

The following characters and sequences have special meaning in TAML:

- **Tab (U+0009)**: Used for indentation and key-value separation
- **Line Feed (U+000A)**: Line terminator
- **Carriage Return (U+000D)**: Line terminator (when followed by line feed)
- **Hash (U+0023)**: Comment indicator (at start of line only)
- **Period sequence `...` (U+002E × 3)**: Raw text block indicator (when used as entire value)
- **Tilde (U+007E)**: Null value indicator (when used as entire value)
- **Double Quote pair `""` (U+0022 × 2)**: Empty string indicator (when used as entire value)

### 5.3 Prohibited Characters in Context

- Tab characters (U+0009) MUST NOT appear within keys or values
- Space characters (U+0020) MUST NOT be used for indentation

---

## 6. Lexical Structure

### 6.1 Whitespace

#### 6.1.1 Indentation Whitespace

Indentation MUST use the horizontal tab character (U+0009) exclusively. Each level of nesting MUST increase indentation by exactly one tab character.

**Requirement 6.1.1-1**: Implementations MUST reject documents using space characters (U+0020) for indentation.

**Requirement 6.1.1-2**: Implementations MUST reject documents mixing tabs and spaces in indentation.

#### 6.1.2 Separator Whitespace

Key-value separators MUST consist of one or more tab characters. Multiple consecutive tabs MAY be used for visual alignment purposes and SHALL be treated equivalently to a single tab.

**Requirement 6.1.2-1**: At least one tab character MUST separate a key from its value.

### 6.2 Line Terminators

#### 6.2.1 Recognized Terminators

TAML recognizes the following line terminator sequences:

- Line Feed (LF): U+000A
- Carriage Return + Line Feed (CRLF): U+000D U+000A

**Requirement 6.2.1-1**: Implementations MUST accept both LF and CRLF line terminators.

**Requirement 6.2.1-2**: Standalone CR (U+000D) without following LF is invalid.

#### 6.2.2 Blank Lines

Lines containing only whitespace or no characters are considered blank lines and SHALL be ignored by parsers.

### 6.3 Comments

#### 6.3.1 Comment Syntax

A comment line begins with the hash character (`#`) as the first non-whitespace character.

```ebnf
comment ::= {TAB} "#" {any-character} line-terminator
```

**Requirement 6.3.1-1**: Comments MUST start at the beginning of a line (after optional indentation).

**Requirement 6.3.1-2**: Mid-line comments are NOT supported. Hash characters within keys or values are literal.

#### 6.3.2 Comment Processing

**Requirement 6.3.2-1**: Parsers MUST ignore comment lines entirely.

**Requirement 6.3.2-2**: Comments MUST NOT affect the interpretation of surrounding lines.

### 6.4 Tokens

#### 6.4.1 Key Token

A key is a non-empty sequence of Unicode characters excluding:
- Tab characters (U+0009)
- Line terminators (U+000A, U+000D)

All other Unicode characters — including space characters (U+0020), hash characters (U+0023), punctuation, hyphens, and non-ASCII characters — are permitted as literal characters within keys.

```ebnf
key ::= key-character {key-character}
key-character ::= any-character - (TAB | line-terminator)
```

**Requirement 6.4.1-1**: Keys MUST contain at least one character.

**Requirement 6.4.1-2**: Keys MUST NOT contain tab characters (U+0009) or line terminator characters (U+000A, U+000D).

**Requirement 6.4.1-3**: The hash character (`#`) within a key is literal. Only a `#` at the very start of a line (after optional indentation) SHALL introduce a comment.

**Requirement 6.4.1-4**: Implementations SHOULD recommend alphanumeric keys with underscores or hyphens for maximum interoperability but MUST accept any key conforming to the grammar above.

#### 6.4.2 Value Token

A value is a sequence of Unicode characters (possibly empty) excluding:
- Tab characters (U+0009)
- Line terminators

```ebnf
value ::= {value-character} | null-value | empty-string | raw-text-indicator
value-character ::= any-character - (TAB | line-terminator)
```

**Requirement 6.4.2-1**: Values MAY be empty (zero-length string after separator).

**Requirement 6.4.2-2**: Implementations SHOULD trim trailing whitespace from values by default. Trailing whitespace is nearly always unintentional (editor artifacts, copy-paste errors) and preserving it creates cross-platform inconsistencies. Implementations that need to preserve trailing whitespace MUST provide an explicit opt-in option.

**Requirement 6.4.2-3**: For values requiring intentional trailing whitespace, raw text blocks (Section 7.6) SHALL be used.

---

## 7. Syntax Specification

### 7.1 Document Structure

#### 7.1.1 Document Definition

A TAML document consists of zero or more nodes, comments, and blank lines.

```ebnf
document ::= {line}
line ::= blank-line | comment | node
blank-line ::= {whitespace} line-terminator
```

#### 7.1.2 Root Level Nodes

Nodes at indentation level zero are root-level nodes.

**Requirement 7.1.2-1**: A document MAY contain multiple root-level nodes.

**Requirement 7.1.2-2**: Root-level nodes MUST have zero indentation.

### 7.2 Key-Value Pairs

#### 7.2.1 Simple Key-Value Syntax

A simple key-value pair consists of a key, separator tabs, and a value on the same line.

```ebnf
key-value-pair ::= {TAB}^n key TAB {TAB} value line-terminator
```

Where `n` is the indentation level (number of leading tabs).

**Example:**
```taml
name	John Doe
age	30
email	john@example.com
```

#### 7.2.2 Visual Alignment

Multiple tabs between key and value MAY be used for visual alignment.

**Example:**
```taml
short		value1
medium_key	value2
very_long_key	value3
```

**Requirement 7.2.2-1**: Implementations MUST treat multiple consecutive separator tabs as equivalent to a single tab.

### 7.3 Hierarchical Structures

#### 7.3.1 Parent Node Syntax

A parent node is a key without a value on the same line, followed by one or more child nodes at the next indentation level.

```ebnf
parent-node ::= {TAB}^n key line-terminator {child-node}^1+
child-node ::= {TAB}^(n+1) node
```

**Example:**
```taml
server
	host	localhost
	port	8080
```

**Requirement 7.3.1-1**: A parent node SHOULD have at least one child node. A parent node with no children SHALL resolve to an empty map (empty dictionary/object).

**Requirement 7.3.1-2**: A parent node MUST NOT have a value on the same line.

**Requirement 7.3.1-3**: Child nodes MUST be indented exactly one tab more than their parent.

**Requirement 7.3.1-4**: An empty parent (a bare key followed by no indented children) MUST resolve to an empty map in the parsed output. This ensures deterministic behavior across all implementations. If an empty list is needed, the parent SHALL be omitted entirely or represented explicitly with application-level conventions.

#### 7.3.2 Nested Hierarchies

Hierarchies MAY be nested to arbitrary depth.

**Example:**
```taml
application
	server
		http
			host	localhost
			port	8080
		database
			host	db.example.com
			port	5432
```

**Requirement 7.3.2-1**: Each nesting level MUST increase indentation by exactly one tab.

**Requirement 7.3.2-2**: Implementations SHOULD NOT impose arbitrary depth limits.

### 7.4 List Structures

#### 7.4.1 List Syntax

A list consists of a parent key followed by value-only child lines.

```ebnf
list ::= {TAB}^n key line-terminator {list-item}^1+
list-item ::= {TAB}^(n+1) value line-terminator
```

**Example:**
```taml
colors
	red
	green
	blue
```

**Requirement 7.4.1-1**: List items MUST NOT have keys (value-only lines).

**Requirement 7.4.1-2**: List items MUST be indented exactly one tab from the parent.

#### 7.4.2 Complex Lists

List items MAY themselves be parent nodes with children.

**Example:**
```taml
servers
	development
		host	dev.example.com
		port	3000
	production
		host	prod.example.com
		port	443
```

#### 7.4.3 Collection of Objects via Duplicate Bare Keys

When multiple bare keys with the same name appear at the same indentation level under the same parent, the parent SHALL be interpreted as a collection (ordered array) of objects.

```ebnf
collection-of-objects ::= {TAB}^n key line-terminator {collection-item}^2+
collection-item      ::= {TAB}^(n+1) duplicate-bare-key line-terminator {child-node}+
```

**Example:**
```taml
users
	user
		name	Alice
		email	alice@example.com
		role	admin
	user
		name	Bob
		email	bob@example.com
		role	user
```

**Requirement 7.4.3-1**: Duplicate bare keys at the same indentation level under the same parent SHALL cause the parent to be interpreted as a collection of objects.

**Requirement 7.4.3-2**: Each duplicate bare key instance MUST have children; a duplicate bare key without children is invalid in a collection context.

**Requirement 7.4.3-3**: The order of duplicate bare key instances SHALL be preserved in the resulting collection.

**Requirement 7.4.3-4**: Parsers MUST detect duplicate bare keys during parsing and convert the parent node's representation accordingly.

### 7.5 Special Values

#### 7.5.1 Null Values

The tilde character (`~`) represents a null value (absence of data).

```ebnf
null-value ::= "~"
```

**Example:**
```taml
username	alice
password	~
email	alice@example.com
```

**Requirement 7.5.1-1**: The tilde character MUST be the only content in the value position to represent null.

**Requirement 7.5.1-2**: Null values are semantically distinct from empty strings.

#### 7.5.2 Empty Strings

Two consecutive double-quote characters (`""`) represent an empty string.

```ebnf
empty-string ::= '""'
```

**Example:**
```taml
username	alice
nickname	""
bio	Software developer
```

**Requirement 7.5.2-1**: Empty strings MUST be represented by exactly two double-quote characters.

**Requirement 7.5.2-2**: Empty strings are semantically distinct from null values.

#### 7.5.3 Regular String Values

Non-empty string values do not require quotes.

**Example:**
```taml
message	Hello, world!
status	active
version	1.0.0
```

**Requirement 7.5.3-1**: Regular string values MUST NOT be enclosed in quotes.

**Requirement 7.5.3-2**: Trailing whitespace in values SHOULD be trimmed by default (see Requirement 6.4.2-2).

### 7.6 Raw Text Blocks

#### 7.6.1 Raw Text Syntax

A raw text block provides a mechanism for values that need to contain tab characters, newlines, or other characters that would otherwise be structurally significant. A raw text block is introduced by the raw text indicator `...` (three period characters) as the value of a key.

```ebnf
raw-text-indicator ::= "..."
raw-text-block     ::= {TAB}^n key TAB {TAB} raw-text-indicator line-terminator {raw-text-line}+
raw-text-line      ::= {TAB}^(n+1) {TAB} {any-character} line-terminator
```

**Example:**
```taml
script	...
	if [ $1 -eq 1 ]; then
		echo "Tab indented"
	fi
description	This is after the raw block
```

**Requirement 7.6.1-1**: The raw text indicator MUST be exactly the three-character sequence `...` (U+002E U+002E U+002E) with no additional characters preceding or following it on the same line.

**Requirement 7.6.1-2**: Raw text lines MUST be indented at least one tab more than the key bearing the `...` value.

**Requirement 7.6.1-3**: The first tab beyond the parent key's indentation level is structural indentation and SHALL NOT be included in the raw text content.

**Requirement 7.6.1-4**: Any additional tab characters beyond the structural indentation tab SHALL be preserved as literal tab characters in the raw text content.

**Requirement 7.6.1-5**: All newline characters between raw text lines SHALL be preserved in the resulting value.

**Requirement 7.6.1-6**: The raw text block terminates at the first line whose indentation level is equal to or less than the indentation level of the key bearing `...`.

**Requirement 7.6.1-7**: If no indented lines follow `...`, the resulting value SHALL be an empty string.

#### 7.6.2 Raw Text Content Preservation

Within a raw text block, all content — including tab characters, hash characters, tilde characters, and double-quote pairs — SHALL be treated as literal text. The special meanings defined in Sections 6.3, 7.5.1, and 7.5.2 do not apply within raw text blocks.

**Example — Literal special characters:**
```taml
content	...
	""
```
The value of `content` is the literal two-character string `""`, not an empty string.

```taml
content	...
	~
```
The value of `content` is the literal one-character string `~`, not null.

**Requirement 7.6.2-1**: Parsers MUST preserve all content within raw text blocks without interpreting special value indicators (`~`, `""`, `#`).

**Requirement 7.6.2-2**: Type coercion (Section 8.2.2) MUST NOT be applied to raw text block values. Raw text values are always strings.

#### 7.6.3 Raw Text Indentation Examples

**Single-line raw text:**
```taml
message	...
	Hello	World	with	tabs
```
Result: `"Hello\tWorld\twith\ttabs"`

**Multi-line raw text with additional indentation:**
```taml
indented	...
		This line starts with a tab character
	This line has no leading tabs
```
Result: First line is `"\tThis line starts with a tab character"`, second line is `"This line has no leading tabs"`, separated by a newline.

**Raw text in nested context:**
```taml
config
	database
		host	localhost
		port	5432
	sql_query	...
		SELECT *
		FROM users u
		WHERE u.created_at > '2024-01-01'
			AND u.status = 'active'
		ORDER BY u.name
	another_setting	normal_value
```

### 7.7 Data Structure Types

TAML supports two fundamental data structure types that are automatically detected based on the structure and naming of immediate children.

#### 7.7.1 Maps

A Map is a collection of unique key-value pairs or unique key-object pairs.

**Syntax Options:**
- **Map of Values**: All children are unique keys with tab-separated values.
- **Map of Objects**: All children are unique bare keys, each with its own children.

**Characteristics:**
- Keys SHALL be unique identifiers within the map.
- Order is preserved during parsing but is not semantically significant.

**Example — Map of Values:**
```taml
server
	host	localhost
	port	8080
	ssl	true
```

**Example — Map of Objects:**
```taml
environments
	development
		debug	true
		log_level	verbose
	production
		debug	false
		log_level	error
```

**Requirement 7.7.1-1**: A node whose children are all unique keys with tab-separated values SHALL be interpreted as a Map of Values.

**Requirement 7.7.1-2**: A node whose children are all unique bare keys with children SHALL be interpreted as a Map of Objects.

#### 7.7.2 Collections

A Collection is an ordered list of values or objects where order is semantically significant.

**Syntax Options:**
- **Collection of Strings**: All children are text-only lines with no tab separators and no children. The line content becomes the string value.
- **Collection of Objects**: Children contain duplicate bare keys, each with children (see Section 7.4.3).

**Example — Collection of Strings:**
```taml
features
	authentication
	logging
	caching
```

**Example — Collection of Objects:**
```taml
games
	game
		home	Philadelphia
		away	Dallas
		score	120
	game
		home	New York
		away	Boston
		score	~
```

**Requirement 7.7.2-1**: A node whose children are all text-only lines (no tab separators, no children of their own) SHALL be interpreted as a Collection of Strings.

**Requirement 7.7.2-2**: A node whose children contain duplicate bare keys with children SHALL be interpreted as a Collection of Objects.

**Requirement 7.7.2-3**: The order of items in a Collection SHALL be preserved and is semantically significant.

#### 7.7.3 Structure Detection Rules

Parsers SHALL determine the structure type of a parent node by examining its immediate children according to the following precedence:

1. **Map of Values**: ALL children are unique keys with tab-separated values.
2. **Map of Objects**: ALL children are unique bare keys with children.
3. **Collection of Strings**: ALL children are text-only lines (no tab separators, no children).
4. **Collection of Objects**: Children contain duplicate bare keys with children.
5. **Mixed Structure**: Any combination that does not fit the above patterns is **invalid**.

**Requirement 7.7.3-1**: Structure type determination SHALL occur at parse time based on immediate children only. Grandchildren and deeper descendants SHALL NOT affect parent structure type detection.

**Requirement 7.7.3-2**: The first child encountered typically determines the expected structure type for validation, except when duplicate bare keys are detected later among siblings.

**Requirement 7.7.3-3**: A parent node with mixed child types (e.g., some children with keys and values, others as bare text) SHALL be rejected as invalid in strict mode.

---

## 8. Semantic Rules

### 8.1 Indentation Semantics

#### 8.1.1 Hierarchy Representation

Indentation level determines the hierarchical relationship between nodes.

**Requirement 8.1.1-1**: A node at indentation level `n+1` is a child of the nearest preceding node at level `n`.

**Requirement 8.1.1-2**: Sibling nodes MUST have the same indentation level.

#### 8.1.2 Scope Rules

A parent node's scope extends to the next node at the same or lower indentation level.

**Example:**
```taml
parent1
	child1a
	child1b
parent2
	child2a
```

### 8.2 Value Interpretation

#### 8.2.1 String Values

All values are initially interpreted as strings.

**Requirement 8.2.1-1**: Parsers MUST preserve the exact byte sequence of value content (excluding trailing whitespace if trimmed).

#### 8.2.2 Type Coercion

Implementations MAY provide type coercion for common data types. When enabled, parsers SHALL attempt type detection in the following precedence order (first match wins):

1. **Null**: `^~$`
2. **Empty String**: `^""$`
3. **Raw Text**: Value is `...` followed by indented content (Section 7.6)
4. **Boolean**: Case-insensitive match against recognized boolean patterns
5. **Date/Time**: ISO 8601 formatted date, time, or combined values (Section 8.2.3)
6. **Number**: Integer and decimal number patterns
7. **String**: Any other text value (default)

**Boolean Patterns (case-insensitive):**

| Value | Boolean Result |
|-------|---------------|
| `true` | `true` |
| `false` | `false` |
| `yes` | `true` |
| `no` | `false` |
| `on` | `true` |
| `off` | `false` |

**Requirement 8.2.2-1**: Type coercion MUST be optional and configurable.

**Requirement 8.2.2-2**: Implementations MUST provide access to raw string values regardless of type coercion settings.

**Requirement 8.2.2-3**: Boolean detection MUST be case-insensitive. The values `true`, `TRUE`, `True`, and any other casing SHALL all be recognized as boolean true. The same applies to all other boolean patterns.

**Requirement 8.2.2-4**: The values `1` and `0` MUST be parsed as integers, not booleans. Because TAML has no schema system, there is no mechanism to determine when a numeric value contextually represents a boolean. Compliant parsers MUST NOT treat `1` or `0` as boolean values in the absence of a schema. Implementations MAY provide an opt-in parser option (e.g., `numeric_booleans: true`) to enable `1`/`0` as boolean aliases.

**Requirement 8.2.2-5**: When serializing boolean values, implementations SHOULD use the canonical lowercase forms `true` and `false`.

**Number Patterns:**

- **Integer**: `^[+-]?[0-9]+$` — Optional sign followed by one or more digits, no decimal point, no leading zeros (except `0` itself)
- **Decimal/Float**: `^[+-]?(?:\d+\.?\d*|\d*\.\d+)(?:[eE][+-]?\d+)?$` — Optional sign, digits with optional decimal point, optional scientific notation

**Requirement 8.2.2-6**: A bare four-digit value (e.g., `2024`) MUST be parsed as an integer, not a date. The minimum pattern for automatic date detection is `YYYY-MM` (see Section 8.2.3).

#### 8.2.3 Date and Time Detection (ISO 8601)

Implementations providing type coercion SHOULD support automatic detection of date and time values conforming to ISO 8601. This enables unambiguous representation of temporal data across different locales and systems.

##### 8.2.3.1 Supported Formats

**Date Only (Calendar Dates):**
- Extended format: `YYYY-MM-DD` (e.g., `2024-01-15`)
- Year and month: `YYYY-MM` (e.g., `2024-01`) — minimum pattern for date detection

**Time Only:**
- With seconds: `HH:MM:SS` (e.g., `14:30:00`)
- Without seconds: `HH:MM` (e.g., `14:30`)
- With fractional seconds: `HH:MM:SS.sss` (e.g., `14:30:00.123`)

**Combined Date and Time:**
- Date and time with `T` separator: `YYYY-MM-DDTHH:MM:SS` (e.g., `2024-01-15T14:30:00`)
- With fractional seconds: `YYYY-MM-DDTHH:MM:SS.sss`

**With Time Zone / UTC Offset:**
- UTC (Zulu time): Trailing `Z` (e.g., `2024-01-15T14:30:00Z`)
- Positive offset: `+HH:MM` (e.g., `2024-01-15T23:30:00+09:00`)
- Negative offset: `-HH:MM` (e.g., `2024-01-15T09:30:00-05:00`)

**Week Dates:**
- Year and week: `YYYY-Www` (e.g., `2024-W03`)
- Year, week, and day: `YYYY-Www-D` (e.g., `2024-W03-4`, where 1=Monday, 7=Sunday)

**Ordinal Dates:**
- Year and day of year: `YYYY-DDD` (e.g., `2024-015`)

**Durations:**
- Period designator format: Starting with `P`, containing designators `Y`, `M`, `D`, `T`, `H`, `M`, `S` (e.g., `P3Y6M4DT12H30M5S`, `PT1H30M`, `P7D`)

**Time Intervals:**
- Start and end: `<datetime>/<datetime>` (e.g., `2024-01-01/2024-12-31`)
- Start and duration: `<datetime>/<duration>` (e.g., `2024-01-01/P1M`)
- Duration and end: `<duration>/<datetime>` (e.g., `P1M/2024-02-01`)

##### 8.2.3.2 Detection Rules

**Requirement 8.2.3-1**: Parsers SHOULD recognize ISO 8601 formats by the following patterns:
1. **Date**: `YYYY-MM-DD` or `YYYY-MM` where YYYY is 4 digits, MM is 01-12, DD is 01-31
2. **Time**: `HH:MM:SS` or `HH:MM`, with optional fractional seconds `.sss`
3. **Combined**: Date + `T` + Time
4. **UTC Indicator**: Trailing `Z` (case-insensitive)
5. **Offset**: `±HH:MM` or `±HHMM` or `±HH`
6. **Week Date**: `YYYY-Www` or `YYYY-Www-D`
7. **Ordinal Date**: `YYYY-DDD`
8. **Duration**: Starting with `P`, containing valid designators
9. **Interval**: Two date/times or date/time and duration separated by `/`

**Requirement 8.2.3-2**: A bare four-digit year value (e.g., `2024`) MUST NOT be detected as a date. The minimum date pattern is `YYYY-MM`.

**Requirement 8.2.3-3**: Date/time detection SHOULD take precedence over number detection but SHALL follow boolean detection in the type detection order.

##### 8.2.3.3 Serialization Requirements

**Requirement 8.2.3-4**: When serializing date/time values, implementations SHOULD:
- Use the extended format with separators (hyphens and colons)
- Use uppercase `T` as the date-time separator
- Use uppercase `Z` for UTC
- Preserve the original precision and time zone information when possible

### 8.3 Type System

#### 8.3.1 Supported Types

TAML recognizes the following conceptual data types:

1. **String**: The default type for all values
2. **Null**: Represented by `~`
3. **Empty String**: Represented by `""`
4. **Boolean**: Represented by `true`/`false`/`yes`/`no`/`on`/`off` (case-insensitive)
5. **Integer**: Whole numbers (e.g., `42`, `-17`, `0`)
6. **Float/Decimal**: Decimal numbers (e.g., `3.14`, `-0.5`, `6.022e23`)
7. **Date/Time**: ISO 8601 formatted temporal values (e.g., `2024-01-15`, `14:30:00`, `2024-01-15T14:30:00Z`)
8. **Duration**: ISO 8601 duration values (e.g., `P1DT2H30M`)
9. **Raw Text**: Multiline string preserving tabs and newlines, introduced by `...`
10. **Map**: A node with uniquely-keyed children (key-value pairs or key-object pairs)
11. **Collection**: A node with ordered children (value-only lines or duplicate bare keys)

#### 8.3.2 Type Inference Rules

**Requirement 8.3.2-1**: A node with children containing unique keys with values is a Map of Values.

**Requirement 8.3.2-2**: A node with children that are unique bare keys with children is a Map of Objects.

**Requirement 8.3.2-3**: A node with children that are value-only (no tab separator, no children) is a Collection of Strings.

**Requirement 8.3.2-4**: A node with children containing duplicate bare keys with children is a Collection of Objects.

**Requirement 8.3.2-5**: A node with a value and no children is a leaf node (scalar value).

**Requirement 8.3.2-6**: An empty parent node (bare key with no children) SHALL resolve to an empty Map.

---

## 9. Validation Requirements

### 9.1 Structural Validation

#### 9.1.1 Indentation Validation

**Rule 9.1.1-1**: All indentation MUST use tab characters exclusively.

**Error Condition**: Space character (U+0020) used for indentation.

**Rule 9.1.1-2**: Indentation MUST increase by exactly one tab between parent and child.

**Error Condition**: Indentation increase other than one tab.

**Rule 9.1.1-3**: Mixed tabs and spaces MUST be rejected.

**Error Condition**: Line begins with combination of spaces and tabs.

#### 9.1.2 Hierarchy Validation

**Rule 9.1.2-1**: Child nodes MUST have a valid parent.

**Error Condition**: Indented line follows key-value pair with no intermediate parent.

**Rule 9.1.2-2**: Parent nodes MUST NOT have values on the same line.

**Error Condition**: Line has both children and value content.

### 9.2 Lexical Validation

#### 9.2.1 Character Validation

**Rule 9.2.1-1**: Keys MUST NOT contain tab characters.

**Error Condition**: Tab character within key content.

**Rule 9.2.1-2**: Values MUST NOT contain tab characters.

**Error Condition**: Tab character within value content.

#### 9.2.2 Content Validation

**Rule 9.2.2-1**: Keys MUST NOT be empty.

**Error Condition**: Line begins with tab separator without preceding key content.

**Rule 9.2.2-2**: Values MAY be empty (but consider using `""` for clarity).

### 9.3 Semantic Validation

#### 9.3.1 Structural Consistency

**Rule 9.3.1-1**: All children of a node MUST be consistently formatted according to the structure detection rules in Section 7.7.3.

**Error Condition**: Mixed list items (some with keys, some without) under same parent.

**Rule 9.3.1-2**: In strict mode, duplicate bare keys with children are valid only when forming a Collection of Objects. Mixing duplicate bare keys with unique key-value pairs under the same parent SHALL be rejected.

**Error Condition**: Parent has both duplicate bare key children and key-value pair children.

### 9.4 Raw Text Validation

#### 9.4.1 Raw Text Indicator Validation

**Rule 9.4.1-1**: The raw text indicator MUST be exactly the three-character sequence `...` with no preceding or following characters on the value portion of the line.

**Error Condition**: Value contains `...` with additional characters (e.g., `...more`, `.. .`).

#### 9.4.2 Raw Text Indentation Validation

**Rule 9.4.2-1**: All raw text content lines MUST be indented at least one tab more than the key bearing the `...` value.

**Error Condition**: Raw text line has insufficient indentation relative to the parent key.

**Rule 9.4.2-2**: Mixed tabs and spaces within raw text indentation MUST be rejected per the same rules as standard indentation (Rule 9.1.1-3).

**Error Condition**: Raw text line begins with combination of spaces and tabs.

---

## 10. Error Handling

### 10.1 Error Categories

TAML validation errors are classified into the following categories:

#### 10.1.1 Lexical Errors

Errors in character usage and tokenization:
- `SPACE_INDENTATION`: Space character used for indentation
- `MIXED_INDENTATION`: Mixed tabs and spaces in indentation
- `TAB_IN_KEY`: Tab character within key content
- `TAB_IN_VALUE`: Tab character within value content
- `INVALID_LINE_TERMINATOR`: Standalone CR without LF

#### 10.1.2 Structural Errors

Errors in document structure:
- `INCONSISTENT_INDENTATION`: Indentation increase other than one tab
- `ORPHANED_INDENTATION`: Indented line without valid parent
- `PARENT_WITH_VALUE`: Parent key has value on same line
- `EMPTY_KEY`: Line has no key content
- `MISSING_SEPARATOR`: Key-value pair missing tab separator
- `INVALID_RAW_TEXT_INDICATOR`: Raw text indicator is not exactly `...` (e.g., contains extra characters)
- `INVALID_RAW_TEXT_INDENTATION`: Raw text content line has insufficient indentation relative to the parent key

#### 10.1.3 Semantic Errors

Errors in meaning and interpretation:
- `MIXED_CHILDREN`: Node has both object and array children (inconsistent structure type)
- `INVALID_NULL`: Malformed null value (not exactly `~`)
- `INVALID_EMPTY_STRING`: Malformed empty string (not exactly `""`)
- `DUPLICATE_KEY_IN_MAP`: Duplicate key names within a Map context (where children have tab-separated values)

### 10.2 Error Reporting

#### 10.2.1 Required Error Information

**Requirement 10.2.1-1**: Error reports MUST include:
- Error category and specific error code
- Line number (1-indexed)
- Column number (1-indexed) when applicable
- Human-readable error message
- Excerpt of problematic line

#### 10.2.2 Error Message Format

**Recommended Format:**
```
Line {line}: {ERROR_CODE}: {description}
  {line_content}
  {position_indicator}
```

**Example:**
```
Line 5: SPACE_INDENTATION: Indentation must use tabs, not spaces
    host	localhost
^^^^
```

### 10.3 Parser Modes

#### 10.3.1 Strict Mode

**Requirement 10.3.1-1**: In strict mode, parsers MUST reject invalid documents immediately upon encountering the first error.

#### 10.3.2 Lenient Mode

**Requirement 10.3.2-1**: In lenient mode, parsers MAY:
- Skip invalid lines and continue parsing
- Collect multiple errors before reporting
- Apply heuristic error recovery

**Requirement 10.3.2-2**: Lenient mode SHOULD emit warnings for recovered errors.

---

## 11. Conformance Requirements

### 11.1 Parser Conformance

A conforming TAML parser MUST:

1. Accept all valid TAML documents as defined in Section 7
2. Reject all documents violating rules in Section 9
3. Support UTF-8 encoding (Section 5.1)
4. Recognize both LF and CRLF line terminators (Section 6.2.1)
5. Ignore comment lines (Section 6.3)
6. Handle null values (`~`) correctly (Section 7.5.1)
7. Handle empty strings (`""`) correctly (Section 7.5.2)
8. Handle raw text blocks (`...`) correctly (Section 7.6)
9. Detect and represent Maps and Collections according to Section 7.7
10. Report errors according to Section 10.2
11. Provide both strict and lenient parsing modes (Section 10.3)
12. Trim trailing whitespace from values by default (Section 6.4.2)

A conforming parser MAY:

- Support additional Unicode encodings beyond UTF-8
- Provide type coercion for common data types including extended booleans and ISO 8601 dates (Section 8.2)
- Impose reasonable depth or size limits with appropriate error messages
- Provide an opt-in option for numeric boolean interpretation of `1`/`0`

### 11.2 Serializer Conformance

A conforming TAML serializer MUST:

1. Generate documents conforming to syntax in Section 7
2. Use tab characters exclusively for indentation (Section 6.1.1)
3. Use at least one tab for key-value separation (Section 6.1.2)
4. Produce UTF-8 encoded output (Section 5.1)
5. Represent null values as `~` (Section 7.5.1)
6. Represent empty strings as `""` (Section 7.5.2)
7. Not include tab characters within keys or values (Section 5.3), except within raw text blocks (Section 7.6)
8. Properly represent hierarchical structures (Section 7.3)
9. Properly represent list structures (Section 7.4)
10. Emit raw text blocks using `...` for values containing tab or newline characters (Section 7.6)
11. Serialize collections of objects using duplicate bare keys (Section 7.4.3)
12. Serialize boolean values using canonical lowercase `true`/`false` (Section 8.2.2)

A conforming serializer MAY:

- Add comments for documentation purposes
- Use multiple tabs for visual alignment (Section 7.2.2)
- Include blank lines for readability
- Choose between LF and CRLF line terminators consistently
- Use extended ISO 8601 format for date/time serialization (Section 8.2.3)

### 11.3 Validator Conformance

A conforming TAML validator MUST:

1. Implement all validation rules in Section 9
2. Report errors according to Section 10
3. Identify error categories correctly (Section 10.1)
4. Provide line and column numbers for errors (Section 10.2.1)

---

## 12. Security Considerations

### 12.1 Denial of Service

**Consideration 12.1.1**: Implementations SHOULD impose reasonable limits on:
- Maximum document size
- Maximum nesting depth
- Maximum key/value length
- Maximum number of nodes
- Maximum raw text block size

**Recommendation**: Reject documents exceeding limits with appropriate error messages before exhausting system resources.

### 12.2 Tab Character Visibility

**Consideration 12.2.1**: Tab characters may be invisible in some text editors, potentially leading to:
- Unintentional structure modifications
- Confusion between tabs and spaces

**Recommendation**: Tools SHOULD provide visual indicators for tab characters or convert tabs to visible symbols in display mode.

### 12.3 Unicode Security

**Consideration 12.3.1**: TAML documents may contain Unicode characters that:
- Appear visually similar but are distinct code points
- Contain bidirectional override characters
- Include invisible or zero-width characters

**Recommendation**: Implementations SHOULD consider security implications of Unicode handling and MAY restrict allowed characters in keys.

### 12.4 Content Injection

**Consideration 12.4.1**: Applications using TAML MUST validate and sanitize data before:
- Executing as code
- Interpolating into queries
- Displaying in web contexts

**Recommendation**: TAML parsers SHOULD NOT provide features for code execution, file inclusion, or external entity references.

### 12.5 Raw Text Block Security

**Consideration 12.5.1**: Raw text blocks (Section 7.6) preserve all content verbatim, including characters that may be significant in other contexts (e.g., SQL, HTML, shell commands). Applications MUST NOT execute, interpolate, or render raw text block content without appropriate sanitization.

**Consideration 12.5.2**: Raw text blocks may contain arbitrarily large content. Implementations SHOULD impose size limits on raw text block values to prevent resource exhaustion.

**Recommendation**: Applications consuming raw text block values SHOULD apply context-appropriate escaping (e.g., HTML entity encoding, SQL parameterization) before using the content in security-sensitive contexts.

---

## 13. IANA Considerations

### 13.1 Media Type Registration

This specification defines the following media type for TAML documents:

**Type name:** text

**Subtype name:** x-taml

**Required parameters:** None

**Optional parameters:**
- `charset`: Character encoding (default: UTF-8)

**Encoding considerations:** TAML is UTF-8 text

**Security considerations:** See Section 12

**Interoperability considerations:** None

**Published specification:** This document

**File extension:** .taml

**Macintosh file type code:** TEXT

**Fragment identifier:** None defined

### 13.2 Alternative Media Type

**Type name:** application

**Subtype name:** x-taml

(Same parameters as text/x-taml)

---

## 14. Examples

### 14.1 Simple Configuration

```taml
# Application Configuration
application	MyWebApp
version	2.1.0
debug	false
port	8080
```

### 14.2 Nested Structure

```taml
database
	host	localhost
	port	5432
	credentials
		username	admin
		password	~
	pool
		min	5
		max	20
```

### 14.3 List Structure

```taml
allowed_origins
	https://example.com
	https://app.example.com
	http://localhost:3000
```

### 14.4 Complex Document

```taml
# Production Configuration
environment	production
version	3.0.0

server
	host	0.0.0.0
	port	443
	ssl	true
	cert_path	/etc/ssl/certs/server.crt
	key_path	/etc/ssl/private/server.key

database
	type	postgresql
	primary
		host	db-primary.internal
		port	5432
		database	webapp_prod
		username	webapp_user
		password	~
	replicas
		db-replica-1.internal
		db-replica-2.internal
		db-replica-3.internal

cache
	enabled	true
	provider	redis
	nodes
		redis-1.internal:6379
		redis-2.internal:6379
		redis-3.internal:6379

features
	user_authentication	true
	api_rate_limiting	true
	background_jobs	true
	email_notifications	false

logging
	level	info
	outputs
		stdout
		/var/log/webapp/app.log
```

### 14.5 Special Values Example

```taml
user
	id	12345
	username	johndoe
	email	john@example.com
	nickname	""
	bio	Software engineer and open source contributor
	phone	~
	verified	true
```

### 14.6 Extended Booleans Example

```taml
# All boolean variants (case-insensitive)
feature_flags
	authentication	true
	legacy_mode	false
	dark_theme	yes
	notifications	no
	maintenance	on
	beta_features	off

# Numeric values — NOT booleans
counters
	active_users	1
	failed_attempts	0
```

### 14.7 Raw Text Block Example

```taml
# Raw text preserves tabs and newlines
startup_script	...
	#!/bin/bash
	if [ "$NODE_ENV" = "production" ]; then
		npm start	--port=8080
	else
		npm run dev
	fi

sql_template	...
	SELECT u.id, u.name, u.email
	FROM users u
	WHERE u.created_at > ?
		AND u.status = 'active'
	ORDER BY u.created_at DESC
	LIMIT 100

html_template	...
	<div class="container">
		<h1>Welcome</h1>
		<p>This	preserves	tabs</p>
	</div>
```

### 14.8 Date and Time Example

```taml
event
	name	Annual Conference
	start	2024-06-15T09:00:00-04:00
	end	2024-06-17T17:00:00-04:00
	duration	P2DT8H
	registration_deadline	2024-05-01
	created_at	2024-01-15T10:30:00Z

schedule
	session
		title	Opening Keynote
		time	09:00
		duration	PT1H30M
	session
		title	Workshop
		time	11:00
		duration	PT2H
```

### 14.9 Collection of Objects Example

```taml
games
	game
		home	Philadelphia
		away	Dallas
		scorehome	120
		scoreaway	~
	game
		home	New York
		away	Boston
		scorehome	~
		scoreaway	~

database_connections
	connection
		name	primary
		host	db1.example.com
		port	5432
	connection
		name	replica
		host	db2.example.com
		port	5432
```

### 14.10 Comprehensive Document Example

```taml
# Application configuration demonstrating all v0.2 features
application	MyApp
version	2.0.0
author	Developer Name
license	~
deployed	2024-06-15T09:00:00Z

server
	host	0.0.0.0
	port	8080
	ssl	true
	maintenance_mode	off

database
	type	postgresql
	connection
		host	db.example.com
		port	5432
		database	myapp_db
		password	~

startup_script	...
	#!/bin/bash
	if [ "$NODE_ENV" = "production" ]; then
		npm start
	else
		npm run dev
	fi

features
	user-authentication
	api-gateway
	rate-limiting
	logging

environments
	development
		debug	yes
		log_level	verbose
	production
		debug	no
		log_level	error

services
	service
		name	web
		port	8080
		health_check	/health
	service
		name	api
		port	3000
		health_check	/api/status
```

---

## 15. Appendices

### Appendix A: Formal Grammar

#### A.1 Complete EBNF Grammar

```ebnf
(* TAML Grammar in Extended Backus-Naur Form — v0.2 *)

document              ::= { line }
line                  ::= blank-line | comment | node

blank-line            ::= { whitespace } line-terminator
comment               ::= { TAB } "#" { any-character } line-terminator

node                  ::= key-value-node | raw-text-node | parent-node
key-value-node        ::= indentation key separator value line-terminator
raw-text-node         ::= indentation key separator raw-text-indicator line-terminator { raw-text-line }
parent-node           ::= indentation key line-terminator { child-node }+

child-node            ::= node (* with indentation one level deeper *)
raw-text-line         ::= indentation TAB { any-character } line-terminator
                          (* indentation here is at least parent level + 1 *)

indentation           ::= { TAB }
key                   ::= key-character { key-character }
separator             ::= TAB { TAB }
value                 ::= { value-character } | null-value | empty-string

null-value            ::= "~"
empty-string          ::= '""'
raw-text-indicator    ::= "..."

key-character         ::= any-character - ( TAB | line-terminator )
value-character       ::= any-character - ( TAB | line-terminator )

(* Type detection patterns — applied when type coercion is enabled *)
boolean-value         ::= ( "true" | "false" | "yes" | "no" | "on" | "off" )
                          (* case-insensitive matching *)
integer-value         ::= [ "+" | "-" ] digit { digit }
decimal-value         ::= [ "+" | "-" ] ( digit { digit } "." { digit }
                        | { digit } "." digit { digit } )
                          [ ( "e" | "E" ) [ "+" | "-" ] digit { digit } ]
date-value            ::= year "-" month [ "-" day ]
time-value            ::= hour ":" minute [ ":" second [ "." fraction ] ]
datetime-value        ::= date-value "T" time-value [ timezone ]
timezone              ::= "Z" | ( "+" | "-" ) hour ":" minute
duration-value        ::= "P" { duration-component } [ "T" { time-component } ]
duration-component    ::= digit { digit } ( "Y" | "M" | "D" )
time-component        ::= digit { digit } ( "H" | "M" | "S" )

year                  ::= digit digit digit digit
month                 ::= digit digit     (* 01-12 *)
day                   ::= digit digit     (* 01-31 *)
hour                  ::= digit digit     (* 00-23 *)
minute                ::= digit digit     (* 00-59 *)
second                ::= digit digit     (* 00-60, allowing leap second *)
fraction              ::= digit { digit }
digit                 ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"

(* Structure detection patterns — applied at parse time *)
map-of-values         ::= parent-node where all children are unique key-value-nodes
map-of-objects        ::= parent-node where all children are unique parent-nodes
collection-of-strings ::= parent-node where all children are value-only lines
collection-of-objects ::= parent-node where children contain duplicate bare keys with children

line-terminator       ::= LF | CRLF
LF                    ::= U+000A
CRLF                  ::= U+000D U+000A
TAB                   ::= U+0009
whitespace            ::= TAB | SPACE

any-character         ::= (* any Unicode code point *)
```

#### A.2 Grammar Notes

1. The grammar is context-sensitive with respect to indentation levels
2. Child nodes must have exactly one more tab than their parent
3. Multiple consecutive tabs in separator are equivalent to a single tab
4. Blank lines and comments can appear anywhere in the document
5. Raw text blocks terminate at the first line with indentation ≤ the parent key level
6. The first tab beyond the parent key level in raw text lines is structural (not content)
7. Boolean patterns are matched case-insensitively; `1` and `0` are integers, not booleans
8. Date detection requires at minimum the `YYYY-MM` pattern; bare `YYYY` is an integer
9. Structure detection (Map vs Collection) is determined by immediate children only
10. Duplicate bare keys under the same parent trigger Collection of Objects interpretation

### Appendix B: Comparison with Other Formats

#### B.1 TAML vs. YAML

| Feature | TAML | YAML |
|---------|------|------|
| **Indentation** | Tabs only | Spaces only |
| **Key-value separator** | Tab(s) | Colon + space |
| **Lists** | Indented values | Dash prefix |
| **Quotes** | Only for empty strings | Optional/required |
| **Special syntax** | Minimal (~, "") | Anchors, aliases, pipes, etc. |
| **Complexity** | Low | High |
| **Ambiguity** | Low | Moderate |

#### B.2 TAML vs. JSON

| Feature | TAML | JSON |
|---------|------|------|
| **Human readability** | High | Moderate |
| **Whitespace sensitivity** | Yes (tabs) | No (flexible) |
| **Comments** | Yes | No (standard) |
| **Syntax overhead** | Minimal | Moderate (brackets, quotes) |
| **Data types** | String-based | Explicit types |
| **Tool support** | Limited | Extensive |

#### B.3 TAML vs. TOML

| Feature | TAML | TOML |
|---------|------|------|
| **Syntax style** | Indentation-based | Section-based |
| **Hierarchy notation** | Tabs | Dot notation / sections |
| **Lists** | Indented values | Brackets |
| **Complexity** | Very low | Moderate |
| **Type system** | Implicit | Explicit |

### Appendix C: Migration Guide

#### C.1 Converting from YAML to TAML

**YAML:**
```yaml
server:
  host: localhost
  port: 8080
  features:
    - ssl
    - compression
```

**TAML:**
```taml
server
	host	localhost
	port	8080
	features
		ssl
		compression
```

**Conversion Rules:**
1. Replace space indentation with tabs (one tab per level)
2. Replace `: ` (colon-space) with tab separator
3. Remove list dashes (`-`) and indent list items
4. Replace `null` with `~`
5. Remove quotes from strings
6. Use `""` for empty strings

#### C.2 Converting from JSON to TAML

**JSON:**
```json
{
  "database": {
    "host": "localhost",
    "port": 5432,
    "features": ["ssl", "pooling"]
  }
}
```

**TAML:**
```taml
database
	host	localhost
	port	5432
	features
		ssl
		pooling
```

**Conversion Rules:**
1. Remove all braces and brackets
2. Replace `:` with tab separator
3. Remove commas
4. Remove quotes from strings
5. Convert arrays to indented values
6. Add tab indentation for nesting
7. Replace `null` with `~`

#### C.3 Migration Best Practices

1. **Validate after conversion**: Always validate converted TAML documents
2. **Test type preservation**: Ensure numeric and boolean values are correctly interpreted
3. **Handle null vs empty**: Explicitly choose between `~` (null) and `""` (empty string)
4. **Comment preservation**: Manually migrate comments as structure may differ
5. **Tool support**: Use conversion tools where available, validate manually
6. **Boolean mapping**: Map source format booleans to TAML's canonical `true`/`false` (or `yes`/`no`/`on`/`off` if preferred)
7. **Multiline content**: Convert multiline strings, heredocs, or block scalars to raw text blocks (`...`)
8. **Arrays of objects**: Convert arrays of homogeneous objects to duplicate bare keys (Section 7.4.3)
9. **Date/time values**: Ensure date/time values use ISO 8601 format for automatic type detection

---

## Acknowledgments

TAML was designed to address the complexity and ambiguity challenges present in existing data serialization formats. The design draws inspiration from Python's significant whitespace, Make's tab-based syntax, and the simplicity principles of Unix tools.

---

## References

### Normative References

- IETF RFC 2119: Key words for use in RFCs to Indicate Requirement Levels
- IETF RFC 3629: UTF-8, a transformation format of ISO 10646
- ISO 8601: Data elements and interchange formats — Information interchange — Representation of dates and times
- The Unicode Consortium: The Unicode Standard, Version 15.0.0

### Informative References

- YAML Ain't Markup Language (YAML) Version 1.2
- TOML: Tom's Obvious, Minimal Language
- JSON: JavaScript Object Notation (ECMA-404)
- XML: Extensible Markup Language 1.0

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 0.1 | December 15, 2025 | Initial draft specification |
| 0.2 | March 5, 2026 | Added raw text blocks (Section 7.6); extended booleans with yes/no/on/off (Section 8.2.2); ISO 8601 date/time detection (Section 8.2.3); collection-of-objects via duplicate bare keys (Section 7.4.3); data structure types with Maps and Collections (Section 7.7); clarified 1/0 as integers not booleans; clarified YYYY-only as integer; clarified empty parents as empty maps; defined key character rules explicitly; standardized trailing whitespace trimming; added raw text security considerations (Section 12.5); updated EBNF grammar and conformance requirements |

---

**End of Specification**
