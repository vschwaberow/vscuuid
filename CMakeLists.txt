cmake_minimum_required(VERSION 3.16)
project(vscuuid)

set(CMAKE_CXX_STANDARD 26)

find_package(OpenSSL REQUIRED)

include_directories(include ${OPENSSL_INCLUDE_DIR})

file(GLOB_RECURSE SOURCES "src/*.cc")

add_library(vscuuid ${SOURCES})
target_link_libraries(vscuuid OpenSSL::SSL OpenSSL::Crypto)

add_executable(test_vscuuid test/test_main.cc)
target_link_libraries(test_vscuuid vscuuid)

add_executable(basic_example examples/basic_example.cc)
target_link_libraries(basic_example vscuuid)