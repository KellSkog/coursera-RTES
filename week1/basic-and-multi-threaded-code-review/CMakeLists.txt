#https://www.coursera.org/learn/real-time-embedded-systems-concepts-practices/peer/Pm6ME/basic-and-multi-threaded-code-review
cmake_minimum_required(VERSION 3.0)

project(theProject)

set(TARGET assignment1)
add_executable(${TARGET}
                ${TARGET}/pthread.c)

target_compile_options(${TARGET} PUBLIC -O0 -g)
target_link_libraries(${TARGET} -pthread)

set(TARGET assignment2)
add_executable(${TARGET}
                ${TARGET}/pthread.c)

target_compile_options(${TARGET} PUBLIC -O0 -g)
target_link_libraries(${TARGET} -pthread)