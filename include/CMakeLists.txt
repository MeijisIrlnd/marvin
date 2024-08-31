# ========================================================================================================
#  _______ _______ ______ ___ ___ _______ _______
# |   |   |   _   |   __ \   |   |_     _|    |  |
# |       |       |      <   |   |_|   |_|       |
# |__|_|__|___|___|___|__|\_____/|_______|__|____|
#
# This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
#
# ========================================================================================================

set(MARVIN_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/containers/marvin_BufferView.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/containers/marvin_SwapBuffer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/containers/marvin_StrideView.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/containers/marvin_FIFO.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/containers/marvin_FixedCircularBuffer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/dsp/marvin_DelayLine.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/dsp/spectral/marvin_FFT.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/dsp/filters/marvin_SVF.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/dsp/filters/marvin_APF.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/dsp/filters/marvin_LPF.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/dsp/filters/biquad/marvin_BiquadCoefficients.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/dsp/filters/biquad/marvin_SmoothedBiquadCoefficients.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/dsp/filters/biquad/marvin_Biquad.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/dsp/filters/biquad/marvin_RBJCoefficients.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/dsp/oscillators/marvin_Oscillator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/library/marvin_Concepts.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/library/marvin_PropagateConst.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/math/marvin_Math.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/math/marvin_Reciprocal.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/math/marvin_Windows.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/math/marvin_Interpolators.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/math/marvin_LeakyIntegrator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/math/marvin_VecOps.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/math/marvin_MixMatrix.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/utils/marvin_Utils.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/utils/marvin_SmoothedValue.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/utils/marvin_Random.h
        ${CMAKE_CURRENT_SOURCE_DIR}/marvin/utils/marvin_Range.h
        PARENT_SCOPE
)

