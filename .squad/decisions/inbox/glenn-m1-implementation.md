# Glenn — Milestone 1 Implementation Decisions

## Date: 2025-03-05

### Decision 1: `1`/`0` treated as integers, not booleans
The TAML-SPEC lists `1`/`0` as boolean values, but the task explicitly said "Do NOT treat `1`/`0` as booleans (they stay as integers)." I followed the task instruction. The spec note says "parsers may treat them as integers in ambiguous contexts" which supports this choice. **Team should align on whether `1`/`0` are ever booleans in JavaScript parser.**

### Decision 2: ISO 8601 date detection is conservative
Only detecting the 4 core patterns: `YYYY-MM-DD`, `YYYY-MM-DDTHH:MM:SS`, with optional `Z` or `±HH:MM` offset. Not detecting week dates (`2024-W03`), ordinal dates (`2024-015`), durations (`P1DT2H`), intervals, or time-only values. These can be added later without breaking changes.

### Decision 3: Raw text trailing blank lines are trimmed
When a raw text block ends with blank lines before the next non-indented line, those trailing blank lines are removed. This prevents spurious trailing newlines in parsed values. Leading and interior blank lines are preserved.

### Decision 4: Collection-of-objects serializer reuses parent key name
When serializing `{ games: [{...}, {...}] }`, the output uses the parent key as the repeated child key: `games\n\tgames\n\t\t...`. This ensures correct round-trip parsing. The original child key name (e.g., `game` vs `games`) is not preserved since JS objects don't carry that metadata.

### Decision 5: Duplicate bare key detection is two-pronged
Both lookahead (for nested patterns) and runtime key-exists check (for same-level patterns) are used. This handles both `parent\n\tchild\n\t\t...\n\tchild\n\t\t...` and `key\n\t...\nkey\n\t...` patterns correctly.
