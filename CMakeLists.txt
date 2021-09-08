cmake_minimum_required(VERSION 3.16)
#set(CMAKE_C_COMPILER /usr/bin/gcc-10)
project(PassMann C)
project(PassMann VERSION 1.0.1)

add_subdirectory(src)
set(PROJECT_VERSION_MAJOR 1 CACHE STRING "major version" FORCE)
set(PROJECT_VERSION_MINOR 0 CACHE STRING "minor version" FORCE)
set(PROJECT_VERSION_PATCH 1 CACHE STRING "patch version" FORCE)

set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE Debug)

LIST(APPEND included_headers 
    "stdio.h" "stdarg.h" "stdlib.h" "time.h" "string.h" "errno.h" "fcntl.h" "unistd.h" "termios.h" "dirent.h" 
    "malloc.h" "sys/stat.h" "stdbool.h" "stdint.h" "limits.h")

foreach(header ${included_headers}) 
   find_path( header PATHS /usr/include)
    if(NOT header)
        message(FATAL_ERROR "Can't find folder containing ${header}")
    else()
        message("-- ${header} -- found")
    endif()
endforeach()

install(TARGETS PassMann
        DESTINATION /usr/bin)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Filip Ivanusec")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PassMann")
set(CPACK_PACKAGE_VENDOR "Filip Ivanusec")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_CONTACT "Filip Ivanusec")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsodium-dev (>= 1.0.18)")
include(CPack)


