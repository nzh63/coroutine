# coroutine

C++实现的协程库

## 经过测试的平台

| | x86 | x64 | AArch64 | MIPS64 | RISC-V64 |
| - | - | - | - | - | - |
| Linux | [![Build Status](https://dev.azure.com/nzh21/coroutine/_apis/build/status/nzh63.coroutine?branchName=master&jobName=Build_And_Test_QEMU&configuration=Build_And_Test_QEMU%20x86_Linux)](https://dev.azure.com/nzh21/coroutine/_build/latest?definitionId=6&branchName=master) | [![Build Status](https://dev.azure.com/nzh21/coroutine/_apis/build/status/nzh63.coroutine?branchName=master&jobName=Build_And_Test&configuration=Build_And_Test%20x64_Linux)](https://dev.azure.com/nzh21/coroutine/_build/latest?definitionId=6&branchName=master) | [![Build Status](https://dev.azure.com/nzh21/coroutine/_apis/build/status/nzh63.coroutine?branchName=master&jobName=Build_And_Test_QEMU&configuration=Build_And_Test_QEMU%20AArch64_Linux)](https://dev.azure.com/nzh21/coroutine/_build/latest?definitionId=6&branchName=master) | [![Build Status](https://dev.azure.com/nzh21/coroutine/_apis/build/status/nzh63.coroutine?branchName=master&jobName=Build_And_Test_QEMU&configuration=Build_And_Test_QEMU%20MIPS64_Linux)](https://dev.azure.com/nzh21/coroutine/_build/latest?definitionId=6&branchName=master) | [![Build Status](https://dev.azure.com/nzh21/coroutine/_apis/build/status/nzh63.coroutine?branchName=master&jobName=Build_And_Test_QEMU&configuration=Build_And_Test_QEMU%20RISCV64_Linux)](https://dev.azure.com/nzh21/coroutine/_build/latest?definitionId=6&branchName=master) |
| Windows | [![Build Status](https://dev.azure.com/nzh21/coroutine/_apis/build/status/nzh63.coroutine?branchName=master&jobName=Build_And_Test&configuration=Build_And_Test%20x86_Windows)](https://dev.azure.com/nzh21/coroutine/_build/latest?definitionId=6&branchName=master) | [![Build Status](https://dev.azure.com/nzh21/coroutine/_apis/build/status/nzh63.coroutine?branchName=master&jobName=Build_And_Test&configuration=Build_And_Test%20x64_Windows)](https://dev.azure.com/nzh21/coroutine/_build/latest?definitionId=6&branchName=master) | - | - | - |
| MacOS | - | [![Build Status](https://dev.azure.com/nzh21/coroutine/_apis/build/status/nzh63.coroutine?branchName=master&jobName=Build_And_Test&configuration=Build_And_Test%20x64_MacOS)](https://dev.azure.com/nzh21/coroutine/_build/latest?definitionId=6&branchName=master) | - | - | - |

## 使用方法

### yield 模式

yield 时会将控制权转移给其他协程，并在重新获得控制权后返回、从 yield 之后的语句继续执行。

```cpp
auto* runtime = co::Runtime::instance();
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

### Promise/await 模式

如果想要让出控制权并等待某个数据，那么类似于 ECMAScript 的 Promise 模式可能更适合。并且你还可以使用 await 来如同同步调用一样来使用异步 API。

```cpp
auto async_task = co::Promise<int>([](auto resolver) {
        // do something
        resolver->resolve(42);
    })
    .then([](int res /* res = 42 */) { return res * 2; })
    .then([](int res /* res = 84 */) { return std::to_string(res); });
std::string final_result /* finally_result = "84" */ = await async_task;
```

在 [example/tcp-echo.cpp](./example/tcp-echo.cpp) 中，展示了如何创建并使用一个基于 Promise 的异步 API。


### 引入作为依赖

若要将此库作为依赖，可在CmakeLists.txt中添加以下内容：
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


## License
Apache-2.0
