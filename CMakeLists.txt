cmake_minimum_required(VERSION 3.16)

project(SimpleATC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CURL REQUIRED)

add_executable(SimpleATC
        src/main.cpp
        src/huggingface/HuggingFace.h
        src/utils/FileNode.h
        src/huggingface/HuggingFace.cpp
        src/net/HttpClient.h
        src/net/HttpClient.cpp
        include/json.hpp
        src/net/DownloadManager.h
)

target_link_libraries(SimpleATC PRIVATE CURL::libcurl)

target_include_directories(SimpleATC PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)
target_include_directories(SimpleATC PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)