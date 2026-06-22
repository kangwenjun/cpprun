## `noexcept` 本质是个布尔值

| 写法 | 等价于 | 含义 |
|------|--------|------|
| `noexcept` | `noexcept(true)` | 我保证不抛异常 |
| `noexcept(false)` | 不写 `noexcept` | 我可能抛异常 |

---

## 为什么原代码的 `noexcept` 是错的

```cpp
inline std::vector<uint8_t> read_file(...) noexcept {  // ← 承诺不抛
    std::vector<uint8_t> result(size);  // ← 分配失败会抛 bad_alloc
}
```

你承诺了 `noexcept`，但 `vector` 分配内存失败时会抛 `std::bad_alloc`。

**后果：异常抛出 → 程序直接 `std::terminate` 崩掉**，比不标 `noexcept` 还惨。

---

## 三种改法对比

| 写法 | 含义 | catch 行为 | 推荐度 |
|------|------|-----------|--------|
| ~~`noexcept`~~ | 虚假承诺,会崩 | 异常抛出 → `std::terminate` 直接终止,无法 catch | ❌ |
| `noexcept(false)` | 显式说"可能抛" | 正常传播,可以被外层 try-catch 捕获 | ✅ 诚实,方便搜索 |
| 不写(默认) | 默认就是可能抛 | 正常传播,可以被外层 try-catch 捕获 | ✅ 最简洁 |

`noexcept(false)` 的唯一价值是**显式文档化**——代码审查时一眼看出"这人知道这里可能抛"。实际效果和不写完全一样。