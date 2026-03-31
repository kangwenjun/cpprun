# C++ 示例项目

[English](README.en.md)

这是一个包含示例程序和单元测试的简单 C++ 项目（使用 CMake 构建）。

目录结构（简要）：

- `tests/`：示例和测试用例
  - `main.cpp`：示例可执行程序
  - `calc/`：计算示例及其测试
    - `main.cpp`
    - `test_add.cpp`
    - `test_sub.cpp`
  - `timestamp/`：头文件及其测试
    - `main.cpp`
    - `current_time.hpp`

## 构建（Windows / PowerShell）

```powershell
# 在项目根目录下执行
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

构建完成后，可执行文件位于 `build` 或相应的生成目录中（取决于生成器）。

## 运行示例

在 `build` 目录中运行生成的可执行文件，或直接运行示例的源代码生成的可执行文件。

## 运行测试

项目包含一个用于运行测试的 Python 脚本 `cpprun`。示例用法：

```powershell
# 在项目根目录下执行：

# 提供源文件列表
python cpprun.py -s "tests\main.cpp"  # 向 cmake 传入 SOURCES 列表
python cpprun.py -s "tests\main.cpp" --repeat 3  # 重复运行 ctest 3 次
python cpprun.py -s "tests\calc\main.cpp;tests\calc\test_add.cpp;tests\calc\test_sub.cpp"

# 仅提供头文件
# 但头文件必须包含main函数的实现，否则构建或测试会失败。
python cpprun.py -s "tests\timestamp\current_time.hpp"  

# 提供目录路径
python cpprun.py -s "tests\calc"
python cpprun.py -s "tests\timestamp"
```

`-s` 参数用于指定要运行的测试目录或测试用例路径。

`--repeat`（或 `-r`）用于指定运行 ctest 的次数，默认值为 1。
`;` 分号用于分割多个文件或多个目录


