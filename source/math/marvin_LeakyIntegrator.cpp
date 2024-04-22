#include "marvin/math/marvin_LeakyIntegrator.h"
#include "marvin/library/enable_warnings.h"
namespace Audio {
    template <FloatType SampleType>
    SampleType LeakyIntegrator<SampleType>::operator()(SampleType x, SampleType a) noexcept {
        // y[n] = A * x[n] + (1 - A) * y[n-1]
        const auto res = a * x + (1 - a) * m_prev;
        m_prev = res;
        return res;
    }

    template class LeakyIntegrator<float>;
    template class LeakyIntegrator<double>;
} // namespace Audio
#include "marvin/library/disable_warnings.h"
