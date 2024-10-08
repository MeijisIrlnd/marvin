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
        ${CMAKE_CURRENT_SOURCE_DIR}/containers/marvin_BufferView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/marvin_DelayLine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/spectral/marvin_FFT.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/oscillators/marvin_Oscillator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/marvin_APF.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/marvin_LPF.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/marvin_SVF.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_VecOps.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_Reciprocal.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_LeakyIntegrator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_Utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_SmoothedValue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_Random.cpp
        PARENT_SCOPE
)

