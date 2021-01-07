include_directories("../rpc")

file(GLOB srcs "*.c")
add_executable(dm-kv ${srcs})
target_link_libraries(dm-kv rpc pthread)