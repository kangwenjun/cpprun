# cpprun 项目目录结构

```
cpprun/
├── .github/
│   └── workflows/
│       └── ci.yml
├── .ruff_cache/
│   ├── 0.15.8/
│   ├── .gitignore
│   └── CACHEDIR.TAG
├── .venv/
├── __pycache__/
├── build/
├── docs/
│   ├── en/
│   │   ├── CHANGELOG.md
│   │   ├── README.md
│   │   ├── RELEASE_NOTES.md
│   │   ├── cmake.md
│   │   ├── cpprun.py
│   │   └── project-layout.md
│   ├── format.md
│   └── project-layout.md
├── logs/
│   ├── cJSON.log
│   ├── spdlog.log
│   ├── tests.log
│   └── wait.log
├── prompts/
│   ├── cpprun_json.md
│   ├── dir.md
│   ├── file.md
│   └── tests.py.md
├── scripts/
│   ├── CMakeLists.txt
│   ├── cpprun.json
│   ├── cpprun.py
│   ├── test_all.py
│   ├── test_list.bat
│   └── tests.py
├── src/
│   ├── boostorg/
│   │   └── gpu.cpp
│   ├── cJSON/
│   │   ├── LICENSE
│   │   ├── README.md
│   │   ├── cJSON.c
│   │   ├── cJSON.h
│   │   ├── cJSON_Utils.c
│   │   ├── cJSON_Utils.h
│   │   └── cpprun_test.c
│   ├── cv/
│   │   ├── auto_reset_event.hpp
│   │   ├── event.hpp
│   │   └── manual_reset_event.hpp
│   ├── dir/
│   │   └── dir.hpp
│   ├── file/
│   │   ├── file.hpp
│   │   └── file_name_cache.hpp
│   ├── format/
│   │   ├── std_format.hpp
│   │   ├── std_vformat.hpp
│   │   └── va_format.hpp
│   ├── mutex/
│   │   └── safe_data.hpp
│   ├── network/
│   │   ├── wifi.h
│   │   ├── win_wifi_connect.cpp
│   │   └── win_wifi_get_status.cpp
│   ├── num/
│   │   └── stoi.hpp
│   ├── spdlog/
│   │   ├── spdlog/
│   │   │   ├── cfg/
│   │   │   │   ├── argv.h
│   │   │   │   ├── env.h
│   │   │   │   ├── helpers-inl.h
│   │   │   │   └── helpers.h
│   │   │   ├── details/
│   │   │   │   ├── backtracer-inl.h
│   │   │   │   ├── backtracer.h
│   │   │   │   ├── circular_q.h
│   │   │   │   ├── console_globals.h
│   │   │   │   ├── file_helper-inl.h
│   │   │   │   ├── file_helper.h
│   │   │   │   ├── fmt_helper.h
│   │   │   │   ├── log_msg-inl.h
│   │   │   │   ├── log_msg.h
│   │   │   │   ├── log_msg_buffer-inl.h
│   │   │   │   ├── log_msg_buffer.h
│   │   │   │   ├── mpmc_blocking_q.h
│   │   │   │   ├── null_mutex.h
│   │   │   │   ├── os-inl.h
│   │   │   │   ├── os.h
│   │   │   │   ├── periodic_worker-inl.h
│   │   │   │   ├── periodic_worker.h
│   │   │   │   ├── registry-inl.h
│   │   │   │   ├── registry.h
│   │   │   │   ├── synchronous_factory.h
│   │   │   │   ├── tcp_client-windows.h
│   │   │   │   ├── tcp_client.h
│   │   │   │   ├── thread_pool-inl.h
│   │   │   │   ├── thread_pool.h
│   │   │   │   ├── udp_client-windows.h
│   │   │   │   ├── udp_client.h
│   │   │   │   └── windows_include.h
│   │   │   ├── fmt/
│   │   │   │   ├── bundled/
│   │   │   │   │   ├── args.h
│   │   │   │   │   ├── base.h
│   │   │   │   │   ├── chrono.h
│   │   │   │   │   ├── color.h
│   │   │   │   │   ├── compile.h
│   │   │   │   │   ├── core.h
│   │   │   │   │   ├── fmt.license.rst
│   │   │   │   │   ├── format-inl.h
│   │   │   │   │   ├── format.h
│   │   │   │   │   ├── os.h
│   │   │   │   │   ├── ostream.h
│   │   │   │   │   ├── printf.h
│   │   │   │   │   ├── ranges.h
│   │   │   │   │   ├── std.h
│   │   │   │   │   └── xchar.h
│   │   │   │   ├── bin_to_hex.h
│   │   │   │   ├── chrono.h
│   │   │   │   ├── compile.h
│   │   │   │   ├── fmt.h
│   │   │   │   ├── ostr.h
│   │   │   │   ├── ranges.h
│   │   │   │   ├── std.h
│   │   │   │   └── xchar.h
│   │   │   ├── sinks/
│   │   │   │   ├── android_sink.h
│   │   │   │   ├── ansicolor_sink-inl.h
│   │   │   │   ├── ansicolor_sink.h
│   │   │   │   ├── base_sink-inl.h
│   │   │   │   ├── base_sink.h
│   │   │   │   ├── basic_file_sink-inl.h
│   │   │   │   ├── basic_file_sink.h
│   │   │   │   ├── callback_sink.h
│   │   │   │   ├── daily_file_sink.h
│   │   │   │   ├── dist_sink.h
│   │   │   │   ├── dup_filter_sink.h
│   │   │   │   ├── hourly_file_sink.h
│   │   │   │   ├── kafka_sink.h
│   │   │   │   ├── mongo_sink.h
│   │   │   │   ├── msvc_sink.h
│   │   │   │   ├── null_sink.h
│   │   │   │   ├── ostream_sink.h
│   │   │   │   ├── qt_sinks.h
│   │   │   │   ├── ringbuffer_sink.h
│   │   │   │   ├── rotating_file_sink-inl.h
│   │   │   │   ├── rotating_file_sink.h
│   │   │   │   ├── sink-inl.h
│   │   │   │   ├── sink.h
│   │   │   │   ├── stdout_color_sinks-inl.h
│   │   │   │   ├── stdout_color_sinks.h
│   │   │   │   ├── stdout_sinks-inl.h
│   │   │   │   ├── stdout_sinks.h
│   │   │   │   ├── syslog_sink.h
│   │   │   │   ├── systemd_sink.h
│   │   │   │   ├── tcp_sink.h
│   │   │   │   ├── udp_sink.h
│   │   │   │   ├── win_eventlog_sink.h
│   │   │   │   ├── wincolor_sink-inl.h
│   │   │   │   └── wincolor_sink.h
│   │   │   ├── async.h
│   │   │   ├── async_logger-inl.h
│   │   │   ├── async_logger.h
│   │   │   ├── common-inl.h
│   │   │   ├── common.h
│   │   │   ├── formatter.h
│   │   │   ├── fwd.h
│   │   │   ├── logger-inl.h
│   │   │   ├── logger.h
│   │   │   ├── mdc.h
│   │   │   ├── pattern_formatter-inl.h
│   │   │   ├── pattern_formatter.h
│   │   │   ├── spdlog-inl.h
│   │   │   ├── spdlog.h
│   │   │   ├── stopwatch.h
│   │   │   ├── tweakme.h
│   │   │   └── version.h
│   │   ├── README.md
│   │   ├── async.cpp
│   │   ├── bundled_fmtlib_format.cpp
│   │   ├── cfg.cpp
│   │   ├── color_sinks.cpp
│   │   ├── cpprun_test.cpp
│   │   ├── file_sinks.cpp
│   │   ├── spdlog.cpp
│   │   └── stdout_sinks.cpp
│   ├── str/
│   │   ├── bits_to_hex.hpp
│   │   └── hex_to_bits.hpp
│   ├── time/
│   │   ├── stopwatch.hpp
│   │   ├── timestamp.hpp
│   │   └── wait.hpp
│   └── singleton.hpp
├── tests/
│   ├── calc/
│   │   ├── main.cpp
│   │   ├── test_add.cpp
│   │   └── test_sub.cpp
│   ├── bench_wait.cpp
│   └── main.cpp
├── tmp/
│   ├── file_test.bin
│   └── file_test.txt
├── www/
│   ├── cn/
│   │   ├── assets/
│   │   │   ├── index.json
│   │   │   ├── logo.svg
│   │   │   ├── main.css
│   │   │   ├── main.js
│   │   │   └── styles.css
│   │   ├── src/
│   │   │   ├── bits_to_hex.html
│   │   │   ├── hex_to_bits.html
│   │   │   └── stopwatch.html
│   │   ├── about.html
│   │   ├── footer.html
│   │   ├── header.html
│   │   ├── index.html
│   │   └── sidebar.html
│   ├── docs/
│   │   └── 配色.md
│   └── prompts.md
├── .gitignore
├── CHANGELOG.md
├── LICENSE
├── README.md
├── RELEASE_NOTES.md
├── RULES.md
└── code_style.md
```

## 目录说明

- **.github/**: GitHub 相关配置文件，包含 CI/CD 工作流
- **docs/**: 项目文档，包括中英文版本
- **logs/**: 运行时生成的日志文件
- **prompts/**: AI 提示词模板文件
- **scripts/**: 构建和测试脚本
- **src/**: 源代码目录，包含各种 C++ 工具类和库
  - **boostorg/**: Boost 相关示例
  - **cJSON/**: JSON 解析库
  - **cv/**: 条件变量相关实现
  - **dir/**: 目录操作工具
  - **file/**: 文件操作工具
  - **format/**: 字符串格式化功能
  - **mutex/**: 互斥锁和数据安全工具
  - **network/**: 网络相关功能（WiFi）
  - **num/**: 数字处理工具
  - **spdlog/**: 高性能日志库
  - **str/**: 字符串转换工具
  - **time/**: 时间相关工具
- **tests/**: 测试代码
- **tmp/**: 临时文件
- **www/**: Web 界面相关文件