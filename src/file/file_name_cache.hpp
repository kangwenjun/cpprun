#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <shared_mutex>

namespace cpprun::file {

class file_name_cache
{
    using mutex_type = std::shared_mutex;
    using read_lock_type = std::shared_lock<mutex_type>;
    using write_lock_type = std::unique_lock<mutex_type>;
public:
    file_name_cache() = default;
    ~file_name_cache() = default;

    const char* get(const char* file_path)
    {
        {
            read_lock_type lock(mtx);
            auto it = cache.find(file_path);
            if (it != cache.end())
                return it->second.c_str();
        }

        write_lock_type lock(mtx);
        auto it = cache.find(file_path);
        if (it != cache.end())
            return it->second.c_str();

        const auto& [emplaced_it, _] = cache.try_emplace(file_path, get_file_name(file_path));
        return emplaced_it->second.c_str();
    }

    // /test.exe - > test
    static std::string get_file_name(std::string_view file_path)
    {
        const std::filesystem::path path{std::string(file_path)};
        const std::string stem = path.stem().string();
        if (!stem.empty())
            return stem;

        const std::string filename = path.filename().string();
        if (filename == "." || filename == "..")
            return {};
        return filename;
    }

private:
    mutex_type mtx;
    std::unordered_map<std::string, std::string> cache;
};

} // namespace cpprun::file


#ifdef __MAIN__
#include <iostream>
#include <cassert>

int main()
{
    cpprun::file::file_name_cache cache;
    const char* name = cache.get("/test.exe");
    assert(std::string(name) == "test");
    std::cout << name << std::endl;
    return 0;
}
#endif