# Milestone 1 Verification Report

**Author:** Dale (Tester/QA)
**Date:** 2025-07-22
**Status:** ⚠️ DISCREPANCIES FOUND

---

## Step 1: Test Suite Results

| Implementation | Total Tests | Passed | Failed | Status |
|---|---|---|---|---|
| **Python** (pytest) | 95 | 95 | 0 | ✅ ALL PASS |
| **JavaScript** (node) | 54 | 54 | 0 | ✅ ALL PASS |
| **.NET** (xUnit) | 235 | 235 | 0 | ✅ ALL PASS |

**Total: 384 tests, 0 failures across all implementations.**

### Test Breakdown by Feature

| Feature | Python | JavaScript | .NET |
|---|---|---|---|
| Extended Booleans | 26 tests | 5 tests | ~14 tests (Theory+Fact) |
| Raw Text Blocks | 14 tests | 11 tests | ~12 tests |
| ISO 8601 Dates | 16 tests | 10 tests | ~13 tests |
| Duplicate Bare Keys | 8 tests | 5 tests | ~6 tests |
| Pre-existing tests | 31 | 23 | ~190 |

---

## Step 2: Cross-Implementation Consistency Matrix

### Feature 1: Extended Booleans ✅ CONSISTENT

| Behavior | Python | JavaScript | .NET | Spec |
|---|---|---|---|---|
| true/True/TRUE → true | ✅ | ✅ | ✅ | ✅ |
| yes/Yes/YES → true | ✅ | ✅ | ✅ | ✅ |
| on/On/ON → true | ✅ | ✅ | ✅ | ✅ |
| false/False/FALSE → false | ✅ | ✅ | ✅ | ✅ |
| no/No/NO → false | ✅ | ✅ | ✅ | ✅ |
| off/Off/OFF → false | ✅ | ✅ | ✅ | ✅ |
| Case-insensitive | ✅ | ✅ | ✅ | ✅ |
| 1/0 → integer (NOT bool) | ✅ | ✅ | ✅ | ✅ |
| Serialize as lowercase true/false | ✅ | ✅ | ✅ | ✅ |
| typeConversion=false preserves string | ✅ | ✅ | N/A | ✅ |

**Verdict: All three implementations are fully consistent.**

### Feature 2: Raw Text Blocks ✅ CONSISTENT

| Behavior | Python | JavaScript | .NET | Spec |
|---|---|---|---|---|
| `...` syntax recognized | ✅ | ✅ | ✅ | ✅ |
| Preserves tabs in content | ✅ | ✅ | ✅ | ✅ |
| Preserves newlines | ✅ | ✅ | ✅ | ✅ |
| Preserves blank lines | ✅ | ✅ | ✅ | ✅ |
| Extra indentation → content tabs | ✅ | ✅ | ✅ | ✅ |
| Ends at same/lesser indentation | ✅ | ✅ | ✅ | ✅ |
| Empty raw text → empty string | ✅ | ✅ | ✅ | ✅ |
| Nested raw text in structures | ✅ | ✅ | ✅ | ✅ |
| Serializer emits `...` for multiline | ✅ | ✅ | ✅ | ✅ |
| Round-trip preserves content | ✅ | ✅ | ✅ | ✅ |

**Verdict: All three implementations are fully consistent.**

### Feature 3: ISO 8601 Dates ⚠️ MINOR DISCREPANCIES

| Behavior | Python | JavaScript | .NET | Spec |
|---|---|---|---|---|
| YYYY-MM-DD → date type | ✅ `date` | ✅ `Date` | ✅ `DateTime` | ✅ |
| YYYY-MM-DDTHH:MM:SS → datetime | ✅ `datetime` | ✅ `Date` | ✅ `DateTime` | ✅ |
| Z timezone → UTC | ✅ | ✅ | ✅ | ✅ |
| +HH:MM offset | ✅ | ✅ | ✅ | ✅ |
| -HH:MM offset | ✅ | ✅ | ✅ | ✅ |
| Fractional seconds | ✅ | ✅ | ✅ | ✅ |
| Bare year (2024) → integer | ✅ | ✅ | ✅ | ✅ |
| **YYYY-MM (2024-01) → date** | ❌ string | ❌ string | ✅ DateTime | ✅ Required |
| Serialize as ISO 8601 | ✅ | ✅ | ✅ | ✅ |

