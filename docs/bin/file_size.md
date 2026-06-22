**用 `std::error_code` 重载，这才是真正的 noexcept**：

```cpp
inline size_t file_size(const std::filesystem::path& p) noexcept(true) {
    std::error_code ec;
    auto sz = std::filesystem::file_size(p, ec);
    if (ec) return 0;
    return static_cast<size_t>(sz);
}
```

---

### 为什么这个才对

| 方式 | 异常规格 | 失败时行为 |
|------|----------|-----------|
| `file_size(p)` | 抛 `filesystem_error` | 异常传播 |
| `file_size(p, ec)` | **noexcept** | 设置 `ec`，不抛 |

`std::filesystem::file_size` 有两个重载：
- `file_size(path)` → 抛异常
- `file_size(path, error_code&)` → **不抛异常**，错误写入 `ec`

---

### 对比你原来的 ifstream 版本

| | ifstream 版 | filesystem + ec 版 |
|--|-------------|-------------------|
| noexcept | ❌ 构造可能抛 `bad_alloc` | ✅ 真正不抛 |
| 性能 | 打开文件 + seek | 只查元数据，**快得多** |
| 正确性 | 大文件可能溢出 `tellg()` | `uintmax_t`，转 `size_t` 有截断风险但可控 |

**截断风险提醒**：`file_size` 返回 `uintmax_t`，转 `size_t` 在 32 位平台上 >4GB 的文件会被截断。如果需要精确处理大文件，返回 `std::uintmax_t` 或 `std::optional<std::uintmax_t>`。