# ========================================================================================================
#  _______ _______ ______ ___ ___ _______ _______ 
# |   |   |   _   |   __ \   |   |_     _|    |  |
# |       |       |      <   |   |_|   |_|       |
# |__|_|__|___|___|___|__|\_____/|_______|__|____|
#                                                
# This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
#
# ========================================================================================================

cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(
        marvin
        VERSION 0.0.1
        LANGUAGES CXX
        DESCRIPTION "Pure C++ audio helper library"
        HOMEPAGE_URL "https://github.com/MeijisIrlnd/marvin.git"
)
if (APPLE)
    set(MARVIN_EXTRA_DEFS MARVIN_MACOS)
    set(MARVIN_EXTRA_LINK_LIBS "-framework Accelerate")
    set(MARVIN_COMPILE_OPTS "-ffinite-math-only"
            "-fno-signed-zeros"
            "-fno-trapping-math"
            "-fassociative-math"
            "-fno-math-errno"
            "-freciprocal-math"
            "-ftree-vectorize"
    )
else ()
    if (WIN32)
        set(MARVIN_EXTRA_DEFS MARVIN_WINDOWS)
    elseif (UNIX)
        set(MARVIN_EXTRA_DEFS MARVIN_LINUX)
        set(MARVIN_LINUX TRUE)
    endif ()
    find_package(IPP)
    if (IPP_FOUND)
        set(MARVIN_EXTRA_DEFS ${MARVIN_EXTRA_DEFS} MARVIN_HAS_IPP)
        set(MARVIN_EXTRA_LINK_LIBS IPP::ippcore IPP::ipps IPP::ippi)
    endif ()
endif ()
if (${FORCE_FALLBACK_FFT})
    set(MARVIN_EXTRA_DEFS ${MARVIN_EXTRA_DEFS} MARVIN_FORCE_FALLBACK_FFT)
endif ()

include(GNUInstallDirs)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(MARVIN_INSTALL_DEST " ${CMAKE_INSTALL_LIBDIR}/cmake/marvin"
        CACHE STRING "Path where package files will be installed, relative to the install prefix"
)
if (MSVC)
    message(STATUS "MSVC-Like")
    set(MARVIN_WARNING_FLAGS "/W4 /WX")
else ()
    # Need no unused command line arg because not all TU-s need -mfpu=neon
    set(MARVIN_WARNING_FLAGS "-Wall -Wextra -pedantic -Wno-unused-command-line-argument -Werror")
endif ()

set(MIPP_STATIC_LIB OFF)
set(MIPP_EXAMPLES_EXE OFF)
set(MIPP_TESTS_EXE OFF)
include(FetchContent)

FetchContent_Declare(
        readerwriterqueue
        GIT_REPOSITORY https://github.com/cameron314/readerwriterqueue.git
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(readerwriterqueue)

FetchContent_Declare(
        concurrentqueue
        GIT_REPOSITORY https://github.com/cameron314/concurrentqueue
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(concurrentqueue)

# Disable xsimd unit tests
set(BUILD_TESTS OFF)
FetchContent_Declare(xsimd
        GIT_REPOSITORY https://github.com/xtensor-stack/xsimd.git
        GIT_TAG master
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(xsimd)

add_subdirectory(source)
add_subdirectory(include)
add_subdirectory(tests)
add_subdirectory(docs)
source_group(TREE source)
add_library(marvin STATIC ${MARVIN_SOURCES})
add_library(slma::marvin ALIAS marvin)
set_source_files_properties(${MARVIN_SOURCES} PROPERTIES COMPILE_FLAGS "${MARVIN_WARNING_FLAGS}")
target_compile_definitions(marvin PUBLIC ${MARVIN_EXTRA_DEFS})
target_compile_features(marvin PUBLIC cxx_std_20)
target_compile_options(marvin PUBLIC ${MARVIN_SIMD_FLAGS} ${MARVIN_COMPILE_OPTS})
target_include_directories(marvin PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(marvin PUBLIC
        ${MARVIN_EXTRA_LINK_LIBS}
        readerwriterqueue
        concurrentqueue
        xsimd
)

install(DIRECTORY "include/" # source directory
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.h" # select header files
)

install(
        TARGETS marvin
        EXPORT marvin
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT marvin
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT marvin
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(
        EXPORT marvin
        NAMESPACE slma::
        FILE Targets.cmake
        DESTINATION "${MARVIN_INSTALL_DEST}"
        COMPONENT marvin
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(marvin-config-version.cmake
        VERSION "${marvin_VERSION}"
        COMPATIBILITY SameMajorVersion
)

configure_package_config_file(cmake/marvin-config.in marvin-config.cmake
        INSTALL_DESTINATION "${MARVIN_INSTALL_DEST}" NO_SET_AND_CHECK_MACRO
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/marvin-config-version.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/marvin-config.cmake"
        DESTINATION "${MARVIN_INSTALL_DEST}"
        COMPONENT marvin
)

include(CPackComponent)

cpack_add_component(
        marvin DISPLAY_NAME "Marvin Audio Helpers"
        DESCRIPTION "A pure C++ dsp helper library"
)

# Set up testing
if (MARVIN_TESTING)
    message(STATUS "Marvin tests enabled")
    include(FetchContent)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.0.1 # or a later release
            GIT_SHALLOW ON
            # FIND_PACKAGE_ARGS 3.0.1 GLOBAL
    )
    FetchContent_MakeAvailable(Catch2)

    FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt
            GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(fmt)
    FetchContent_Declare(
            AudioFile
            GIT_REPOSITORY https://github.com/adamstark/AudioFile.git
            GIT_SHALLOW ON
    )

    FetchContent_MakeAvailable(AudioFile)

    add_executable(marvin-tests ${MARVIN_TEST_SOURCE})
    target_include_directories(marvin-tests PRIVATE include)
    if (${MARVIN_LINUX})
        set(MARVIN_TESTS_EXTRA_LIBS pthread)
    endif ()
    target_link_libraries(marvin-tests PRIVATE Catch2::Catch2WithMain marvin fmt AudioFile ${MARVIN_TESTS_EXTRA_LIBS})
    target_compile_definitions(marvin-tests PRIVATE FMT_HEADER_ONLY)
    if (${MARVIN_TESTING_ANALYSIS})
        target_compile_definitions(marvin-tests PRIVATE MARVIN_ANALYSIS)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests/resources/break.wav DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests/resources/Sine.wav DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif ()
    add_dependencies(marvin-tests marvin)
else ()
    message(STATUS "Marvin tests disabled")
endif ()



