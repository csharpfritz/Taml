# TAML Project Decisions

---

## Decision: TAML Project State Analysis & Milestone Plan

**Author:** Rick (Lead/Architect)  
**Date:** 2026-03-05  
**Status:** Proposed  
**Requested By:** Jeffrey T. Fritz

### Context

Jeff asked for a comprehensive analysis of the TAML project: what we have, what the spec defines, where the gaps are, and a plan for three milestones going forward. A full audit of every specification document, all three parser implementations, the VSCode extension, the LSP server, examples, and standards test suite has been completed.

### Key Findings

#### Spec vs Implementation Drift
- TAML-SPEC.md is at v0.2 but TAML-IEEE-SPEC.md is at v0.1 — version mismatch
- The main spec defines raw text blocks (`...`), ISO 8601 dates, extended booleans (yes/no/on/off), and collection-of-objects via duplicate keys — **none** of these are implemented in any parser
- Python and JS parsers only recognize `true`/`false` for booleans (not the full set)
- No parser handles the `...` raw text escape mechanism
- No parser handles duplicate bare keys for collections of objects

#### Implementation Parity Issues
- .NET has the most complete ecosystem (serializer, validator, converter, configuration provider, document model)
- Python and JS are minimal parse/stringify only — no validation API, no converter, no document model
- .NET's deserializer always runs in strict mode (throws on spaces); Python/JS have lenient/strict toggle
- Type detection differs: .NET handles DateTime, Guid, TimeSpan in serialization; Python/JS have no date handling

#### Tooling Gaps
- VSCode extension is syntax-only (TextMate grammar) — no LSP integration in the packaged extension
- LSP server exists as separate package but isn't bundled with the VSCode extension
- LSP provides structural validation only — no type checking, no raw text support, no completion
- No CLI tool is built (dotnet TAML.CLI exists as empty project)

### Decision

Propose three milestones as described in the full analysis report. Priority order:
1. **Foundation** — spec alignment, implementation parity, missing core features
2. **Tooling & Quality** — editor experience, test suites, packaging
3. **Growth** — new features, ecosystem integration, adoption

### Rationale

The project has a solid spec and three working parsers, but significant feature gaps between what the spec promises and what the code delivers. Closing these gaps is prerequisite to any public release or adoption push.

**Decision Owner:** Jeffrey T. Fritz  
**Status:** Awaiting approval

---

## Decision: Raw Text Implementation Priority

**Authors:** Glenn (JavaScript), Shane (.NET), Daryl (Python)  
**Date:** 2026-03-05  
**Status:** Proposed  
**Severity:** 🔴 CRITICAL

### Issue

TAML-SPEC.md defines a raw text feature (Rules 10, 34-37, lines 92-200) that allows values to contain tabs, newlines, and special formatting via the `...` syntax. **This feature is not implemented in any parser and has zero test coverage.**

### Current State

**Missing from all three implementations:**
- ❌ Parser doesn't handle `...` indicator
- ❌ Serializer can't output raw text blocks
- ❌ LSP has no validation for raw text rules
- ❌ Zero test coverage in any implementation

### Impact

Users **cannot:**
- Parse/serialize TAML with embedded SQL, shell scripts, HTML, XML
- Include text with literal tabs or newlines
- Use TAML for documentation with code examples
- Work with formatted content without pre-escaping

**Workaround:** Escape content manually or use external files.

### Fix Effort Estimate

- **JavaScript:** ~8-12 hours (parser + serializer + 10 tests)
- **Python:** ~8-12 hours (similar scope)
- **C#:** ~8-12 hours (similar scope)
- **LSP:** ~4-6 hours (validation + tests)
- **Total:** ~28-42 hours across team

### Questions for Leadership

1. Should we implement raw text before publishing any release?
2. Can we ship v1.0 with raw text as "planned feature"?
3. Should this be prioritized as Milestone 1 blocker?

### Recommendation

Implement in all three parsers as part of Foundation milestone (Milestone 1) to achieve spec compliance before any public release.

**Status:** Decision required from leadership

---

## Decision: Extended Boolean Support Gap

**Authors:** Shane (.NET), Glenn (JavaScript), Daryl (Python)  
**Date:** 2026-03-05  
**Status:** Proposed  
**Severity:** ⚠️ MEDIUM

### Issue

TAML-SPEC.md (Section 3.3.2) defines extended boolean support: `true`, `false`, `yes`, `no`, `on`, `off`, `1`, `0` (all case-insensitive). Current implementations only recognize a subset.

### Current Implementation Status

