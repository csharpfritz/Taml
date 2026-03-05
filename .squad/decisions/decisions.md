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
