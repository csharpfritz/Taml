# Shane — History

## Project Context

- **Project:** TAML (Tab Annotated Markup Language)
- **Description:** A minimalist hierarchical data serialization format using only tabs and newlines for structure. Implementations in Python, JavaScript, and C#/.NET, plus VSCode extension and LSP.
- **User:** Jeffrey T. Fritz
- **Tech Stack:** C#/.NET, xUnit/NUnit, file-scoped namespaces, nullable reference types
- **Key Files:** dotnet/, dotnet/TAML.Core/, dotnet/TAML.Tests/, dotnet/TAML.Configuration/

## Learnings

- Session started 2026-03-05. Team assembled with Walking Dead S1 casting.

### Audit Session (Current)

#### .NET Implementation Status
- **Build Status:** Debug builds successfully. Release builds fail with MSBuild error in GenerateTargetFrameworkMonikerAttribute.
- **Test Coverage:** 180 unit tests, all passing. 3,203 lines of test code. Good coverage for implemented features.
- **Projects:** 3 active (TAML.Core, TAML.Configuration, TAML.Tests), 1 empty (TAML.CLI).
- **Code Quality:** Well-structured, good error handling with line numbers, comprehensive type support.

#### Critical Gaps Found
1. **Raw Text (`...`) NOT IMPLEMENTED** - Violates TAML-SPEC.md section 3.10. Entire feature missing. Tests: 0/15-20 needed.
2. **Extended Boolean Support INCOMPLETE** - Only recognizes `true`/`false`, not `yes`/`no`/`on`/`off`/`1`/`0`. Affects: TamlSerializer.ConvertValue() line 738.
3. **Strict Parsing Mode NOT IMPLEMENTED** - No way to enforce strict validation. Parameter missing from all Deserialize methods.
4. **Two Validation Rules Missing** - TabInKey and ParentWithValue enums exist but not implemented in TamlValidator.
5. **File Watching Missing** - TamlConfigurationProvider returns static IChangeToken, no file system watching.
6. **NuGet Packaging NOT READY** - No package metadata, Release build broken, can't distribute.

#### What's Working Well
- ✅ Parsing/deserialization with proper line number tracking
- ✅ Serialization with correct format (lowercase booleans, quoted empty strings, ~ for null)
- ✅ Multi-tab separator support (visual alignment)
- ✅ Nested structures, collections, complex objects
- ✅ Type conversion (primitives, DateTime/ISO 8601, Guid, TimeSpan, Enum)
- ✅ Configuration integration (ASP.NET Core compatible)
- ✅ Format conversion (JSON, XML, YAML → TAML)
- ✅ File I/O with async support
- ✅ TAMLException with line context

#### Root Causes
- **Raw Text:** TamlSerializer.ParseLines() method (lines 275-350) doesn't check for `...` value indicator.
- **Boolean:** Line 738 hardcodes `value.ToLower() == "true"`. Needs expansion to all 8 truthy/falsy patterns.
- **Validation:** TamlValidator.ValidateLine() has enum values but missing switch/if checks (lines 275-284).

#### Architecture Notes
- Single responsibility: TamlValidator (syntax), TamlSerializer (parse/serialize), TamlDocument (wrapper), TamlConfigurationProvider (config).
- Type detection happens in Deserialize methods; doesn't use separate type detector.
- Reflection-based for complex object serialization (uses BindingFlags.Public | BindingFlags.Instance).
- Internal line parsing class: TamlLine (immutable record with IndentLevel, Key, Value, HasValue).

#### Deliverable
Wrote comprehensive 12-section audit report to `.squad/agents/shane/DOTNET-AUDIT-REPORT.md` covering:
- Project structure (solution, 4 projects)
- All 7 public classes with full API signatures
- Features implemented vs. missing
- Test coverage analysis
- Build/release status
- Specification compliance matrix
- Priority 1-3 gaps with complexity/impact
- NuGet packaging needs
- 23KB report with code examples

#### Next Steps (For Resolver)
1. Implement raw text feature (highest priority, blocks spec compliance)
2. Extend boolean detection (quick fix, 5 lines)
3. Implement strict mode (medium effort, good for validation)
4. Fix MSBuild release config (unknown effort, needed for packaging)

### Milestone 1 Implementation

#### Features Implemented
1. **Extended Booleans** — Added `TruthyValues`/`FalsyValues` HashSets with case-insensitive matching for `true/yes/on` and `false/no/off`. Updated `ConvertValue` for typed `bool` targets and added `InferTypedValue` for untyped `object` auto-detection. `1`/`0` intentionally stay as integers per spec note about ambiguous contexts.
2. **Raw Text Blocks (`...`)** — Rewrote `ParseLines` to handle raw text mode: when value is `...`, collects subsequent indented lines verbatim, stripping structural indent (parent+1). Preserves content tabs, newlines, and blank lines. Added `SerializeRawTextBlock` for serialization (strings with `\n` → `...` format).
3. **ISO 8601 Date Detection** — Added `Iso8601Pattern` regex for `YYYY-MM-DD` through full datetime+timezone patterns. Bare years like `2024` don't match (require `-MM` minimum). Detection order in `InferTypedValue`: booleans → dates → numbers → strings.
4. **Duplicate Bare Keys → Collection** — Pre-scans for duplicate bare keys at current indent in `DeserializeDictionary`. Duplicate keys are collected into `List<Dictionary<string, object?>>`. Added `SerializeDuplicateKeyCollection` and `IsListOfDictionaries` for round-trip serialization.

#### Supporting Changes
- Added `System.Globalization` and `System.Text.RegularExpressions` imports.
- Fixed `TamlDocument.FlattenInternal` to output lowercase booleans (`true`/`false`) and ISO format dates instead of .NET default `ToString()`.
- Changed `ParseLines` line splitting from `Split(new[] { '\n', '\r' }, RemoveEmptyEntries)` to proper `\r\n` normalization to support blank lines in raw text blocks.

#### Test Results
- 55 new tests added (18 boolean, 12 raw text, 10 date, 6 duplicate keys, 9 Theory variants).
- Total: 235 tests, all passing. Zero regressions.

## Milestone 1 Cross-Implementation Status

**Completion Date:** 2026-03-05  
**Status:** ✅ COMPLETE  

### Implementation Notes for All Agents

All three parsers (JavaScript, Python, .NET) completed Milestone 1 simultaneously:

**Spec Clarifications (Rick):**
- 5 ambiguities resolved in TAML-SPEC.md v0.2.1
- Extended booleans = `true/false/yes/no/on/off` (6 values, not 8)
- `1`/`0` strictly numeric
- Empty parents → empty maps (deterministic)
- Keys allow any chars except `\t` and `\n`/`\r`
- Trailing whitespace trimmed by default
- All decisions merged into `.squad/decisions/decisions.md`

**Feature Parity Achieved:**
- Extended Booleans: Case-insensitive, canonical serialization
- Raw Text: `...` indicator, indented collection, content preservation
- ISO 8601: Conservative 4-core patterns (date-only, datetime+timezone)
- Duplicate Keys: List-of-dicts representation for round-trip fidelity

**Test Results:**
- JavaScript: 54 total (29 new), 100% pass ✅
- .NET: 235 total (55 new), 100% pass ✅
- Python: 95 total (64 new), 100% pass ✅
- **Total: 384 tests passing**

**Release Status:** All implementations spec-compliant and ready for v0.3 release candidate.
