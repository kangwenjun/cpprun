`std::format` 是 C++20 引入的现代字符串格式化库，它提供了一个类型安全、高效且易于使用的格式化方案，常被看作是 `printf` 家族和 `iostream` 的现代替代品。使用它的主要优势在于**类型安全**（由编译器保证）、**更高的性能**（通常优于 `iostream`）和**更直观的语法**。

### 📝 概述与使用准备
在使用 `std::format` 之前，需要了解以下关键信息：

*   **引入头文件**：所有与 `std::format` 相关的功能都定义在 `<format>` 头文件中。
*   **编译器支持**：需要编译器支持 C++20 标准。建议使用 GCC 13+、Clang 14+ 或 MSVC 19.29+ 版本。
*   **命名空间**：所有功能都位于 `std` 命名空间下。
*   **第三方库**：如果你的项目暂时无法升级到 C++20，可以优先考虑使用功能相似的 `{fmt}` 库。事实上，`std::format` 的设计正是基于 `{fmt}` 库。

### ✨ 核心：基础用法
`std::format` 的核心语法非常简单：`std::format("格式化字符串", 参数列表)`。

1.  **基本占位符 `{}`**：最常用的占位符，会按参数传递的顺序自动替换。

    ```cpp
    #include <iostream>
    #include <format>

    int main() {
        std::string name = "Alice";
        int age = 30;
        std::string info = std::format("Name: {}, Age: {}", name, age);
        std::cout << info << std::endl; // 输出：Name: Alice, Age: 30
        return 0;
    }
    ```
    

2.  **位置参数 `{N}`**：通过数字索引来指定使用哪个参数，索引从 `0` 开始。这允许你在格式化字符串中以任意顺序引用参数，甚至可以重复使用同一个参数。

    ```cpp
    #include <iostream>
    #include <format>

    int main() {
        std::string name = "Alice";
        int age = 30;
        // {1} 是第二个参数 "Alice", {0} 是第一个参数 age
        std::string info = std::format("{1} is {0} years old.", age, name);
        std::cout << info << std::endl; // 输出：Alice is 30 years old.

        // 重复使用同一个参数
        std::string repeated = std::format("{0} {0} {1}", "a", "b");
        std::cout << repeated << std::endl; // 输出：a a b
        return 0;
    }
    ```
    

3.  **格式化说明符**：这是 `std::format` 最强大的部分。通过在 `{}` 中 `:` 之后添加特定的指令，可以精确控制输出格式。其通用语法为 `{:[填充][对齐][符号][#][0][宽度][.精度][L][类型]}`。所有部分都是可选的，其中“填充”和“对齐”需要一起使用。

### 🛠️ 进阶：格式说明符详解
下表详细列出了各个格式化选项，让你能像拼积木一样组合出所需的格式。

| 类别 | 选项 | 描述 | 示例 (`std::format` 调用) | 输出结果 |
| :--- | :--- | :--- | :--- | :--- |
| **对齐与填充** | `<` | 左对齐（默认，对于非数值类型） | `"{:<10}"` | `"value     "` |
| | `>` | 右对齐（默认，对于数值类型） | `"{:>10}"` | `"     value"` |
| | `^` | 居中对齐 | `"{:^10}"` | `"  value   "` |
| | `*<` | 指定填充字符 `*` 并左对齐 | `"{:*<10}"` | `"value*****"` |
| **符号** | `+` | 强制为正负数都显示符号（`+`/`-`） | `"{:+d} {:+d}"` | `"+42 -42"` |
| | `-` | 仅负数显示符号（默认） | `"{:-d} {:-d}"` | `"42 -42"` |
| | 空格 | 为正数预留一个空格 | `"{: d}"` | `" 42"` |
| **进制** | `b` | 二进制 | `"{:b}"` | `"1010"` |
| | `o` | 八进制 | `"{:o}"` | `"12"` |
| | `d` | 十进制 | `"{:d}"` | `"10"` |
| | `x` | 小写十六进制 | `"{:x}"` | `"a"` |
| | `X` | 大写十六进制 | `"{:X}"` | `"A"` |
| | `#` | 添加进制前缀 (`0b`, `0o`, `0x`) | `"{:#b}"` | `"0b1010"` |
| **宽度与精度** | `{n}` | 设置最小宽度 `n` | `"{:10}"` | `"        42"` |
| | `{0:n}` | 动态指定宽度（使用第0个参数） | `std::format("{:>{}}", 42, 10)` | `"        42"` |
| | `.{n}` | 设置浮点数精度或字符串最大宽度 | `"{:.2f}"` | `"3.14"` |
| | `{0:.{1}f}` | 动态指定精度 | `std::format("{:.{}f}", 3.14159, 2)` | `"3.14"` |
| **浮点数类型** | `e` / `E` | 科学计数法 | `"{:.2e}"` | `"3.14e+00"` |
| | `f` / `F` | 定点十进制 | `"{:.2f}"` | `"3.14"` |
| | `g` / `G` | 根据数值自动选择 `f` 或 `e` | `"{:g}"` | `"3.14159"` |
| | `a` / `A` | 十六进制科学计数法 | `"{:a}"` | `"0x1.921fap+1"` |
| **其他类型** | `p` | 指针地址 | `"{:p}"` | `"0x7ffd6e7d4c14"` |
| | `s` | 字符串（默认） | `"{}"` | `"text"` |
| | `{0:d}` | 布尔值格式化为 `1` / `0` | `std::format("{:d}", true)` | `"1"` |
| **本地化与补零** | `L` | 使用当前 locale 的本地化规则 | `"{:L}"` | 取决于 locale 设置 |
| | `0` | 对于数值，用前导零填充至指定宽度 | `"{:05d}"` | `"00042"` |

