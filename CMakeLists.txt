cmake_minimum_required(VERSION 3.5)

project(FirefoxBookmarks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("lz4/build/cmake")

add_executable(firefox_boomarks firefox_boomarks.cpp)
target_include_directories(firefox_boomarks PUBLIC "lz4/lib")
target_link_libraries(firefox_boomarks LINK_PUBLIC lz4_static)
