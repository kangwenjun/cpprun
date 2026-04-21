# spdlog - 1.17.0

LICENSE: MIT
URL: https://github.com/gabime/spdlog

简介
----

`spdlog` 是一个快速、轻量级且可配置的 C++ 日志库，提供多种输出 `sink`（控制台、文件、滚动文件、异步等）、灵活的格式化（基于 `fmt` 库）以及高性能的同步/异步记录。此仓库中包含 `spdlog` 的集成与使用示例。

主要特性
----

- 极高性能，适合高吞吐量场景
- 支持同步与异步日志
- 多种 sink：控制台、文件、旋转文件、每日文件、syslog 等
- 基于 `fmt` 的格式化语法
- 线程安全（同一 logger 并发使用时需要按需选择异步或保证外部同步）

快速开始（最小示例）
----

示例：在源码中直接包含并使用默认控制台 logger

```cpp
#include <spdlog/spdlog.h>

int main() {
	auto console = spdlog::stdout_color_mt("console");
	spdlog::set_level(spdlog::level::info); // 全局日志级别

	spdlog::info("Hello {}", "world");
	spdlog::error("An error occurred: {}", 123);

	return 0;
}
```

文件与旋转日志示例
----

```cpp
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

int main() {
	auto logger = spdlog::rotating_logger_mt("file_logger", "logs/mylog.txt", 1048576 * 5, 3);
	logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
	logger->info("Rotating logger initialized");
	return 0;
}
```

CMake 集成（建议）
----

如果通过 `add_subdirectory(spdlog)` 或安装到系统路径，可用 `find_package(spdlog REQUIRED)`：

```cmake
find_package(spdlog REQUIRED)
target_link_libraries(my_target PRIVATE spdlog::spdlog)
```

另外，若需要异步日志（提供更高并发写入性能），可链接 `spdlog::spdlog_async`，并在创建 logger 时使用异步工厂函数。

常见配置项与注意事项
----

- 日志级别：`trace` < `debug` < `info` < `warn` < `err` < `critical` < `off`。
- 异步日志会在后台线程写入，程序退出时应确保调用 `spdlog::shutdown()` 完成刷盘与释放资源。
- `spdlog` 使用 `fmt` 作为格式化后端，请确保 `fmt` 版本兼容（若是独立安装或系统包，检查 ABI）。
- 在多线程高并发场景下，异步 logger 通常比同步 logger 提供更好的吞吐，但会有消息丢失风险（取决于队列配置），根据需求选择。

故障排查与参考
----

- 官方仓库与文档： https://github.com/gabime/spdlog
- 常见问题：检查 `fmt` 依赖、C++ 标准（通常需要 C++11 及以上）以及编译时的线程库链接。

本仓库集成说明
----

本项目将 `spdlog` 作为第三方库集成在 `src/spdlog` 下，具体使用方式可参考上面的 CMake 示例以及仓库中的 demo/示例代码。

许可
----

MIT


