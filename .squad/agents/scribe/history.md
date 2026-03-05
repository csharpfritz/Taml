# Project Context

- **Project:** NewTaml
- **Created:** 2026-03-05

## Core Context

Agent Scribe initialized as documentation specialist maintaining history, decisions, and technical records for the TAML project.

**Key Project Facts:**
- TAML is a minimalist hierarchical data serialization format using only tabs and newlines
- Three parser implementations: Python, JavaScript, C#/.NET
- VSCode extension and Language Server Protocol implementation exist
- Specification: TAML-SPEC.md (v0.2) and TAML-IEEE-SPEC.md (v0.1)

## Team Findings — 2026-03-05 Project Analysis

### Critical Gaps Across All Implementations

**Raw Text Mode (`...` syntax):**
- NOT IMPLEMENTED in any parser
- ZERO test coverage across all implementations
- Blocks users from embedding SQL, shell scripts, HTML, XML
- CRITICAL priority for Milestone 1
- Estimated fix: 8-12 hours per language + 4-6 hours for LSP

**ISO 8601 Date/Time Detection:**
- NOT IMPLEMENTED in any parser
- ZERO test coverage
- All date/time values currently parsed as strings
- Spec requires: dates, times, datetimes, week dates, ordinal dates
- CRITICAL priority for Milestone 1
- Estimated fix: 6-8 hours per language

**Extended Boolean Support:**
- PARTIALLY IMPLEMENTED
- Python and JavaScript only recognize `true`/`false`
- C#/.NET only recognizes `true`/`false` (despite more complete ecosystem)
- Spec requires: yes, no, on, off, 1, 0 (case-insensitive)
- MEDIUM priority, quick fix (< 10 lines per implementation)

**Collection-of-Objects via Duplicate Keys:**
- NOT IMPLEMENTED in any parser
- Feature defined in spec but not prioritized for initial milestone

### Implementation-Specific Notes

**Python (Daryl):**
- 77% feature complete, 31 tests passing
- Excellent code quality with full type hints
- Production-ready core functionality
- Missing: raw text, ISO dates
- Minimal implementation (parser/stringify only)

**JavaScript (Glenn):**
- 85% feature complete, 25 tests passing
- Well-engineered parser with solid fundamentals
- VSCode extension has comprehensive syntax highlighting
- LSP server provides structural validation only
- Missing: raw text blocks, LSP integration in packaged extension

**C#/.NET (Shane):**
- 75% spec compliance, 180 tests passing
- Most complete ecosystem (serializer, validator, converter, config provider, document model)
- Type-aware serialization (DateTime, Guid, TimeSpan)
- Build issue: Release configuration fails (MSBuild problem)
- Missing: raw text, extended boolean tests, date detection tests

### Test Coverage Analysis (Dale)

- **Total Spec Rules:** 42
- **Fully Tested:** 17 (40%)
- **Partially Tested:** 15 (36%)
- **Zero Coverage:** 10 (24%)
- **Total Tests:** 236 across all implementations

### Specification Issues

- Version mismatch: TAML-SPEC.md (v0.2) vs TAML-IEEE-SPEC.md (v0.1)
- Need version synchronization before public release

### Tooling Assessment

- VSCode extension is syntax-only (TextMate grammar)
- LSP not integrated into packaged extension
- CLI tool (TAML.CLI) exists as empty project

## Recent Updates

📌 Full-team analysis session completed: Rick, Glenn, Shane, Daryl, Dale  
📌 5 orchestration logs created documenting individual audit findings  
📌 Session log created summarizing full-team analysis  
📌 Decision inbox consolidated into decisions.md with all 5 critical decisions  
📌 Team-wide findings documented for cross-agent awareness

## Learnings

**Consistent Findings Across All Three Implementations:**
1. Raw text is the #1 missing feature blocking real-world usage
2. Date/time support is needed but lower priority than raw text
3. Extended booleans is a quick fix across all implementations
4. Test coverage has significant gaps (24% zero coverage)

**Next Phase:**
- Await leadership decision on milestone prioritization
- Foundation Milestone should address: raw text, ISO dates, extended booleans, test coverage
- Total estimated effort: 40-50 hours for Milestone 1, 30-40 hours for Milestone 2

**Architecture Notes:**
- Python and JS are minimal (parse/stringify); .NET has full ecosystem
- No validation API in Python/JS; .NET has validator but raw text not validated
- Strict vs lenient mode: inconsistent across implementations (.NET always strict, Python/JS have toggle)

## Cross-Agent Notes

All team members should be aware of:
- Raw text gap is CRITICAL and blocks all implementations equally
- Python/JavaScript need extended boolean fixes (C# only partially implemented)
- Test coverage is insufficient (only 40% of spec rules fully tested)
- ISO dates not implemented but need comprehensive test suite when added
- Implementation parity issues: .NET has more features but Python/JS are simpler and easier to maintain
