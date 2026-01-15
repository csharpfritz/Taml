# TAML - Tab Annotated Markup Language

<p align="center">
  <img src="./img/logo.webp" alt="TAML Logo" width="200">
</p>

**The simplest hierarchical data format ever created. Just tabs and newlines. That's it.**

TAML uses only two characters for structure: tabs for hierarchy and newlines for separation. No brackets, no braces, no colons, no quotes (except for empty strings), no hyphens. If you can hit the Tab key, you can write TAML.

What started as a playful jab at YAML's complexity has grown on us. After iterating on the spec and building parsers, we realized something: simplicity isn't just elegant—it's powerful. TAML eliminates the cognitive overhead of remembering complex markup rules. Tabs do all the work.

## ✨ Why TAML?

- **🎯 Minimal Markup**: Only tabs and newlines—no brackets, braces, colons, quotes, or hyphens
- **📐 Tab-Based Hierarchy**: Indentation defines structure; tabs separate keys from values
- **👀 Visual Clarity**: Structure is immediately visible and keyboard-navigable
- **🧠 Simple Mental Model**: One tab = one level deeper
- **📏 Flexible Alignment**: Use as many tabs as you want between keys and values to align them in columns
- **⚡ Easy to Write**: Less typing, less thinking about syntax

## 📋 Quick Example

```taml
application		MyApp
version				1.0.0
author				Developer Name

server
	host	0.0.0.0
	port	8080
	ssl		true

database
	type	postgresql
	connection
		host			db.example.com
		port			5432
		database	myapp_db

features
	user-authentication
	api-gateway
	logging
```

Here's a video where I explain some of the ideas about TAML:  https://www.youtube.com/watch?v=wX5PMvSOVLk

## 🎯 Basic Syntax

### Key-Value Pairs
```taml
key			value
name		John Doe
count		42
```

### Nested Structures
```taml
parent
	child						value
	another_child		value
```

### Lists
```taml
items
	first 	item
	second 	item
	third 	item
```

### Special Values
```taml
# User profile data
username	alice
password	~
nickname	""
bio				Hello world
```
- `~` represents null (not set)
- `""` represents an empty string (explicitly empty)
- Regular values need no quotes
- Lines starting with `#` are comments

## 📚 Documentation

- **[Full Specification](./TAML-SPEC.md)** - Complete language specification with validation rules
- **[Examples](./examples/)** - Real-world examples across different domains (web apps, APIs, cloud infrastructure, games, recipes, and more)

## 🛠️ Implementations

### Languages

#### Python
Full-featured parser and serializer with type inference and validation.

```bash
cd python
pip install -e .
```

**Features:**
- Parse TAML to Python dictionaries and lists
- Serialize Python objects to TAML
- Type inference (numbers, booleans, null)
- Comprehensive test suite

[📖 Python Documentation](./python/README.md)

#### JavaScript/Node.js
Lightweight parser and serializer for JavaScript environments.

```bash
cd javascript
npm install
```

**Features:**
- Parse TAML to JavaScript objects
- Serialize objects to TAML
- Browser and Node.js compatible
- Zero dependencies

[📖 JavaScript Documentation](./javascript/README.md)

#### .NET/C#
Robust implementation with strict validation and excellent error reporting.

```bash
cd dotnet
dotnet build
```

**Features:**
- Strong typing support
- Strict validation with detailed error messages
- Serialization and deserialization
- Full test coverage

### Editor Support & Tools

#### Visual Studio Code Extension
Syntax highlighting and language support for `.taml` files.

**Features:**
- 🎨 Syntax highlighting
- 💬 Comment support
- 📁 Code folding
- ⚙️ Auto-configuration for tabs

[📖 VSCode Extension Guide](./tools/vscode-taml/)

#### Language Server Protocol (LSP)
Real-time validation with error detection as you type.

**Features:**
- ✅ Real-time validation
- 🔴 Red squiggles for errors
- ⚠️ Yellow squiggles for warnings
- 📋 Problems panel integration
- ⚙️ Configurable validation

[📖 Language Server Guide](./tools/taml-language-server/)

[📖 All Tools Documentation](./tools/)

## 🚀 Getting Started

1. **Read the spec**: Check out [TAML-SPEC.md](./TAML-SPEC.md) to understand the syntax
2. **Try examples**: Browse [examples/](./examples/) to see TAML in action
3. **Pick your language**: Install the parser for your preferred language
4. **Set up your editor**: Install the VSCode extension for syntax highlighting

## 🆚 TAML vs Other Formats

| Feature | TAML | YAML | JSON | XML |
|---------|------|------|------|-----|
| **Indentation** | Tabs only | Spaces (usually 2 or 4) | Optional | Optional |
| **Key-Value Separator** | Tab character | Colon + space | Colon + space | Tags |
| **Lists** | Indented values | Dash + space | Brackets `[]` | Repeated tags |
| **Quotes** | Only for empty strings (`""`) | Complex quoting rules | Required for strings | Not needed |
| **Closing Tags** | None | None | Braces `}`, brackets `]` | Required `</tag>` |
| **Comments** | `#` prefix | `#` prefix | Not supported | `<!-- -->` |
| **Multi-line Strings** | Not supported | Multiple syntaxes | Escape `\n` | Natural |
| **Anchors/References** | Not supported | Yes | Not supported | Not supported |
| **Complexity** | Minimal | High | Medium | High |
| **Human Readability** | Excellent | Good | Fair | Poor |
| **Learning Curve** | Minutes | Hours | Minutes | Hours |

**TAML Philosophy**: Less expressive by design. Simple problems shouldn't require complex solutions.

## 🎯 Use Cases

TAML excels at:

- **Configuration Files**: Application settings, environment configs
- **API Documentation**: Simple, readable endpoint definitions
- **Data Exchange**: When human readability matters more than machine efficiency
- **Prototyping**: Quick data structure sketches
- **Teaching**: Introducing hierarchical data concepts
- **Accessibility**: Tab-based navigation for keyboard users

## 📝 Validation Rules

TAML enforces strict rules to maintain consistency:

- ✅ Tabs only for indentation (no spaces)
- ✅ One tab per nesting level
- ✅ Keys and values cannot contain tabs
- ✅ Parent keys have no values on the same line
- ✅ Comments start with `#`

See the [Validation section](./TAML-SPEC.md#validation-rules) in the spec for complete details.

## 📦 Assets & Branding

TAML's visual identity is available in multiple formats for use in editors, documentation, and presentations.

<p align="center">
  <img src="./img/logo.webp" alt="TAML Logo" width="300">
  <br>
  <em>Full Logo</em>
</p>

<p align="center">
  <img src="./img/logo-illustration.webp" alt="TAML Logo Illustration" width="300">
  <br>
  <em>Logo Illustration</em>
</p>

<p align="center">
  <img src="./img/icon.svg" alt="TAML Icon" width="128">
  <br>
  <em>Icon (SVG)</em>
</p>

All assets are available in the [img/](./img/) directory in various formats (PNG, SVG, WebP).

## 🤝 Contributing

Contributions are welcome! Whether you're:
- Implementing TAML in a new language
- Improving existing implementations
- Adding editor support for other IDEs
- Creating examples
- Improving documentation

## 📄 License

See individual implementation directories for license information.

## 🔗 Links

- [Specification](./TAML-SPEC.md)
- [Examples](./examples/)
- [Python Implementation](./python/)
- [JavaScript Implementation](./javascript/)
- [.NET Implementation](./dotnet/)
- [Editor Tools](./tools/)

---

**TAML: Because sometimes less markup is more.** 🚀
