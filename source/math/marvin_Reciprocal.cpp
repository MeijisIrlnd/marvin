//
// Created by Syl on 31/08/2024.
//
#include <marvin/math/marvin_Reciprocal.h>
namespace marvin::math {
    template <FloatType T>
    Reciprocal<T>::Reciprocal(ReciprocalRange<T> src, ReciprocalRange<T> dest) {
        const auto kx = (src.centre - src.min) / (src.max - src.centre);
        const auto ky = (dest.centre - dest.min) / (dest.max - dest.min);
        m_a = ((kx * src.max) * dest.min) - ((ky * src.min) * dest.max);
        m_b = (ky * dest.max) - (kx * dest.min);
        m_c = (kx * src.max) - (ky * src.min);
        m_d = (ky - kx);
    }

    template <FloatType T>
    Reciprocal<T>::Reciprocal(ReciprocalRange<T> dest) : Reciprocal<T>::Reciprocal({ .min = static_cast<T>(0.0), .centre = static_cast<T>(0.5), .max = static_cast<T>(1.0) }, dest) {
    }

    template <FloatType T>
    T Reciprocal<T>::operator()(T toMap) noexcept {
        return (m_a + (m_b * toMap)) / (m_c + (m_d * toMap));
    }

    template class Reciprocal<float>;
    template class Reciprocal<double>;


} // namespace marvin::math