**C#/.NET:**
```csharp
if (targetType == typeof(bool))
    return value.ToLower() == "true";  // Line 738 - INCOMPLETE
```
Only recognizes: `true`, `false`

**Python:** Only `true`, `false`

**JavaScript:** Only `true`, `false`

### Questions for Team

1. Fix immediately (5-line change) or defer?
2. Should absence be documented as limitation?
3. Add tests for each boolean variant?

### Complexity

LOW — < 10 lines per implementation

### Recommendation

Fix immediately as part of Foundation milestone. Quick win that closes spec gap.

**Status:** Awaiting approval

---

## Decision: ISO 8601 Date/Time Support

**Author:** Dale (QA/Tester)  
**Date:** 2026-03-05  
**Status:** Proposed  
**Severity:** 🔴 CRITICAL

### Issue

TAML-SPEC.md (Rules 17-19) defines automatic detection and type conversion for ISO 8601 dates, times, and datetimes. **Zero tests exist in any implementation, and no parser implements date detection.**

### Current Behavior

All date/time values are currently treated as strings.

### Spec Requires Support For

- **Dates:** `2024-01-15`, `2024-01`, `2024`
- **Times:** `14:30:00`, `14:30`, `14:30:00.123`
- **DateTime:** `2024-01-15T14:30:00Z`, with timezone offsets (`+09:00`, `-05:00`)
- **Week dates:** `2024-W03`, `2024-W03-4`
- **Ordinal dates:** `2024-015`

### Test Coverage Impact

Currently 0% — requires comprehensive test suite for all variants in each implementation.

### Recommendation

Include in Foundation milestone as Milestone 1.2 task following raw text implementation.

**Status:** Awaiting approval

---

## Decision: Test Coverage Gaps

**Author:** Dale (QA/Tester)  
**Date:** 2026-03-05  
**Status:** Proposed  
**Severity:** 🔴 CRITICAL

### Current Coverage Analysis

- **Total Spec Rules:** 42
- **Fully Tested:** 17 (40%)
- **Partially Tested:** 15 (36%)
- **Not Tested:** 10 (24%)
- **Total Tests:** 236 across all implementations

### Zero-Coverage Features

1. **Raw Text Mode (Rules 10, 34-37)** — Blocks any content with tabs/newlines
2. **ISO 8601 Dates/Times (Rules 17-19)** — No automatic type detection
3. **Collection-of-Objects via Duplicate Keys (Rules 35-37)** — No implementation
4. **Extended Booleans (partially)** — Missing test variants

### Test Files to Create

- `python/tests/test_raw_text.py`
- `javascript/test_raw_text.js`
- `dotnet/TAML.Tests/TamlRawTextTests.cs`
- `python/tests/test_iso_dates.py`
- `javascript/test_iso_dates.js`
- `dotnet/TAML.Tests/TamlIsoDateTests.cs`

### Recommendation

As part of Tooling & Quality milestone (Milestone 2), establish 95%+ spec rule coverage across all implementations.

**Status:** Awaiting approval

---

## Summary of Proposed Actions

### Milestone 1: Foundation (Spec Alignment & Implementation Parity)
- [ ] Implement raw text mode in all three parsers
- [ ] Implement ISO 8601 date/time detection in all three parsers
- [ ] Fix extended boolean support in Python and JavaScript
- [ ] Synchronize TAML-SPEC.md and TAML-IEEE-SPEC.md versions
- [ ] Achieve 95% spec rule coverage in test suite

**Estimated Effort:** 40-50 hours

### Milestone 2: Tooling & Quality
- [ ] Integrate LSP into VSCode extension
- [ ] Fix .NET Release build configuration
- [ ] Implement CLI tool (TAML.CLI)
- [ ] Performance baseline and optimization
- [ ] Comprehensive documentation

**Estimated Effort:** 30-40 hours

### Milestone 3: Growth
- [ ] Community feedback and feature requests
- [ ] Ecosystem integration opportunities
- [ ] Public release (v1.0)

---

**Status:** All decisions in Proposed state, awaiting leadership review and prioritization gate.

---

## Clarification Set: TAML-SPEC.md v0.2.1

**Author:** Rick (Lead/Architect)  
**Date:** 2026-03-05  
**Status:** Approved  
**Scope:** Spec ambiguity resolutions for Milestone 1 implementation

### Context

Five critical ambiguities in TAML-SPEC.md were preventing consistent implementation across Python, JavaScript, and .NET parsers. Rick resolved all five clarifications to unblock Milestone 1 work.

### Clarification 1: `1`/`0` Are Integers, Not Booleans

