// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_LEAKYINTEGRATOR_H
#define MARVIN_LEAKYINTEGRATOR_H
#include "marvin/library/marvin_Concepts.h"
#include "marvin/library/enable_warnings.h"
namespace marvin::math {
    template <FloatType SampleType>
    class LeakyIntegrator final {
    public:
        [[nodiscard]] SampleType operator()(SampleType x, SampleType a) noexcept;

    private:
        SampleType m_prev;
    };
} // namespace marvin::math
#include "marvin/library/disable_warnings.h"
#endif
