// Minimal file utilities using the C++ standard library
#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iterator>
#include <system_error>
#include <chrono>
#include <optional>
#include <random>
#include <thread>


namespace cpprun::file {

inline bool exists(const std::filesystem::path& p) noexcept {
    std::error_code ec;
    return std::filesystem::exists(p, ec);
}

inline uintmax_t get_file_size(const std::filesystem::path& p) noexcept {
    std::error_code ec;
    auto s = std::filesystem::file_size(p, ec);
    if (ec) return 0u;
    return s;
}

inline std::string read_all_text(const std::filesystem::path& p) noexcept {
    std::ifstream ifs(p, std::ios::in | std::ios::binary);
    if (!ifs) return {};
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

inline std::vector<uint8_t> read_all_bytes(const std::filesystem::path& p) noexcept {
    std::ifstream ifs(p, std::ios::binary);
    if (!ifs) return {};
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
}

inline std::vector<std::string> read_lines(const std::filesystem::path& p) noexcept {
    std::ifstream ifs(p);
    if (!ifs) return {};
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(ifs, line)) lines.push_back(line);
    return lines;
}

inline bool write_all_text(const std::filesystem::path& p, const std::string& content) noexcept {
    if (!p.has_parent_path()) {
        // no parent directory
    } else {
        std::error_code ec;
        std::filesystem::create_directories(p.parent_path(), ec);
    }
    std::ofstream ofs(p, std::ios::binary);
    if (!ofs) return false;
    ofs.write(content.data(), static_cast<std::streamsize>(content.size()));
    return static_cast<bool>(ofs);
}

inline bool write_all_bytes(const std::filesystem::path& p, const std::vector<uint8_t>& bytes) noexcept {
    if (!p.has_parent_path()) {
        // no parent directory
    } else {
        std::error_code ec;
        std::filesystem::create_directories(p.parent_path(), ec);
    }
    std::ofstream ofs(p, std::ios::binary);
    if (!ofs) return false;
    if (!bytes.empty()) ofs.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    return static_cast<bool>(ofs);
}

inline bool append_text(const std::filesystem::path& p, const std::string& content) noexcept {
    if (!p.has_parent_path()) {
    } else {
        std::error_code ec;
        std::filesystem::create_directories(p.parent_path(), ec);
    }
    std::ofstream ofs(p, std::ios::binary | std::ios::app);
    if (!ofs) return false;
    ofs.write(content.data(), static_cast<std::streamsize>(content.size()));
    return static_cast<bool>(ofs);
}

inline bool append_bytes(const std::filesystem::path& p, const std::vector<uint8_t>& bytes) noexcept {
    if (!p.has_parent_path()) {
    } else {
        std::error_code ec;
        std::filesystem::create_directories(p.parent_path(), ec);
    }
    std::ofstream ofs(p, std::ios::binary | std::ios::app);
    if (!ofs) return false;
    if (!bytes.empty()) ofs.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    return static_cast<bool>(ofs);
}

inline bool remove_file(const std::filesystem::path& p) noexcept {
    std::error_code ec;
    return std::filesystem::remove(p, ec);
}

inline bool copy_file(const std::filesystem::path& from, const std::filesystem::path& to, bool overwrite = false) noexcept {
    std::error_code ec;
    std::filesystem::copy_options opts = overwrite ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::skip_existing;
    std::filesystem::copy(from, to, opts, ec);
    return !ec;
}

// Move/rename file (fallback to copy+remove on cross-device)
inline bool move_file(const std::filesystem::path& from, const std::filesystem::path& to) noexcept {
    std::error_code ec;
    std::filesystem::rename(from, to, ec);
    if (!ec) return true;
    // fallback: copy then remove
    std::filesystem::copy_options opts = std::filesystem::copy_options::overwrite_existing;
    std::filesystem::copy(from, to, opts, ec);
    if (ec) return false;
    std::error_code ec2;
    std::filesystem::remove(from, ec2);
    return !ec2;
}

// List directory entries (non-recursive or recursive)
inline std::vector<std::filesystem::path> list_directory(const std::filesystem::path& dir, bool recursive = false) noexcept {
    std::vector<std::filesystem::path> out;
    std::error_code ec;
    if (!std::filesystem::exists(dir, ec) || ec) return out;
    try {
        if (recursive) {
            for (auto &entry : std::filesystem::recursive_directory_iterator(dir)) {
                out.push_back(entry.path());
            }
        } else {
            for (auto &entry : std::filesystem::directory_iterator(dir)) {
                out.push_back(entry.path());
            }
        }
    } catch (...) {
        // swallow exceptions, return collected entries so far
    }
    return out;
}

// Atomic write: write to temp file in same directory then rename
inline bool atomic_write_text(const std::filesystem::path& p, const std::string& content) noexcept {
    std::error_code ec;
    if (p.has_parent_path()) std::filesystem::create_directories(p.parent_path(), ec);
    auto dir = p.has_parent_path() ? p.parent_path() : std::filesystem::current_path();
    std::filesystem::path tmp;
    // find a unique temp name
    for (int i = 0; i < 100; ++i) {
        tmp = dir / (p.filename().string() + ".tmp" + std::to_string(i));
        if (!std::filesystem::exists(tmp, ec)) break;
    }
    if (!write_all_text(tmp, content)) return false;
    std::filesystem::rename(tmp, p, ec);
    if (ec) {
        // try remove tmp
        std::error_code ignore;
        std::filesystem::remove(tmp, ignore);
        return false;
    }
    return true;
}

// --- timestamps / permissions ---
inline std::filesystem::file_time_type get_last_write_time(const std::filesystem::path& p) noexcept {
    std::error_code ec;
    auto t = std::filesystem::last_write_time(p, ec);
    if (ec) return std::filesystem::file_time_type{};
    return t;
}

inline bool set_last_write_time(const std::filesystem::path& p, const std::filesystem::file_time_type& t) noexcept {
    std::error_code ec;
    std::filesystem::last_write_time(p, t, ec);
    return !ec;
}

inline std::filesystem::perms get_permissions(const std::filesystem::path& p) noexcept {
    std::error_code ec;
    auto pr = std::filesystem::status(p, ec).permissions();
    if (ec) return std::filesystem::perms::unknown;
    return pr;
}

inline bool set_permissions(const std::filesystem::path& p, std::filesystem::perms pr) noexcept {
    std::error_code ec;
    std::filesystem::permissions(p, pr, ec);
    return !ec;
}

// --- read range / stream utilities ---
inline std::vector<uint8_t> read_range(const std::filesystem::path& p, std::streamoff offset, std::size_t count) noexcept {
    std::ifstream ifs(p, std::ios::binary);
    if (!ifs) return {};
    ifs.seekg(0, std::ios::end);
    std::streamoff sz = ifs.tellg();
    if (offset < 0 || offset >= sz) return {};
    ifs.seekg(offset, std::ios::beg);
    std::vector<uint8_t> out;
    out.resize(count);
    ifs.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(count));
    std::streamsize got = ifs.gcount();
    out.resize(static_cast<std::size_t>(got));
    return out;
}

