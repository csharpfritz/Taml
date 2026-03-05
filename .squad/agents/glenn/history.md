# Glenn — History

## Project Context

- **Project:** TAML (Tab Annotated Markup Language)
- **Description:** A minimalist hierarchical data serialization format using only tabs and newlines for structure. Implementations in Python, JavaScript, and C#/.NET, plus VSCode extension and LSP.
- **User:** Jeffrey T. Fritz
- **Tech Stack:** JavaScript/Node.js, ES modules, JSDoc, Node native test runner
- **Key Files:** javascript/, tools/vscode-taml/, tools/taml-language-server/

## Learnings

- Session started 2026-03-05. Team assembled with Walking Dead S1 casting.

### Audit Learnings (2025-03-05)

**Parser Implementation (`javascript/index.js`):**
- 268 lines of clean, readable ES6 code with good error handling
- Stack-based parsing with lookahead detection for arrays vs objects
- 25 test cases with 100% pass rate; good coverage of core features
- Type conversion works for `true`/`false`, integers, and floats
- TAMLError class properly tracks line numbers for debugging
- Custom `convertType()` helper is efficient and correct

**Critical Gap - Raw Text (`...`) Not Implemented:**
- Both parser and serializer missing raw text support (TAML-SPEC.md rules 92-200)
- No parser logic for `...` value indicator or raw text blocks
- No serializer output for `...` blocks
- LSP has no validation for raw text indentation rules
- This gap exists across all three implementations (Python, JavaScript, C#)
- Affects real-world use: SQL queries, shell scripts, embedded HTML/XML all blocked
- Estimated fix: 20-40 hours for all three languages

**VSCode Extension (vscode-taml):**
- TextMate grammar complete with 9 syntax element patterns
- Smart editor configuration: forces tabs-only, disables space insertion
- Folding by indentation; comment toggle works with `#`
- Configuration includes unneeded patterns (`yes`/`no` as booleans, colon rules)
- Snippets/completion not yet implemented (documented as future work)

**Language Server (taml-language-server):**
- TypeScript implementation with 136 lines of validation logic
- 8 out of 9 validation rules implemented; good diagnostic messages
- Real-time error detection: space indentation, mixed tabs/spaces, orphaned indent, tab in values
- Configuration system for enable/disable + warnings toggle
- LSP features: only 3/9 implemented (text sync, diagnostics, config)
- Missing: hover, completion, formatting, code actions

**Test Coverage:**
- 25 tests all passing; covers basic parsing, serialization, round-trip
- Gaps: No raw text tests, no strict mode edge cases, no option combination tests
- Error message validation missing (only checks error type, not message)

**Cross-Implementation Consistency:**
- Parser, serializer, validation logic identical across Python, JavaScript, C#
- All have same raw text gap; all have same error handling patterns
- Excellent alignment for a multi-language project

**Code Quality Notes:**
- Parser: Unused `typeConversion` option in stringify (line 195)
- Parser: No input validation on `text` parameter
- LSP: No JSDoc/TSDoc comments
- No Unicode normalization handling
- No performance/stress testing

**Recommendations (Priority Order):**
1. **CRITICAL:** Implement raw text parsing + serialization + LSP validation
2. **Important:** Add 15+ new tests for raw text, strict mode, option combinations
3. **Important:** Validate error messages in tests
4. **Nice-to-have:** Add JSDoc comments to parser
5. **Nice-to-have:** Implement LSP hover/completion features (v0.2)

### Milestone 1 Implementation (2025-03-05)

**Implemented all 4 features in `javascript/index.js`:**

1. **Extended Booleans** — `convertType()` now uses case-insensitive Sets for `true/yes/on` and `false/no/off`. `1`/`0` stay as integers per task spec (differs from TAML-SPEC which lists them as booleans).

2. **Raw Text Blocks (`...`)** — Parser consumes indented lines after `...` value, strips structural indentation, preserves content tabs/newlines. Serializer detects strings with `\t` or `\n` and emits `...` blocks. Trailing blank lines in raw blocks are trimmed.

3. **ISO 8601 Dates** — Regex `^\d{4}-\d{2}-\d{2}...` detects date-only, datetime, Z, and offset patterns. Checked after booleans, before numbers (per spec type detection order). Bare years like `2024` stay as integers. Serializer uses `toISOString()`.

4. **Duplicate Bare Keys → Collection of Objects** — Two-pronged detection: (a) lookahead during parent key processing detects duplicate children, (b) runtime check when a bare key already exists in parent object converts to array. Serializer emits parent wrapper + repeated child keys at level+1.

**Test coverage:** 54 tests total (25 original + 29 new), 100% pass rate.

**Key design decisions:**
- Raw text uses line-by-line collection, not regex, for reliability
- Date regex intentionally conservative (no week dates, durations, or intervals — just the 4 core patterns)
- Collection-of-objects serializer reuses parent key name as child key (round-trip preserving)
