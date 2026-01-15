# TAML Specification v0.2
## Tab Annotated Markup Language

### Overview
TAML is a minimalist hierarchical data serialization format that uses only tabs and newlines for structure. It's designed to be simpler and more accessible than YAML while maintaining human readability.

### Design Philosophy
- **Minimal markup**: Only tabs and newlines
- **Tab- and line-based hierarchy**: Tabs define both structure and key-value separation; new lines represent the end of a value.
- **Minimal special characters**: Only tabs, newlines, `...`, `~`, and `""` have special meanings. Brackets, braces, colons, most quotation marks, and hyphens have no special meaning. This allows most TAML documents to be completely free of special characters and very human readable.
- **Visual clarity**: Structure is immediately visible

These TAML documents consistently use an actual tab character to represent the tab character as used in TAML. It is expected that those who work with TAML, when necessary, will use text editor tools (such as Visual Studio Code's "Toggle Render Whitespace" option) to distinguish tab characters from spaces or other white-space characters. 

### Basic Syntax

#### Key-Value Pairs
Key and value separated by one or more tabs:
```
key	value
key		value
key			value
```
All three examples above are equivalent. Multiple tabs can be used for better visual alignment.

#### Null and Empty Values

**Null Values**: Use the tilde character `~` to represent a null value (absence of data):
```
key	~
name	value
optional_field	~
```

**Empty Strings**: Use two double-quote characters `""` to represent an empty string:
```
key	""
name	value
empty_field	""
```

TAML distinguishes between null (unknown/not applicable) and empty string (known to be empty):
- `key	~` → null value
- `key	""` → empty string value
- `key	value` → non-empty string value (no quotes needed)

#### Nested Structures
Children are indented with tabs. If a key has children, it has no value on the same line:
```
parent
	child	value
	another_child	value
	nested
		deeper	value
```

#### Lists
List items are just values indented one tab from their parent:
```
items
	first item
	second item
	third item
```

#### Nested Lists and Objects
```
config
	database
		host	localhost
		port	5432
		credentials
			username	admin
			password	secret
	features
		authentication
		logging
		caching
```

### Rules

1. **Indentation**: One tab character = one level of nesting
2. **Key-Value Separator**: One or more tab characters between key and value. Multiple tabs can be used for visual alignment.
3. **Line Breaks**: Each key-value pair on its own line
4. **Parent Keys**: Keys with children have no value (just the key alone on the line)
5. **Lists**: Just values indented one tab from their parent key (no special syntax)
6. **No Quotes (except for empty strings)**: Values are literal strings with no quotes or escaping needed. The only exception is `""` which represents an empty string value. Regular non-empty values never use quotes.
7. **No Tabs in Content**: Keys and values cannot contain tab characters. Only the separator between key and value may contain tabs. **Exception**: Raw text blocks (using `...`) preserve tabs as literal content.
8. **Comments**: Lines starting with `#` are ignored. Mid-line comments are not supported - `#` characters within keys or values are treated as literal characters.
9. **Null and Empty Values**: Use `~` to represent null values. Use `""` (two double-quote characters) to represent empty strings. These are semantically distinct values.
10. **Raw Text**: Use `...` as a value to indicate that following indented lines contain raw text with preserved tabs and newlines.

### Raw Lines and Text Escaping

For values that need to contain tabs, newlines, or other special characters, TAML provides a raw text mode using the `...` value indicator.

#### Raw Text Syntax

When a key has the value `...` (three dots), the following lines that are indented exactly one level more than the key become raw text content. All tabs and newlines within the raw text block are preserved as literal content.

**Syntax:**
```taml
key	...
	raw content with	tabs preserved
	multiple lines
	all preserved exactly
next_key	regular_value
```

**Rules for Raw Text:**
1. The `...` value indicates the start of raw text mode
2. Raw text lines must be indented at least one tab more than the parent key
3. The first tab beyond the parent key level is structural indentation (not part of content)
4. Any additional tabs beyond the structural indentation are preserved as literal tab characters in the content
5. All newlines within the raw text content are preserved
6. Raw text mode ends when a line is encountered with indentation equal to or less than the parent key
7. The raw text value is the concatenation of all raw text lines, with their preserved tabs and newlines

#### Raw Text Examples

**Single-line raw text:**
```taml
message	...
	Hello	World	with	tabs
```
Result: The value of `message` is `"Hello\tWorld\twith\ttabs"`

**Raw text with leading tabs:**
```taml
indented	...
		This line starts with a tab character
	This line has no leading tabs
```
Result: First line contains `"\tThis line starts with a tab character"`, second line contains `"This line has no leading tabs"`

**Multi-line raw text:**
```taml
script	...
	if [ $1 -eq 1 ]; then
		echo "Tab indented"
	fi
description	This is after the raw block
```
Result: The value of `script` preserves the original shell script with all tabs and newlines.

**Complex example:**
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

**HTML content example:**
```taml
template	...
	<div class="container">
		<h1>Welcome</h1>
		<p>This	preserves	tabs</p>
	</div>
```

#### Validation Rules for Raw Text

1. **Raw Text Indicator**: The `...` must be the exact value (three periods, no spaces)
2. **Minimum Indentation**: Raw text lines must be indented at least one tab more than the key with `...`
3. **Structural vs Content Tabs**: The first tab beyond parent level is structural; additional tabs become content
4. **Termination**: Raw text ends at the first line with indentation ≤ parent key indentation
5. **Empty Raw Text**: If no indented lines follow `...`, the value is an empty string

#### Raw Text Indentation Examples

**✅ Valid - Additional tabs preserved as content:**
```taml
key	...
		tab character in content
	correct tabs
```
Result: First line contains a literal tab character, second line has no leading tabs.

**✅ Valid - Tabs within content:**
```taml
script	...
	if condition:
			indented with 2 literal tabs
		indented with 1 literal tab
	back to no literal tabs
```

**❌ Invalid - Insufficient indentation:**
```taml
key	...
no indentation
```

**❌ Invalid - Mixed indentation:**
```taml
key	...
 	space plus tab
	just tab
```

### Data Types

TAML is intentionally simple. All values are stored as strings in the document format. However, parsers SHOULD perform automatic type detection and conversion during deserialization, and serializers SHOULD convert native types back to their canonical string representations.

#### Type Detection Order

Parsers should attempt type detection in the following order (first match wins):

1. **Null**: `~` (tilde character)
2. **Empty String**: `""` (two double-quote characters)
3. **Raw Text**: `...` followed by indented content (preserves tabs and newlines)
4. **Boolean**: Truthy and falsy value patterns (case-insensitive)
5. **Date/Time**: ISO 8601 formatted date and time values
6. **Number**: Integer and decimal number patterns
7. **String**: Any other text value (no quotes needed)

#### Null Values

The tilde character `~` represents a null value (absence of data, unknown, or not applicable):

```taml
optional_field	~
password	~
```

#### Empty String Values

Two double-quote characters `""` represent an empty string (known to be empty, zero-length string):

```taml
middle_name	""
description	""
```

#### Boolean Values

TAML supports automatic detection of boolean values using common truthy and falsy patterns. Detection is **case-insensitive**.

**Truthy Values** (evaluate to boolean `true`):
- `true`
- `yes`
- `on`
- `1`

**Falsy Values** (evaluate to boolean `false`):
- `false`
- `no`
- `off`
- `0`

**Examples:**
```taml
# All of these are boolean true
enabled	true
active	True
confirmed	TRUE
feature_flag	yes
toggle	on
binary	1

# All of these are boolean false
disabled	false
inactive	False
unconfirmed	FALSE
legacy_mode	no
switched	off
binary_off	0
```

**Serialization**: When serializing boolean values, implementations SHOULD use the canonical lowercase forms `true` and `false`.

**Note**: The values `1` and `0` are interpreted as booleans only when the context clearly expects a boolean. In ambiguous contexts, parsers may treat them as integers. Implementations may provide options to control this behavior.

#### Number Values

TAML automatically detects numeric values during parsing. Numbers are recognized by their format without any special syntax.

**Integer Numbers** (whole numbers):
- Optional leading sign: `+` or `-`
- One or more digits: `0-9`
- No decimal point
- No leading zeros (except for the number `0` itself)

**Decimal Numbers** (floating-point):
- Optional leading sign: `+` or `-`
- At least one digit before OR after the decimal point (both is preferred but not required)
- A decimal point (`.`)
- Optional scientific notation: `e` or `E` followed by optional sign and digits

**Valid Integer Examples:**
```taml
count	42
negative	-17
zero	0
positive	+100
large	9999999999
```

**Valid Decimal Examples:**
```taml
price	19.99
temperature	-40.5
pi	3.14159
small	0.001
scientific	6.022e23
negative_exp	1.5e-10
explicit_exp	2.998E+8

# Flexible decimal formats
percentage	.75        # No leading zero (equivalent to 0.75)
whole	42.            # Trailing decimal (equivalent to 42.0)
explicit_zero	0.5    # Explicit leading zero (recommended)
```

**Invalid Number Patterns** (treated as strings):
```taml
# These are strings, not numbers
leading_zero	007        # Leading zeros not allowed
multiple_dots	1.2.3      # Multiple decimal points
just_dot	.              # Decimal point alone (no digits)
comma_sep	1,000        # Commas not supported
currency	$100         # Currency symbols not supported
spaces	1 000          # Spaces not allowed
```

**Recommended Number Detection RegEx:**

Implementations can use the following regular expression pattern to detect numeric values:

```regex
^[+-]?(?:\d+\.?\d*|\d*\.\d+)(?:[eE][+-]?\d+)?$
```

**Pattern Explanation:**
- `^[+-]?` - Optional leading sign
- `(?:\d+\.?\d*|\d*\.\d+)` - Number with optional decimal point:
  - `\d+\.?\d*` matches integers (42) or decimals with leading digits (42.5 or 42.)
  - `\d*\.\d+` matches decimals with leading decimal (.5 or 0.5)
- `(?:[eE][+-]?\d+)?` - Optional scientific notation
- `$` - End of string

This pattern accepts: `42`, `-17`, `3.14`, `.5`, `42.`, `6.022e23`, `-1.5e-10`

This pattern rejects: `007`, `.`, `1.2.3`, `1,000`, `$100`

**Serialization**: When serializing numbers, implementations SHOULD:
- Use no leading zeros for integers
- Use standard decimal notation for decimals
- Preserve reasonable precision (implementation-defined)
- Use lowercase `e` for scientific notation when needed

#### Date and Time Values (ISO 8601)

TAML supports automatic detection of date and time values following the ISO 8601 standard. This enables unambiguous representation of temporal data across different locales and systems.

##### Supported ISO 8601 Formats

**Date Only (Calendar Dates):**
```taml
# Extended format (recommended)
date	2024-01-15

# Year and month only
month	2024-01

# Year only
year	2024
```

**Time Only:**
```taml
# With seconds
time	14:30:00

# Without seconds
time_short	14:30

# With fractional seconds
time_precise	14:30:00.123
```

**Combined Date and Time:**
```taml
# Date and time with T separator (recommended)
timestamp	2024-01-15T14:30:00

# With fractional seconds
precise_time	2024-01-15T14:30:00.123
```

**With Time Zone / UTC Offset:**
```taml
# UTC (Zulu time)
utc_time	2024-01-15T14:30:00Z

# Positive UTC offset
tokyo_time	2024-01-15T23:30:00+09:00

# Negative UTC offset
nyc_time	2024-01-15T09:30:00-05:00

# Zero offset (equivalent to Z)
london_time	2024-01-15T14:30:00+00:00
```

**Week Dates:**
```taml
# Year and week
week	2024-W03

# Year, week, and day (1=Monday, 7=Sunday)
week_day	2024-W03-4
```

**Ordinal Dates:**
```taml
# Year and day of year (001-366)
ordinal	2024-015
```

**Durations:**
```taml
# Period designator format
duration	P3Y6M4DT12H30M5S
simple_duration	PT1H30M
days_only	P7D
```

**Time Intervals:**
```taml
# Start and end
interval	2024-01-01/2024-12-31

# Start and duration
interval_dur	2024-01-01/P1M

# Duration and end
dur_interval	P1M/2024-02-01
```

##### ISO 8601 Detection Rules

Parsers should recognize ISO 8601 formats by these patterns:

1. **Date**: `YYYY-MM-DD`, `YYYY-MM`, or `YYYY` where YYYY is 4 digits, MM is 01-12, DD is 01-31
2. **Time**: `HH:MM:SS`, `HH:MM`, with optional fractional seconds `.sss`
3. **Combined**: Date + `T` + Time
4. **UTC Indicator**: Trailing `Z` (case-insensitive)
5. **Offset**: `±HH:MM` or `±HHMM` or `±HH`
6. **Week Date**: `YYYY-Www` or `YYYY-Www-D`
7. **Ordinal Date**: `YYYY-DDD`
8. **Duration**: Starting with `P`, containing designators Y, M, D, T, H, M, S
9. **Interval**: Two date/times or date/time and duration separated by `/`

**Serialization**: When serializing date/time values, implementations SHOULD:
- Use the extended format with separators (hyphens and colons)
- Use uppercase `T` as the date-time separator
- Use uppercase `Z` for UTC
- Preserve the original precision and time zone information when possible

##### Complete Date/Time Example

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

#### String Values

Any value that does not match the patterns above is treated as a plain string. Strings require no quotes or escaping (except for empty strings which use `""`).

```taml
name	John Doe
message	Hello, World!
path	/usr/local/bin
email	user@example.com
```

#### Type Conversion Summary

| TAML Value | Detected Type | Example |
|------------|---------------|---------|
| `~` | Null | `field	~` |
| `""` | Empty String | `field	""` |
| `true`, `yes`, `on`, `1` | Boolean (true) | `enabled	true` |
| `false`, `no`, `off`, `0` | Boolean (false) | `enabled	false` |
| `42`, `-17`, `+100` | Integer | `count	42` |
| `3.14`, `-0.5`, `6.022e23` | Decimal/Float | `price	19.99` |
| `2024-01-15` | Date | `date	2024-01-15` |
| `14:30:00` | Time | `time	14:30:00` |
| `2024-01-15T14:30:00Z` | DateTime | `timestamp	2024-01-15T14:30:00Z` |
| `P1DT2H30M` | Duration | `duration	P1DT2H30M` |
| Any other value | String | `name	John Doe` |

#### Parser Options

Implementations MAY provide options to control type conversion behavior:

- **strict_types**: When enabled, perform strict type validation and reject ambiguous values
- **type_conversion**: When disabled (`false`), treat all values as strings (no automatic conversion)
- **boolean_strings**: Custom list of truthy/falsy string values to recognize

**Example with options (language-specific):**
```python
# Python example
data = taml.parse(text, type_conversion=True, strict_types=False)
```

```javascript
// JavaScript example
const data = taml.parse(text, { typeConversion: true, strictTypes: false });
```

```csharp
// C# example
var data = TamlDocument.Parse(text, new TamlOptions { TypeConversion = true, StrictTypes = false });
```

### Example Document

```taml
# TAML Example
application	MyApp
version	1.0.0
author	Developer Name
license	~

server
	host	0.0.0.0
	port	8080
	ssl	true
	
database
	type	postgresql
	connection
		host	db.example.com
		port	5432
		database	myapp_db
		password	~

# Example of raw text with preserved formatting
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
		
features
	user-authentication
	api-gateway
	rate-limiting
	logging

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

environments
	development
		debug	true
		log_level	verbose
	production
		debug	false
		log_level	error
```

### Advantages Over YAML

- **Simpler**: No complex syntax for anchors, aliases, or multi-line strings
- **Tab-only**: Tabs do all the work - hierarchy and separation
- **No colons**: One less character to type
- **No ambiguity**: No complex rules about when quotes are needed
- **Easier to write**: Less cognitive load on markup syntax

### Data Structure Types

TAML supports two fundamental data structure types that are automatically detected based on the structure and naming of children:

#### Maps (Key-Value Collections)

**Definition**: A Map is a collection of unique key-value pairs or unique key-object pairs.

**Syntax Options**:
- **Key-Value Pairs**: `key\tvalue`
- **Key-Object Pairs**: Unique bare keys (keys with no value on the same line) that have children

**Characteristics**:
- Keys are unique identifiers within the map
- Order is preserved during parsing but not semantically significant
- Suitable for: configuration objects, dictionaries, records

**Examples**:

*Map of Values*:
```taml
server
	host	localhost
	port	8080
	ssl	true
```

*Map of Objects*:
```taml
environments
	development
		debug	true
		log_level	verbose
	production
		debug	false
		log_level	error
```

#### Collections/Vectors (Ordered Collections)

**Definition**: A Collection is an ordered list of values or objects, identified by duplicate bare keys or consistent value-only children.

**Syntax Options**:
- **Collection of Values**: Children are just values with no keys (no tab separators)
- **Collection of Strings**: Unique bare keys without children (key names become the values)
- **Collection of Objects**: Duplicate bare keys with children

**Characteristics**:
- Order is semantically significant
- Items are accessed by position/index
- Suitable for: lists, arrays, sequences

**Examples**:

*Collection of Values*:
```taml
features
	authentication
	logging
	caching
```

*Collection of Strings (from bare keys)*:
```taml
allowed_methods
	GET
	POST
	PUT
	DELETE
```

*Collection of Objects (duplicate bare keys)*:
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
	user
		name	Charlie
		email	charlie@example.com
		role	viewer
```

#### Advanced Structures

TAML supports nested combinations of Maps and Collections:

**Collection of Maps (using duplicate bare keys)**:
```taml
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

**Map containing Collections**:
```taml
application
	name	MyApp
	features
		auth
		api
		cache
	ports
		8080
		8443
```

**Nested Collections**:
```taml
matrix
	row
		1
		2
		3
	row
		4
		5
		6
```

#### Structure Detection Rules

Parsers determine structure type by examining immediate children:

1. **Map of Values**: If ALL children are unique keys with tab-separated values
2. **Map of Objects**: If ALL children are unique bare keys with children
3. **Collection of Strings**: If ALL children are text-only lines (no tab separators, no children)
4. **Collection of Objects**: If children contain duplicate bare keys with children
5. **Mixed Structure**: Any combination that doesn't fit the above patterns is **invalid**

#### Examples of Invalid Mixed Structures

**❌ Invalid - Mixed Map and Collection children:**
```taml
config
    host	localhost        # Map child (key-value pair)
    authentication           # Collection child (value only)
    port	8080            # Map child (key-value pair)
```

**✅ Valid - Consistent Collection of Strings:**
```taml
features
    authentication
    logging
    caching
```

**✅ Valid - Consistent Collection of Objects:**
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

#### Parser Implementation Notes

- Structure type determination occurs at parse time based on immediate children only
- Grandchildren and deeper descendants do not affect parent structure type detection
- **Duplicate bare key detection**: When a parser encounters duplicate bare keys at the same level, it should convert the parent to a Collection of Objects
- **Bare keys without children**: Treated as Collection of Strings where the key names become the string values in the collection
- Empty parents (no children) can be treated as either Maps or Collections based on implementation preference
- The first child encountered typically determines the expected structure type for validation, except when duplicate bare keys are detected

### Validation Rules

#### Valid TAML

A TAML document is **valid** if it follows all the rules above. Valid TAML:

```taml
# Valid: Simple key-value
name	value

# Valid: Parent key with children
server
	host	localhost
	port	8080

# Valid: Multiple tabs for alignment
short		value1
long_key	value2

# Valid: Lists
items
	item1
	item2

# Valid: Nested structure
parent
	child
		grandchild	value
```

#### Invalid TAML

A TAML document is **invalid** if it violates any of the following:

##### 1. Spaces Used for Indentation

**❌ Invalid:**
```taml
server
    host	localhost
	port	8080
```
(Spaces used instead of tab for indentation)

**✅ Valid:**
```taml
server
	host	localhost
	port	8080
```

**Rule:** Only tab characters (`\t`) may be used for indentation. Spaces at the start of a line are invalid.

##### 2. Mixed Tabs and Spaces in Indentation

**❌ Invalid:**
```taml
server
 	host	localhost
	port	8080
```
(Space + tab used for indentation)

**✅ Valid:**
```taml
server
	host	localhost
	port	8080
```

**Rule:** Indentation must be pure tabs. No mixing of spaces and tabs.

##### 3. Tabs Within Keys

**❌ Invalid:**
```taml
server	name	localhost
```
(The key "server	name" contains a tab character)

**✅ Valid:**
```taml
server_name	localhost
```

**Rule:** Keys cannot contain tab characters. The first tab(s) after text define the key-value separator.

##### 4. Tabs Within Values

**❌ Invalid:**
```taml
message	Hello	World
```
(Value contains a tab character)

**✅ Valid:**
```taml
message	Hello World
```

**Rule:** Values cannot contain tab characters. Everything after the separator tabs until the line break is the value, but tabs within that content are invalid.

##### 5. Inconsistent Indentation Levels

**❌ Invalid:**
```taml
server
	host	localhost
			port	8080
```
(Skips a level - uses 3 tabs instead of 2)

**✅ Valid:**
```taml
server
	host	localhost
	port	8080
```

**Rule:** Each nesting level must increase indentation by exactly one tab. Skipping levels is invalid.

##### 6. Content on Parent Key Line

**❌ Invalid:**
```taml
server localhost
	port	8080
```
(Parent key has content but no tab separator)

**✅ Valid:**
```taml
server
	host	localhost
	port	8080
```

**Or, if single value:**
```taml
server	localhost
```

**Rule:** A key with children (parent key) must not have a value on the same line. Either have children on subsequent lines, or have a tab-separated value with no children.

##### 7. Invalid Indentation for List Items

**❌ Invalid:**
```taml
items
item1
	item2
```
(First item not indented)

**✅ Valid:**
```taml
items
	item1
	item2
```

**Rule:** List items must be indented exactly one tab from their parent key.

##### 8. Orphaned Indentation

**❌ Invalid:**
```taml
name	value
	orphan	value
```
(Indented but previous line has no children)

**✅ Valid:**
```taml
name	value
other	value
```

**Or:**
```taml
name
	child	value
```

**Rule:** Indented lines must have a parent. You cannot increase indentation after a key-value pair unless the previous line was a parent key.

##### 9. Empty Keys

**❌ Invalid:**
```taml
	value
name	
```
(First line has no key, second line has empty value)

**✅ Valid:**
```taml
parent
	value
name	value
```

**Rule:** Every line must have content after indentation (unless it's a comment or blank line).

##### 10. Invalid Characters in Keys (Optional Strictness)

Depending on implementation, keys may be restricted:

**Potentially Invalid (implementation-dependent):**
```taml
server name	value
my-key!	value
```
(Space in key, or special character in key)

**Always Valid:**
```taml
server_name	value
my_key	value
```

**Rule:** Keys should typically be identifiers (alphanumeric + underscore/hyphen). Spaces and special characters may be rejected by strict parsers.

##### 11. Invalid Raw Text Indentation

**❌ Invalid:**
```taml
content	...
no indentation
```
(Raw text line has insufficient indentation)

**✅ Valid:**
```taml
content	...
		extra tab preserved as content
	minimum indentation
```

**Rule:** All raw text lines must be indented at least one tab more than the key with `...` value. Additional tabs become literal content. 

##### 12. Invalid Raw Text Indicator

**❌ Invalid:**
```taml
content	.. .
	raw content
```
(Invalid spacing in raw text indicator)

**❌ Invalid:**
```taml
content	...more
	raw content
```
(Additional text after `...`)

**✅ Valid:**
```taml
content	...
	raw content
```

**Rule:** Empty lines are literal content as well.

**✅ Valid:**
```taml
content	...

	raw content
```

`content` contains the value `\nraw content`

In the very rare case that the entire content conflicts with a special meaning in TAML, the same syntax makes it possible to represent such content.

**✅ Valid: content is exactly two literal quotation marks (not an empty string) **
```taml
content	...
	""
```

**✅ Valid: content is exactly one literal tilde (not null) **
```taml
content	...
	~
```

**Rule:** The raw text indicator must be exactly `...` (three periods) with no additional characters.

##### 13. Distinguishing Null from Empty String

**Valid - Different semantic meanings:**
```taml
username	alice
password	~
nickname	""
bio	Hello world
```

In this example:
- `password\t~` means password is null (not set)
- `nickname\t""` means nickname is an empty string (explicitly empty)
- `bio\tHello world` is a regular string (no quotes)

All three are valid but have different meanings that should be preserved during serialization/deserialization.

### Validation Error Types

Implementations should provide clear error messages:

| Error Type | Example Message |
|------------|----------------|
| **Space Indentation** | Line 5: Indentation must use tabs, not spaces |
| **Tab in Key** | Line 3: Key "server	name" contains invalid tab character |
| **Tab in Value** | Line 7: Value contains invalid tab character |
| **Inconsistent Indent** | Line 10: Invalid indentation level (expected 2 tabs, found 3) |
| **Orphaned Line** | Line 8: Indented line has no parent |
| **Parent with Value** | Line 4: Parent key cannot have value on same line |
| **Empty Key** | Line 12: Line has no key |
| **Mixed Indent** | Line 6: Mixed spaces and tabs in indentation |
| **Invalid Raw Text Indent** | Line 15: Raw text line has invalid indentation (expected 2 tabs, found 3) |
| **Invalid Raw Text Indicator** | Line 9: Invalid raw text indicator (expected '...') |

### Parser Behavior

#### Lenient Parsing (Recommended for Deserialization)
- Ignore blank lines
- Ignore lines with only whitespace
- Trim trailing whitespace from values
- Accept comments anywhere
- Skip invalid lines and continue parsing

#### Strict Parsing (Recommended for Validation)
- Reject any invalid TAML immediately
- Provide detailed error messages with line numbers
- Halt on first error or collect all errors

### File Extension

`.taml`

### MIME Type

`text/x-taml` or `application/x-taml`

---

*TAML: Because sometimes less markup is more.*
