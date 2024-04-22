#ifndef MARVIN_LINEARSMOOTHEDVALUE_H
#define MARVIN_LINEARSMOOTHEDVALUE_H
#include "library/marvin_Concepts.h"
#include "library/enable_warnings.h"
namespace Audio {
    template <FloatType SampleType>
    class LinearSmoothedValue {
    public:
        void reset(int stepsSamples);
        void reset(double sampleRate, double timeMs);
        void setCurrentAndTargetValue(SampleType newValue);
        void setTargetValue(SampleType newValue);
        [[nodiscard]] SampleType operator()() noexcept;
        [[nodiscard]] bool isSmoothing() noexcept;

    private:
        int m_duration{ 1 };
        int m_samplesRemaining{ 0 };
        SampleType m_currentValue{ static_cast<SampleType>(0.0) };
        SampleType m_targetValue{ static_cast<SampleType>(0.0) };
        SampleType m_slew{ static_cast<SampleType>(0.0) };
    };
} // namespace Audio
#include "library/disable_warnings.h"
#endif
