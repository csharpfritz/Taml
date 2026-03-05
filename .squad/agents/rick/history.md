# Rick — History

## Project Context

- **Project:** TAML (Tab Annotated Markup Language)
- **Description:** A minimalist hierarchical data serialization format using only tabs and newlines for structure. Implementations in Python, JavaScript, and C#/.NET, plus VSCode extension and LSP.
- **User:** Jeffrey T. Fritz
- **Tech Stack:** Python, JavaScript/Node.js, C#/.NET, VSCode Extension, LSP
- **Key Files:** TAML-SPEC.md (authoritative spec), python/, javascript/, dotnet/, tools/

## Learnings

- Session started 2026-03-05. Team assembled with Walking Dead S1 casting.
- **2026-03-05 — Full Project Audit Completed.** Key architectural findings:
  - TAML-SPEC.md (v0.2) and TAML-IEEE-SPEC.md (v0.1) are out of sync. IEEE spec lacks raw text, dates, extended booleans, collection-of-objects.
  - Raw text blocks (`...`) are specified but not implemented in ANY parser (Python, JS, .NET).
  - ISO 8601 date/time detection is specified but not implemented in any parser.
  - Extended booleans (yes/no/on/off/1/0) are specified but Python and JS only recognize true/false.
  - Duplicate bare keys for collection-of-objects are specified but not implemented.
  - .NET has the richest ecosystem: TamlDocument, TamlValidator, TamlConverter (JSON/XML/YAML→TAML), TamlConfigurationProvider for Microsoft.Extensions.Configuration.
  - Python and JS are parse+stringify only — no validation API, no converters, no document model.
  - VSCode extension provides TextMate syntax highlighting but does NOT integrate the LSP server.
  - LSP server exists separately, provides structural validation (spaces, indentation, tabs-in-values) but no type checking or completion.
  - TAML.CLI project directory exists but is empty (just bin/obj).
  - Standards test suite at `/standards/0.1/` has 7 test files covering basic features.
  - Examples directory has 7 real-world .taml files (web config, API docs, cloud infra, game design, recipes, team directory).
  - Pattern: all three parsers use the same core algorithm (line-by-line, stack-based indent tracking, look-ahead for array detection). Good consistency foundation.
  - Python parser `_convert_type()` only handles true/false/int/float — no yes/no/on/off, no dates.
  - JS `convertType()` same — only true/false/int/float.
  - .NET serializer handles DateTime/DateTimeOffset via ISO 8601 `"o"` format, but the deserializer's ConvertValue doesn't auto-detect date strings.
- **2026-03-05 — Spec Clarifications (v0.2.1) Completed.** Resolved 5 ambiguities in TAML-SPEC.md to unblock Milestone 1:
  1. `1`/`0` are integers, not booleans. Removed from boolean truthy/falsy lists. No schema = no context for boolean interpretation.
  2. Bare `2024` is an integer, not a date. Minimum date pattern is `YYYY-MM`. Removed `YYYY`-only from date detection.
  3. Empty parents resolve to empty maps. No more "implementation preference" — deterministic behavior across all parsers.
  4. Keys allow any characters except `\t` and `\n`/`\r`. Spaces, `#`, punctuation, Unicode all valid. No more "implementation-dependent."
  5. Trailing whitespace SHOULD be trimmed by default in both lenient and strict modes. Raw text blocks for intentional whitespace.
  - All clarifications tagged with `> **Clarification (v0.2.1):**` for traceability.
  - Decisions documented in `.squad/decisions/decisions.md`.
  - NOTE: Extended booleans now = `true/false/yes/no/on/off` (6 values, not 8). `1`/`0` are strictly numeric.

## Milestone 1 Implementation Coordination

**Completion Date:** 2026-03-05  
**Status:** ✅ COMPLETE  
**Team:** Glenn (JavaScript), Shane (.NET), Daryl (Python)

### Cross-Implementation Summary

All three parsers now implement Milestone 1 features in parallel:

**Features Delivered (All 4):**
1. **Extended Booleans** — `true/yes/on` and `false/no/off`, case-insensitive. `1`/`0` stay as integers per clarification.
2. **Raw Text Blocks (`...`)** — All parsers collect indented lines, preserve content tabs/newlines, strip structural indent.
3. **ISO 8601 Dates** — Conservative 4-core patterns (date-only, datetime+timezone). Bare years stay as integers.
4. **Duplicate Bare Keys** — All represent as list-of-dicts for round-trip fidelity.

**Spec Compliance:** 100% for M1 features. All decisions merged into `.squad/decisions/decisions.md`.

**Test Results:**
- JavaScript: 54 total (25 original + 29 new), 100% pass ✅
- .NET: 235 total (180 original + 55 new), 100% pass ✅
- Python: 95 total (31 original + 64 new), 100% pass ✅
- **TOTAL: 384 tests, 100% pass rate**

**Release Readiness:** All implementations spec-compliant and ready for v0.3 release candidate.
