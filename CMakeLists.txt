cmake_minimum_required(VERSION 3.16)
project(dm-kv C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

file(GLOB SOURCES "src/*.c")
add_executable(dm-kv ${SOURCES})

set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -Werror")
set(CMAKE_C_FLAGS_DEBUG "-g3")
set(CMAKE_C_FLAGS_RELEASE "-O3")

target_link_libraries(dm-kv pthread)


execute_process(WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND git rev-parse --abbrev-ref HEAD OUTPUT_VARIABLE GIT_BRANCH)
execute_process(WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND git log --pretty=format:'%h' -n 1 OUTPUT_VARIABLE GIT_REVISION ERROR_QUIET)
execute_process(WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND git describe --exact-match --tags OUTPUT_VARIABLE GIT_TAG ERROR_QUIET)

string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
string(STRIP "${GIT_REVISION}" GIT_REVISION)
string(STRIP "${GIT_TAG}" GIT_TAG)

add_definitions(-DGIT_BRANCH="${GIT_BRANCH}")
add_definitions(-DGIT_REVISION="${GIT_REVISION}")
add_definitions(-DGIT_BRANCH="${GIT_BRANCH}")