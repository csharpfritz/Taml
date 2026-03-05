# Decision: IEEE Spec Sync to v0.2

**Author:** Rick (Lead/Architect)  
**Date:** 2026-03-05  
**Status:** Completed  
**Requested by:** Jeffrey T. Fritz

## Summary

Synchronized TAML-IEEE-SPEC.md from v0.1 to v0.2 to match the authoritative TAML-SPEC.md (v0.2.1). The IEEE spec was missing 6 major feature areas that are now specified and implemented across all three parsers.

## Changes Made

### New Sections Added
1. **Section 7.6 — Raw Text Blocks**: Complete formal specification of the `...` raw text mechanism with 7 requirements covering syntax, indentation, content preservation, and termination.
2. **Section 7.7 — Data Structure Types**: Formalized Maps and Collections with structure detection rules and 3 detection requirements.
3. **Section 7.4.3 — Collection of Objects**: Duplicate bare keys mechanism with 4 requirements.
4. **Section 8.2.3 — ISO 8601 Date/Time Detection**: Full coverage of dates, times, combined, timezones, week dates, ordinal dates, durations, and intervals with 4 requirements.
5. **Section 9.4 — Raw Text Validation**: 2 validation rules for raw text indicator and indentation.
6. **Section 12.5 — Raw Text Security**: Injection risk considerations and size limit recommendations.

### Existing Sections Updated
- **Section 8.2.2 (Type Coercion)**: Extended with 6 boolean patterns (yes/no/on/off), type detection precedence order, explicit 1/0 integer rule, number patterns.
- **Section 6.4.1 (Key Token)**: Explicit key character rules — any Unicode except tab/newline.
- **Section 6.4.2 (Value Token)**: Trailing whitespace trimming standardized as default.
- **Section 7.3.1 (Parent Node)**: Empty parents → empty maps rule.
- **Section 8.3 (Type System)**: Expanded from 5 to 11 types; 6 inference rules.
- **Section 10.1 (Error Categories)**: 3 new error codes.
- **Section 11 (Conformance)**: Updated parser (12 MUST) and serializer (12 MUST) requirements.
- **Appendix A (EBNF Grammar)**: Fully rewritten with raw text, boolean, date, and structure productions; 10 grammar notes.

### Other
- Version bumped to 0.2, date updated.
- ISO 8601 added to normative references.
- 10 new terms added to definitions.
- 6 new examples (14.6–14.10).
- Migration guide expanded with 4 new best practices.

## Rationale

The IEEE spec serves as the formal, implementer-facing document. Having it lag behind the main spec by multiple feature areas creates risk — implementers consulting only the IEEE spec would miss critical features. All added features are already implemented in at least one parser and specified in TAML-SPEC.md.

## Impact

- No code changes required (spec-only update).
- All three parser implementations already support (or are being updated to support) these features.
- Future implementers now have a complete formal reference.