**Problem:** Spec said `1`/`0` are "interpreted as booleans only when context clearly expects a boolean," but TAML has no schema system to provide context.

**Decision:** `1` and `0` are **integers by default**. They are removed from the boolean truthy/falsy lists. Compliant parsers MUST NOT treat them as booleans without an explicit opt-in option (e.g., `numeric_booleans: true`). A future schema extension MAY reintroduce context-dependent interpretation.

**Rationale:** Without a schema system, there is no "context." Treating `1`/`0` as booleans would make it impossible to store the integer values 1 and 0 in a TAML document. Integers are the safer default.

**Spec Impact:** Boolean detection list updated: `true/false/yes/no/on/off` (6 values, not 8).

---

### Clarification 2: Bare `2024` Is an Integer, Not a Date

**Problem:** The spec listed `YYYY` as a valid ISO 8601 date pattern, but `2024` is ambiguous — it matches both integer and the year-only date pattern. The type detection order had dates before numbers, meaning `2024` would become a date.

**Decision:** Bare four-digit values like `2024` are **integers**. The minimum pattern for automatic date detection is `YYYY-MM` (year-month with hyphen). The `YYYY`-only pattern is removed from the date detection rules.

**Rationale:** Four-digit numbers are overwhelmingly used as integers (counts, years-as-data, ports, etc.). Treating them as date objects would be surprising and lossy. Users who need a year-only date should use `2024-01-01`.

**Spec Impact:** Supported ISO 8601 Formats section updated. Detection rule #1 now requires `YYYY-MM` minimum.

---

### Clarification 3: Empty Parents Resolve to Empty Maps

**Problem:** Spec said empty parents "can be treated as either Maps or Collections based on implementation preference." This guaranteed inconsistency across Python, JS, and .NET parsers.

**Decision:** An empty parent with no children MUST resolve to an **empty map** (`{}` / `dict()` / `Dictionary<string, object>`).

**Rationale:** Maps are the more common and safer default. A parent key implies a namespace/container, and in most real-world use cases (configs, objects), the empty state of a container is an empty object. If an empty list is needed, omit the parent or use application conventions.

**Spec Impact:** Section 3.7 updated to mandate empty map behavior across all implementations.

---

### Clarification 4: Key Characters — Only Tab and Newline Are Forbidden

**Problem:** Spec said key character restrictions were "implementation-dependent," guaranteeing that the same document would parse differently across implementations.

**Decision:** Keys may contain **any Unicode characters except tab (`\t`) and newline (`\n`/`\r`)**. All other characters — spaces, hyphens, punctuation, `#`, non-ASCII — are literal parts of the key. The `#` character is only a comment indicator at the start of a line.

**Rationale:** TAML's parsing is tab-based. The tab character is the only structural delimiter within a line, so it's the only character that cannot appear in keys. Restricting other characters (spaces, special chars) would be arbitrary and reduce TAML's utility for representing diverse data. Implementations SHOULD recommend simple alphanumeric keys for interop, but MUST accept all valid keys.

**Spec Impact:** Section 2.2 clarified. Validation rule renamed from "Invalid Characters in Keys (Optional Strictness)" to "Key Character Rules (Deterministic)."

---

### Clarification 5: Trailing Whitespace SHOULD Be Trimmed

**Problem:** Spec said trailing whitespace "MAY be trimmed" — the weakest RFC 2119 keyword, providing no real guidance.

**Decision:** Trailing whitespace SHOULD be trimmed from values **by default** in both lenient and strict modes. Implementations that need to preserve trailing whitespace MUST provide an explicit opt-in option (e.g., `preserve_trailing_whitespace: true`). Raw text blocks (`...`) are the canonical way to represent values with intentional trailing whitespace.

**Rationale:** Trailing whitespace is nearly always unintentional. Preserving it by default creates invisible bugs and cross-platform inconsistencies. The "MAY" wording left parsers free to do anything, but users expect consistent behavior. Trimming is the safe default, and raw text blocks handle the rare intentional case.

**Spec Impact:** Section 3.2 updated. Raw text blocks documented as the proper way to preserve whitespace.

---

## Implementation Decision: Extended Booleans (Glenn, Shane, Daryl)

**Authors:** Glenn (JavaScript), Shane (.NET), Daryl (Python)  
**Date:** 2026-03-05  
**Status:** Implemented ✅  
**Scope:** All 3 parsers

### Decisions Made

