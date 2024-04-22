# ========================================================================================================
#  _______ _______ ______ ___ ___ _______ _______
# |   |   |   _   |   __ \   |   |_     _|    |  |
# |       |       |      <   |   |_|   |_|       |
# |__|_|__|___|___|___|__|\_____/|_______|__|____|
#
# This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
#
# ========================================================================================================

set(MARVIN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/marvin_DelayLine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/marvin_APF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/marvin_LPF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_LeakyIntegrator.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/oscillators/marvin_Oscillator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_LinearSmoothedValue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_Random.cpp
    PARENT_SCOPE
)

