// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/math/marvin_VecOps.h"
#include "marvin/library/marvin_Concepts.h"
#if defined(MARVIN_MACOS)
#include <Accelerate/Accelerate.h>
#elif defined(MARVIN_HAS_IPP)
#include <ipp.h>
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

#elif defined(MARVIN_HAS_IPP)
    template <>
    void add<float>(float* lhs, const float* rhs, size_t size) noexcept {
        // get absolutely fucked mississippi bastard
        ippsAdd_32f_I(rhs, lhs, static_cast<int>(size));
    }

    template <>
    void add<double>(double* lhs, const double* rhs, size_t size) noexcept {
        ippsAdd_64f_I(rhs, lhs, static_cast<int>(size));
    }

    template <>
    void add<float>(float* arr, float scalar, size_t size) noexcept {
        ippsAddC_32f_I(scalar, arr, static_cast<int>(size));
    }

    template <>
    void add<double>(double* arr, double scalar, size_t size) noexcept {
        ippsAddC_64f_I(scalar, arr, static_cast<int>(size));
    }

    template <>
    void subtract<float>(float* lhs, const float* rhs, size_t size) noexcept {
        ippsSub_32f_I(rhs, lhs, static_cast<int>(size));
    }

    template <>
    void subtract<double>(double* lhs, const double* rhs, size_t size) noexcept {
        ippsSub_64f_I(rhs, lhs, static_cast<int>(size));
    }

    template <>
    void subtract<float>(float* arr, float scalar, size_t size) noexcept {
        ippsSubC_32f_I(scalar, arr, static_cast<int>(size));
    }

    template <>
    void subtract<double>(double* arr, double scalar, size_t size) noexcept {
        ippsSubC_64f_I(scalar, arr, static_cast<int>(size));
    }

    template <>
    void multiply<float>(float* lhs, const float* rhs, size_t size) noexcept {
        ippsMul_32f_I(rhs, lhs, static_cast<int>(size));
    }

    template <>
    void multiply<double>(double* lhs, const double* rhs, size_t size) noexcept {
        ippsMul_64f_I(rhs, lhs, static_cast<int>(size));
    }

    template <>
    void multiply<float>(float* arr, float scalar, size_t size) noexcept {
        ippsMulC_32f_I(scalar, arr, static_cast<int>(size));
    }

    template <>
    void multiply<double>(double* arr, double scalar, size_t size) noexcept {
        ippsMulC_64f_I(scalar, arr, static_cast<int>(size));
    }

    template <>
    void divide<float>(float* lhs, const float* rhs, size_t size) noexcept {
        ippsDiv_32f_I(rhs, lhs, static_cast<int>(size));
    }

    template <>
    void divide<double>(double* lhs, const double* rhs, size_t size) noexcept {
        ippsDiv_64f_I(rhs, lhs, static_cast<int>(size));
    }

    template <>
    void divide<float>(float* arr, float scalar, size_t size) noexcept {
        ippsDivC_32f_I(scalar, arr, static_cast<int>(size));
    }

    template <>
    void divide<double>(double* arr, double scalar, size_t size) noexcept {
        ippsDivC_64f_I(scalar, arr, static_cast<int>(size));
    }
#else
    template <>
    void add<float>(float* lhs, const float* rhs, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            lhs[i] += rhs[i];
        }
    }

    template <>
    void add<double>(double* lhs, const double* rhs, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            lhs[i] += rhs[i];
        }
    }

    template <>
    void add<float>(float* arr, float scalar, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            arr[i] += scalar;
        }
    }

    template <>
    void add<double>(double* arr, double scalar, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            arr[i] += scalar;
        }
    }

    template <>
    void subtract<float>(float* lhs, const float* rhs, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            lhs[i] -= rhs[i];
        }
    }

    template <>
    void subtract<double>(double* lhs, const double* rhs, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            lhs[i] -= rhs[i];
        }
    }

    template <>
    void subtract<float>(float* arr, float scalar, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            arr[i] -= scalar;
        }
    }

    template <>
    void subtract<double>(double* arr, double scalar, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            arr[i] -= scalar;
        }
    }

    template <>
    void multiply<float>(float* lhs, const float* rhs, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            lhs[i] *= rhs[i];
        }
    }

    template <>
    void multiply<double>(double* lhs, const double* rhs, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            lhs[i] *= rhs[i];
        }
    }

    template <>
    void multiply<float>(float* arr, float scalar, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            arr[i] *= scalar;
        }
    }

    template <>
    void multiply<double>(double* arr, double scalar, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            arr[i] *= scalar;
        }
    }

    template <>
    void divide<float>(float* lhs, const float* rhs, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            lhs[i] /= rhs[i];
        }
    }

    template <>
    void divide<double>(double* lhs, const double* rhs, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            lhs[i] /= rhs[i];
        }
    }

    template <>
    void divide<float>(float* arr, float scalar, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            arr[i] /= scalar;
        }
    }

    template <>
    void divide<double>(double* arr, double scalar, size_t size) noexcept {
        for (auto i = 0_sz; i < size; ++i) {
            arr[i] /= scalar;
        }
    }


#endif


} // namespace marvin::vecops