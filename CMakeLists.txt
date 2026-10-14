cmake_minimum_required(VERSION 3.16)
project(shell_so C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

add_executable(p3
    p3.c
    shell.c
    command.c
    command2.c
    command3.c
    list.c
    listNodes.c
    dir_proc_list.c
    open.c
)

target_compile_options(p3 PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(p3 PRIVATE _POSIX_C_SOURCE=200809L)
target_link_libraries(p3 PRIVATE ${CMAKE_DL_LIBS})


