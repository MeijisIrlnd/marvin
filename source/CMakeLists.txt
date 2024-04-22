set(MARVIN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/marvin_DelayLine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filters/marvin_APF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filters/marvin_LPF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_LeakyIntegrator.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/oscillators/marvin_Oscillator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_LinearSmoothedValue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_Random.cpp
    PARENT_SCOPE
)

set(MARVIN_HEADERS 
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_basics/marvin_Buffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/marvin_DelayLine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/filters/marvin_APF.h
    ${CMAKE_CURRENT_SOURCE_DIR}/filters/marvin_LPF.h
    ${CMAKE_CURRENT_SOURCE_DIR}/library/marvin_Concepts.h
    ${CMAKE_CURRENT_SOURCE_DIR}/library/marvin_PropagateConst.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/marvin_LeakyIntegrator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/oscillators/marvin_Oscillator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_LinearSmoothedValue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/marvin_Random.h
    PARENT_SCOPE
)

