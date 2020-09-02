# coroutine

C++实现的协程库

## 经过测试的平台

|         | x64 | AArch64 | MIPS64 | RISC-V64 |
| ------- | --- | ------- | ------ | -------- |
| Linux   | ✔   | ✔       | ✔      | ✔        |
| Windows | ✔   | -       | -      | -        |
| MacOS   | ✔   | -       | -      | -        |

## 使用方法

```cpp
auto* runtime = Runtime::instance();
runtime->spawn([runtime]() {
    std::cout << "THREAD 1 STARTING" << std::endl;
    int id = 1;
    for (int i = 0; i < 10; i++) {
        std::cout << "thread: " << id << " counter: " << i << std::endl;
        runtime->yield();
    }
    std::cout << "THREAD 1 FINISHED" << std::endl;
});
runtime->spawn([runtime]() {
    std::cout << "THREAD 2 STARTING" << std::endl;
    int id = 2;
    for (int i = 0; i < 15; i++) {
        std::cout << "thread: " << id << " counter: " << i << std::endl;
        runtime->yield();
    }
    std::cout << "THREAD 2 FINISHED" << std::endl;
});
runtime->run();
```

若要将此库作为依赖，可在CmakeLists.txt中添加一下内容：
```cmake
include(FetchContent)
FetchContent_Declare(
    co
    GIT_REPOSITORY https://github.com/nzh63/coroutine
)
FetchContent_MakeAvailable(co)
add_executable(your_executable your_code.cpp)
target_link_libraries(your_code coroutine)
```

更多用法请参照[样例](./example)。

## License
Apache-2.0