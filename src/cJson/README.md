# cJSON - 1.7.19

LICENSE: MIT
URL: https://github.com/DaveGamble/cJSON

## `cJSON_Utils.h`

`cJSON_Utils.h` 包含一些常用的辅助函数（并非完整列表，视项目启用的扩展而定）：

- **目的**：提供在解析/生成 JSON 时的便捷工具，例如比较、合并、从 JSON 中提取默认值或转换类型等。
- **常见函数（示例）**：`cJSONUtils_Compare()`、`cJSONUtils_Merge()`、`cJSONUtils_GetString()`。
- **用法说明**：这些工具通常用于简化对 `cJSON` 树的操作，例如深度比较两个节点、将默认配置合并到用户配置中或安全地读取可选字段。

注意：具体可用的工具函数取决于仓库中是否包含 `cJSON_Utils` 的实现文件，使用前请检查头文件或源码。

## `cJSON.h`

以下为 `cJSON.h` 常用接口的简明说明（选取常用函数与宏，非详尽列表）：

- **类型**：
	- `cJSON`：JSON 节点的核心结构，表示对象、数组、字符串、数字、布尔、Null 等。

- **解析与序列化**：
	- `cJSON_Parse(const char *value)`：解析 JSON 字符串，返回 `cJSON*`（成功）或 `NULL`（失败）。
	- `cJSON_ParseWithOpts(const char *value, const char **return_parse_end, int require_null_terminated)`：更灵活的解析接口。
	- `cJSON_Print(const cJSON *item)`：以可读格式（含换行/缩进）返回字符串（调用方需 `free()`）。
	- `cJSON_PrintUnformatted(const cJSON *item)`：紧凑格式输出。

- **节点创建与删除**：
	- `cJSON_CreateObject()` / `cJSON_CreateArray()` / `cJSON_CreateString(const char *s)` / `cJSON_CreateNumber(double n)` / `cJSON_CreateBool(int b)` / `cJSON_CreateNull()`：创建对应类型的节点。
	- `cJSON_Delete(cJSON *item)`：释放整个 JSON 树。

- **节点操作（添加/移除/替换）**：
	- `cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item)`：将项加入对象（以字符串为键）。
	- `cJSON_AddItemToArray(cJSON *array, cJSON *item)`：将项加入数组。
	- `cJSON_DetachItemFromObject(cJSON *object,const char *string)` / `cJSON_DetachItemFromArray(cJSON *array,int which)`：从父节点中分离但不释放。
	- `cJSON_ReplaceItemInObject()` / `cJSON_ReplaceItemInArray()`：替换已有项。

- **查询与工具**：
	- `cJSON_GetObjectItem(const cJSON *object, const char *string)`：按键获取对象子项（不区分大小写的版本也常见）。
	- `cJSON_GetArraySize(const cJSON *array)`：返回数组长度。
	- `cJSON_GetArrayItem(const cJSON *array, int index)`：按索引获取数组元素。
	- `cJSON_IsXXX(...)` 宏：用于判断节点类型（如 `cJSON_IsNumber(item)`）。
	- `cJSON_GetErrorPtr()`：解析失败时返回错误位置指针（如果可用）。

- **错误处理与内存**：
	- 大多数创建/解析函数在失败时返回 `NULL`，调用方应检查返回值并在不再需要时调用 `cJSON_Delete()` 来释放内存。
	- `cJSON_Print` 等函数返回的字符串需由调用方 `free()`。

示例：解析、读取字段、修改并序列化

```c
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
		const char *json = "{\"name\":\"alice\",\"age\":30}";
		cJSON *root = cJSON_Parse(json);
		if (!root) {
				const char *err = cJSON_GetErrorPtr();
				fprintf(stderr, "Parse error at: %s\n", err ? err : "unknown");
				return 1;
		}

		cJSON *name = cJSON_GetObjectItem(root, "name");
		if (cJSON_IsString(name)) {
				printf("name=%s\n", name->valuestring);
		}

		cJSON *age = cJSON_GetObjectItem(root, "age");
		if (cJSON_IsNumber(age)) {
				age->valuedouble = age->valueint = age->valueint + 1; // 年龄加 1
		}

		char *out = cJSON_Print(root);
		if (out) {
				puts(out);
				free(out);
		}

		cJSON_Delete(root);
		return 0;
}
```

常见注意事项：

- 在多线程环境中，`cJSON` 本身不做线程同步；并发访问同一颗树时需外部同步。
- 字符串输出需要 `free()`；解析失败时需检查 `cJSON_GetErrorPtr()` 获取位置提示。
- 若需要更复杂的操作（深拷贝、差异合并、路径查询），请查看或启用 `cJSON_Utils` 扩展函数或上游仓库文档。

更多细节、完整函数表和示例请参见上游文档： https://github.com/DaveGamble/cJSON