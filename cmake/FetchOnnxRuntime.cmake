include(FetchContent)

set(ORT_VERSION "1.21.0")
set(ORT_BASE_URL "https://github.com/microsoft/onnxruntime/releases/download/v${ORT_VERSION}")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
        set(ORT_ARCHIVE "onnxruntime-linux-aarch64-${ORT_VERSION}.tgz")
    else()
        set(ORT_ARCHIVE "onnxruntime-linux-x64-${ORT_VERSION}.tgz")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # macOS universal (x86_64 + arm64 Apple Silicon)
    set(ORT_ARCHIVE "onnxruntime-osx-universal2-${ORT_VERSION}.tgz")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|aarch64")
        set(ORT_ARCHIVE "onnxruntime-win-arm64-${ORT_VERSION}.zip")
    else()
        set(ORT_ARCHIVE "onnxruntime-win-x64-${ORT_VERSION}.zip")
    endif()
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(ORT_URL "${ORT_BASE_URL}/${ORT_ARCHIVE}")

message(STATUS "Fetching ONNX Runtime from: ${ORT_URL}")

FetchContent_Declare(
        onnxruntime
        URL "${ORT_URL}"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(onnxruntime)

set(ONNXRUNTIME_ROOT "${onnxruntime_SOURCE_DIR}" CACHE PATH "ONNX Runtime root" FORCE)
message(STATUS "ONNXRUNTIME_ROOT set to: ${ONNXRUNTIME_ROOT}")

# Find the actual library and include path for the imported target
find_path(_ONNXRUNTIME_INCLUDE_DIR onnxruntime_cxx_api.h
    HINTS ${ONNXRUNTIME_ROOT}/include
    PATH_SUFFIXES onnxruntime/core/session
)
find_library(_ONNXRUNTIME_LIB onnxruntime
    HINTS ${ONNXRUNTIME_ROOT}/lib
)

if(_ONNXRUNTIME_INCLUDE_DIR AND _ONNXRUNTIME_LIB)
    message(STATUS "Found ONNX Runtime: ${_ONNXRUNTIME_LIB}")
    add_library(onnxruntime::onnxruntime UNKNOWN IMPORTED)
    set_target_properties(onnxruntime::onnxruntime PROPERTIES
        IMPORTED_LOCATION "${_ONNXRUNTIME_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${_ONNXRUNTIME_INCLUDE_DIR}"
    )
    
    set(ONNXRUNTIME_INCLUDE_DIR "${_ONNXRUNTIME_INCLUDE_DIR}" CACHE PATH "ONNX Runtime include dir" FORCE)
    set(ONNXRUNTIME_LIB "${_ONNXRUNTIME_LIB}" CACHE FILEPATH "ONNX Runtime library" FORCE)
else()
    message(FATAL_ERROR "ONNX Runtime not found at ${ONNXRUNTIME_ROOT}")
endif()