> **补充示例**：
> *   进制转换：`std::format("十进制: {}, 十六进制: {:x}, 二进制: {:b}", 255, 255, 255);` 输出 `十进制: 255, 十六进制: ff, 二进制: 11111111`。
> *   动态宽度：`int w = 10; std::format("{:>{}}", "hello", w);` 输出 `"     hello"`。
> *   布尔值：`std::format("{} {:d}", true, false);` 输出 `"true 0"`。

### 🚀 高级技巧与最佳实践
1.  **转义花括号**：如果需要在输出字符串中显示 `{` 或 `}`，需要使用两个连续的左花括号 `{{` 或右花括号 `}}` 来进行转义。

    ```cpp
    #include <iostream>
    #include <format>

    int main() {
        // 使用 {{ 和 }} 来转义，内部的 {} 仍然是占位符
        std::string result = std::format("The value {{ {} }} is important.", 42);
        std::cout << result << std::endl; // 输出：The value { 42 } is important.
        return 0;
    }
    ```
    

2.  **自定义类型格式化**：要让自定义类型（如 `struct Point`）也能被 `std::format` 处理，需要为该类型特化 `std::formatter` 模板。这需要实现两个关键方法：
    *   `parse`：解析格式说明符（如 `{.2f}`）。
    *   `format`：将值格式化到输出缓冲区。

    ```cpp
    #include <format>
    #include <iostream>
    #include <string_view>

    struct Point {
        double x, y;
    };

    // 为 Point 特化 std::formatter
    template <>
    struct std::formatter<Point> {
        // 可选：继承内置类型的格式化器，以复用其格式解析能力
        // 例如，这里我们复用 double 的格式化器来处理 x 和 y
        std::formatter<double> formatter;

        // 1. parse 函数：解析格式说明符，例如 {:.1f}
        constexpr auto parse(std::format_parse_context& ctx) {
            // 将解析工作委托给内部的 double 格式化器
            // 这允许我们使用如 {:.1f} 这样的格式说明符
            return formatter.parse(ctx);
        }

        // 2. format 函数：执行实际格式化
        auto format(const Point& p, std::format_context& ctx) const {
            // std::format_to 用于将格式化结果写入到 ctx.out() 这个输出迭代器
            // 最终返回写入结束后的迭代器
            return std::format_to(ctx.out(), "({}, {})",
                                  formatter.format(p.x, ctx),
                                  formatter.format(p.y, ctx));
        }
    };

    int main() {
        Point pt{3.14159, 2.71828};
        // 使用自定义格式说明符 .2f
        std::string s = std::format("Point: {:.2f}", pt);
        std::cout << s << std::endl; // 输出：Point: (3.14, 2.72)
        return 0;
    }
    ```
    

3.  **更多格式化函数**：`<format>` 库还提供了其他有用的函数以满足不同的需求。
    *   `std::format_to`：将格式化结果直接写入一个输出迭代器（如 `std::back_inserter` 或原始缓冲区），对于性能敏感或需要避免内存分配的场景非常有用。
    *   `std::formatted_size`：计算格式化结果所需的字符数（不包括结尾的 `\0`），便于提前分配缓冲区。
    *   `std::vformat`：类似于 `std::format`，但接受一个 `std::format_args` 对象，用于处理参数列表数量不确定的运行时格式化。

### 💎 总结
为了帮你更快地查阅，这里有一份用法速查表：

| 类别 | 格式说明符 | 示例 | 输出 |
| :--- | :--- | :--- | :--- |
| **基本** | `{}` | `std::format("{}, {}", "Hello", "World")` | `Hello, World` |
| **位置** | `{0} {1}` | `std::format("{1}, {0}", "World", "Hello")` | `Hello, World` |
| **进制** | `{:b}`, `{:x}`, `{:o}` | `std::format("{:b} {:x} {:o}", 10, 10, 10)` | `1010 a 12` |
| **对齐/宽** | `{:<10}`, `{:>10}`, `{:^10}` | `std::format("{:<10}|{:>10}", "left", "right")` | `left      |     right` |
| **填充** | `{:*^10}` | `std::format("{:*^10}", "hi")` | `****hi****` |
| **精度** | `{:.2f}` | `std::format("{:.2f}", 3.14159)` | `3.14` |
| **符号** | `{:+d}`, `{: d}` | `std::format("{:+d} {: d}", 42, 42)` | `+42  42` |
| **动态宽/精** | `{:{}}`, `{:.{}}` | `std::format("{:>{}}", 42, 10)` | `        42` |

### ⚠️ 常见注意事项
*   **占位符数量**：格式字符串中 `{}` 的数量不能**多于**提供的参数数量，否则会抛出 `std::format_error` 异常。
*   **`format_error` 异常**：在运行时，如果格式字符串解析失败或参数索引无效，`std::format` 会抛出 `std::format_error` 类型的异常。
*   **类型安全**：与 `printf` 不同，`std::format` 在编译期就能检查格式说明符和参数类型是否匹配，极大地增强了程序的安全性。