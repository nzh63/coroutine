cmake_minimum_required(VERSION 3.0.0)
project(coroutine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)


set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/coroutine.cpp
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux" AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    ENABLE_LANGUAGE(ASM)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/coroutine-linux-x64.S
    )
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux" AND CMAKE_SYSTEM_PROCESSOR MATCHES "riscv")
    ENABLE_LANGUAGE(ASM)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/coroutine-linux-riscv.S
    )
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    enable_language(ASM_MASM)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_SOURCE_DIR}/src/coroutine-windows-x64.asm
    )
ENDIF()

include_directories(include)
add_executable(coroutine ${SOURCES})
