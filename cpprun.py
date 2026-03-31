#!/usr/bin/env python3
"""
cpprun - 配置、构建并运行指定 CMake 项目的 CTest

本脚本是一个小型辅助工具。
它负责对指定的包含 CMakeLists.txt 的目录执行 cmake 配置、构建并运行 ctest。

使用示例：
 	# 提供源文件列表
	python cpprun.py -s "tests/main.cpp"  # 向 cmake 传入 SOURCES 列表
	python cpprun.py -s "tests/calc/main.cpp;tests/calc/test_add.cpp;tests/calc/test_sub.cpp"
	python cpprun.py -s "tests/main.cpp" --repeat 3  # 重复运行 ctest 3 次

 	# 仅提供头文件
 	# 但头文件必须包含main函数的实现，否则构建或测试会失败。
	python cpprun.py -s "tests/timestamp/current_time.hpp"

 	# 提供目录路径
	python cpprun.py -s "tests/calc"
	python cpprun.py -s "tests/timestamp"

Summary:
 - Purpose: Configure, build and run CTest for a specified CMake project directory.
 - This helper runs cmake configure, build and ctest for a directory containing
   a CMakeLists.txt.

Examples:
	python cpprun.py -s "tests/main.cpp"                # pass SOURCES to cmake
	python cpprun.py -s "tests/main.cpp" --repeat 3     # run ctest three times
	python cpprun.py -s "tests/calc"                   # pass a directory path
	python cpprun.py -s "tests/timestamp/current_time.hpp"  # single header (must contain main)
"""
import argparse
import os
import shutil
import subprocess
import sys
from datetime import datetime
from typing import List, Optional


def run(cmd: List[str], cwd: Optional[str] = None) -> None:
	"""打印并执行外部命令（同步）。

	Args:
		cmd: 要执行的命令及参数列表。
		cwd: 可选的工作目录。

	在外部命令返回非零退出码或发生错误时，脚本将以相应退出码终止。
	
	Run an external command synchronously and exit on failure.
	"""
	try:
		print("运行命令:", " ".join(cmd))
		res = subprocess.run(cmd, cwd=cwd)
		if res.returncode != 0:
			sys.exit(res.returncode)
	except FileNotFoundError:
		print(f"错误: 未找到命令: {cmd[0]}。请确认已安装并添加到 PATH。")
		sys.exit(127)
	except KeyboardInterrupt:
		print("用户中断")
		sys.exit(130)
	except Exception as e:
		print(f"错误: 运行命令 {cmd[0]} 失败: {e}")
		sys.exit(1)