**Discrepancy D1: YYYY-MM date pattern**
- **Spec says:** `YYYY-MM` is the minimum pattern for date detection (line 373, 450)
- **.NET:** Correctly detects `2024-01` as DateTime ✅
- **Python:** Treats `2024-01` as a string ❌
- **JavaScript:** Treats `2024-01` as a string ❌
- **Impact:** Medium — Python and JS miss a spec-required date format
- **Fix needed:** Python and JavaScript parsers need YYYY-MM detection

### Feature 4: Duplicate Bare Keys → Collections ❌ MAJOR DISCREPANCY

| Behavior | Python | JavaScript | .NET | Spec |
|---|---|---|---|---|
| Top-level duplicate keys → list | ✅ | ✅ | ✅ | ✅ |
| Three duplicate keys → list of 3 | ✅ | ✅ | ✅ | ✅ |
| Mixed duplicates + unique keys | ✅ | ✅ | ✅ | ✅ |
| Serializer emits duplicate keys | ✅ | ✅ | ✅ | ✅ |
| Round-trip for duplicates | ✅ | ✅ | ✅ | ✅ |
| **Nested duplicate keys structure** | ❌ | ✅ | ❌ (untested) | See below |

**Discrepancy D2: Nested duplicate bare key structure (CRITICAL)**

For the TAML input:
```taml
games
	game
		home	Philadelphia
		away	Dallas
	game
		home	New York
		away	Boston
```

| Parser | Parsed Structure | Correct? |
|---|---|---|
| **Python** | `{games: {game: [{...}, {...}]}}` — games is a dict, game is the list | ❌ |
| **JavaScript** | `{games: [{...}, {...}]}` — games IS the list (game key dropped) | ✅ |
| **.NET** | Likely `{games: {game: [{...}, {...}]}}` (based on code analysis, not tested for nested case) | ❌ Probably |

**Spec Reference (Structure Detection Rule 4):**
> "Collection of Objects: If children contain duplicate bare keys with children"
> "When a parser encounters duplicate bare keys at the same level, it should convert the **parent** to a Collection of Objects"

The spec says the **parent** (`games`) should become the Collection. JavaScript correctly converts `games` to a list. Python keeps `games` as a dict with `game` as an intermediate key holding the list.

**Impact:** HIGH — This is a fundamental data structure difference for the same input. Applications switching between parsers would get incompatible results.

**Recommendation:** Team needs to decide:
1. Follow the spec literally → JavaScript is correct → fix Python and .NET
2. Keep the intermediate key (better for serialization, preserves key name) → fix JavaScript and update spec

---

## Step 3: Summary of Findings

### ✅ What's Working Well
- All 384 tests pass across all implementations
- Extended booleans: Fully consistent (same set, case-insensitive, 1/0 as integers)
- Raw text blocks: Fully consistent (tabs, newlines, blank lines all preserved)
- Top-level duplicate keys: Consistent
- Serialization: All use canonical lowercase booleans, ISO 8601 dates, `...` for raw text

### ⚠️ Discrepancies Found

| ID | Severity | Feature | Description |
|---|---|---|---|
| D1 | Medium | ISO 8601 Dates | Python & JS don't detect YYYY-MM format (spec requires it) |
| D2 | **Critical** | Duplicate Bare Keys | Nested duplicate keys produce different structures: Python keeps intermediate key, JS drops it |

### 📋 Recommendations

1. **D2 (Critical):** Hold a team decision on nested duplicate key behavior. The spec says "convert the parent to a Collection" which favors JS. But the Python/NET approach preserves key names for round-trips. Need Fritz to adjudicate.

2. **D1 (Medium):** Add YYYY-MM date detection to Python and JavaScript parsers. The spec explicitly lists it as the minimum pattern for date detection. .NET already handles it correctly.

3. **Test gap:** .NET needs a test for nested duplicate bare keys (the `games/game` pattern) to confirm its behavior matches whatever decision is made.
