#!/usr/bin/env python3
"""
cpprun - configure, build and run CTest for a CMake project

This is a small helper script to quickly configure, build and run ctest
for a project directory that contains a CMakeLists.txt.

Usage examples:
    python cpprun.py -s "tests/main.cpp"
    python cpprun.py -s "tests/calc"
    python cpprun.py -s "tests/timestamp/current_time.hpp"

Summary of features:
 - Create a CMake build tree for the specified sources
 - Invoke `cmake --build` to build targets
 - Optionally perform `cmake --install`
 - Run `ctest -V` to show test output

This script focuses on ease-of-use and cross-platform compatibility
(Windows / Unix).
"""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys
from datetime import datetime
from typing import List, Optional, Tuple


def run(cmd: List[str], cwd: Optional[str] = None) -> None:
    """Print and run an external command (synchronously).

    Args:
      cmd: command and argument list.
      cwd: optional working directory.

    If the external command returns a non-zero exit code the script
    exits with that code.
    """
    try:
        print("cpprun: Running command:", " ".join(cmd))
        res = subprocess.run(cmd, cwd=cwd)
        if res.returncode != 0:
            sys.exit(res.returncode)
    except FileNotFoundError:
        print(f"cpprun: Error: Command not found: {cmd[0]}. Please ensure it is installed and on PATH.")
        sys.exit(127)
    except KeyboardInterrupt:
        print("cpprun: Interrupted by user")
        sys.exit(130)
    except Exception as e:
        print(f"cpprun: Error: Failed to run command {cmd[0]}: {e}")
        sys.exit(1)


