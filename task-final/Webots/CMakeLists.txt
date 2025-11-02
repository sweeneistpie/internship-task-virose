cmake_minimum_required(VERSION 3.15)

# Ambil nama folder sebagai nama project
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})

# Pastikan Webots terdeteksi
if(NOT DEFINED ENV{WEBOTS_HOME})
  message(FATAL_ERROR "WEBOTS_HOME environment variable not set. Please set it to your Webots installation directory.")
endif()

set(WEBOTS_HOME "$ENV{WEBOTS_HOME}")
string(REPLACE "\\" "/" WEBOTS_HOME "${WEBOTS_HOME}")

# === Sumber kode utama ===
set(SOURCES
    controller-fp.cpp
)

# === Tambahkan path library Serial ===
include_directories(
    ${WEBOTS_HOME}/include/controller/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serial-main/include
    ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include
)

# === Tambahkan path library Serial (hasil build) ===
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/serial-main/build/Release)

# === Library Webots ===
set(LIBRARIES
    ${WEBOTS_HOME}/lib/controller/libCppController.a
)

# === Build executable controller ===
add_executable(${PROJECT} ${SOURCES})
target_compile_definitions(${PROJECT} PRIVATE WEBOTS_USED_BY_CONTROLLER)

# === Link ke library serial ===
target_link_libraries(${PROJECT} PRIVATE ${LIBRARIES} serial)

# === Setelah build, salin executable ke folder controller ===
add_custom_command(TARGET ${PROJECT} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
  copy $<TARGET_FILE:${PROJECT}> ${CMAKE_CURRENT_SOURCE_DIR}
)
