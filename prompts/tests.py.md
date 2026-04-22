1. 补充和完善 scripts/tests.py 要能够运行一系列的测试，比如：python cpprun.py -s ../src/timestamp.hpp
2. 不要输入参数，直接写死要执行的指令 `python cpprun.py -s ../src/timestamp.hpp`
3. 完善 scripts\tests.py 下述代码
```
file_list = [
        p for d in dir_list
        for p in (repo_root / d).glob("**/*")
        if p.is_file() and p.suffix in postfix_list
    ]
```
含文件 cpprun_test.cpp 或 含 cpprun_test.c 的目录，如：src\spdlog，此目录作为 file_list 的一个子项，方便后续以目录进行测试。