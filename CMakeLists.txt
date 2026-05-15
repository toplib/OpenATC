cmake_minimum_required(VERSION 3.16)

project(SimpleATC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(GGML_VULKAN ON CACHE BOOL "" FORCE)
set(GGML_CUDA OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(external/llama.cpp)

set(WHISPER_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(WHISPER_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/whisper.cpp EXCLUDE_FROM_ALL)

get_target_property(WHISPER_INCLUDES whisper INTERFACE_INCLUDE_DIRECTORIES)
if(WHISPER_INCLUDES)
    list(FILTER WHISPER_INCLUDES EXCLUDE REGEX "ggml")
    set_target_properties(whisper PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${WHISPER_INCLUDES}")
endif()

find_package(CURL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

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
        src/core/llm-backend/ILLMBackend.h
        src/core/llm-backend/Message.h
        src/core/llm-backend/llamacpp/LlamacppBackend.h
        src/core/llm-backend/llamacpp/LlamacppBackend.cpp
)

target_include_directories(SimpleATC PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/core
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external/llama.cpp/include
        ${CMAKE_SOURCE_DIR}/external/whisper.cpp/include
)

target_link_libraries(SimpleATC PRIVATE
        CURL::libcurl
        llama
        whisper
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)