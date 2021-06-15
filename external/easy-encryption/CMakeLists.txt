cmake_minimum_required(VERSION 3.5)

project (easy-encryption LANGUAGES CXX VERSION 2.3.8) 
 
add_library(easy-encryption STATIC easy-encryption.cpp)
target_include_directories(easy-encryption PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
