# Dale — History

## Project Context

- **Project:** TAML (Tab Annotated Markup Language)
- **Description:** A minimalist hierarchical data serialization format using only tabs and newlines for structure. Implementations in Python, JavaScript, and C#/.NET, plus VSCode extension and LSP.
- **User:** Jeffrey T. Fritz
- **Tech Stack:** pytest (Python), Node.js native test runner (JS), xUnit/NUnit (.NET)
- **Key Files:** python/, javascript/test.js, dotnet/TAML.Tests/, TAML-SPEC.md, examples/

## Learnings

### Test Coverage Audit Findings (2026-01-15)

#### Critical Gaps Identified
1. **Raw Text Mode Completely Untested** - The `...` syntax for preserving tabs/newlines in values (Rules 10, 34-37) is a major spec feature with zero test coverage across all three implementations. This represents a significant feature gap.

2. **ISO 8601 Date/Time Detection Missing** - Rules 17-19 define automatic detection of dates, times, and datetimes in ISO 8601 format, but no tests verify this functionality in any implementation. If not implemented in parsers, this is a breaking gap.

3. **Validation Consistency Issue** - Python and JavaScript have minimal validation testing (only space indentation). .NET has comprehensive validation but Python/JS are missing:
   - Mixed indentation detection
   - Inconsistent indentation levels
   - Orphaned indentation detection
   - Tab-in-value validation (Python has this)

4. **Quote Usage Rules** - Rule-6 (no quotes except `""`) is only partially tested in .NET (rejects quoted non-empty). Python and JavaScript don't test this at all.

#### Cross-Implementation Insights
- **Python (57% coverage):** Best test suite with 31 tests covering parsing, serialization, and roundtrip. Has strict/lenient mode testing.
- **JavaScript (50% coverage):** Basic but solid parsing tests. Lacks validation testing and format conversion.
- **.NET (48% coverage):** Most comprehensive for validation and format conversion (JSON/XML/YAML). Weakest for type detection edge cases.

#### Type Detection Completeness
- All implementations test basic types (bool, int, float)
- Missing edge cases:
  - Boolean variants: `yes`, `no`, `on`, `off`, `1`, `0` (case-insensitive)
  - Decimal edge cases: scientific notation, leading/trailing decimals
  - ISO 8601 formats entirely absent

#### Examples Not Used in Tests
- 6 example files exist in `/examples/` (web-app-config, api-documentation, cloud-infrastructure, etc.)
- None are used in any test suite
- Opportunity: Create integration tests that parse and validate examples

#### Recommendations Priority
1. **Phase 1 (Critical):** Add raw text tests, comprehensive validation tests, ISO 8601 support
2. **Phase 2 (Important):** Type detection edge cases, cross-impl validation consistency, integration tests
3. **Phase 3 (Enhancement):** Quote validation, deep nesting stress tests, real-world roundtrip tests

#### Test Count by Implementation
| Implementation | Total Tests | Rules Covered | Gaps |
|---|---|---|---|
| Python | 31 | 24/42 (57%) | Raw text, ISO dates, quote rules |
| JavaScript | 25 | 21/42 (50%) | Validation, raw text, ISO dates |
| .NET | 34 | 20/42 (48%) | Raw text, ISO dates, type detection edge cases |

---

- Session started 2026-03-05. Team assembled with Walking Dead S1 casting.
- Test coverage audit completed 2026-01-15. Identified 10 critical/important rules with no test coverage.
