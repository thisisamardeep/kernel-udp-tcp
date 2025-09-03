cmake_minimum_required(VERSION 3.28)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(kernel_udp_tcp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "This project only supports Linux.")
endif()
execute_process(
        COMMAND uname -r
        OUTPUT_VARIABLE KERNEL_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX MATCH "^[0-9]+" KERNEL_MAJOR_VERSION "${KERNEL_VERSION}")
if(KERNEL_MAJOR_VERSION LESS 6)
    message(FATAL_ERROR "Linux kernel version 6 or higher is required. Detected: ${KERNEL_VERSION}")
endif()
# i need  gcc since i use gcc specfic macros freely .i dont intend this project to be portable.
#this project is mainly to implement kernel bypass
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "GCC is required to build this project. Detected: ${CMAKE_CXX_COMPILER_ID}")
endif()

# thread.h is not sufficent since i need to set affinity. we need pthread.h. most kernels have it.
find_package(Threads REQUIRED)
if(NOT CMAKE_USE_PTHREADS_INIT)
    message(FATAL_ERROR "POSIX threads (pthreads) are required but not found!")
endif()
# asio needs ssl   uuid
find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL must be found to build this project.")
endif()
find_path(LIBUUID_INCLUDE_DIR uuid/uuid.h)
find_library(LIBUUID_LIBRARY uuid)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibUUID DEFAULT_MSG LIBUUID_LIBRARY LIBUUID_INCLUDE_DIR)



if(LIBUUID_FOUND)
    add_definitions(-DLIBUUID_SUPPORT)
    set(LIBUUID_LIBRARIES ${LIBUUID_LIBRARY})
endif()


add_executable(udp_server_without_kernel_bypass server.cpp)
add_executable(udp_client_without_kernel_bypass client.cpp)
add_subdirectory(external/fmt/fmt-11.2.0)

file(GLOB ASIO_SRC "${CMAKE_SOURCE_DIR}/external/asio/asio/src/*.cpp")
add_library(asio STATIC ${ASIO_SRC})
target_include_directories(asio PUBLIC
        ${CMAKE_SOURCE_DIR}/external/asio/asio/include
        "${CMAKE_CURRENT_SOURCE_DIR}/include/server"
)
target_link_libraries(asio ${OPENSSL_LIBRARIES} uuid)

target_compile_definitions(udp_server_without_kernel_bypass PRIVATE ASIO_STANDALONE)
target_compile_definitions(udp_client_without_kernel_bypass PRIVATE ASIO_STANDALONE)
file(GLOB_RECURSE LIB_HEADER_FILES "include/*.h" "source/*.h")
file(GLOB_RECURSE LIB_SOURCE_FILES "include/*.cpp" "source/*.cpp")

add_library(udpserver ${LIB_HEADER_FILES}  ${LIB_SOURCE_FILES})
target_include_directories(udpserver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/server")
target_link_libraries(udpserver asio fmt::fmt ${LIBUUID_LIBRARIES})
target_link_libraries(udp_server_without_kernel_bypass PUBLIC udpserver asio ${LIBUUID_LIBRARIES})
target_include_directories(udp_server_without_kernel_bypass PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/server")

target_link_libraries(udp_client_without_kernel_bypass PUBLIC udpserver asio ${LIBUUID_LIBRARIES})
target_include_directories(udp_client_without_kernel_bypass PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/server")

