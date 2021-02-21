cmake_minimum_required(VERSION 3.17)
project(PassMann C)

set(PROJECT_VERSION_MAJOR 1 CACHE STRING "major version" FORCE)
set(PROJECT_VERSION_MINOR 0 CACHE STRING "minor version" FORCE)

set(CMAKE_C_STANDARD 99)

add_executable(PassMann src/main.c src/user.c src/session.c src/database.c src/hashing.c src/encryption.c)
target_link_libraries(PassMann PUBLIC m)
target_link_libraries(PassMann PUBLIC ssl)
target_link_libraries(PassMann PUBLIC crypto)
