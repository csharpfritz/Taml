# TAML Language Support for Visual Studio Code

Complete language support for **TAML** (Tab Accessible Markup Language) with syntax highlighting and real-time validation.

![TAML Logo](images/taml-banner.png)

## Features

✨ **Syntax Highlighting** for TAML files (`.taml`)
- Comments (`#`)
- Keys and values
- Parent keys (section headers)
- List items
- Special values: `true`, `false`, `null`, numbers
- Environment variables: `${VAR_NAME}`

🔴 **Real-Time Validation** (Language Server)
- **Space indentation** - Must use tabs, not spaces
- **Mixed indentation** - No mixing tabs and spaces
- **Inconsistent indentation** - Can't skip indent levels
- **Orphaned lines** - Indented lines need a parent
- **Tabs in values** - Values can't contain tabs
- **Empty keys** - All lines need content

⚠️ **Smart Warnings**
- **Double spaces in keys** - Might have meant to use tabs
- Context-aware error messages with line numbers

🎨 **Smart Indentation**
- Automatic tab-based indentation
- Folding support based on indentation levels
- Configures VSCode to use tabs (not spaces)

🔧 **Language Configuration**
- Line comments with `#`
- Indentation-based folding
- Tab-aware editing

## What is TAML?

TAML (Tab Accessible Markup Language) is a minimalist hierarchical data format that uses only tabs and newlines for structure. It's designed to be:

- **Simpler** than YAML
- **More accessible** with keyboard navigation
- **Easier to edit** with minimal syntax
- **Perfectly aligned** for visual clarity

### Example

```taml
# Web Application Config
application		MyApp
version			1.0.0

server
	host		0.0.0.0
	port		3000
	ssl
		enabled		true
		cert		/etc/ssl/app.crt

features
	dark_mode		enabled
	analytics		true
	api_gateway
```

## Installation

### From VSIX File

1. Download the `.vsix` file from releases
2. Open VSCode
3. Go to Extensions view (`Ctrl+Shift+X` or `Cmd+Shift+X`)
4. Click `...` menu → `Install from VSIX...`
5. Select the downloaded `.vsix` file

### From Source

```bash
cd tools/vscode-taml

# Install dependencies
npm install

# Compile TypeScript (client and server)
npm run compile

# Package extension
npm run package

# Install in VSCode
code --install-extension taml-*.vsix
```

### From Marketplace (Coming Soon)

Search for "TAML" in the VSCode Extensions marketplace.

## Validation and Error Detection

The language server provides instant feedback as you type:

![Validation Example](images/validation-demo.png)

### Errors Detected

Try introducing errors to see validation in action:

```taml
# ❌ This will show an error (space indentation):
server
    host	localhost
```

```taml
# ❌ This will show an error (tab in value):
name	Hello	World
```

```taml
# ❌ This will show an error (inconsistent indentation):
server
			port	8080    # Skipped indent level
```

### Configuration

Customize validation behavior in your VSCode `settings.json`:

```json
{
  "taml.validation.enable": true,
  "taml.validation.showWarnings": true,
  "taml.trace.server": "off"
}
```

| Setting | Default | Description |
|---------|---------|-------------|
| `taml.validation.enable` | `true` | Enable/disable validation |
| `taml.validation.showWarnings` | `true` | Show warnings in addition to errors |
| `taml.trace.server` | `"off"` | LSP communication trace (`"off"`, `"messages"`, `"verbose"`) |

## Editor Configuration

The extension automatically configures VSCode to use tabs for TAML files:

```json
{
  "[taml]": {
    "editor.insertSpaces": false,
    "editor.detectIndentation": false,
    "editor.tabSize": 1
  }
}
```

You can customize these settings in your VSCode settings.

## Syntax Highlighting

The extension provides syntax highlighting for:

| Element | Description | Scope |
|---------|-------------|-------|
| **Comments** | Lines starting with `#` | `comment.line.number-sign.taml` |
| **Keys** | Text before tab separator | `entity.name.tag.taml` |
| **Parent Keys** | Keys without values (sections) | `entity.name.section.taml` |
| **Values** | Text after tab separator | `string.unquoted.taml` |
| **Booleans** | `true`, `false`, `yes`, `no` | `constant.language.boolean.taml` |
| **Null** | `null`, `nil` | `constant.language.null.taml` |
| **Numbers** | Integer and decimal numbers | `constant.numeric.taml` |
| **Env Vars** | `${VAR_NAME}` | `variable.other.environment.taml` |

## Theme Support

TAML syntax highlighting works with all VSCode themes. The extension assigns standard TextMate scopes that themes already know how to color.

Popular themes tested:
- Dark+ (default dark)
- Light+ (default light)
- Monokai
- Solarized Dark/Light
- Dracula
- One Dark Pro

## Language Features

### Code Folding

Fold sections based on indentation:

```taml
server                    ▼ server
	host	localhost       	host	localhost
	port	3000            	port	3000
	ssl                     	ssl
		enabled	true        		enabled	true
```

### Comment Toggle

Use `Ctrl+/` (or `Cmd+/` on Mac) to toggle line comments:

```taml
# This is a comment
application	MyApp
# version	1.0.0
```

## TAML Specification

For the complete TAML specification, see [TAML-SPEC.md](../../TAML-SPEC.md).

Key rules:
- **Indentation**: One tab = one level of nesting
- **Key-Value Separator**: One or more tabs between key and value
- **Parent Keys**: Keys with children have no value
- **Lists**: Just values indented one tab from parent
- **No Tabs in Content**: Keys and values cannot contain tabs
- **Comments**: Lines starting with `#`

## Examples

Check out real-world examples in the [examples](../../examples/) directory:
- Web application configuration
- REST API documentation
- Cloud infrastructure definitions
- Game level design
- Recipe database
- Team directories

## Contributing

Found a bug or have a suggestion? Please open an issue on [GitHub](https://github.com/your-repo/taml).

## License

MIT License - See [LICENSE](../../LICENSE) file for details.

## Links

- [TAML Specification](../../TAML-SPEC.md)
- [Examples](../../examples/)
- [GitHub Repository](https://github.com/your-repo/taml)

---

**TAML: Because sometimes less markup is more.** ✨