def check_cmake() -> bool:
    """Check whether CMake is available and print version information.

    Returns:
      True if CMake is available; False otherwise.
    """
    try:
        res = subprocess.run(["cmake", "--version"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        print(res.stdout)
    except FileNotFoundError:
        print("cpprun: Error: CMake not found. Please install CMake and add it to PATH. See https://cmake.org/download/.")
        return False
    return True


def resolve_user_path(path: str) -> str:
    """Expand `~` and return a non-strict absolute path string."""
    return str(Path(path).expanduser().resolve(strict=False))


def normalize_cmake_path(path: str) -> str:
    """Convert a path to POSIX style (suitable for passing to CMake)."""
    return Path(path).as_posix()


def join_cmake_path_list(paths: List[str]) -> str:
    """Join a list of paths into the semicolon-separated string CMake expects."""
    return ";".join(normalize_cmake_path(path) for path in paths)


def compute_generated_include_path(header_path: str, build_dir: str) -> str:
    """Compute an include path relative to `build_dir`, or return a normalized absolute path.

    If the path can be relativized against the build directory a POSIX relative
    path is returned. Otherwise an absolute normalized path is returned.
    """
    try:
        relative_path = os.path.relpath(header_path, build_dir)
        return Path(relative_path).as_posix()
    except ValueError:
        return normalize_cmake_path(header_path)


def configure_cmake(
    cmake_dir: str,
    build_dir: str,
    sources: List[str],
    dir_list: Optional[List[str]] = None,
    generator: Optional[str] = None,
    config: Optional[str] = None,
    test_timeout: int = 0,
    target_name: Optional[str] = None,
    install_dir: Optional[str] = None,
) -> None:
    """Generate and run the CMake configure command for the given source directory.

    Optional variables are passed to CMake via -D flags: SOURCES, DIR_LIST,
    TEST_TIMEOUT, TARGET_NAME, CMAKE_INSTALL_PREFIX, etc.
    """
    cmake_cmd = ["cmake", "-S", cmake_dir, "-B", build_dir]

    if generator:
        cmake_cmd += ["-G", generator]

    if sources:
        cmake_cmd += ["-DSOURCES=" + join_cmake_path_list(sources)]

    if config:
        cmake_cmd += ["-DCMAKE_BUILD_TYPE=" + config]

    if dir_list:
        cmake_cmd += ["-DDIR_LIST=" + join_cmake_path_list(dir_list)]

    if test_timeout > 0:
        cmake_cmd += [f"-DTEST_TIMEOUT={test_timeout}"]

    if target_name:
        cmake_cmd += ["-DTARGET_NAME=" + target_name]

    if install_dir:
        cmake_cmd += ["-DINSTALL_DIR=" + normalize_cmake_path(install_dir)]

    run(cmake_cmd)


def build_cmake(build_dir: str, config: Optional[str] = None, jobs: Optional[int] = None) -> None:
    """Invoke `cmake --build` to build the specified directory.

    Args:
        build_dir: path to the build directory.
        config: optional build configuration (e.g. Release/Debug for multi-config generators).
        jobs: number of parallel build jobs.
    """
    build_cmd = ["cmake", "--build", build_dir]

    if config:
        build_cmd += ["--config", config]

    if jobs:
        build_cmd += ["--parallel", str(jobs)]

    run(build_cmd)


def run_ctest(build_dir: str, config: Optional[str] = None, repeat: int = 1) -> None:
    """Run `ctest -V`. The run can be repeated multiple times to compare outputs."""
    ctest_cmd = ["ctest", "-V"]
    if config:
        ctest_cmd += ["-C", config]

    if repeat > 1:
        for index in range(repeat):
            print(f"cpprun: ctest run {index+1}/{repeat}")
            print("===")
            run(ctest_cmd, cwd=build_dir)
            print("\n---\n")
    else:
        run(ctest_cmd, cwd=build_dir)


def install_cmake(build_dir: str, config: Optional[str] = None, install_dir: Optional[str] = None) -> None:
    """Run `cmake --install` to install build artifacts."""
    install_cmd = ["cmake", "--install", build_dir]
    if install_dir:
        install_cmd += ["--prefix", normalize_cmake_path(install_dir)]
    if config:
        install_cmd += ["--config", config]
    run(install_cmd)


def parse_sources(sources_str: str, sep: str = ';') -> List[str]:
    """Parse a semicolon-separated sources string into a list of absolute paths, filtering empties."""
    if not sources_str:
        return []
    parts = [resolve_user_path(s.strip()) for s in sources_str.split(sep)]
    return [p for p in parts if p]


def is_source_file(filename: str) -> bool:
    """Return True if filename has a common C/C++ source file extension."""
    source_exts = {'.c', '.cc', '.cpp', '.cxx', '.c++'}
    _, ext = os.path.splitext(filename)
    return ext.lower() in source_exts


def is_header_file(filename: str) -> bool:
    """Return True if filename has a common C/C++ header extension."""
    header_exts = {'.h', '.hh', '.hpp', '.hxx'}
    _, ext = os.path.splitext(filename)
    return ext.lower() in header_exts


def is_all_headers(sources_list: List[str]) -> bool:
    """Return True if the list contains only header files."""
    if not sources_list:
        return False
    for s in sources_list:
        if not is_header_file(s):
            return False
    return True


def split_input_paths(paths: List[str]) -> Tuple[List[str], List[str]]:
    """Split a list of input paths into file list and directory list."""
    file_list: List[str] = []
    dir_list: List[str] = []
    for path in paths:
        if os.path.isdir(path):
            dir_list.append(path)
        else:
            file_list.append(path)
    return file_list, dir_list


def update_sources_list(sources_list: List[str], build_dir: str) -> List[str]:
    """Post-process the sources list:

    - If the list contains only headers, generate a simple `main.cpp` in `build_dir`
      and add it to the list so CMake has a compilable source file.
    - Print warnings for missing files.

    Returns a possibly modified copy of the list.
    """
    if not sources_list:
        return []

    updated = list(sources_list)

    for s in sources_list:
        if not os.path.exists(s):
            print(f"cpprun: Warning: {s} not found; please check the path.")

    if is_all_headers(sources_list):
        main_cpp_path = os.path.join(build_dir, "main.cpp")
        with open(main_cpp_path, "w", encoding="utf-8", newline="\n") as f:
            f.write("#define __MAIN__\n")
            for header in sources_list:
                include_path = compute_generated_include_path(header, build_dir)
                f.write(f"#include \"{include_path}\"\n")
        print(f"cpprun: Note: only headers provided; generated {main_cpp_path} to allow CMake configure.")
        updated.append(main_cpp_path)

    return updated


def compute_build_dir(build_dir_arg: str, clean: bool = False) -> str:
    """Compute/create the build directory from the argument.

    If none is specified a timestamped `build/<timestamp>` under the script
    directory is used. If `clean` is True and the directory exists it will
    be removed and recreated. The absolute path to the build directory is returned.
    """
    if build_dir_arg:
        build_dir = os.path.abspath(build_dir_arg)
    else:
        current_dir = os.path.abspath(os.path.dirname(__file__))
        timestamp = datetime.now().strftime("%Y%m%dT%H%M%S")
        build_dir = os.path.join(current_dir, "build", timestamp)

    if clean and os.path.isdir(build_dir):
        print("cpprun: Removing existing build directory:", build_dir)
        shutil.rmtree(build_dir)

    os.makedirs(build_dir, exist_ok=True)
    return build_dir


def parse_args() -> argparse.Namespace:
    """Build and parse command-line arguments and return the Namespace."""
    try:
        desc = "Configure, build and run CTest for a specified CMake directory"
        help_cmake_dir = "Directory that contains CMakeLists.txt (default: script directory)"
        help_build_dir = "Build output directory (absolute or relative path)"
        help_target_name = "Target name (passed as TARGET_NAME to CMake, default: project_bin)"
        help_config = "Build configuration (for multi-config generators, e.g. Release/Debug)"
        help_jobs = "Number of parallel build jobs (passed to --parallel)"
        help_sources = 'Semicolon-separated list of source/header files or directories, e.g. "tests/main.cpp;tests/utils"'
        help_repeat = "Number of times to run ctest (default: 1)"
        help_timeout = "Test timeout in seconds; 0 means no timeout"
        help_generator = 'CMake generator name (e.g. Ninja or "Visual Studio")'
        help_install_dir = "Install directory (passed to CMAKE_INSTALL_PREFIX)"
        help_clean = "Remove build directory before configuring"
        help_no_configure = "Skip the cmake configure step"
        help_no_build = "Skip the build step (only run ctest)"
        help_no_install = "Skip install step even if --install-dir is given"
        help_no_test = "Skip running ctest"

        parser = argparse.ArgumentParser(description=desc)
        parser.add_argument("--cmake-dir", "-m", default="", help=help_cmake_dir)
        parser.add_argument("--build-dir", "-b", default="", help=help_build_dir)
        parser.add_argument("--target-name", "-n", default="project_bin", help=help_target_name)
        parser.add_argument("--config", "-c", default="Release", help=help_config)
        parser.add_argument("--jobs", "-j", type=int, default=None, help=help_jobs)
        parser.add_argument("--sources", "-s", default="", help=help_sources)
        parser.add_argument("--repeat", "-r", type=int, default=1, help=help_repeat)
        parser.add_argument("--timeout", "-t", type=int, default=0, help=help_timeout)
        parser.add_argument("--generator", "-g", default=None, help=help_generator)
        parser.add_argument("--install-dir", "-i", default="", help=help_install_dir)
        parser.add_argument("--clean", action="store_true", help=help_clean)
        parser.add_argument("--no-configure", action="store_true", help=help_no_configure)
        parser.add_argument("--no-build", action="store_true", help=help_no_build)
        parser.add_argument("--no-install", action="store_true", help=help_no_install)
        parser.add_argument("--no-test", action="store_true", help=help_no_test)
        args = parser.parse_args()

        if args.repeat < 1:
            args.repeat = 1
            print("cpprun: Warning: --repeat must be at least 1; reset to 1.")
        if args.timeout < 0:
            args.timeout = 0
            print("cpprun: Warning: --timeout cannot be negative; reset to 0.")
        if args.jobs is not None and args.jobs < 1:
            args.jobs = None
            print("cpprun: Warning: --jobs must be at least 1; reset to auto.")

        return args
    except SystemExit:
        print("cpprun: Argument parsing exited.")
        raise
    except KeyboardInterrupt:
        print("cpprun: Argument parsing interrupted by user (Ctrl+C)")
        sys.exit(130)


def main():
    """Main entry point: check CMake, parse args, configure, build and run tests."""

    if not check_cmake():
        sys.exit(1)

    here = os.path.abspath(os.path.dirname(__file__))

    try:
        args = parse_args()

        input_paths = parse_sources(args.sources)

        build_dir = compute_build_dir(args.build_dir, clean=args.clean)
        sources_list, dir_list = split_input_paths(input_paths)
        if not dir_list:
            sources_list = update_sources_list(sources_list, build_dir)

        if not sources_list and not dir_list:
            print("cpprun: Error: No source files or directories provided. If CMakeLists.txt does not define SOURCES configure may fail.")
            sys.exit(1)

        cmake_dir = os.path.abspath(args.cmake_dir) if args.cmake_dir else os.path.abspath(here)

        if args.target_name and args.target_name.lower() == "test":
            print("cpprun: Warning: target name 'test' may conflict with CTest built-in test; renamed to 'Test'.")
            args.target_name = "Test"

        if not args.no_configure:
            configure_cmake(
                cmake_dir,
                build_dir,
                sources_list,
                dir_list,
                args.generator,
                args.config,
                args.timeout,
                args.target_name,
                args.install_dir,
            )

        if not args.no_build:
            build_cmake(build_dir, args.config, args.jobs)

        if args.install_dir and not args.no_install:
            install_cmake(build_dir, args.config, args.install_dir)

        if not args.no_test:
            run_ctest(build_dir, args.config, args.repeat)

    except Exception as e:
        print(f"cpprun: Unhandled error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
