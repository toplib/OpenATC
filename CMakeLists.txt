cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(OpenATC LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GGML_VULKAN ON CACHE BOOL "" FORCE)
set(GGML_CUDA OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(SHERPA_ONNX_ENABLE_PYTHON OFF CACHE BOOL "" FORCE)
set(SHERPA_ONNX_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SHERPA_ONNX_ENABLE_BINARY OFF CACHE BOOL "" FORCE)
set(SHERPA_ONNX_ENABLE_SPEAKER_DIARIZATION OFF CACHE BOOL "" FORCE)
set(SHERPA_ONNX_ENABLE_WEBSOCKET OFF CACHE BOOL "" FORCE)
set(SHERPA_ONNX_ENABLE_PORTAUDIO OFF CACHE BOOL "" FORCE)

set(_sonnx_dir "${CMAKE_BINARY_DIR}/_deps/onnxruntime-shared")
set(_sonnx_lib "${_sonnx_dir}/lib/libonnxruntime.so")

if(NOT EXISTS "${_sonnx_lib}")
    set(_sonnx_ver "1.24.4")
    set(_sonnx_zip "onnxruntime-linux-x64-glibc2_17-Release-${_sonnx_ver}.zip")
    set(_sonnx_url "https://github.com/csukuangfj/onnxruntime-libs/releases/download/v${_sonnx_ver}/${_sonnx_zip}")

    message(STATUS "Downloading shared onnxruntime ${_sonnx_ver}...")
    file(DOWNLOAD "${_sonnx_url}" "${CMAKE_BINARY_DIR}/${_sonnx_zip}" SHOW_PROGRESS)

    set(_tmp_dir "${CMAKE_BINARY_DIR}/_deps/onnxruntime-tmp")
    file(MAKE_DIRECTORY "${_tmp_dir}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf "${CMAKE_BINARY_DIR}/${_sonnx_zip}"
                    WORKING_DIRECTORY "${_tmp_dir}")
    file(GLOB _extracted_dirs "${_tmp_dir}/*")
    list(GET _extracted_dirs 0 _extracted_dir)
    file(COPY "${_extracted_dir}/" DESTINATION "${_sonnx_dir}")
    file(REMOVE_RECURSE "${_tmp_dir}" "${CMAKE_BINARY_DIR}/${_sonnx_zip}")
endif()

set(ENV{SHERPA_ONNXRUNTIME_INCLUDE_DIR} "${_sonnx_dir}/include")
set(ENV{SHERPA_ONNXRUNTIME_LIB_DIR} "${_sonnx_dir}/lib")
message(STATUS "Using shared onnxruntime from ${_sonnx_dir}")

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(external/sherpa-onnx EXCLUDE_FROM_ALL)
set(CMAKE_CXX_STANDARD 20)

foreach(_t sherpa-onnx-cxx-api sherpa-onnx-c-api sherpa-onnx-core sherpa-onnx)
    if(TARGET ${_t})
        set_target_properties(${_t} PROPERTIES
            INTERFACE_COMPILE_FEATURES ""
        )
    endif()
endforeach()

set(LLAMA_BUILD_COMMON ON)
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
target_include_directories(webrtcvad PUBLIC
        ${CMAKE_SOURCE_DIR}/external/webrtcvad
        ${CMAKE_SOURCE_DIR}/external/webrtcvad/vad/include
)

set(WHISPER_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(WHISPER_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/whisper.cpp EXCLUDE_FROM_ALL)

get_target_property(WHISPER_INCLUDES whisper INTERFACE_INCLUDE_DIRECTORIES)
if(WHISPER_INCLUDES)
    list(FILTER WHISPER_INCLUDES EXCLUDE REGEX "ggml")
    set_target_properties(whisper PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${WHISPER_INCLUDES}")
endif()

find_package(CURL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(spdlog REQUIRED)
find_package(OpenAL REQUIRED)

qt_add_executable(OpenATC
        src/main.cpp
        src/clients/huggingface/HuggingFace.h
        src/clients/huggingface/HuggingFace.cpp
        src/utils/FileNode.h
        src/utils/Vector3f.h
        src/core/net/HttpClient.h
        src/core/net/HttpClient.cpp
        src/core/net/DownloadManager.h
        src/core/net/DownloadManager.cpp
        third_party/json.hpp
        src/ai/stt/ISTTBackend.h
        src/ai/stt/Whisper/WhisperBackend.h
        src/ai/stt/Whisper/WhisperBackend.cpp
        src/utils/WavLoader.h
        src/ai/llm/ILLMBackend.h
        src/ai/llm/Message.h
        src/ai/llm/llamacpp/LlamacppBackend.h
        src/ai/llm/llamacpp/LlamacppBackend.cpp
        src/gui/main/MainWindow.h
        src/gui/main/MainWindow.ui
        src/gui/main/MainWindow.cpp
        src/gui/Manager.h
        src/gui/Manager.cpp
        src/Constants.h
        src/gui/settings/Settings.h
        src/gui/settings/Settings.ui
        src/gui/settings/Settings.cpp
        third_party/cpptoml.h
        src/core/microphone/Microphone.h
        src/core/microphone/Microphone.cpp
        src/ai/tts/ITTSBackend.h
        src/core/pipeline/ATCPipeline.h
        src/core/pipeline/ATCPipeline.cpp
        src/utils/MicrophoneActivationType.h
        src/utils/NoiseSuppressionType.h
        src/core/pipeline/ATCEventType.h
        src/core/microphone/MicrophoneInputAudioData.h
        src/ai/tts/SherpaOnnx/SherpaOnnxBackend.h
        src/ai/tts/SherpaOnnx/SherpaOnnxBackend.cpp
        src/core/audio/Sound.h
        src/core/audio/IAudioBackend.h
        src/core/audio/OpenAL/OpenALBackend.h
        src/core/audio/OpenAL/OpenALBackend.cpp
        src/utils/LLMTaskType.h
        src/utils/LLMTask.h
        src/core/pipeline/microphone/MicrophonePipeline.h
        src/core/pipeline/microphone/MicrophonePipeline.cpp
        src/navigation/Point.h
        src/navigation/Airway.h
        src/navigation/Position.h
        src/navigation/airac/IAIRAC.h
        src/gui/home/openatc.ui
        src/gui/home/HomeWindow.h
        src/gui/home/HomeWindow.cpp
        src/main.cpp
        src/simulation/Simulation.h
        src/simulation/Simulation.cpp
        src/simulation/aircraft/Aircraft.h
        src/simulation/aircraft/Aircraft.cpp
        src/simulation/airport/Airport.h
        src/simulation/Runway.h
        src/navigation/WorldPosition.h
        src/navigation/Path.h
)

set_target_properties(OpenATC PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
)
target_compile_features(OpenATC PRIVATE cxx_std_20)

target_compile_options(OpenATC PRIVATE -std=gnu++20)

target_include_directories(OpenATC PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/core
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external/llama.cpp/include
        ${CMAKE_SOURCE_DIR}/external/llama.cpp/common
        ${CMAKE_SOURCE_DIR}/external/whisper.cpp/include
        ${CMAKE_SOURCE_DIR}/external/portaudio/include
        ${CMAKE_SOURCE_DIR}/external/webrtcvad
        ${CMAKE_SOURCE_DIR}/external/sherpa-onnx
        ${CMAKE_BINARY_DIR}/_deps/json-src/include
)

target_link_libraries(OpenATC PRIVATE
        CURL::libcurl
        llama
        llama-common
        whisper
        portaudio
        webrtcvad
        sherpa-onnx-cxx-api
        OpenAL::OpenAL
        spdlog::spdlog
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

if(EXISTS "${_sonnx_dir}/lib/libonnxruntime.so")
    add_custom_command(TARGET OpenATC POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_sonnx_dir}/lib/libonnxruntime.so"
                $<TARGET_FILE_DIR:OpenATC>
    )
endif()
