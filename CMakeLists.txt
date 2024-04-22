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

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
add_subdirectory(source)
add_subdirectory(tests)
source_group(TREE source)
add_library(marvin STATIC ${MARVIN_SOURCES} ${MARVIN_HEADERS})
target_include_directories(marvin PRIVATE source)

install(FILES 
    ${MARVIN_HEADERS} 
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/marvin"
    COMPONENT marvin 
)

install(
    TARGETS marvin 
    EXPORT marvin 
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT marvin 
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT marvin
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" ${MARVIN_HEADERS} 
)


