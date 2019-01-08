# Doesn't work; I don't know enough CMake-fu.
cmake_minimum_required(VERSION 3.1)
project(browser)

set(CMAKE_CXX_STANDARD 17)

add_executable(browser src/browser.cpp)
target_link_libraries(browser user32.lib)

install(TARGETS browser DESTINATION bin)