def check_cmake() -> bool:
	"""检查是否安装了 CMake 并打印版本信息。

	Returns:
		True 表示 CMake 可用，False 表示不可用。
	"""
	"""
	Check whether CMake is installed and print its version.
	"""
	try:
		res = subprocess.run(["cmake", "--version"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
		print(res.stdout)
	except FileNotFoundError:
		print("错误: 未找到 CMake。请安装 CMake 并将其加入 PATH。")
		print("可从 https://cmake.org/download/ 下载 CMake")
		return False
	return True

def configure_cmake(cmake_dir: str,
					build_dir: str,
					sources: str,
					dir_list: List[str] = [],
					generator: Optional[str] = None,
					config: Optional[str] = None,
					test_timeout: int = 0) -> None:
	"""为指定的 cmake 源目录和构建目录生成 cmake 配置命令并执行。

	参数为具体值，而非 argparse Namespace。
	"""
	"""
	Generate and run a cmake configure command for the given source and build directories.

	Parameters are concrete values, not an argparse Namespace.
	"""

	# 构造 cmake 配置命令：指定源码目录和构建目录
	# Construct cmake configure command: specify source and build directories
	cmake_cmd = ["cmake", "-S", cmake_dir, "-B", build_dir]

	# 可选的生成器（例如 "Ninja" 或 Visual Studio 名称）
	# Optional generator (e.g., "Ninja" or Visual Studio name)
	if generator:
		cmake_cmd += ["-G", generator]

	# 可选的源码列表，按 CMake 项目约定传递为 -DSOURCES="a.c;b.cc;c.cpp"
	# Optional sources list, passed to CMake as -DSOURCES="a.c;b.cc;c.cpp"
	if sources:
		# Normalize Windows backslashes to forward slashes to avoid CMake escape issues
		cmake_cmd += ["-DSOURCES=" + sources.replace('\\', '/')]

	# 可选的单配置构建类型（对多配置生成器无影响）
	# Optional single-configuration build type (no effect on multi-config generators)
	if config:
		cmake_cmd += ["-DCMAKE_BUILD_TYPE=" + config]

	# 可选的目录列表，按 CMake 项目约定传递为 -DDIR_LIST="dir1;dir2;dir3"
	# Optional directory list, passed to CMake as -DDIR_LIST="dir1;dir2;dir3"
	if dir_list:
		# Normalize directory separators for CMake (use forward slashes)
		cmake_cmd += ["-DDIR_LIST=" + ";".join(d.replace('\\', '/') for d in dir_list)]

	# 可选的测试超时时间，单位为秒；0 表示不设置超时。
	# Optional test timeout in seconds; 0 means do not set a timeout.
	if test_timeout > 0:
		cmake_cmd += [f"-DTEST_TIMEOUT={test_timeout}"]

	# 运行配置命令，失败则退出
	# Run the configure command and exit on failure
	run(cmake_cmd)

def build_cmake(build_dir: str, config: Optional[str] = None, jobs: Optional[int] = None) -> None:
	"""调用 `cmake --build` 构建指定的 `build_dir`。

	Args:
		build_dir: 构建目录路径。
		config: 可选的构建配置（如 Release/Debug）。
		jobs: 并行构建作业数。
	"""
	# 使用 cmake 的跨平台构建接口
	# Use CMake's cross-platform build interface
	build_cmd = ["cmake", "--build", build_dir]

	# 对于多配置生成器（Visual Studio）需要通过 --config 指定配置
	# For multi-config generators (Visual Studio) specify configuration via --config
	if config:
		build_cmd += ["--config", config]

	# 并行构建作业数（传给 cmake --parallel）
	# Number of parallel build jobs (passed to cmake --parallel)
	if jobs:
		build_cmd += ["--parallel", str(jobs)]

	# 运行构建命令，失败则退出
	# Run the build command and exit on failure
	run(build_cmd)


def run_ctest(build_dir: str, config: Optional[str] = None, repeat: int = 1) -> None:
	"""运行 `ctest -V` 并显示详细输出。

	Args:
		build_dir: 运行测试的构建目录。
		config: 可选的多配置生成器配置名。
		repeat: 运行 ctest 的次数，至少为 1。
	
	Run `ctest -V` and show verbose test output.

	Args:
		build_dir: build directory to run tests in.
		config: optional multi-config generator configuration name.
		repeat: number of times to run ctest, must be at least 1.
	"""
	ctest_cmd = ["ctest", "-V"]
	if config:
		ctest_cmd += ["-C", config]

	if repeat > 1:
		for index in range(repeat):
			print(f"{index}/{repeat}")
			print("===")
			run(ctest_cmd, cwd=build_dir)
			print("\n---\n")
	else:
		run(ctest_cmd, cwd=build_dir)

def parse_sources(sources_str: str, sep: str = ';') -> List[str]:
	"""将以分号分隔的 sources 字符串解析为字符串列表，去除空项与多余空白。

	例如: "a.cpp;b.cpp;; c.cpp" -> ["a.cpp", "b.cpp", "c.cpp"]

	Parse a semicolon-separated SOURCES string into a list of paths,
	trimming empty entries and whitespace.
	"""
	if not sources_str:
		return []
	parts = [os.path.abspath(os.path.expanduser(s.strip())) for s in sources_str.split(sep)]
	return [p for p in parts if p]

def is_source_file(filename: str) -> bool:
	"""判断给定的 filename 是否具有常见的 C/C++ 源文件扩展名。
	
	Return True if filename has a common C/C++ source file extension.
	"""
	source_exts = {'.c', '.cc', '.cpp', '.cxx', '.c++'}
	_, ext = os.path.splitext(filename)
	return ext.lower() in source_exts

def is_header_file(filename: str) -> bool:
	"""判断给定的 filename 是否具有常见的 C/C++ 头文件扩展名。

	Return True if filename has a common C/C++ header file extension.
	"""
	header_exts = {'.h', '.hh', '.hpp', '.hxx', '.hhpp'}
	_, ext = os.path.splitext(filename)
	return ext.lower() in header_exts

def is_all_headers(sources_list: List[str]) -> bool:
	"""检查列表是否只包含头文件（按扩展名判断 .h/.hpp/.hh/.hxx 等）。

	Return True iff the list is non-empty and all entries are header files.
	"""
	if not sources_list:
		return False
	for s in sources_list:
		if not is_header_file(s):
			return False
	return True

def get_dir_list(sources_list: List[str]) -> List[str]:
	"""返回列表中所有目录路径的子列表。

	Return the sublist of entries that are directory paths.
	"""
	dir_list = []	
	for s in sources_list:
		if os.path.isdir(s):
			dir_list.append(s)
	return dir_list


def update_sources_list(sources_list: List[str], build_dir: str) -> List[str]:
	"""对 sources_list 做最终更新：

	- 如果只包含头文件，自动在 build_dir 生成一个 main.cpp 并加入列表。
	- 检查每个非头文件是否存在，若不存在打印警告。
	返回可能被修改的列表副本。

	Finalize the sources_list:

	- If only headers are provided, auto-generate a main.cpp under build_dir and add it.
	- Warn if any non-header files do not exist.
	Return a possibly modified copy of the list.
	"""
	if not sources_list:
		return []
	
	updated = list(sources_list)
	
	for s in sources_list:
		if not os.path.exists(s):
			print(f"警告: {s} 未找到。请确认路径正确。")

	# 若仅包含头文件，则生成一个简单的 main.cpp 放入 build_dir
	# If only headers are present, generate a simple main.cpp into build_dir
	if is_all_headers(sources_list):
		main_cpp_path = os.path.join(build_dir, "main.cpp")
		with open(main_cpp_path, "w") as f:
			f.write("#define __MAIN__\n")
			for header in sources_list:
				f.write("// 此处注释是必须的 / This comment is necessary\n")
				f.write(f"#include \"{header}\"\n")
		print(f"警告: 仅提供头文件。已生成 {main_cpp_path} 以允许 CMake 配置。")
		updated.append(main_cpp_path)		

	return updated


def compute_build_dir(build_dir_arg: str, clean: bool = False) -> str:
	"""根据用户传入的 build_dir_arg 计算最终的构建目录路径并返回。

	如果 build_dir_arg 非空则返回其绝对路径；否则以脚本所在目录为 cmake 源目录，
	在该源所在磁盘根下创建 build/<timestamp> 路径。
	
	Compute the absolute build directory path. If not provided, create a
	timestamped directory under the drive root: <drive>:\\build\\<timestamp>.
	"""
	build_dir = None
	if build_dir_arg:
		build_dir = os.path.abspath(build_dir_arg)
	else:
		current_dir = os.path.abspath(os.path.dirname(__file__))
		root = os.path.splitdrive(current_dir)[0] + os.sep
		timestamp = datetime.now().strftime("%Y%m%dT%H%M%S")
		build_dir = os.path.join(root, "build", timestamp)
	if clean and os.path.isdir(build_dir):
		print("正在移除已存在的构建目录:", build_dir)
		shutil.rmtree(build_dir)
	os.makedirs(build_dir, exist_ok=True)
	return build_dir

# 解析命令行参数
# Parse command line arguments
def parse_args() -> argparse.Namespace:
	"""构建并解析命令行参数，返回 `argparse.Namespace`。

	Build and parse command line arguments and return an `argparse.Namespace`.
	"""
	try:
		parser = argparse.ArgumentParser(description="为指定的 CMake 目录配置、构建并运行 CTest / Configure, build and run CTest for a specified CMake directory")
		# description: Configure, build and run CTest for a specified CMake directory
		parser.add_argument("--cmake-dir", "-m", default="", help="包含 CMakeLists.txt 的目录（默认：脚本所在目录） / Directory containing CMakeLists.txt (default: script directory)")
		parser.add_argument("--build-dir", "-b", default="", help="构建输出目录（绝对或相对路径） / Build output directory (absolute or relative path)")
		parser.add_argument("--config", "-c", default="Release", help="构建配置（多配置生成器使用，例如 Release/Debug） / Build configuration (used for multi-config generators, e.g. Release/Debug)")
		parser.add_argument("--jobs", "-j", type=int, default=None, help="并行构建作业数（传递给 --parallel） / Number of parallel build jobs (passed to --parallel)")
		parser.add_argument("--sources", "-s", default="", help='以分号分隔的源文件列表，传递给 CMake（例如: "main.cpp;lib.cpp"） / Semicolon-separated list of source files to pass to CMake (e.g. "main.cpp;lib.cpp")')
		parser.add_argument("--repeat", "-r", type=int, default=1, help="运行 ctest 的次数，默认为 1 / Number of times to run ctest, default: 1")
		parser.add_argument("--timeout", "-t", type=int, default=0, help="测试超时时间（秒）；0 表示不设置超时 / Test timeout in seconds; 0 means no timeout")
		parser.add_argument("--generator", "-g", default=None, help='CMake 生成器名称 / CMake generator name (e.g. Ninja, "Visual Studio 18 2026")')
		parser.add_argument("--clean", action="store_true", help="在配置前删除构建目录 / Remove build directory before configure")
		parser.add_argument("--no-configure", action="store_true", help="跳过 cmake 配置步骤 / Skip cmake configure step")
		parser.add_argument("--no-build", action="store_true", help="跳过构建步骤（仅运行 ctest） / Skip build step (only run ctest)")
		parser.add_argument("--no-test", action="store_true", help="跳过运行 ctest 测试 / Skip running ctest tests")
		args = parser.parse_args()
		if args.repeat < 1:
			args.repeat = 1
			print("警告: --repeat 的值必须至少为 1。已重置为 1 / Warning: --repeat must be at least 1. Reset to 1.")
		if args.timeout < 0:
			args.timeout = 0
			print("警告: --timeout 不能小于 0。已重置为 0 / Warning: --timeout cannot be negative. Reset to 0.")
		return args
	except SystemExit as e:
		# argparse 调用 parser.exit() 会抛出 SystemExit
		# 保持原始行为但给出更友好的提示
		print("Argument parsing exited.")
		raise
	except KeyboardInterrupt:
		print("Argument parsing interrupted by user (Ctrl+C)")
		sys.exit(130)


def main():
	"""主入口：检查 CMake、解析参数、配置、构建并运行测试。

	Main entry point: check for CMake, parse arguments, configure, build,
	and run tests via CTest.

	功能流程：
	- 检查是否安装并可用 CMake（打印版本信息）
	- 解析命令行参数
	- 计算/创建构建目录（可选择清理旧目录）
	- 根据需要执行 cmake 配置、构建和运行 ctest

	Workflow:
	- verify CMake is available (print version)
	- parse CLI arguments
	- compute/create build directory (optionally clean)
	- run cmake configure, build, and ctest as requested
	"""

	# 检查 CMake 是否可用并打印版本信息。
	# Check whether CMake is available and print its version.
	if not check_cmake():
		sys.exit(1)

	here = os.path.abspath(os.path.dirname(__file__))

	try:
		args = parse_args()

		# 将原始的 sources 字符串解析为列表，方便后续处理。
		# Parse the raw SOURCES string into a list for subsequent processing.
		sources_list = parse_sources(args.sources)
		
		# 先计算构建目录（更新 sources 时可能需要写入 build_dir）。
		# Compute the build directory first because updating sources may write into it.
		build_dir = compute_build_dir(args.build_dir, clean=args.clean)
		dir_list = get_dir_list(sources_list)
		if not dir_list:
			sources_list = update_sources_list(sources_list, build_dir)

		if not sources_list:
			print("错误: 未提供源文件列表。如果 CMakeLists.txt 未定义 SOURCES，配置可能会失败。")
			sys.exit(1)

		cmake_dir = os.path.abspath(args.cmake_dir) if args.cmake_dir else os.path.abspath(here)

		# 配置阶段：调用 cmake 生成构建文件（已拆分为函数）。
		# Configure stage: invoke cmake to generate build files.
		if not args.no_configure:
			configure_cmake(cmake_dir, build_dir, ";".join(sources_list), dir_list, args.generator, args.config, args.timeout)

		# 构建阶段：调用 cmake --build（若未禁用）。
		# Build stage: invoke cmake --build unless it is disabled.
		if not args.no_build:
			build_cmake(build_dir, args.config, args.jobs)

		# 运行测试（可被 --no-test 跳过）。
		# Test stage: run ctest unless --no-test is specified.
		if not args.no_test:
			run_ctest(build_dir, args.config, args.repeat)

	except Exception as e:
		print(f"Unhandled error: {e}")
		sys.exit(1)

if __name__ == "__main__":
	main()