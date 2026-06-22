// Copyright(c) 2015-present, 康文君 & cpprun contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <cstdio>      // fopen, fwrite, fclose
#include <filesystem>  // std::filesystem::path
#include <vector>      // std::vector
#include <string>


namespace cpprun::bin {

inline std::vector<uint8_t> read_file(const std::filesystem::path& p) noexcept(false) {
    std::ifstream ifs(p, std::ios::binary | std::ios::ate);
    if (!ifs) return {};
    
    const auto size = static_cast<size_t>(ifs.tellg());
    ifs.seekg(0);
    
    std::vector<uint8_t> result;
    result.resize(size);
    ifs.read(reinterpret_cast<char*>(result.data()), size);
    return result;
}

// inline size_t file_size(const std::filesystem::path& p) noexcept(true) {
//     std::error_code ec;
//     auto sz = std::filesystem::file_size(p, ec);
//     if (ec) return 0;
//     return static_cast<size_t>(sz);
// }

inline size_t file_size(const std::filesystem::path& p) noexcept(true) {
    FILE* fp = std::fopen(p.c_str(), "rb");
    if (!fp) return 0;
    std::fseek(fp, 0, SEEK_END);
    size_t size = static_cast<size_t>(std::ftell(fp));
    std::fclose(fp);
    return size;
}

inline size_t read_file(const std::string& p, uint8_t* data, size_t size) noexcept(true) {
    if (!data) return 0;
    if (size == 0) return 0;

    FILE* fp = std::fopen(p.c_str(), "rb");
    if (!fp) return 0;

    std::fseek(fp, 0, SEEK_END);
    long pos = std::ftell(fp);
    if (pos < 0) { std::fclose(fp); return 0; }
    size_t size_to_read = std::min(size, static_cast<size_t>(pos));

    std::fseek(fp, 0, SEEK_SET);
    std::fread(data, 1, size_to_read, fp);
    std::fclose(fp);
    return size_to_read;
}

inline bool write_file(const std::filesystem::path& p, const std::vector<uint8_t>& data) noexcept(true) {
    FILE* fp = std::fopen(p.c_str(), "wb");
    if (!fp) return false;
    const size_t written = std::fwrite(data.data(), 1, data.size(), fp);
    std::fclose(fp);
    return written == data.size();
}

}