1. **`1`/`0` Stay as Integers**
   - Do NOT treat `1`/`0` as booleans in auto-detection
   - Only convert to bool when target type is explicitly `bool`
   - Rationale: Prevents data loss (port number `0` shouldn't become `false`)

2. **Boolean Detection Order**
   - Check booleans BEFORE dates in type detection
   - Prevents date-like strings from being misclassified
   - Order: booleans → dates → numbers → strings

3. **Case-Insensitive Matching**
   - All implementations use case-insensitive detection for `yes/no/on/off/true/false`
   - Serializer always emits canonical lowercase form (`true`/`false`)

### Implementation Notes

**JavaScript:** Uses Set-based detection for O(1) lookup  
**Python:** Uses frozenset for O(1) immutable lookup, PEP 8 compliant  
**.NET:** Uses HashSet with StringComparer.OrdinalIgnoreCase for case-insensitive matching

---

## Implementation Decision: Raw Text Blocks (Glenn, Shane, Daryl)

**Authors:** Glenn (JavaScript), Shane (.NET), Daryl (Python)  
**Date:** 2026-03-05  
**Status:** Implemented ✅  
**Scope:** All 3 parsers

### Decisions Made

1. **`...` Indicator Semantics**
   - Value of `...` indicates start of raw text block
   - Subsequent indented lines (indent > parent) collected as raw text content
   - Structural indentation stripped (lines dedented by parent+1 level)

2. **Content Preservation**
   - All tabs and newlines within raw text preserved as literal characters
   - Blank lines within raw text treated as content (preserved)
   - Trailing blank lines stripped before joining (cleanup)

3. **Serialization Detection**
   - Any string containing `\n` or `\t` serialized as raw text block
   - Parent key followed by `...` with indented content lines
   - All three parsers support round-trip serialization

### Implementation Notes

**JavaScript:** Line-by-line collection without regex, preserves content tabs/newlines  
**Python:** Helper `_collect_raw_text()` with blank line tracking  
**.NET:** Rewrote ParseLines to preserve blank lines; line splitting changed from RemoveEmptyEntries

---

## Implementation Decision: ISO 8601 Date Detection (Glenn, Shane, Daryl)

**Authors:** Glenn (JavaScript), Shane (.NET), Daryl (Python)  
**Date:** 2026-03-05  
**Status:** Implemented ✅  
**Scope:** All 3 parsers

### Decisions Made

1. **Supported Patterns**
   - **Date-only:** `YYYY-MM-DD` (e.g., `2024-01-15`)
   - **DateTime:** `YYYY-MM-DDTHH:MM:SS` with optional fractional seconds
   - **Timezone:** Optional Z suffix or ±HH:MM offset
   - **Bare years** (e.g., `2024`) do NOT match (stay as integers)

2. **Conservative Scope**
   - No support for week dates (`2024-W03`), ordinal dates (`2024-015`), durations (`P1DT2H`), intervals
   - These features can be added in future milestones without breaking changes
   - Design intentionally avoids over-matching to prevent false positives

3. **Detection Order**
   - ISO dates checked after booleans, before numbers
   - Ensures `2024-01-15` detected as date, not subtraction expression

### Implementation Notes

**JavaScript:** Regex-based with toISOString() serialization  
**Python:** Regex + datetime.fromisoformat() with Z suffix preprocessing  
**.NET:** Regex with DateTime.TryParseExact using ISO format

---

## Implementation Decision: Duplicate Bare Keys (Glenn, Shane, Daryl)

**Authors:** Glenn (JavaScript), Shane (.NET), Daryl (Python)  
**Date:** 2026-03-05  
**Status:** Implemented ✅  
**Scope:** All 3 parsers

### Decisions Made

1. **Representation**
   - Duplicate bare keys at same level create collection of objects
   - Stored as `list` of `dict`/`object` in all implementations
   - Enables round-trip serialization (serialize back to repeated keys)

2. **Detection Method**
   - **JavaScript:** Lookahead + runtime key-exists check
   - **Python:** isinstance() checks on existing values
   - **.NET:** Pre-scan for duplicate bare keys in DeserializeDictionary()

3. **Serialization**
   - List of dicts serialized as repeated parent key with indented children
   - Parent key reused as child key (preserves round-trip structure)
   - Serializer detects pattern: `list` of `dict` values

### Example

**TAML Input:**
```
games
    games
        title  Pac-Man
    games
        title  Space Invaders
```

**Parsed:**
```python
{"games": [{"title": "Pac-Man"}, {"title": "Space Invaders"}]}
```

**Serialized back:**
```
games
    games
        title  Pac-Man
    games
        title  Space Invaders
```

---

**Status:** All Milestone 1 implementation decisions approved and completed. Ready for v0.3 release candidate.
