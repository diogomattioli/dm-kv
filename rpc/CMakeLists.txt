project(rpc CXX)


file(GLOB protos "*.proto")
foreach(proto ${protos})
    get_filename_component(proto_name ${proto} NAME_WLE)
    get_filename_component(proto_path ${proto} PATH)

    set(proto_src    "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc")
    set(proto_hdr    "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h")
    set(grpc_src     "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.cc")
    set(grpc_hdr     "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.h")

    add_custom_command(
            OUTPUT "${proto_src}" "${proto_hdr}" "${grpc_src}" "${grpc_hdr}"
            COMMAND protoc
            ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}" --cpp_out "${CMAKE_CURRENT_BINARY_DIR}" -I "${proto_path}" --plugin=protoc-gen-grpc=/usr/bin/grpc_cpp_plugin "${proto}"
            DEPENDS "${proto}")

    list(APPEND proto_srcs ${proto_src})
    list(APPEND grpc_srcs ${grpc_src})
endforeach()


include_directories("../src")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB srcs "*.cpp")
add_library(rpc ${srcs} ${grpc_srcs} ${proto_srcs})
target_link_libraries(rpc stdc++ grpc++ protobuf)