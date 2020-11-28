cmake_minimum_required(VERSION 3.7)

project(theProject)

set(TARGET rt-simple)
add_executable(${TARGET}
                ${TARGET}/rt_simplethread.c)

target_compile_options(${TARGET} PUBLIC -O0 -g)
target_link_libraries(${TARGET} -pthread)

set(TARGET simple)
add_executable(${TARGET}
                ${TARGET}/pthread.c)
target_compile_options(${TARGET} PUBLIC -O0 -g)
target_link_libraries(${TARGET} -pthread)

set(TARGET twoproc)
add_executable(${TARGET}
                ${TARGET}/twoprocs.c)
target_compile_options(${TARGET} PUBLIC -O0 -g)
target_link_libraries(${TARGET} -pthread)

set(TARGET simple-affinity)
add_executable(${TARGET}
                ${TARGET}/pthread.c)
target_compile_options(${TARGET} PUBLIC -O0 -g)
target_link_libraries(${TARGET} -pthread)