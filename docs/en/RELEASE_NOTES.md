# Release Notes — 2026-04-01

This release includes small performance improvements, test utilities additions, and repository housekeeping.

## Highlights

- Performance: improved `hex_to_bits` by switching to a fast hex lookup table. This reduces conversion overhead in test utilities. (referenced commit: e89ce0d)
- New files: `RULES.md` (project rules and contribution guidelines), `tests/utils/bits_to_hex.hpp` (utility for hex/bit conversions), `tests/utils/stopwatch.hpp` (simple timing helper for tests).
- Removed: `tests/bin/bin2hex.cpp` — replaced by improved utilities.
- Documentation: synchronized `CHANGELOG.md` and `CHANGELOG.en.md`.

## Details and Rationale

1. `hex_to_bits` lookup table
   - Replaced a per-character algorithm with a static lookup table for hexadecimal-to-bits conversion. This reduces branching and string parsing cost, which matters in tight test loops.

2. Testing utilities
   - `bits_to_hex.hpp` and `stopwatch.hpp` provide small, reusable helpers used across unit tests to reduce duplication and improve test clarity.

3. Repository housekeeping
   - Added `RULES.md` to document repository conventions and contribution expectations.
   - Removed `tests/bin/bin2hex.cpp` because its functionality is superseded by the new utilities.

## Impact

- No public API changes — changes are internal to tests and documentation.
- CI and local test runs may be slightly faster due to the conversion optimization.

## Commits

- e89ce0d — performance change in `hex_to_bits` (tests/utils)
- f3ae91b — workspace changes (added/removed files)
- 273ae4b — docs: add changelog entry for recent workspace changes

If you want, I can open a pull request with these notes or tag a release on GitHub.
