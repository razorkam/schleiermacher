cmake_minimum_required(VERSION 3.6)
project(schleiermacher)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -Wl,--no-undefined")
set (schleiermacher_VERSION_MAJOR 2)
set (schleiermacher_VERSION_MINOR 0)


find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(OLD_SOURCES old/main2.cpp)
set(SOURCES Schleiermacher.cpp main.cpp)
set(HEADERS Schleiermacher.h Tokenizer.h)

add_executable(schleiermacher_old ${OLD_SOURCES})
add_executable(schleiermacher ${SOURCES} ${HEADERS})