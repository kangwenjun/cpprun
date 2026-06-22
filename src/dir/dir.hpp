// Copyright(c) 2026-present, 康文君 & cpprun contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <system_error>

namespace cpprun::dir {


inline bool exists(const std::filesystem::path& p) noexcept {
	std::error_code ec;
	return std::filesystem::exists(p, ec);
}

inline bool is_directory(const std::filesystem::path& p) noexcept {
	std::error_code ec;
	return std::filesystem::is_directory(p, ec);
}

inline bool create(const std::filesystem::path& p, bool recursive = true) noexcept {
	std::error_code ec;
	if (recursive) return std::filesystem::create_directories(p, ec);
	return std::filesystem::create_directory(p, ec);
}

// remove(empty_dir) or remove(a.txt)
inline bool remove(const std::filesystem::path& p) noexcept {
	std::error_code ec;
	return std::filesystem::remove(p, ec);
}

inline bool remove_all(const std::filesystem::path& p) noexcept {
	std::error_code ec;
	std::filesystem::remove_all(p, ec);
	return !ec;
}

// list_files + list_directories
inline std::vector<std::filesystem::path> list(const std::filesystem::path& dir, bool recursive = false) noexcept {
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

inline std::vector<std::filesystem::path> list_files(const std::filesystem::path& dir, bool recursive = false) noexcept {
	std::vector<std::filesystem::path> out;
	for (auto &p : list(dir, recursive)) {
		std::error_code ec;
		if (std::filesystem::is_regular_file(p, ec) && !ec) out.push_back(p);
	}
	return out;
}

inline std::vector<std::filesystem::path> list_directories(const std::filesystem::path& dir, bool recursive = false) noexcept {
	std::vector<std::filesystem::path> out;
	for (auto &p : list(dir, recursive)) {
		std::error_code ec;
		if (std::filesystem::is_directory(p, ec) && !ec) out.push_back(p);
	}
	return out;
}

inline uintmax_t directory_size(const std::filesystem::path& dir) noexcept {
	std::error_code ec;
	if (!std::filesystem::exists(dir, ec) || ec) return 0u;
	uintmax_t total = 0;
	try {
		for (auto &entry : std::filesystem::recursive_directory_iterator(dir)) {
			std::error_code e2;
			if (std::filesystem::is_regular_file(entry.path(), e2) && !e2) {
				auto s = std::filesystem::file_size(entry.path(), e2);
				if (!e2) total += s;
			}
		}
	} catch (...) {
		// ignore
	}
	return total;
}

// copy(dir1, dir2)
inline bool copy(const std::filesystem::path& from, const std::filesystem::path& to, bool overwrite = false) noexcept {
	std::error_code ec;
	if (!std::filesystem::exists(from, ec) || ec) return false;
	// create target root
	if (!std::filesystem::exists(to, ec)) {
		if (!std::filesystem::create_directories(to, ec)) return false;
	}
	try {
		for (auto &entry : std::filesystem::recursive_directory_iterator(from)) {
			const auto rel = std::filesystem::relative(entry.path(), from, ec);
			if (ec) continue;
			const auto dest = to / rel;
			if (entry.is_directory()) {
				std::error_code e2;
				std::filesystem::create_directories(dest, e2);
			} else if (entry.is_regular_file()) {
				std::error_code e2;
				if (overwrite) {
					std::filesystem::copy_file(entry.path(), dest, std::filesystem::copy_options::overwrite_existing, e2);
				} else {
					std::filesystem::copy_file(entry.path(), dest, std::filesystem::copy_options::skip_existing, e2);
				}
			}
		}
	} catch (...) {
		return false;
	}
	return true;
}

/**
 * 将目录从 `from` 移动/重命名为 `to`�?
 * 优先尝试 `std::filesystem::rename`，跨设备时回退为复�?删除�?
 * 成功返回 true�?
 */
inline bool move(const std::filesystem::path& from, const std::filesystem::path& to) noexcept {
	std::error_code ec;
	std::filesystem::rename(from, to, ec);
	if (!ec) return true;
	// fallback: copy then remove
	if (!copy(from, to, true)) return false;
	std::error_code ec2;
	std::filesystem::remove_all(from, ec2);
	return !ec2;
}

} // namespace cpprun::dir


#ifdef __MAIN__
#include <iostream>

int main()
{
    

	std::filesystem::path test_dir = "test_dir";
	std::filesystem::path sub_dir = test_dir / "sub";
	std::filesystem::path file1 = test_dir / "file1.txt";
	std::filesystem::path file2 = sub_dir / "file2.txt";

	// Clean up before test
	cpprun::dir::remove_all(test_dir);

	// Test create
	if (!cpprun::dir::create(sub_dir)) {
		std::cerr << "Failed to create directory: " << sub_dir << std::endl;
		return __LINE__;
	}

	// Test exists and is_directory
	if (!cpprun::dir::exists(sub_dir) || !cpprun::dir::is_directory(sub_dir)) {
		std::cerr << "Directory does not exist or is not a directory: " << sub_dir << std::endl;
		return __LINE__;
	}

	// Create files
	std::ofstream(file1) << "Hello, World!";
	std::ofstream(file2) << "Hello, Subdirectory!";

	// Test list_files and list_directories
	auto files = cpprun::dir::list_files(test_dir);
	auto dirs = cpprun::dir::list_directories(test_dir);
	
	std::cout << "Files in " << test_dir << ":\n";
	for (const auto& f : files) {
		std::cout << "  " << f.filename() << "\n";
	}
	
	std::cout << "Directories in " << test_dir << ":\n";
	for (const auto& d : dirs) {
		std::cout << "  " << d.filename() << "\n";
	}

	// Test directory_size
	auto size = cpprun::dir::directory_size(test_dir);
	std::cout << "Total size of files in " << test_dir << ": " << size << " bytes\n";

	// Test copy
	if (!cpprun::dir::copy(test_dir, "test_dir_copy")) {
		std::cerr << "Failed to copy directory\n";
		return __LINE__;
	}

	// Test move
	if (!cpprun::dir::move("test_dir_copy", "test_dir_moved")) {
		std::cerr << "Failed to move directory\n";
		return __LINE__;
	}

	// Clean up after test
	cpprun::dir::remove_all(test_dir);
	cpprun::dir::remove_all("test_dir_moved");

	return 0;
}
#endif
