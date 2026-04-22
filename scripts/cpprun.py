#!/usr/bin/env python3
"""
cpprun - 配置、构建并运行指定 CMake 项目的 CTest

这是一个用于快速对包含 CMakeLists.txt 的工程目录执行配置、构建并运行 ctest 的小型辅助脚本。

使用示例：
	python cpprun.py -s "tests/main.cpp"
	python cpprun.py -s "tests/calc"
	python cpprun.py -s "tests/timestamp/current_time.hpp"

功能摘要：
 - 对指定源码生成 CMake 构建目录
 - 调用 `cmake --build` 构建目标
 - 可选执行 `cmake --install`
 - 调用 `ctest -V` 以显示测试输出

脚本侧重于易用性与跨平台兼容（Windows/Unix）。
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
	"""打印并执行外部命令（同步）。

	参数:
	  cmd: 命令及参数列表。
	  cwd: 可选的工作目录。

	若外部命令返回非零退出码，脚本将以该退出码退出。
	"""
	try:
		print("cpprun: 运行命令:", " ".join(cmd))
		res = subprocess.run(cmd, cwd=cwd)
		if res.returncode != 0:
			sys.exit(res.returncode)
	except FileNotFoundError:
		print(f"cpprun: 错误: 未找到命令: {cmd[0]}，请确认已安装并添加到 PATH。")
		sys.exit(127)
	except KeyboardInterrupt:
		print("cpprun: 用户中断")
		sys.exit(130)
	except Exception as e:
		print(f"cpprun: 错误: 运行命令 {cmd[0]} 失败: {e}")
		sys.exit(1)


def check_cmake() -> bool:
	"""检查 CMake 是否可用，并打印版本信息。

	返回:
	  True: CMake 可用；False: 不可用。
	"""
	try:
		res = subprocess.run(["cmake", "--version"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
		print(res.stdout)
	except FileNotFoundError:
		print("cpprun: 错误: 未找到 CMake。请安装 CMake 并将其加入 PATH。可从 https://cmake.org/download/ 获取。")
		return False
	return True


def resolve_user_path(path: str) -> str:
	"""展开 `~` 并返回非严格解析的绝对路径字符串。"""
	return str(Path(path).expanduser().resolve(strict=False))


def normalize_cmake_path(path: str) -> str:
	"""将路径转换为 POSIX 风格（适合传给 CMake）。"""
	return Path(path).as_posix()


def join_cmake_path_list(paths: List[str]) -> str:
	"""将路径列表拼接为 CMake 所需的分号分隔字符串。"""
	return ";".join(normalize_cmake_path(path) for path in paths)


def compute_generated_include_path(header_path: str, build_dir: str) -> str:
	"""计算相对于 `build_dir` 的 include 路径，如果不可相对化则返回规范化的绝对路径。"""
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
	"""为指定源码目录生成并执行 cmake 配置命令。

	可选通过 -DSOURCES, -DDIR_LIST, -DTEST_TIMEOUT, -DTARGET_NAME, -DINSTALL_DIR） 传递给 CMake。
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
	"""调用 `cmake --build` 构建指定的目录。

	参数:
		build_dir: 构建目录路径。
		config: 可选的构建配置（如 Release/Debug）。
		jobs: 并行构建作业数。
	"""
	build_cmd = ["cmake", "--build", build_dir]

	if config:
		build_cmd += ["--config", config]

	if jobs:
		build_cmd += ["--parallel", str(jobs)]

	run(build_cmd)


def run_ctest(build_dir: str, config: Optional[str] = None, repeat: int = 1) -> None:
	"""运行 `ctest -V`，可重复运行多次以便比较输出。"""
	ctest_cmd = ["ctest", "-V", '--test-dir', build_dir]
	if config:
		ctest_cmd += ["-C", config]

	if repeat > 1:
		for index in range(repeat):
			print(f"cpprun: ctest 运行 {index+1}/{repeat}")
			print("===")
			run(ctest_cmd)
			print("\n---\n")
	else:
		#run(ctest_cmd, cwd=build_dir)
		run(ctest_cmd)