template <class F>
inline bool for_each_line(const std::filesystem::path& p, F&& func) noexcept {
    std::ifstream ifs(p);
    if (!ifs) return false;
    std::string line;
    while (std::getline(ifs, line)) {
        // if func returns false, stop iteration
        if (!func(line)) break;
    }
    return true;
}

// --- temp file / directory helpers ---
inline std::filesystem::path make_temp_path(const std::string& prefix = "cpprun_") noexcept {
    std::error_code ec;
    auto td = std::filesystem::temp_directory_path(ec);
    if (ec) td = std::filesystem::current_path();
    // random suffix
    static thread_local std::mt19937_64 rng((std::random_device())());
    std::uniform_int_distribution<uint64_t> dist;
    for (int i = 0; i < 100; ++i) {
        auto suf = std::to_string(dist(rng));
        auto p = td / (prefix + suf);
        if (!std::filesystem::exists(p, ec)) return p;
    }
    return td / (prefix + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
}

inline std::filesystem::path create_temp_file(const std::string& prefix = "cpprun_") noexcept {
    auto p = make_temp_path(prefix);
    // create an empty file
    if (write_all_bytes(p, {})) return p;
    return {};
}

inline std::filesystem::path create_temp_directory(const std::string& prefix = "cpprun_") noexcept {
    auto p = make_temp_path(prefix);
    std::error_code ec;
    if (std::filesystem::create_directory(p, ec)) return p;
    return {};
}

} // namespace cpprun::file

