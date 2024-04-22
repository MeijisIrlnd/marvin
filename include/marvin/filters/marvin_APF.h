#ifndef MARVIN_APF_H
#define MARVIN_APF_H
#include "marvin/dsp/marvin_DelayLine.h"
#include "marvin/library/enable_warnings.h"
namespace Audio {
    template <FloatType SampleType>
    class LatticeAPF final {
    public:
        ~LatticeAPF() noexcept = default;
        void initialise(double sampleRate);
        void setCoeff(SampleType newCoeff) noexcept;
        void setDelay(SampleType newDelaySamples) noexcept;
        [[nodiscard]] SampleType operator()(SampleType x) noexcept;
        void reset() noexcept;
        [[nodiscard]] SampleType tap(SampleType delaySamples) noexcept;

    private:
        DelayLine<SampleType, DelayLineInterpolationTypes::Linear> m_delay;
        SampleType m_coeff{ 0.0 };
    };

} // namespace Audio
#include "marvin/library/disable_warnings.h"
#endif