def install_cmake(build_dir: str, config: Optional[str] = None, install_dir: Optional[str] = None) -> None:
	"""执行 `cmake --install` 安装构建产物。"""
	install_cmd = ["cmake", "--install", build_dir]
	if install_dir:
		install_cmd += ["--prefix", normalize_cmake_path(install_dir)]
	if config:
		install_cmd += ["--config", config]
	run(install_cmd)


def parse_sources(sources_str: str, sep: str = ';') -> List[str]:
	"""解析以分号分隔的 sources 字符串为绝对路径列表，过滤空项。"""
	if not sources_str:
		return []
	parts = [resolve_user_path(s.strip()) for s in sources_str.split(sep)]
	return [p for p in parts if p]


def is_source_file(filename: str) -> bool:
	"""判断给定的 filename 是否具有常见的 C/C++ 源文件扩展名。"""
	source_exts = {'.c', '.cc', '.cpp', '.cxx', '.c++'}
	_, ext = os.path.splitext(filename)
	return ext.lower() in source_exts


def is_header_file(filename: str) -> bool:
	"""判断文件名是否为常见 C/C++ 头文件扩展名。"""
	header_exts = {'.h', '.hh', '.hpp', '.hxx'}
	_, ext = os.path.splitext(filename)
	return ext.lower() in header_exts


def is_all_headers(sources_list: List[str]) -> bool:
	"""如果列表仅包含头文件返回 True，否则返回 False。"""
	if not sources_list:
		return False
	for s in sources_list:
		if not is_header_file(s):
			return False
	return True


def split_input_paths(paths: List[str]) -> Tuple[List[str], List[str]]:
	"""将输入路径列表拆分为文件列表和目录列表。"""
	file_list: List[str] = []
	dir_list: List[str] = []
	for path in paths:
		if os.path.isdir(path):
			dir_list.append(path)
		else:
			file_list.append(path)
	return file_list, dir_list


def update_sources_list(sources_list: List[str], build_dir: str) -> List[str]:
	"""对 sources 列表做最终处理：

	- 若所有项均为头文件，则在 `build_dir` 生成一个简单的 `main.cpp` 并加入列表，
	  以便 CMake 有可编译的源文件。
	- 对于缺失的文件会打印警告。
	返回可能修改后的列表副本。
	"""
	if not sources_list:
		return []

	updated = list(sources_list)

	for s in sources_list:
		if not os.path.exists(s):
			print(f"cpprun: 警告: 未找到 {s}，请确认路径是否正确。")

	if is_all_headers(sources_list):
		main_cpp_path = os.path.join(build_dir, "main.cpp")
		with open(main_cpp_path, "w", encoding="utf-8", newline="\n") as f:
			f.write("#define __MAIN__\n")
			for header in sources_list:
				include_path = compute_generated_include_path(header, build_dir)
				f.write(f"#include \"{include_path}\"\n")
		print(f"cpprun: 提示: 仅提供头文件，已生成 {main_cpp_path} 以允许 CMake 配置。")
		updated.append(main_cpp_path)

	return updated


def compute_build_dir(build_dir_arg: str, clean: bool = False) -> str:
	"""根据参数计算/创建构建目录。

	若未指定则在脚本目录下创建 `build/<timestamp>`。
	若 `clean` 为 True 且目录存在则会删除后重建。
	返回构建目录的绝对路径。
	"""
	if build_dir_arg:
		build_dir = os.path.abspath(build_dir_arg)
	else:
		current_dir = os.path.abspath(os.path.dirname(__file__))
		timestamp = datetime.now().strftime("%Y%m%dT%H%M%S")
		build_dir = os.path.join(current_dir, "build", timestamp)

	if clean and os.path.isdir(build_dir):
		print("cpprun: 正在移除已存在的构建目录:", build_dir)
		shutil.rmtree(build_dir)

	os.makedirs(build_dir, exist_ok=True)
	return build_dir


