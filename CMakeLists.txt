cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(SimpleATC LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(GGML_VULKAN ON CACHE BOOL "" FORCE)
set(GGML_CUDA OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(external/llama.cpp)
add_subdirectory(external/portaudio)
add_library(webrtcvad STATIC
        external/webrtcvad/vad/webrtc_vad.c
        external/webrtcvad/vad/vad_sp.c
        external/webrtcvad/vad/vad_filterbank.c
        external/webrtcvad/vad/vad_gmm.c
        external/webrtcvad/vad/vad_core.c
        external/webrtcvad/vad/vad.cc
        external/webrtcvad/vad/signal_processing_library.c
)

set(WHISPER_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(WHISPER_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/whisper.cpp EXCLUDE_FROM_ALL)

add_subdirectory(external/voxtral-rs)

get_target_property(WHISPER_INCLUDES whisper INTERFACE_INCLUDE_DIRECTORIES)
if(WHISPER_INCLUDES)
    list(FILTER WHISPER_INCLUDES EXCLUDE REGEX "ggml")
    set_target_properties(whisper PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${WHISPER_INCLUDES}")
endif()

find_package(CURL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(spdlog REQUIRED)
find_package(OpenAL REQUIRED)

qt_add_executable(SimpleATC
        src/main.cpp
#        src/stt_test.main.cpp
        src/clients/huggingface/HuggingFace.h
        src/clients/huggingface/HuggingFace.cpp
        src/utils/FileNode.h
        src/core/net/HttpClient.h
        src/core/net/HttpClient.cpp
        src/core/net/DownloadManager.h
        src/core/net/DownloadManager.cpp
        include/json.hpp
        src/core/stt/ISTTBackend.h
        src/core/stt/Whisper/WhisperBackend.h
        src/core/stt/Whisper/WhisperBackend.cpp
        src/utils/WavLoader.h
        src/core/llm/ILLMBackend.h
        src/core/llm/Message.h
        src/core/llm/llamacpp/LlamacppBackend.h
        src/core/llm/llamacpp/LlamacppBackend.cpp
        src/gui/main/MainWindow.h
        src/gui/main/MainWindow.ui
        src/gui/main/MainWindow.cpp
#        src/core/config/Config.h
#        src/core/config/Config.cpp
        src/gui/Manager.h
        src/gui/Manager.cpp
        src/Constants.h
        src/gui/settings/Settings.h
        src/gui/settings/Settings.ui
        src/gui/settings/Settings.cpp
        include/cpptoml.h
        src/simulation/Engine.h
        src/simulation/Entity.h
        src/simulation/Coordinates.h
        src/simulation/aircraft/AirCraft.h
        src/core/microphone/Microphone.h
        src/core/microphone/Microphone.cpp
        src/core/tts/ITTSBackend.h
        src/core/tts/Voxtral/VoxtralBackend.h
        src/core/tts/Voxtral/VoxtralBackend.cpp
)
set_target_properties(SimpleATC PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
)
target_include_directories(SimpleATC PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/core
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external/llama.cpp/include
        ${CMAKE_SOURCE_DIR}/external/whisper.cpp/include
        ${CMAKE_SOURCE_DIR}/external/portaudio/include
        ${CMAKE_SOURCE_DIR}/external/webrtcvad
        ${CMAKE_SOURCE_DIR}/external/voxtral-rs/include
)


target_link_libraries(SimpleATC PRIVATE
        CURL::libcurl
        llama
        whisper
        portaudio
        webrtcvad
        voxtral_rust
        OpenAL::OpenAL
        spdlog::spdlog
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)
target_include_directories(webrtcvad PUBLIC
        ${CMAKE_SOURCE_DIR}/external/webrtcvad
        ${CMAKE_SOURCE_DIR}/external/webrtcvad/vad/include
)