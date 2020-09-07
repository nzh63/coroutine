cmake_minimum_required(VERSION 3.0.0)
project(coroutine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

option(CO_BUILD_TEST "Build test for coroutine" ON)
option(CO_BUILD_EXAMPLE "Build example for coroutine" ON)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/Routine.cpp
    ${CMAKE_SOURCE_DIR}/src/Runtime.cpp
    ${CMAKE_SOURCE_DIR}/src/Stack.cpp
    ${CMAKE_SOURCE_DIR}/src/StackPool.cpp
)
if (CMAKE_SYSTEM_NAME MATCHES "Windows" AND MSVC AND ((CMAKE_GENERATOR_PLATFORM MATCHES "x64") OR (NOT CMAKE_GENERATOR_PLATFORM AND CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")))
    add_definitions(-DARCH_x64)
    enable_language(ASM_MASM)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/arch/x64-windows.asm
    )
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows" AND MSVC AND ((CMAKE_GENERATOR_PLATFORM MATCHES "win32") OR (NOT CMAKE_GENERATOR_PLATFORM AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86")))
    add_definitions(-DARCH_x86)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
    enable_language(ASM_MASM)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/arch/x86-windows.asm
    )
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows" AND NOT MSVC AND CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
    add_definitions(-DARCH_x64)
    enable_language(ASM)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/arch/x64-windows.S
    )
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    add_definitions(-DARCH_x64)
    ENABLE_LANGUAGE(ASM)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/arch/x64.S
    )
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "i[3-7]86")
    add_definitions(-DARCH_x86)
    ENABLE_LANGUAGE(ASM)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/arch/x86.S
    )
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "riscv64")
    add_definitions(-DARCH_RISCV)
    ENABLE_LANGUAGE(ASM)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/arch/riscv64.S
    )
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    add_definitions(-DARCH_AARCH64)
    ENABLE_LANGUAGE(ASM)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/arch/aarch64.S
    )
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "mips64")
    add_definitions(-DARCH_MIPS64)
    ENABLE_LANGUAGE(ASM)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/arch/mips64.S
    )
else()
    message(FATAL_ERROR "Unsupporte platform: " ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR})
endif()

include_directories(include)
add_library(coroutine STATIC
    ${SOURCES}
)

if (CO_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if (CO_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
