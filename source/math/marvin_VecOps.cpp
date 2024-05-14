#include "marvin/math/marvin_VecOps.h"
#include "marvin/library/marvin_Concepts.h"
#if defined(MARVIN_MACOS)
#include <Accelerate/Accelerate.h>
#endif

namespace marvin::vecops {
#if defined(MARVIN_MACOS)
    template <>
    void add<float>(float* lhs, const float* rhs, size_t size) noexcept {
        vDSP_vadd(lhs, 1, rhs, 1, lhs, 1, size);
    }
    template <>
    void add<double>(double* lhs, const double* rhs, size_t size) noexcept {
        vDSP_vaddD(lhs, 1, rhs, 1, lhs, 1, size);
    }

    template <>
    void add<float>(float* arr, float scalar, size_t size) noexcept {
        vDSP_vsadd(arr, 1, &scalar, arr, 1, size);
    }

    template <>
    void add<double>(double* arr, double scalar, size_t size) noexcept {
        vDSP_vsaddD(arr, 1, &scalar, arr, 1, size);
    }

    template <>
    void subtract<float>(float* lhs, const float* rhs, size_t size) noexcept {
        vDSP_vsub(rhs, 1, lhs, 1, lhs, 1, size);
    }

    template <>
    void subtract<double>(double* lhs, const double* rhs, size_t size) noexcept {
        vDSP_vsubD(rhs, 1, lhs, 1, lhs, 1, size);
    }

    template <>
    void subtract<float>(float* arr, float scalar, size_t size) noexcept {
        const auto invertedScalar = -scalar;
        add(arr, invertedScalar, size);
    }

    template <>
    void subtract<double>(double* arr, double scalar, size_t size) noexcept {
        const auto invertedScalar = -scalar;
        add(arr, invertedScalar, size);
    }

    template <>
    void multiply<float>(float* lhs, const float* rhs, size_t size) noexcept {
        vDSP_vmul(lhs, 1, rhs, 1, lhs, 1, size);
    }

    template <>
    void multiply<double>(double* lhs, const double* rhs, size_t size) noexcept {
        vDSP_vmulD(lhs, 1, rhs, 1, lhs, 1, size);
    }

    template <>
    void multiply<float>(float* arr, float scalar, size_t size) noexcept {
        vDSP_vsmul(arr, 1, &scalar, arr, 1, size);
    }

    template <>
    void multiply<double>(double* arr, double scalar, size_t size) noexcept {
        vDSP_vsmulD(arr, 1, &scalar, arr, 1, size);
    }

    template <>
    void divide<float>(float* lhs, const float* rhs, size_t size) noexcept {
        vDSP_vdiv(rhs, 1, lhs, 1, lhs, 1, size);
    }

    template <>
    void divide<double>(double* lhs, const double* rhs, size_t size) noexcept {
        vDSP_vdivD(rhs, 1, lhs, 1, lhs, 1, size);
    }

    template <>
    void divide<float>(float* arr, float scalar, size_t size) noexcept {
        vDSP_vsdiv(arr, 1, &scalar, arr, 1, size);
    }

    template <>
    void divide<double>(double* arr, double scalar, size_t size) noexcept {
        vDSP_vsdivD(arr, 1, &scalar, arr, 1, size);
    }


#endif


} // namespace marvin::vecops