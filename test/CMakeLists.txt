find_package(GTest)

if(NOT ${GTEST_FOUND})
    cmake_minimum_required(VERSION 3.14.0)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
    )
    set(INSTALL_GTEST OFF CACHE BOOL "")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    set(GTEST_INCLUDE_DIRS ${googletest_SOURCE_DIR}/googletest/include)
    set(GTEST_LIBRARIES gtest)
    set(GTEST_MAIN_LIBRARIES gtest_main)
endif()

add_executable(co-test
    main.cpp
    PromiseTest.cpp
    RuntimeTest.cpp
)
target_include_directories(co-test PUBLIC ${GTEST_INCLUDE_DIRS})
target_link_libraries(co-test
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    coroutine
)

gtest_discover_tests(co-test)
