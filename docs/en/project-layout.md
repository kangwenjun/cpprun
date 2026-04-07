# Project layout

This document breaks the repository into clearly scoped sections so readers can quickly understand what the script, the build layer, the example inputs are, and what CI validates.

## Top-level responsibilities

- `cpprun.py`: CLI entrypoint. Parses arguments, prepares the build directory, invokes CMake for configure/build, and runs CTest.
- `CMakeLists.txt`: Generic build script. It does not bind to a single example; instead it generates `project_bin` from `SOURCES` or `DIR_LIST`.
- `tests/`: example inputs and smoke-test data. These files serve as demos and the minimal test set used by CI.
- `.github/workflows/ci.yml`: automation entrypoint that ensures the main usage patterns shown in the README remain valid.
- `README.md` / `README.en.md`: quick overview for the repository homepage.

## Role of the `tests` directory

The repository intentionally places examples and test inputs together rather than separating them:

- `tests/main.cpp`: minimal single-file example to validate the simplest `-s file.cpp` scenario.
- `tests/calc/main.cpp`: entry for a multi-source example.
- `tests/calc/test_add.cpp`: addition example for the calc sample.
- `tests/calc/test_sub.cpp`: subtraction example for the calc sample.
- `tests/timestamp/current_time.hpp`: header-mode example that provides an entry point when `__MAIN__` is defined.
- `tests/timestamp/main.cpp`: ordinary entry example when not using header injection.

## cpprun workflow

cpprun's execution can be summarized in four steps:

1. Parse `-s/--sources` and treat the input as either a file list or directory list.
2. If directories are provided, hand them to CMake via `DIR_LIST` so CMake can recursively collect source files.
3. If the provided inputs are only header files, generate a temporary `main.cpp` in the build directory, define `__MAIN__`, and include the target headers.
4. Invoke CMake configure, build, then run `ctest -V`.

These are the reasons README highlights single-file / directory / header modes as the primary usage scenarios.

## Why CI should pass an explicit build directory

When `-b` is omitted, `cpprun.py` creates a timestamped build directory. This is convenient locally but in CI it's better to explicitly specify the build directory because:

- logs are easier to locate
- on Linux runners it avoids placing build outputs in unsuitable locations

Therefore, `.github/workflows/ci.yml` specifies separate `build/<case>` directories for each case.

## Suggestions for adding more examples

When adding examples, follow these layering conventions:

- Single-file demos: place under `tests/` or a dedicated subdirectory.
- Multi-file demos: create a separate subdirectory per example group and keep a clear entry filename.
- Header-mode demos: follow the `__MAIN__` convention and document that the example is an "executable header" rather than a normal library header.
- Docs: keep quick start in `README` and put design conventions and directory responsibilities under `docs/`.

This keeps the repository homepage concise while preserving detailed structure notes in `docs`.
