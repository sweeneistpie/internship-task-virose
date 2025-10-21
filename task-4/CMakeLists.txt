cmake_minimum_required(VERSION 3.10)
project(FileTransfer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(serial_include_dir "${CMAKE_CURRENT_SOURCE_DIR}/serial/include")
set(serial_lib_dir "${CMAKE_CURRENT_SOURCE_DIR}/serial/build")

include_directories(${serial_include_dir})
link_directories(${serial_lib_dir})

add_executable(FileTransfer main.cpp)

target_link_libraries(FileTransfer serial)