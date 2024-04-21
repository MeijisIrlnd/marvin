#ifndef MARVIN_LEAKYINTEGRATOR_H
#define MARVIN_LEAKYINTEGRATOR_H
#include "audio/utils/marvin_Concepts.h"
#include <enable_warnings.h>
namespace Audio {
    template <FloatType SampleType>
    class LeakyIntegrator final {
    public:
        [[nodiscard]] SampleType operator()(SampleType x, SampleType a) noexcept;

    private:
        SampleType m_prev;
    };
} // namespace Audio
#include <disable_warnings.h>
#endif
