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
#include "marvin/library/marvin_EnableWarnings.h"
namespace marvin::math {
    /**
        \brief An integrator of a continuous signal which leaks a small amount of said signal over time.

        https://en.wikipedia.org/wiki/Leaky_integrator
    */
    template <FloatType SampleType>
    class LeakyIntegrator final {
    public:
        /**
            Processes the next tick of the integrator.
            \param x The input sample.
            \param a The rate of of the leak.
            \return The leakily-integrated signal.
        */
        [[nodiscard]] SampleType operator()(SampleType x, SampleType a) noexcept;

    private:
        SampleType m_prev;
    };
} // namespace marvin::math
#include "marvin/library/marvin_DisableWarnings.h"
#endif
