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

