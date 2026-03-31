cmake_minimum_required(VERSION 3.15)
project(auto_build_project LANGUAGES C CXX)
# 简介：
# - 目的：使用传入的 `SOURCES` 变量构建可执行目标 `project_bin`。
# - 约定：
#   - `SOURCES` 为必需项；格式为分号分隔的源文件列表，相对于项目根目录，例如：
#       -DSOURCES="main.cpp;src/lib.cpp"
# - 输出：生成可执行文件 `project_bin`
# - 常用命令：
#     cmake -S . -B build -DSOURCES="main.cpp;src/lib.cpp"
#     cmake --build build --parallel 4
#     ctest --output-on-failure -C Release
# 注意：
# - 当前脚本在未提供 `SOURCES` 时会报错并退出；如需自动搜索源文件，请修改为使用 file(GLOB ...) 或其他策略。
#
# Summary:
# - Purpose: Build an executable target `project_bin` from the provided `SOURCES` variable.
# - Convention:
#   - `SOURCES` is required; it should be a semicolon-separated list of source files relative to the project root, e.g.
#       -DSOURCES="main.cpp;src/lib.cpp"
# - Output: Produces an executable named `project_bin`.
# - Common commands:
#     cmake -S . -B build -DSOURCES="main.cpp;src/lib.cpp"
#     cmake --build build --parallel 4
#     ctest --output-on-failure -C Release
# Note:
# - This script will error and exit if `SOURCES` is not provided; to auto-discover sources, consider using file(GLOB ...) or another strategy.

# 设置 C++ 标准
# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 可配置的目标名（默认 project_bin）
# Configurable target name (default: project_bin)
if(NOT DEFINED TARGET_NAME)
  set(TARGET_NAME project_bin)
endif()

# 处理 `SOURCES` 变量
# Handle the `SOURCES` variable
# - 期望：`SOURCES` 为以分号(;)分隔的路径列表（相对路径）
# - Expectation: `SOURCES` should be a semicolon(;) separated list of relative paths
# - 示例： -DSOURCES="a.cpp;b.c;src/foo.c"
# - Example: -DSOURCES="a.cpp;b.c;src/foo.c"
if(NOT DEFINED SOURCES AND NOT DEFINED DIR_LIST)
  message(FATAL_ERROR "Not found source files")
else()
  # 如果通过 -DSOURCES 提供了列表，将其赋值给 PROJECT_SOURCES 供后续使用
  # If a list is provided via -DSOURCES, assign it to PROJECT_SOURCES for later use
  set(PROJECT_SOURCES ${SOURCES})
endif()

if(DEFINED DIR_LIST)
  # 收集目录下的源文件，并记录这些源文件所在的目录以便加入 include 路径
  # Collect source files under directories and record their directories to add to include paths
  set(COLLECTED_INCLUDE_DIRS "")
  foreach(dir ${DIR_LIST})
    file(GLOB_RECURSE dir_sources CONFIGURE_DEPENDS
      ${dir}/*.cpp
      ${dir}/*.c
      ${dir}/*.C
      ${dir}/*.cxx
      ${dir}/*.cc)
    list(APPEND PROJECT_SOURCES ${dir_sources})
    foreach(src ${dir_sources})
      # 获取源文件所在目录并加入集合
      # Get the directory of the source file and append it to the collection
      get_filename_component(src_dir ${src} PATH)
      list(APPEND COLLECTED_INCLUDE_DIRS ${src_dir})
    endforeach()
  endforeach()
  # 去重并将收集到的目录作为私有 include 路径
  # Remove duplicates and use the collected directories as private include paths
  list(REMOVE_DUPLICATES COLLECTED_INCLUDE_DIRS)
  # 若有收集到的目录，则追加到目标的 include 路径
  # If any directories were collected, add them to the target include paths
  if(COLLECTED_INCLUDE_DIRS)
    # 在 add_executable 之后会调用 target_include_directories，但在此处先保证变量存在
    # target_include_directories will be called after add_executable; ensure the variable exists now
    set(EXTRA_INCLUDE_DIRS "${COLLECTED_INCLUDE_DIRS}")
  endif()
endif()

# 主可执行目标：将 PROJECT_SOURCES 中的源文件编译为可执行文件 `project_bin`
# Main executable target: compile sources in PROJECT_SOURCES into executable `project_bin`
add_executable(${TARGET_NAME} ${PROJECT_SOURCES})

# 包含目录：把项目根目录加入私有包含路径，便于直接使用 #include "current_time.hpp"
# Include directories: add project root to private include paths to allow direct #include "current_time.hpp"
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
# 如果有额外收集到的 include 目录，则一并加入（来自 DIR_LIST 的子目录）
if(EXTRA_INCLUDE_DIRS)
  target_include_directories(${TARGET_NAME} PRIVATE ${EXTRA_INCLUDE_DIRS})
endif()

# 测试配置：
# - 启用 CTest 并注册一个简单测试 `run_project`，执行生成的 `project_bin`。
# - 说明：这是一个基础的 smoke test；若可执行文件需要命令行参数或特殊环境，
#   请根据需要修改测试命令或添加更多测试目标。
#
# Test configuration:
# - Enable CTest and register a simple test `run_project` that runs the produced `project_bin`.
# - Note: This is a basic smoke test; if the executable requires CLI arguments or a special environment,
#   adjust the test command or add more test targets as needed.
enable_testing()
add_test(NAME run_project COMMAND ${TARGET_NAME})
if(DEFINED TEST_TIMEOUT AND TEST_TIMEOUT GREATER 0)
  set_tests_properties(run_project PROPERTIES TIMEOUT ${TEST_TIMEOUT})
endif()
