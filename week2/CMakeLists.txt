cmake_minimum_required(VERSION 3.0)

project(theProject)

add_library(syslog STATIC 
    syslogger/syslogger.c)
set(TARGET RT-Clock)
add_executable(${TARGET}
                ${TARGET}/posix_clock.c)
target_include_directories(${TARGET} PUBLIC syslogger)
target_compile_options(${TARGET} PUBLIC -O0 -g)
target_link_libraries(${TARGET} syslog -pthread)

set(TARGET simplethread-affinity)
add_executable(${TARGET}
                ${TARGET}/pthread.c)
target_include_directories(${TARGET} PUBLIC syslogger)
target_compile_options(${TARGET} PUBLIC -O0 -g)
target_link_libraries(${TARGET} syslog -pthread)