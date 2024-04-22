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

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(MARVIN_INSTALL_DEST "${CMAKE_INSTALL_LIBDIR}/cmake/marvin"
        CACHE STRING "Path where package files will be installed, relative to the install prefix"
)
add_subdirectory(source)
add_subdirectory(tests)
source_group(TREE source)
add_library(marvin STATIC ${MARVIN_SOURCES})
target_compile_features(marvin PUBLIC cxx_std_20)
target_include_directories(marvin PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/marvin>
)

install(DIRECTORY "source/" # source directory
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/marvin/marvin" # target directory
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
    NAMESPACE marvin::
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

include(CPackComponent) 

cpack_add_component( 
    marvin DISPLAY_NAME "Marvin Audio Helpers"
    DESCRIPTION "A pure C++ dsp helper library"
)



