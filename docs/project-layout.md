# 项目结构说明

这份文档把仓库拆成几个职责明确的部分，方便公开发布后让读者快速理解：什么是脚本、什么是构建层、什么是示例输入，以及 CI 在验证什么。

## 顶层职责划分

- [cpprun.py](../cpprun.py)：命令行脚本入口。负责解析参数、准备构建目录、调用 CMake 配置/构建，并最终执行 CTest。
- [CMakeLists.txt](../CMakeLists.txt)：通用构建脚本。它不绑定某一个固定示例，而是根据 `SOURCES` 或 `DIR_LIST` 动态生成 `project_bin`。
- [tests/](../tests)：示例输入与 smoke test 数据。这里的文件既是演示材料，也是 CI 运行时的最小测试集。
- [.github/workflows/ci.yml](../.github/workflows/ci.yml)：自动化验证入口。用于保证 README 中展示的主要使用方式持续可用。
- [README.md](../README.md) / [README.en.md](../README.en.md)：面向仓库首页的快速说明。

## tests 目录的角色

当前仓库把“示例”和“测试输入”放在同一个目录下，这是刻意设计，不是混乱堆放：

- [tests/main.cpp](../tests/main.cpp)：最小单文件示例，用来验证最简单的 `-s 文件.cpp` 场景。
- [tests/calc/main.cpp](../tests/calc/main.cpp)：多源文件示例的入口。
- [tests/calc/test_add.cpp](../tests/calc/test_add.cpp)：计算示例中的加法逻辑演示。
- [tests/calc/test_sub.cpp](../tests/calc/test_sub.cpp)：计算示例中的减法逻辑演示。
- [tests/timestamp/current_time.hpp](../tests/timestamp/current_time.hpp)：头文件模式示例，在定义 `__MAIN__` 时提供程序入口。
- [tests/timestamp/main.cpp](../tests/timestamp/main.cpp)：不依赖头文件注入时的普通入口写法参考。

## cpprun 的工作流

cpprun 的执行过程可以概括为四步：

1. 解析 `-s/--sources`，把传入的内容视为“文件列表”或“目录列表”。
2. 如果传入的是目录，则把目录交给 CMake 的 `DIR_LIST`，由 CMake 递归收集源文件。
3. 如果传入的全是头文件，则在构建目录中自动生成一个临时 `main.cpp`，定义 `__MAIN__` 后再包含目标头文件。
4. 调用 CMake 配置、构建，然后执行 `ctest -V`。

这也是为什么 README 把“单文件模式 / 目录模式 / 头文件模式”作为三个核心使用场景。

## 为什么 CI 要显式传 build 目录

[cpprun.py](../cpprun.py) 在未传 `-b` 时，会自动生成一个带时间戳的构建目录。这个行为对本地实验很方便，但在 CI 中显式指定构建目录更可控，原因有两个：

- 日志更容易定位
- Linux runner 下避免把构建目录放到不合适的位置

因此 [.github/workflows/ci.yml](../.github/workflows/ci.yml) 固定为每个用例指定独立的 `build/<case>` 目录。

## 后续扩展示例的建议

如果你要继续往仓库里加示例，建议遵守下面的分层方式：

- 单文件演示：直接放在 [tests/](../tests) 或单独子目录中
- 多文件演示：为每组示例创建一个独立子目录，并保留清晰的入口文件名
- 头文件演示：明确使用 `__MAIN__` 约定，并在 README 中说明该示例是“可执行头文件”而不是普通库头文件
- 文档：只在 README 放快速开始，把设计约定和目录职责沉淀到 docs 下

这样仓库首页保持简洁，而更具体的结构说明不会把首页挤得过长。