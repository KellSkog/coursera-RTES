cmake_minimum_required(VERSION 3.0)

project(week3)

set(TARGET assignment1)
add_executable(${TARGET} ${TARGET}/seqgen3.c)
target_compile_options(${TARGET} PUBLIC -O0 -g)
target_link_libraries(${TARGET} PUBLIC -pthread -lrt)

set(TARGET test1)
add_executable(${TARGET} ${TARGET}/main.c)
target_compile_options(${TARGET} PUBLIC -O0 -g)
target_link_libraries(${TARGET} PUBLIC -pthread -lrt)