#ifndef MARVIN_LEAKYINTEGRATOR_H
#define MARVIN_LEAKYINTEGRATOR_H
#include "marvin/library/marvin_Concepts.h"
#include "marvin/library/enable_warnings.h"
namespace Audio {
    template <FloatType SampleType>
    class LeakyIntegrator final {
    public:
        [[nodiscard]] SampleType operator()(SampleType x, SampleType a) noexcept;

    private:
        SampleType m_prev;
    };
} // namespace Audio
#include "marvin/library/disable_warnings.h"
#endif
