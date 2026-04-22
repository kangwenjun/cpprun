
#define SPDLOG_COMPILED_LIB

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/async.h>
#include <spdlog/fmt/bin_to_hex.h>

#include <iostream>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

void setup_utf8_console()
{
#ifdef _WIN32
    // 设置Windows控制台为UTF-8模式
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

int main()
{
    // 设置UTF-8控制台
    setup_utf8_console();

    // ============================================
    // 1. 基本日志输出 - 使用默认logger
    // ============================================
    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);
    spdlog::warn("Easy padding in numbers like {:08d}", 12);

    // ============================================
    // 2. UTF-8 中文日志输出
    // ============================================
    spdlog::info("这是一段中文日志信息");
    spdlog::info("日本語ログメッセージ");
    spdlog::info("한국어 로그 메시지");
    spdlog::info("Emoji test: 🎉 🚀 💻 🔥");

    // ============================================
    // 3. 设置日志级别
    // ============================================
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("调试信息：当前日志级别为 DEBUG");

    // ============================================
    // 4. 设置日志格式
    // ============================================
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
    spdlog::info("自定义格式日志输出");

    // 恢复默认格式
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    // ============================================
    // 5. 创建文件日志记录器（UTF-8编码）
    // ============================================
    auto file_logger = spdlog::basic_logger_mt("basic_logger", "logs/basic.txt");
    file_logger->info("文件日志记录器已创建");
    file_logger->info("UTF-8 content: 中文 日本語 한국어 🌍");

    // ============================================
    // 6. 创建彩色控制台日志记录器
    // ============================================
    auto console = spdlog::stdout_color_mt("console");
    console->info("彩色控制台输出测试");
    console->error("错误信息：发生了一个错误 ❌");
    console->warn("警告信息：请注意 ⚠️");

    auto err_console = spdlog::stderr_color_mt("stderr");
    err_console->error("标准错误输出测试");

    // ============================================
    // 7. 创建多接收器logger（同时输出到控制台和文件）
    // ============================================
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true));

    auto multi_logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());
    multi_logger->set_level(spdlog::level::debug);
    multi_logger->warn("多接收器日志：同时输出到控制台和文件 📄");

    // ============================================
    // 8. 创建每日日志文件记录器
    // ============================================
    auto daily_logger = spdlog::daily_logger_mt("daily_logger", "logs/daily.txt", 2, 30);
    daily_logger->info("每日日志记录器 📅");

    // ============================================
    // 9. 创建轮转日志文件记录器
    // ============================================
    auto rotating_logger = spdlog::rotating_logger_mt("rotating_logger", "logs/rotating.txt", 1024*1024*5, 3);
    rotating_logger->info("轮转日志记录器（最大5MB，保留3个文件）🔄");

    // ============================================
    // 10. 异步日志记录
    // ============================================
    spdlog::init_thread_pool(8192, 1);
    auto async_file = spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger", "logs/async.txt");
    async_file->info("异步日志消息 ⚡");

    // ============================================
    // 11. 使用宏记录日志（显示源代码位置）
    // ============================================
    SPDLOG_LOGGER_INFO(spdlog::default_logger(), "带源代码位置的日志信息 📍");

    // ============================================
    // 12. 十六进制输出
    // ============================================
    std::vector<unsigned char> data = {0x01, 0x02, 0x03, 0x04, 0x05};
    spdlog::info("二进制数据: {:n}", spdlog::to_hex(data));

    // UTF-8字符串的十六进制表示
    std::string utf8_str = "Hello 世界";
    spdlog::info("UTF-8字符串十六进制: {:n}", spdlog::to_hex(utf8_str));

    // ============================================
    // 13. 刷新日志缓冲区
    // ============================================
    spdlog::flush_on(spdlog::level::info);
    spdlog::info("所有日志已刷新 💾");

    // ============================================
    // 14. 关闭所有日志记录器
    // ============================================
    spdlog::shutdown();

    return 0;
}
