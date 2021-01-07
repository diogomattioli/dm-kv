enable_testing()

file(GLOB srcs "../src/*.c")
list(REMOVE_ITEM srcs ${CMAKE_CURRENT_SOURCE_DIR}/../src/main.c)

file(GLOB tests "test_*.c")
foreach(test ${tests})
    get_filename_component(test_name ${test} NAME_WLE)
    get_filename_component(test_file ${test} NAME)

    file(READ ${test} test_contents)
    string(REGEX REPLACE "\n" ";" test_contents ${test_contents})

    set(counter 0)
    foreach(test_line ${test_contents})
        string(REGEX MATCH "TEST[(]([A-Za-z0-9_]+)[)]" test_cases ${test_line})
        if(${CMAKE_MATCH_COUNT} EQUAL 1)
            add_test(NAME ${test_name}_${CMAKE_MATCH_1} COMMAND ${test_name} ${counter})
            math(EXPR counter "${counter}+1")
        endif()
    endforeach()

    string(SUBSTRING ${test_file} 5 -1 src_file)
    set(src_file "${CMAKE_CURRENT_SOURCE_DIR}/../src/${src_file}")

    list(REMOVE_ITEM srcs ${src_file})
    add_executable(${test_name} ${test_file} ${srcs})
    list(APPEND srcs ${src_file})
endforeach()
