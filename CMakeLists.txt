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
include(GNUInstallDirs)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(MARVIN_INSTALL_DEST "${CMAKE_INSTALL_LIBDIR}/cmake/marvin"
        CACHE STRING "Path where package files will be installed, relative to the install prefix"
)
if(MSVC) 
    set(MARVIN_WARNING_FLAGS "/W4 /WX")
else() 
    set(MARVIN_WARNING_FLAGS "-Wall -Wextra -pedantic -Werror")
endif()

add_subdirectory(source)
add_subdirectory(include)
add_subdirectory(tests)
add_subdirectory(docs)
source_group(TREE source)
add_library(marvin STATIC ${MARVIN_SOURCES})
add_library(slma::marvin ALIAS marvin)
set_source_files_properties(${MARVIN_SOURCES} PROPERTIES COMPILE_FLAGS "${MARVIN_WARNING_FLAGS}")
target_compile_features(marvin PUBLIC cxx_std_20)
target_include_directories(marvin PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
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

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.0.1 # or a later release
    GIT_SHALLOW ON
    FIND_PACKAGE_ARGS 3.0.1 GLOBAL
)
FetchContent_MakeAvailable(Catch2)
add_executable(marvin-tests ${MARVIN_TEST_SOURCE})
target_include_directories(marvin-tests PRIVATE include)
target_link_libraries(marvin-tests PRIVATE Catch2::Catch2WithMain marvin)
add_dependencies(marvin-tests marvin)



