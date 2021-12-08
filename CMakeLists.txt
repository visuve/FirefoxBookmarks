cmake_minimum_required(VERSION 3.5)

project(FirefoxBookmarks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(firefox_boomarks firefox_boomarks.cpp)
