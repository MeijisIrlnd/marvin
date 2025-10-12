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
        ${CMAKE_CURRENT_SOURCE_DIR}/containers/marvin_FIFO.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/containers/marvin_FixedCircularBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/containers/marvin_StrideView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/containers/marvin_SwapBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/marvin_DelayLine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/spectral/marvin_FFT.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/oscillators/marvin_Oscillator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/marvin_APF.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/marvin_LPF.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/marvin_SVF.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/biquad/marvin_SIMDBiquad.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/biquad/marvin_Biquad.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/biquad/marvin_BiquadCoefficients.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/biquad/marvin_RBJCoefficients.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters/biquad/marvin_SmoothedBiquadCoefficients.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/library/marvin_Concepts.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/library/marvin_Literals.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/library/marvin_PropagateConst.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_Conversions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_Interpolators.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_Math.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_MixMatrix.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_VecOps.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_Reciprocal.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_LeakyIntegrator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_Windows.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_Utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_SmoothedValue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_Random.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_Range.cpp
        PARENT_SCOPE
)

