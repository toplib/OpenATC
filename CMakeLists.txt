cmake_minimum_required(VERSION 3.16)

project(SimpleATC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GGML_VULKAN ON CACHE BOOL "" FORCE)

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(external/whisper.cpp)

find_package(CURL REQUIRED)

add_executable(SimpleATC
        src/main.cpp
        src/clients/huggingface/HuggingFace.h
        src/clients/huggingface/HuggingFace.cpp
        src/utils/FileNode.h
        src/core/net/HttpClient.h
        src/core/net/HttpClient.cpp
        src/core/net/DownloadManager.h
        src/core/net/DownloadManager.cpp
        include/json.hpp
        src/core/stt-backend/ISTTBackend.h
        src/core/stt-backend/Whisper/WhisperBackend.h
        src/core/stt-backend/Whisper/WhisperBackend.cpp
        src/utils/WavLoader.h
)

target_link_libraries(SimpleATC PRIVATE
        CURL::libcurl
        whisper
)

target_include_directories(SimpleATC PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/core
        ${CMAKE_SOURCE_DIR}/include
)