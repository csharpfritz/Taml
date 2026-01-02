# TAML Specification v0.1
## Tab Annotated Markup Language

### Overview
TAML is a minimalist hierarchical data serialization format that uses only tabs and newlines for structure. It's designed to be simpler and more accessible than YAML while maintaining human readability.

### Design Philosophy
- **Minimal markup**: Only tabs and newlines
- **Tab- and line-based hierarchy**: Tabs define both structure and key-value separation; new lines represent the end of a value.
- **Minimal special characters**: Only `~` and `""` have special meanings. Brackets, braces, colons, most quotation marks, and hyphens have no special meaning. 
- **Visual clarity**: Structure is immediately visible

These TAML documents consistently use an actual tab character to represent the tab character as it is used in TAML-formatted files and snippets. It is expected that those who work with TAML, when necessary, will use text editor tools (such as Visual Studio Code's "Toggle Render Whitespace" option) to distinguish tab characters from spaces or other white-space characters. 

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
7. **No Tabs in Content**: Keys and values cannot contain tab characters. Only the separator between key and value may contain tabs.
8. **Comments**: Lines starting with `#` are ignored. Mid-line comments are not supported - `#` characters within keys or values are treated as literal characters.
9. **Null and Empty Values**: Use `~` to represent null values. Use `""` (two double-quote characters) to represent empty strings. These are semantically distinct values.

### Data Types

TAML is intentionally simple. All values are strings by default. Parsers may interpret:
- Numbers: `42`, `3.14`
- Booleans: `true`, `false`
- Null: `~` (tilde character)
- Empty String: `""` (two double-quote characters)
- Non-empty String: any other text value (no quotes)

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
		
features
	user-authentication
	api-gateway
	rate-limiting
	logging

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

### Limitations

- Less expressive than YAML (by design)
- Tab characters can be invisible in some editors
- No built-in support for complex data structures or references

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

##### 11. Distinguishing Null from Empty String

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
