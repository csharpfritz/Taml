# Spec Clarifications — v0.2.1 Decisions

**Author:** Rick (Lead/Architect)
**Date:** 2026-03-05
**Scope:** TAML-SPEC.md ambiguity resolutions for Milestone 1

---

## Decision 1: `1`/`0` Are Integers, Not Booleans

**Problem:** Spec said `1`/`0` are "interpreted as booleans only when context clearly expects a boolean," but TAML has no schema system to provide context.

**Decision:** `1` and `0` are **integers by default**. They are removed from the boolean truthy/falsy lists. Compliant parsers MUST NOT treat them as booleans without an explicit opt-in option (e.g., `numeric_booleans: true`). A future schema extension MAY reintroduce context-dependent interpretation.

**Rationale:** Without a schema system, there is no "context." Treating `1`/`0` as booleans would make it impossible to store the integer values 1 and 0 in a TAML document. Integers are the safer default.

**Impact:** Parsers currently treating `1`/`0` as booleans must change to integer. The Type Conversion Summary table is updated.

---

## Decision 2: Bare `2024` Is an Integer, Not a Date

**Problem:** The spec listed `YYYY` as a valid ISO 8601 date pattern, but `2024` is ambiguous — it matches both integer and the year-only date pattern. The type detection order had dates before numbers, meaning `2024` would become a date.

**Decision:** Bare four-digit values like `2024` are **integers**. The minimum pattern for automatic date detection is `YYYY-MM` (year-month with hyphen). The `YYYY`-only pattern is removed from the date detection rules.

**Rationale:** Four-digit numbers are overwhelmingly used as integers (counts, years-as-data, ports, etc.). Treating them as date objects would be surprising and lossy. Users who need a year-only date should use `2024-01-01`.

**Impact:** The "year only" example is removed from Supported ISO 8601 Formats. Detection rule #1 updated to require `YYYY-MM` minimum.

---

## Decision 3: Empty Parents Resolve to Empty Maps

**Problem:** Spec said empty parents "can be treated as either Maps or Collections based on implementation preference." This guaranteed inconsistency across Python, JS, and .NET parsers.

**Decision:** An empty parent with no children MUST resolve to an **empty map** (`{}` / `dict()` / `Dictionary<string, object>`).

**Rationale:** Maps are the more common and safer default. A parent key implies a namespace/container, and in most real-world use cases (configs, objects), the empty state of a container is an empty object. If an empty list is needed, omit the parent or use application conventions.

**Impact:** All three parsers must return an empty map/dictionary/object for parentless keys. Any parser returning `null` or an empty list must be updated.

---

## Decision 4: Key Characters — Only Tab and Newline Are Forbidden

**Problem:** Spec said key character restrictions were "implementation-dependent," guaranteeing that the same document would parse differently across implementations.

**Decision:** Keys may contain **any Unicode characters except tab (`\t`) and newline (`\n`/`\r`)**. All other characters — spaces, hyphens, punctuation, `#`, non-ASCII — are literal parts of the key. The `#` character is only a comment indicator at the start of a line.

**Rationale:** TAML's parsing is tab-based. The tab character is the only structural delimiter within a line, so it's the only character that cannot appear in keys. Restricting other characters (spaces, special chars) would be arbitrary and reduce TAML's utility for representing diverse data. Implementations SHOULD recommend simple alphanumeric keys for interop, but MUST accept all valid keys.

**Impact:** Strict parsers that previously rejected spaces or special characters in keys must be updated to accept them. The validation rule is renamed from "Invalid Characters in Keys (Optional Strictness)" to "Key Character Rules."

---

## Decision 5: Trailing Whitespace SHOULD Be Trimmed

**Problem:** Spec said trailing whitespace "MAY be trimmed" — the weakest RFC 2119 keyword, providing no real guidance.

**Decision:** Trailing whitespace SHOULD be trimmed from values **by default** in both lenient and strict modes. Implementations that need to preserve trailing whitespace MUST provide an explicit opt-in option (e.g., `preserve_trailing_whitespace: true`). Raw text blocks (`...`) are the canonical way to represent values with intentional trailing whitespace.

**Rationale:** Trailing whitespace is nearly always unintentional. Preserving it by default creates invisible bugs and cross-platform inconsistencies. The "MAY" wording left parsers free to do anything, but users expect consistent behavior. Trimming is the safe default, and raw text blocks handle the rare intentional case.

**Impact:** Parsers that preserve trailing whitespace by default should switch to trimming. The raw text block feature already exists for cases needing preserved whitespace.

---

## Summary

All five decisions follow the principle of **removing implementation-dependent behavior** in favor of clear, deterministic rules. Each clarification is marked with `> **Clarification (v0.2.1):**` in the spec for traceability.

These decisions unblock Milestone 1 implementation work by Glenn (Python), Shane (JavaScript), and Daryl (.NET).
