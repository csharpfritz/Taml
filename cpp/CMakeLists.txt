cmake_minimum_required(VERSION 3.19)

project(Taml VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(TAML_BUILD_SHARED "Build TAML as a shared library (default OFF: static)" OFF)
option(TAML_BUILD_TESTS "Build tests" ON)

# Optional: create/populate ThirdParty targets (disabled by default to keep IDE solutions clean)
option(TAML_POPULATE_THIRD_PARTY "Create CMake targets to clone/populate ThirdParty (creates additional CMake targets)" OFF)

# Honor user-provided choice for BUILD_SHARED_LIBS, default to static when not set
if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ${TAML_BUILD_SHARED} CACHE BOOL "Build shared libraries" FORCE)
endif()

# Add Boost.Describe via FetchContent/ExternalProject and place sources under ThirdParty
include(FetchContent)
include(ExternalProject)

# Ensure ThirdParty is used as the base directory for fetched content
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/ThirdParty")

# Fetch Boost.MP11 first (dependency)
# Place sources into ThirdParty/Boost_MP11 and ThirdParty/Boost_Describe (use underscore folder names)
set(BOOST_MP11_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/Boost_MP11")
set(BOOST_DESCRIBE_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/Boost_Describe")

# Ensure the ThirdParty directories exist so the fetched sources land in the expected place
file(MAKE_DIRECTORY ${BOOST_MP11_DIR})
file(MAKE_DIRECTORY ${BOOST_DESCRIBE_DIR})

if(TAML_POPULATE_THIRD_PARTY)
    # Provide a convenient, idempotent bootstrap target to populate ThirdParty for new clones
    # Usage: cmake --build <builddir> --target bootstrap-thirdparty
    find_program(GIT_EXECUTABLE git)

    if(GIT_EXECUTABLE)
        if(NOT EXISTS ${BOOST_MP11_DIR})
            add_custom_target(bootstrap-boost-mp11
                COMMAND ${GIT_EXECUTABLE} clone --depth 1 https://github.com/boostorg/mp11.git ${BOOST_MP11_DIR}
                COMMENT "Cloning Boost.MP11 into ${BOOST_MP11_DIR}"
                VERBATIM
            )
        endif()

        if(NOT EXISTS ${BOOST_DESCRIBE_DIR})
            add_custom_target(bootstrap-boost-describe
                COMMAND ${GIT_EXECUTABLE} clone --depth 1 https://github.com/boostorg/describe.git ${BOOST_DESCRIBE_DIR}
                COMMENT "Cloning Boost.Describe into ${BOOST_DESCRIBE_DIR}"
                VERBATIM
            )
        endif()

        # Aggregate target
        add_custom_target(bootstrap-thirdparty EXCLUDE_FROM_ALL)

        if(TARGET bootstrap-boost-mp11)
            add_dependencies(bootstrap-thirdparty bootstrap-boost-mp11)
        endif()

        if(TARGET bootstrap-boost-describe)
            add_dependencies(bootstrap-thirdparty bootstrap-boost-describe)
        endif()

        # Also make the legacy 'bootstrap-thirdparty' depend on the ExternalProject population target if present
        if(TARGET populate-thirdparty)
            add_dependencies(bootstrap-thirdparty populate-thirdparty)
        endif()

        message(STATUS "Added 'bootstrap-thirdparty' target; run 'cmake --build <builddir> --target bootstrap-thirdparty' to populate ThirdParty")
    else()
        message(STATUS "git not found; 'bootstrap-thirdparty' target not created. Install git to enable cloning ThirdParty dependencies.")
    endif()
endif()

if(TAML_POPULATE_THIRD_PARTY)
    # Use ExternalProject_Add to clone third-party sources into ThirdParty and build inside their own build folders.
    # For header-only projects (like mp11) we skip configure/build steps; for describe we also only fetch sources to keep build simple.
    ExternalProject_Add(boost_mp11_ep
        GIT_REPOSITORY https://github.com/boostorg/mp11.git
        GIT_TAG boost-1.82.0
        SOURCE_DIR ${BOOST_MP11_DIR}
        BINARY_DIR ${BOOST_MP11_DIR}/build
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        EXCLUDE_FROM_ALL 1

        # clone during the build step
    )

    ExternalProject_Add(boost_describe_ep
        GIT_REPOSITORY https://github.com/boostorg/describe.git
        GIT_TAG boost-1.82.0
        SOURCE_DIR ${BOOST_DESCRIBE_DIR}
        BINARY_DIR ${BOOST_DESCRIBE_DIR}/build
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        EXCLUDE_FROM_ALL 1
        DEPENDS boost_mp11_ep
    )

    # Make sure main targets can include the headers from the populated ThirdParty folders
    add_custom_target(populate-thirdparty EXCLUDE_FROM_ALL DEPENDS boost_mp11_ep boost_describe_ep)
    message(STATUS "Added ExternalProject targets boost_mp11_ep and boost_describe_ep; run 'cmake --build <builddir> --target populate-thirdparty' to fetch third-party sources")
endif()

# Make Boost.Describe include directory directly available when building (avoid creating exportable target)
# We intentionally do not create an exported target here to keep install/export simple.

# Sources
file(GLOB_RECURSE TAML_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Library target
add_library(taml ${TAML_SOURCES})

# third-party population is optional; do not force populate during normal build
# add_dependencies(taml populate-thirdparty)
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

# Make Boost.Describe and Boost.MP11 headers available to taml so describe headers can find mp11
target_include_directories(taml PRIVATE
    $<BUILD_INTERFACE:${BOOST_DESCRIBE_DIR}/include>
    $<BUILD_INTERFACE:${BOOST_MP11_DIR}/include>
)

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

    # Link only against taml; make Boost.Describe and Boost.MP11 headers available for tests too
    target_link_libraries(taml_test PRIVATE taml)
    target_include_directories(taml_test PRIVATE
        $<BUILD_INTERFACE:${BOOST_DESCRIBE_DIR}/include>
        $<BUILD_INTERFACE:${BOOST_MP11_DIR}/include>
    )

    # third-party population is optional for tests as well
    # add_dependencies(taml_test populate-thirdparty)
    add_test(NAME taml_test COMMAND taml_test)
endif()

# Provide a CMake config export for downstream usage (optional)
install(EXPORT tamlTargets FILE tamlTargets.cmake NAMESPACE Taml:: DESTINATION lib/cmake/taml)

# Project summary
message(STATUS "Taml ${PROJECT_VERSION} (static by default). To build as shared, configure -DTAML_BUILD_SHARED=ON or -DBUILD_SHARED_LIBS=ON")
