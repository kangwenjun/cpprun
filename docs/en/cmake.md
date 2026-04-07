# CMake configuration (English summary)

This document summarizes the project's top-level `CMakeLists.txt` and explains how to use the available options.

## Purpose

- Build a single executable target (default name `project_bin`) from a provided list of source files.
- Provide a simple test that runs the produced executable via CTest.
- Support optional collection of source files from directory lists and automatic inclusion of their directories.

## Key configuration variables

- `SOURCES` (recommended): semicolon-separated list of source files (relative to project root). Example: `-DSOURCES="main.cpp;src/lib.cpp"`.
- `DIR_LIST` (optional): list of directories to recurse for source files. When used, the script will collect .cpp/.c/.cc/.cxx files and add their directories to private include paths.
- `TARGET_NAME` (optional): name of the executable target. Defaults to `project_bin` when not defined.
- `TEST_TIMEOUT` (optional): if defined and > 0, sets the timeout (seconds) for the `run_project` test.
- `CMAKE_INSTALL_PREFIX` (standard CMake): installation prefix used by `cmake --install` or `cmake --build --target install`.
- `INSTALL_DIR` (project-specific): this top-level CMakeLists accepts an `INSTALL_DIR` variable when provided via `-DINSTALL_DIR="<path>"` and uses it for `install()` destinations. `cpprun.py` passes `INSTALL_DIR` when `--install-dir` is used.

## C++ Standard

- The project sets `CMAKE_CXX_STANDARD` to 20 and requires it (`C++20`), with extensions disabled.

## What the CMake script does

1. Validates that either `SOURCES` or `DIR_LIST` is provided. If neither is supplied, CMake errors out with a helpful message.
2. If `SOURCES` is provided, its value is assigned to an internal `PROJECT_SOURCES` for `add_executable`.
3. If `DIR_LIST` is provided, each directory is scanned recursively for common C/C++ source file extensions; those files are appended to `PROJECT_SOURCES`.
4. Collected source file directories are de-duplicated and added to the target's private include paths so `#include "..."` relative to the project root and subdirs works.
5. An executable target (`${TARGET_NAME}`) is created from `PROJECT_SOURCES`.
6. `enable_testing()` is called and a basic test `run_project` is registered (it runs the produced executable). If `TEST_TIMEOUT` is set, the test timeout property is configured.
7. `install(TARGETS ...)` installs the executable to `${CMAKE_INSTALL_PREFIX}/bin`.
8. If any extra include directories were collected from `DIR_LIST`, the script installs those directories under `include/<relative-path>`.

## Typical usage

Configure and build with explicit sources:

```sh
cmake -S . -B build -DSOURCES="main.cpp;src/lib.cpp"
cmake --build build --parallel 4
ctest --output-on-failure -C Release
```

Or provide directories to collect sources automatically:

```sh
cmake -S . -B build -DDIR_LIST="tests;src"
cmake --build build
```

To install after building:

```sh
cmake --install build --prefix /your/install/prefix
```

To change target name:

```sh
cmake -S . -B build -DTARGET_NAME=my_app -DSOURCES="main.cpp"
```

## Notes and recommendations

- The current script treats `SOURCES` or `DIR_LIST` as the source-of-truth. If you prefer automatic discovery by default, consider switching to `file(GLOB_RECURSE ...)` or providing a fallback when `SOURCES` is not defined.
- If your project requires public headers for an exported library, adapt the install rules to install only the public headers rather than entire directories.
- If your executable requires runtime arguments or environment setup, update the `add_test` command accordingly or add more tests to cover scenarios.

## Where to look in the project

- Top-level CMake file: `CMakeLists.txt` (project root)

---

Generated from the repository's top-level `CMakeLists.txt` on request.
