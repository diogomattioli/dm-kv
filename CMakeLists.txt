cmake_minimum_required(VERSION 3.16)
project(dm-kv C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -Werror")
set(CMAKE_C_FLAGS_DEBUG "-g3")
set(CMAKE_C_FLAGS_RELEASE "-O3")
link_libraries(pthread)

include_directories(src)

file(GLOB SOURCES "src/*.c")
add_executable(dm-kv ${SOURCES})


enable_testing()
file(GLOB TESTS "tests/test_*.c")
foreach(TEST ${TESTS})
    get_filename_component(TESTNAME ${TEST} NAME_WLE)
    get_filename_component(TESTFILE ${TEST} NAME)

    file(GLOB SOURCES "src/*.c")
    string(SUBSTRING ${TESTFILE} 5 -1 SRC_FILE)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/${SRC_FILE})
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)

    add_executable(${TESTNAME} tests/${TESTFILE} ${SOURCES})

    file(READ ${TEST} TEST_CONTENTS)
    STRING(REGEX REPLACE "\n" ";" TEST_CONTENTS "${TEST_CONTENTS}")

    set(COUNTER 0)
    foreach(TEST_LINE ${TEST_CONTENTS})
        string(REGEX MATCH "TEST[(]([A-Za-z0-9_]+)[)]" TEST_CASES ${TEST_LINE})
        if(${CMAKE_MATCH_COUNT} EQUAL 1)
            add_test(NAME ${TESTNAME}_${CMAKE_MATCH_1} COMMAND ${TESTNAME} ${COUNTER})
            MATH(EXPR COUNTER "${COUNTER}+1")
        endif()
    endforeach()

endforeach()


execute_process(WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND git rev-parse --abbrev-ref HEAD OUTPUT_VARIABLE GIT_BRANCH)
execute_process(WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND git log --pretty=format:'%h' -n 1 OUTPUT_VARIABLE GIT_REVISION ERROR_QUIET)
execute_process(WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND git describe --exact-match --tags OUTPUT_VARIABLE GIT_TAG ERROR_QUIET)

string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
string(STRIP "${GIT_REVISION}" GIT_REVISION)
string(STRIP "${GIT_TAG}" GIT_TAG)

add_definitions(-DGIT_BRANCH="${GIT_BRANCH}")
add_definitions(-DGIT_REVISION="${GIT_REVISION}")
add_definitions(-DGIT_BRANCH="${GIT_BRANCH}")