def parse_args() -> argparse.Namespace:
	"""构建并解析命令行参数，返回解析结果的 Namespace。"""
	try:
		desc = "为指定 CMake 目录配置、构建并运行 CTest"
		help_cmake_dir = "包含 CMakeLists.txt 的目录（默认：脚本所在目录）"
		help_build_dir = "构建输出目录（绝对或相对路径）"
		help_target_name = "构建目标名（传递给 CMake 的 TARGET_NAME 变量，默认: project_bin）"
		help_config = "构建配置（多配置生成器使用，例如 Release/Debug）"
		help_jobs = "并行构建作业数（传递给 --parallel）"
		help_sources = '以分号分隔的源文件/头文件或目录列表，例如: "tests/main.cpp;tests/utils"'
		help_repeat = "运行 ctest 的次数，默认为 1"
		help_timeout = "测试超时时间（秒）；0 表示不设置超时"
		help_generator = 'CMake 生成器名称（例如 Ninja 或 Visual Studio）'
		help_install_dir = "安装目录（传递给 CMake 的 INSTALL_DIR）"
		help_clean = "在配置前删除构建目录"
		help_no_configure = "跳过 cmake 配置步骤"
		help_no_build = "跳过构建步骤（仅运行 ctest）"
		help_no_install = "跳过安装步骤（即使指定了 --install-dir）"
		help_no_test = "跳过运行 ctest 测试"

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
			print("cpprun: 警告: --repeat 的值必须至少为 1，已重置为 1。")
		if args.timeout < 0:
			args.timeout = 0
			print("cpprun: 警告: --timeout 不能小于 0，已重置为 0。")
		if args.jobs is not None and args.jobs < 1:
			args.jobs = None
			print("cpprun: 警告: --jobs 的值必须至少为 1，已重置为自动选择。")

		return args
	except SystemExit:
		print("cpprun: 参数解析已退出。")
		raise
	except KeyboardInterrupt:
		print("cpprun: 参数解析被用户中断（Ctrl+C）")
		sys.exit(130)


def main():
	"""主入口：检查 CMake、解析参数、配置、构建并运行测试。"""

	if not check_cmake():
		sys.exit(1)

	here = os.path.abspath(os.path.dirname(__file__))

	try:
		args = parse_args()

		input_paths = parse_sources(args.sources)

		build_dir = compute_build_dir(args.build_dir, clean=args.clean)
		sources_list, dir_list = split_input_paths(input_paths)

		# 如果用户传入的文件名为测试入口（cpprun_test.cpp 或 cpprun_test.c），
		# 我们应当传入整个目录以便运行该目录下的 CMake/CTest 配置。
		# 将匹配的文件替换为其父目录（避免只传入单个测试文件）。
		test_filenames = {"cpprun_test.cpp", "cpprun_test.c"}
		# 处理 file list 中的单文件测试情况
		new_file_list = []
		for f in sources_list:
			basename = os.path.basename(f)
			if basename in test_filenames:
				parent = os.path.abspath(os.path.dirname(f)) or os.path.abspath('.')
				if parent not in dir_list:
					dir_list.append(parent)
				print(f"cpprun: 检测到测试入口 {basename}，将改为使用目录进行配置与构建: {parent}")
			else:
				new_file_list.append(f)
		sources_list = new_file_list
		if not dir_list:
			sources_list = update_sources_list(sources_list, build_dir)

		if not sources_list and not dir_list:
			print("cpprun: 错误: 未提供源文件或目录。若 CMakeLists.txt 未定义 SOURCES，配置可能会失败。")
			sys.exit(1)

		cmake_dir = os.path.abspath(args.cmake_dir) if args.cmake_dir else os.path.abspath(here)

		if args.target_name and args.target_name.lower() == "test":
			print("cpprun: 警告: 目标名 'test' 可能与 CTest 内置测试冲突，已重命名为 'Test'.")
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
		print(f"cpprun: 未处理的错误: {e}")
		sys.exit(1)


if __name__ == "__main__":
	main()