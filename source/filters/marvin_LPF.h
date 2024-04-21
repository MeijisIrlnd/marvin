#ifndef MARVIN_LPF_H
#define MARVIN_LPF_H
#include "audio/utils/marvin_Concepts.h"
#include <enable_warnings.h>
namespace Audio {
    template <FloatType SampleType>
    class LPF {
    public:
        void setCoeff(SampleType newCoeff) noexcept;
        [[nodiscard]] SampleType operator()(SampleType x) noexcept;
        void reset() noexcept;

    private:
        SampleType m_prev{ static_cast<SampleType>(0.0) };
        SampleType m_coeff{ static_cast<SampleType>(0.0) };
    };
} // namespace Audio
#include <disable_warnings.h>
#endif