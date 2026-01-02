cmake_minimum_required(VERSION 3.19)

project(Taml VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(TAML_BUILD_SHARED "Build TAML as a shared library (default OFF: static)" OFF)
option(TAML_BUILD_TESTS "Build tests" ON)

# Honor user-provided choice for BUILD_SHARED_LIBS, default to static when not set
if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ${TAML_BUILD_SHARED} CACHE BOOL "Build shared libraries" FORCE)
endif()

# Sources
file(GLOB_RECURSE TAML_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Library target
add_library(taml ${TAML_SOURCES})

target_include_directories(taml PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(taml PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

# Collect headers and add them to the target so IDEs (e.g., Visual Studio) show them
file(GLOB_RECURSE TAML_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh"
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${TAML_HEADERS})

# Add headers as PRIVATE so they appear in project filters (but are not exported as INTERFACE sources)
target_sources(taml PRIVATE ${TAML_HEADERS})

# Platform name normalization for output file name suffix
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM "windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PLATFORM "mac")
else()
    string(TOLOWER "${CMAKE_SYSTEM_NAME}" PLATFORM)
endif()

string(TOLOWER "${CMAKE_HOST_SYSTEM_PROCESSOR}" _proc_lower)

if(_proc_lower MATCHES "arm" OR _proc_lower MATCHES "aarch64")
    set(ARCH "arm")
else()
    set(ARCH "x86")
endif()

# 32/64 bit detection
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BITS "64")
else()
    set(BITS "32")
endif()

set(PLATFORM_SUFFIX "${PLATFORM}${BITS}")

# Put all built library files in the libs folder next to the include directory (source tree)
set(TAML_LIBS_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")

# Ensure the directory exists when configuring
file(MAKE_DIRECTORY "${TAML_LIBS_OUTPUT_DIR}")

set_target_properties(taml PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${TAML_LIBS_OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${TAML_LIBS_OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${TAML_LIBS_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${TAML_LIBS_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${TAML_LIBS_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${TAML_LIBS_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${TAML_LIBS_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${TAML_LIBS_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${TAML_LIBS_OUTPUT_DIR}"

    # OUTPUT_NAME supports generator expressions to include a debug suffix
    OUTPUT_NAME "$<$<CONFIG:Debug>:taml-d>$<$<NOT:$<CONFIG:Debug>>:taml>-${PLATFORM_SUFFIX}"
)

# Install targets (optional) - the install destinations mirror the build libs directory
install(TARGETS taml
    EXPORT tamlTargets
    ARCHIVE DESTINATION libs
    LIBRARY DESTINATION libs
    RUNTIME DESTINATION libs
)
install(DIRECTORY include/ DESTINATION include)

# Tests
if(TAML_BUILD_TESTS)
    enable_testing()
    add_executable(taml_test tests/test_main.cpp)
    target_link_libraries(taml_test PRIVATE taml)
    add_test(NAME taml_test COMMAND taml_test)
endif()

# Provide a CMake config export for downstream usage (optional)
install(EXPORT tamlTargets FILE tamlTargets.cmake NAMESPACE Taml:: DESTINATION lib/cmake/taml)

# Project summary
message(STATUS "Taml ${PROJECT_VERSION} (static by default). To build as shared, configure -DTAML_BUILD_SHARED=ON or -DBUILD_SHARED_LIBS=ON")
