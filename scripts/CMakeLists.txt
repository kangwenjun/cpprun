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

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure source files are compiled as UTF-8 where supported.
# - MSVC: /utf-8 tells the compiler to treat source files as UTF-8.
# - GCC/Clang: -finput-charset and -fexec-charset set input/output encodings.
# These options are applied globally via `add_compile_options` so all targets
# created afterwards inherit the behavior.
if(MSVC)
  add_compile_options(
    "$<$<COMPILE_LANGUAGE:C>:/utf-8>"
    "$<$<COMPILE_LANGUAGE:CXX>:/utf-8>"
  )
else()
  add_compile_options(
    -finput-charset=UTF-8
    -fexec-charset=UTF-8
  )
endif()

# 可配置的目标名（默认 project_bin）
if(NOT DEFINED TARGET_NAME)
  set(TARGET_NAME project_bin)
endif()

# 处理 `SOURCES` 变量
# - 期望：`SOURCES` 为以分号(;)分隔的路径列表（相对路径）
# - 示例： -DSOURCES="a.cpp;b.c;src/foo.c"
if(NOT DEFINED SOURCES AND NOT DEFINED DIR_LIST)
  message(FATAL_ERROR "未找到源文件。请通过 -DSOURCES=\"main.cpp;src/lib.cpp\" 或 设置 DIR_LIST 来提供源文件列表。")
else()
  # 如果通过 -DSOURCES 提供了列表，将其赋值给 PROJECT_SOURCES 供后续使用
  set(PROJECT_SOURCES ${SOURCES})
endif()

if(DEFINED DIR_LIST)
  # 收集目录下的源文件，并记录这些源文件所在的目录以便加入 include 路径
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
      get_filename_component(src_dir ${src} PATH)
      list(APPEND COLLECTED_INCLUDE_DIRS ${src_dir})
    endforeach()
  endforeach()
  # 去重并将收集到的目录作为私有 include 路径
  list(REMOVE_DUPLICATES COLLECTED_INCLUDE_DIRS)
  # 若有收集到的目录，则追加到目标的 include 路径
  if(COLLECTED_INCLUDE_DIRS)
    # 在 add_executable 之后会调用 target_include_directories，但在此处先保证变量存在
    set(EXTRA_INCLUDE_DIRS "${COLLECTED_INCLUDE_DIRS}")
  endif()
endif()

# 主可执行目标：将 PROJECT_SOURCES 中的源文件编译为可执行文件 `project_bin`
add_executable(${TARGET_NAME} ${PROJECT_SOURCES})

# 包含目录：把项目根目录加入私有包含路径，便于直接使用 #include "current_time.hpp"
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
# 如果有额外收集到的 include 目录，则一并加入（来自 DIR_LIST 的子目录）
if(EXTRA_INCLUDE_DIRS)
  target_include_directories(${TARGET_NAME} PRIVATE ${EXTRA_INCLUDE_DIRS})
endif()

# 如果编译的源文件中包含 spdlog 实现文件，需要定义 SPDLOG_COMPILED_LIB
# 以便 spdlog 的实现文件正确开启库模式（避免头文件中的 #error 宏）。
set(HAS_SPDLOG FALSE)
foreach(_src ${PROJECT_SOURCES})
  if(_src MATCHES "spdlog")
    set(HAS_SPDLOG TRUE)
    break()
  endif()
endforeach()
if(HAS_SPDLOG)
  target_compile_definitions(${TARGET_NAME} PRIVATE SPDLOG_COMPILED_LIB)
endif()

# 测试配置：
# - 启用 CTest 并注册一个简单测试 `run_project`，执行生成的 `project_bin`。
# - 说明：这是一个基础的 smoke test；若可执行文件需要命令行参数或特殊环境，
#   请根据需要修改测试命令或添加更多测试目标。
enable_testing()
add_test(NAME run_project COMMAND ${TARGET_NAME})
if(DEFINED TEST_TIMEOUT AND TEST_TIMEOUT GREATER 0)
  set_tests_properties(run_project PROPERTIES TIMEOUT ${TEST_TIMEOUT})
endif()

# 安装规则：当调用 `cmake --install` 或 `cmake --build --target install` 时，
if(DEFINED INSTALL_DIR)
  install(TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION ${INSTALL_DIR}
  )
endif()

# 可选：如果有收集到的 include 目录，也可将其一并安装到 include/ 下，
# 便于将示例头文件作为库的一部分分发。这里只在 EXTRA_INCLUDE_DIRS 存在时
# 提供一个简单的安装规则（安装整个目录树）。
if(EXTRA_INCLUDE_DIRS)
  foreach(_inc ${EXTRA_INCLUDE_DIRS})
    # 将源目录相对于工程根的路径转换为安装子目录路径
    # 例如 tests/timestamp -> include/tests/timestamp
    file(RELATIVE_PATH _rel ${CMAKE_SOURCE_DIR} ${_inc})
    install(DIRECTORY ${_inc}/ DESTINATION include/${_rel})
  endforeach()
endif()
