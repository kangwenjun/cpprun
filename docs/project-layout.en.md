# Repository Layout

This document separates the repository into clear responsibilities so that public readers can quickly understand what is script logic, what is build logic, what counts as sample input, and what the CI job is validating.

## Top-Level Responsibilities

- [cpprun.py](../cpprun.py): the CLI entry point. It parses arguments, prepares the build directory, invokes CMake configure/build, and runs CTest.
- [CMakeLists.txt](../CMakeLists.txt): the generic build layer. It is not tied to one fixed sample; instead, it builds `project_bin` from `SOURCES` or `DIR_LIST`.
- [tests/](../tests): sample inputs and smoke-test data. These files are both documentation material and the minimal test corpus used by CI.
- [.github/workflows/ci.yml](../.github/workflows/ci.yml): the automation entry point that keeps the documented workflows working.
- [README.md](../README.md) / [README.en.md](../README.en.md): the repository home-page documentation.

## The Role of tests

The repository intentionally keeps examples and smoke-test inputs in the same tree:

- [tests/main.cpp](../tests/main.cpp): the smallest single-file example for the `-s file.cpp` flow.
- [tests/calc/main.cpp](../tests/calc/main.cpp): the entry point for the multi-source example.
- [tests/calc/test_add.cpp](../tests/calc/test_add.cpp): a small add-function demonstration.
- [tests/calc/test_sub.cpp](../tests/calc/test_sub.cpp): a small subtraction demonstration.
- [tests/timestamp/current_time.hpp](../tests/timestamp/current_time.hpp): the header-mode example, able to provide an entry point when `__MAIN__` is defined.
- [tests/timestamp/main.cpp](../tests/timestamp/main.cpp): the regular non-header-injection version of the same sample.

## cpprun Workflow

The script follows four main steps:

1. Parse `-s/--sources` as either a file list or a directory list.
2. If directories are provided, pass them to CMake through `DIR_LIST` and let CMake collect source files recursively.
3. If all provided inputs are headers, generate a temporary `main.cpp` in the build directory, define `__MAIN__`, and include those headers.
4. Run CMake configure, build, and finally `ctest -V`.

That is why the README is organized around three major usage modes: single-file, directory, and header mode.

## Why CI Passes an Explicit Build Directory

When `-b` is omitted, [cpprun.py](../cpprun.py) creates a timestamp-based build directory automatically. That is convenient for local experiments, but CI benefits from an explicit build path because:

- logs are easier to inspect
- Linux runners avoid awkward default locations

For that reason, [.github/workflows/ci.yml](../.github/workflows/ci.yml) gives every sample its own `build/<case>` directory.

## Recommendations for Future Samples

If you keep extending the repository, this split works well:

- single-file demos: place them directly under [tests/](../tests) or in a dedicated subdirectory
- multi-file demos: give each example its own subdirectory and a clear entry-point filename
- header-based demos: document the `__MAIN__` convention explicitly so readers know the header is executable by design
- documentation: keep the README focused on quick start, and move structural conventions into docs

This keeps the repository landing page short while still preserving the deeper layout notes in a dedicated place.