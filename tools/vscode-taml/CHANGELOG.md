# Change Log

All notable changes to the TAML extension will be documented in this file.

## [0.2.0] - 2025-12-21

### Added
- **Language Server Protocol (LSP) implementation** for real-time validation
- **Real-time error detection** with diagnostics:
  - Space indentation errors
  - Mixed indentation errors
  - Inconsistent indentation levels
  - Orphaned indented lines
  - Tabs in values
  - Empty keys
- **Warning system** for potential issues (double spaces in keys)
- **Configuration settings**:
  - `taml.validation.enable` - Enable/disable validation
  - `taml.validation.showWarnings` - Show/hide warnings
  - `taml.trace.server` - LSP communication tracing
- Merged language server functionality into main extension
- Complete language server architecture (client + server)

### Changed
- Extension now includes both syntax highlighting and validation
- Bumped version to 0.2.0
- Updated description to reflect language server capabilities
- Added "Linters" category
- Updated README with validation documentation

## [0.1.0] - 2025-12-12

### Added
- Initial release
- Syntax highlighting for `.taml` files
- Support for comments (`#`)
- Key-value pair highlighting
- Parent key (section) highlighting
- List item highlighting
- Special value highlighting (booleans, null, numbers)
- Environment variable highlighting (`${VAR_NAME}`)
- Automatic tab configuration for TAML files
- Indentation-based code folding
- Line comment toggle support
- Language configuration for TAML

### Features
- **Comments**: Lines starting with `#` are highlighted as comments
- **Keys**: Text before tabs highlighted as entity names
- **Values**: Text after tabs highlighted with type detection
- **Booleans**: `true`, `false`, `yes`, `no`, `on`, `off`
- **Null values**: `null`, `nil`, `~`
- **Numbers**: Integer and floating-point number detection
- **Environment variables**: `${VARIABLE}` pattern matching

### Editor Configuration
- Forces tabs instead of spaces for TAML files
- Sets tab size to 1
- Disables automatic indentation detection
- Enables indentation-based folding

## [Unreleased]

### Planned Features
- Snippet support for common TAML patterns
- Validation and error detection
- Auto-completion for keys
- Document outline view
- Breadcrumb navigation
- Formatting provider
- Hover tooltips with value types
