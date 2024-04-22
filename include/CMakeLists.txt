set(MARVIN_HEADERS 
    ${CMAKE_CURRENT_SOURCE_DIR}/marvin/audio_basics/marvin_Buffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/marvin/dsp/marvin_DelayLine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/marvin/filters/marvin_APF.h
    ${CMAKE_CURRENT_SOURCE_DIR}/marvin/filters/marvin_LPF.h
    ${CMAKE_CURRENT_SOURCE_DIR}/marvin/library/marvin_Concepts.h
    ${CMAKE_CURRENT_SOURCE_DIR}/marvin/library/marvin_PropagateConst.h
    ${CMAKE_CURRENT_SOURCE_DIR}/marvin/math/marvin_LeakyIntegrator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/marvin/oscillators/marvin_Oscillator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/marvin/utils/marvin_LinearSmoothedValue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/marvin/utils/marvin_Random.h
    PARENT_SCOPE
)

