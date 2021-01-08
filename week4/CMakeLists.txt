cmake_minimum_required(VERSION 3.0)
project(ProjectName)

set(TARGET simplethread-affinity)
add_executable(${TARGET}
                ${TARGET}/pthread.c)
target_compile_options(${TARGET} PUBLIC -O0 -g)
target_link_libraries(${TARGET} pthread)

set(TARGET sharpen)
add_executable(${TARGET}
                sharpen-psf-rpi-demo/sharpen.c)
target_compile_options(${TARGET} PUBLIC -O3 -g)
target_link_libraries(${TARGET} pthread)