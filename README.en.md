# C++ Sample Project

[中文](README.md)

This is a simple C++ project that includes sample programs and unit tests, built with CMake.

Project structure (brief):

- `tests/`: sample programs and test cases
  - `main.cpp`: sample executable
  - `calc/`: calculator sample and its tests
    - `main.cpp`
    - `test_add.cpp`
    - `test_sub.cpp`
  - `timestamp/`: header file and its test
    - `main.cpp`
    - `current_time.hpp`

## Build (Windows / PowerShell)

```powershell
# Run in the project root directory
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

After the build is complete, the executables will be located in `build` or the corresponding output directory, depending on the generator.

## Run the Samples

Run the generated executables from the `build` directory, or run the executables built from the sample source files directly.

## Run the Tests

The project includes a Python script named `cpprun` for running tests. Example usage:

```powershell
# Run in the project root directory:

# Provide a list of source files
python cpprun.py -s "tests\main.cpp"  # Pass the SOURCES list to CMake
python cpprun.py -s "tests\calc\main.cpp;tests\calc\test_add.cpp;tests\calc\test_sub.cpp"
python cpprun.py -s "tests\main.cpp" --repeat 3  # Run ctest three times

# Provide only a header file
# The header file must include an implementation of the main function,
# otherwise the build or test will fail.
python cpprun.py -s "tests\timestamp\current_time.hpp"

# Provide a directory path
python cpprun.py -s "tests\calc"
python cpprun.py -s "tests\timestamp"
```

`--repeat` (or `-r`) specifies how many times to run ctest. The default is 1.

The `-s` option specifies the test directory or test case path to run.
Use `;` to separate multiple files or directories.