#ifdef __MAIN__
#include <iostream>

int main()
{
    using namespace cpprun::file;

    namespace fs = std::filesystem;
    const fs::path dir = "tmp";
    const fs::path text_path = dir / "file_test.txt";
    const fs::path bin_path = dir / "file_test.bin";
    const std::string content = "Hello, cpprun file.hpp!\nSecond line\n";

    // ensure base write works
    if (!cpprun::file::write_all_text(text_path, content)) {
        std::cerr << "write_all_text failed" << std::endl;
        return __LINE__;
    }

    // append text
    if (!cpprun::file::append_text(text_path, "Appended line\n")) {
        std::cerr << "append_text failed" << std::endl;
        return __LINE__;
    }

    // atomic write (replace)
    if (!cpprun::file::atomic_write_text(text_path, content + "Replaced\n")) {
        std::cerr << "atomic_write_text failed" << std::endl;
        return __LINE__;
    }

    // binary write/read
    std::vector<uint8_t> bytes = {0, 1, 2, 3, 4, 255};
    if (!cpprun::file::write_all_bytes(bin_path, bytes)) {
        std::cerr << "write_all_bytes failed" << std::endl;
        return __LINE__;
    }
    auto rbytes = cpprun::file::read_all_bytes(bin_path);
    if (rbytes != bytes) {
        std::cerr << "read_all_bytes mismatch" << std::endl;
        return __LINE__;
    }

    // read range
    auto part = cpprun::file::read_range(bin_path, 2, 3);
    if (part.size() != 3 || part[0] != 2) {
        std::cerr << "read_range failed" << std::endl;
        return __LINE__;
    }

    // for_each_line
    bool seen = false;
    if (!cpprun::file::for_each_line(text_path, [&](const std::string &ln){
        if (!ln.empty()) seen = true; return true; })) {
        std::cerr << "for_each_line failed" << std::endl;
        return __LINE__;
    }
    if (!seen) { std::cerr << "for_each_line didn't see content" << std::endl; return __LINE__; }

    // timestamps and permissions (read-only checks)
    auto t = cpprun::file::get_last_write_time(text_path);
    if (t == fs::file_time_type{}) { std::cerr << "get_last_write_time empty" << std::endl; return __LINE__; }
    auto perms = cpprun::file::get_permissions(text_path);
    (void)perms; // just ensure callable

    // temp file and directory
    auto tmp_dir = cpprun::file::create_temp_directory();
    if (tmp_dir.empty()) { std::cerr << "create_temp_directory failed" << std::endl; return __LINE__; }
    auto tmp_file = cpprun::file::create_temp_file();
    if (tmp_file.empty()) { std::cerr << "create_temp_file failed" << std::endl; return __LINE__; }

    // list directory
    auto list = cpprun::file::list_directory(dir, false);
    if (list.empty()) { std::cerr << "list_directory returned empty" << std::endl; return __LINE__; }

    // cleanup
    cpprun::file::remove_file(bin_path);
    cpprun::file::remove_file(text_path);
    std::error_code ec; std::filesystem::remove(tmp_file, ec); std::filesystem::remove(tmp_dir, ec);

    std::cout << "file.hpp demo: all checks passed" << std::endl;
    return 0;
}

#endif // __MAIN__
