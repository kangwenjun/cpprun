#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

int main()
{
    printf("=== cJSON Test Demo ===\n\n");

    /* 1. 创建一个对象并添加不同类型的数据 */
    printf("1. Creating a JSON object with various types:\n");
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "name", "John Doe");
    cJSON_AddNumberToObject(root, "age", 30);
    cJSON_AddBoolToObject(root, "isStudent", 0);
    cJSON_AddNullToObject(root, "nickname");

    char *formatted = cJSON_Print(root);
    printf("%s\n\n", formatted);
    free(formatted);

    /* 2. 创建一个数组 */
    printf("2. Creating a JSON array:\n");
    cJSON *array = cJSON_CreateArray();
    cJSON_AddItemToArray(array, cJSON_CreateString("apple"));
    cJSON_AddItemToArray(array, cJSON_CreateString("banana"));
    cJSON_AddItemToArray(array, cJSON_CreateString("cherry"));

    formatted = cJSON_Print(array);
    printf("%s\n\n", formatted);
    free(formatted);

    /* 3. 嵌套对象和数组 */
    printf("3. Nested objects and arrays:\n");
    cJSON *person = cJSON_CreateObject();
    cJSON_AddStringToObject(person, "name", "Alice");

    cJSON *hobbies = cJSON_CreateArray();
    cJSON_AddItemToArray(hobbies, cJSON_CreateString("reading"));
    cJSON_AddItemToArray(hobbies, cJSON_CreateString("swimming"));
    cJSON_AddItemToArray(hobbies, cJSON_CreateString("coding"));
    cJSON_AddItemToObject(person, "hobbies", hobbies);

    cJSON *address = cJSON_CreateObject();
    cJSON_AddStringToObject(address, "city", "Beijing");
    cJSON_AddStringToObject(address, "country", "China");
    cJSON_AddItemToObject(person, "address", address);

    formatted = cJSON_Print(person);
    printf("%s\n\n", formatted);
    free(formatted);

    /* 4. 解析 JSON 字符串 */
    printf("4. Parsing JSON string:\n");
    const char *json_str = "{\"name\":\"Bob\",\"age\":25,\"scores\":[85,90,78]}";
    cJSON *parsed = cJSON_Parse(json_str);
    if (parsed != NULL) {
        cJSON *name = cJSON_GetObjectItem(parsed, "name");
        cJSON *age = cJSON_GetObjectItem(parsed, "age");
        cJSON *scores = cJSON_GetObjectItem(parsed, "scores");

        if (cJSON_IsString(name)) {
            printf("Name: %s\n", name->valuestring);
        }
        if (cJSON_IsNumber(age)) {
            printf("Age: %d\n", age->valueint);
        }
        if (cJSON_IsArray(scores)) {
            printf("Scores: ");
            int size = cJSON_GetArraySize(scores);
            for (int i = 0; i < size; i++) {
                cJSON *score = cJSON_GetArrayItem(scores, i);
                if (cJSON_IsNumber(score)) {
                    printf("%.0f ", score->valuedouble);
                }
            }
            printf("\n");
        }
        cJSON_Delete(parsed);
    }
    printf("\n");

    /* 5. 修改 JSON 对象 */
    printf("5. Modifying JSON object:\n");
    cJSON *modify_test = cJSON_Parse("{\"count\":10,\"name\":\"test\"}");
    printf("Before: ");
    formatted = cJSON_Print(modify_test);
    printf("%s\n", formatted);
    free(formatted);

    cJSON *count = cJSON_GetObjectItem(modify_test, "count");
    if (count != NULL) {
        cJSON_SetNumberValue(count, 100);
    }

    printf("After:  ");
    formatted = cJSON_Print(modify_test);
    printf("%s\n\n", formatted);
    free(formatted);
    cJSON_Delete(modify_test);

    /* 6. 使用 cJSON_ArrayForEach 遍历数组 */
    printf("6. Iterating array with cJSON_ArrayForEach:\n");
    cJSON *numbers = cJSON_Parse("[1, 2, 3, 4, 5]");
    cJSON *item = NULL;
    printf("Array items: ");
    cJSON_ArrayForEach(item, numbers) {
        if (cJSON_IsNumber(item)) {
            printf("%.0f ", item->valuedouble);
        }
    }
    printf("\n\n");
    cJSON_Delete(numbers);

    /* 7. 打印无格式 JSON */
    printf("7. Unformatted JSON output:\n");
    char *unformatted = cJSON_PrintUnformatted(person);
    printf("%s\n\n", unformatted);
    free(unformatted);

    /* 清理内存 */
    cJSON_Delete(root);
    cJSON_Delete(array);
    cJSON_Delete(person);

    printf("=== Test completed successfully! ===\n");
    return 0;
}
