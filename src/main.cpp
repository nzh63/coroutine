#include <iostream>

#include "coroutine.h"

using namespace CO;

int main() {
    std::cout << "Hello" << std::endl;
    Runtime runtime;
    runtime.spawn([]() {
        std::cout << "THREAD 1 STARTING" << std::endl;
        int id = 1;
        for (int i = 0; i < 10; i++) {
            std::cout <<"thread: " << id << " counter: "<<  i << std::endl;
            g_runtime->yield();
        }
        std::cout << "THREAD 1 FINISHED" << std::endl;
    });
    runtime.spawn([]() {
        std::cout << "THREAD 2 STARTING" << std::endl;
        int id = 2;
        for (int i = 0; i < 15; i++) {
            std::cout << "thread: " << id << " counter: " << i << std::endl;
            g_runtime->yield();
        }
        std::cout << "THREAD 2 FINISHED" << std::endl;
    });
    runtime.run();
    std::cout << "All Done" << std::endl;
}
