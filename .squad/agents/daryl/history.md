# Daryl — History

## Project Context

- **Project:** TAML (Tab Annotated Markup Language)
- **Description:** A minimalist hierarchical data serialization format using only tabs and newlines for structure. Implementations in Python, JavaScript, and C#/.NET, plus VSCode extension and LSP.
- **User:** Jeffrey T. Fritz
- **Tech Stack:** Python, PEP 8, type hints, pytest, pip
- **Key Files:** python/

## Learnings

- Session started 2026-03-05. Team assembled with Walking Dead S1 casting.
- **2026-03-05 Audit**: Completed thorough audit of Python TAML implementation.
  - Implementation is mature and production-ready with 31 passing unit tests
  - All tests pass (test_parser.py: 16 tests, test_serializer.py: 8 tests, test_roundtrip.py: 6 tests + test_basic.py)
  - Type hints are comprehensive across all modules (100% coverage for public APIs)
  - PEP 8 compliant with proper docstrings (PEP 257 compliant)
  - **Major gaps identified:**
    1. **Raw text mode (`...`) not implemented** - Feature in spec section 3.10 and detailed in spec lines 94-209
    2. **Extended boolean type conversion incomplete** - Spec supports `yes/no/on/off`, implementation only supports `true/false`
    3. **Date/Time ISO 8601 support not implemented** - Spec section 4.6 (lines 362-491) specifies comprehensive date/time support
    4. **Scientific notation not tested** - `_convert_type()` can handle it but no test cases
    5. **Negative number handling not tested** - Parser accepts but not explicitly tested
    6. **Decimal without leading zero (.75) not tested**
  - Type conversion works correctly for: null (~), empty string (""), bool (true/false), int, float, string
  - All validation modes working: strict mode, lenient mode, mixed indentation detection
  - Packaging complete: setup.py, pyproject.toml, pip-installable
  - Documentation excellent: README.md, USAGE.md, IMPLEMENTATION.md, docstrings, examples
- **Milestone 1 Implementation Complete** (all 4 features):
  1. **Extended Booleans**: Case-insensitive `true/yes/on` → True, `false/no/off` → False. Used frozenset for O(1) lookup. 1/0 kept as integers per task spec. Serializer always emits canonical `true`/`false`.
  2. **Raw Text Blocks**: `...` value triggers raw text collection. Used `skip_to` index with `_collect_raw_text()` helper. Blank lines within raw text preserved; trailing blanks stripped. Serializer detects strings with `\n` or `\t` and emits `...` blocks.
  3. **ISO 8601 Dates**: Regex patterns for `YYYY-MM-DD` and `YYYY-MM-DDTHH:MM:SS` variants. Returns `date` or `datetime` objects. Z suffix handled via replace for Python compat. Detection order: booleans → dates → numbers → strings.
  4. **Duplicate Bare Keys**: Detects when a bare key already exists as dict, converts to list of dicts. Subsequent duplicates append to existing list. Serializer emits repeated parent keys for list-of-dicts.
  - 95 total tests passing (31 original + 64 new in test_milestone1.py)
  - Remaining gaps: scientific notation tests, negative number tests, decimal without leading zero tests

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
