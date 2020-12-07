cmake_minimum_required(VERSION 3.0)

project(theProject)

set(TARGET assignment3)
add_executable(${TARGET}
                ${TARGET}/pthread.c)

target_compile_options(${TARGET} PUBLIC -O0 -g)
target_link_libraries(${TARGET} -pthread)

