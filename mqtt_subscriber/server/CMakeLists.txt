project (server)
#cmake_minimum_required(VERSION 3.9)
# Tools for using NK parser.
include (platform/nk)

#nk_build_edl_files (client_edl_files NK_MODULE "client" EDL "${CMAKE_SOURCE_DIR}/resources/edl/Client.edl")


# Set compile flags.
project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")
#find_package(Protobuf REQUIRED)
#find_package(OpenCV REQUIRED)
#find_package(gpio REQUIRED)
#include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable (Server "src/server.cpp")
add_dependencies (Server mqtt_subscriber_server_edl_files)
#target_link_libraries(Server ${OpenCV_LIBS})
#set_target_properties (Server PROPERTIES LINK_FLAGS "-no-pie -Ttext 0x00c00000")
target_link_libraries (Server ${gpio_CLIENT_LIB})
#target_link_libraries (Server ${gpio_LIBS})
if ("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "arm|aarch64")
    target_link_libraries (Server ${bsp_CLIENT_LIB})
endif ()

set_target_properties (Server PROPERTIES LINK_FLAGS ${VULN_LDFLAGS})