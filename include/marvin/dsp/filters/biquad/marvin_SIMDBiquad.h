//
// Created by Syl Morrison on 03/05/2025.
//

#ifndef MARVIN_SIMDBIQUAD_H
#define MARVIN_SIMDBIQUAD_H
#include <marvin/dsp/filters/biquad/marvin_BiquadCoefficients.h>
#include <xsimd/xsimd.hpp>
#include <marvin/library/marvin_Concepts.h>
#include <marvin/math/marvin_VecOps.h>
namespace marvin::dsp::filters {
    /**
     * \brief A SIMD optimised biquad, for running N biquads in parallel.
     *
     * From benchmarks, only gives a speedup in certain cases, and even in those cases, only ~100ns.
     * That being said, a robust parallel structure for filters is arguably nicer than a std::array<filter, N>.
     *
     * @tparam SampleType float or double
     * @tparam N The number of parallel biquads to process
     */
    template <marvin::FloatType SampleType, size_t N>
    requires(N > 0)
    class SIMDBiquad final {
    public:
        /**
         * Constructor
         */
        SIMDBiquad() {
            m_working.resize(N, 0.0);
            m_a0.resize(N, 0.0);
            m_a1.resize(N, 0.0);
            m_a2.resize(N, 0.0);
            m_b1.resize(N, 0.0);
            m_b2.resize(N, 0.0);
            m_x1.resize(N, 0.0);
            m_x2.resize(N, 0.0);
            m_y1.resize(N, 0.0);
            m_y2.resize(N, 0.0);
        }

        /**
         * Sets the coefficients for all filters to the ones passed to the `coeffs` arg
         *
         * @param coeffs A BiquadCoefficients<SampleType> containing the coeffs you want to set.
         */
        auto setCoeffs(BiquadCoefficients<SampleType> coeffs) noexcept -> void {
            m_equalCoeffs = true;
            const auto a0 = coeffs.a0 / coeffs.b0;
            const auto a1 = coeffs.a1 / coeffs.b0;
            const auto a2 = coeffs.a2 / coeffs.b0;
            const auto b1 = coeffs.b1 / coeffs.b0;
            const auto b2 = coeffs.b2 / coeffs.b0;

            const auto a0Batch = xsimd::broadcast(a0);
            const auto a1Batch = xsimd::broadcast(a1);
            const auto a2Batch = xsimd::broadcast(a2);
            const auto b1Batch = xsimd::broadcast(b1);
            const auto b2Batch = xsimd::broadcast(b2);
            for (size_t i = 0; i < m_vecSize; i += m_simdSize) {
                a0Batch.store_aligned(&m_a0[i]);
                a1Batch.store_aligned(&m_a1[i]);
                a2Batch.store_aligned(&m_a2[i]);
                b1Batch.store_aligned(&m_b1[i]);
                b2Batch.store_aligned(&m_b2[i]);
            }
            for (size_t i = m_vecSize; i < N; ++i) {
                m_a0[i] = a0;
                m_a1[i] = a1;
                m_a2[i] = a2;
                m_b1[i] = b1;
                m_b2[i] = b2;
            }
        }

        /**
         * Sets the coefficients for a specific biquad
         * @param index
         * @param coeffs
         */
        auto setCoeffs(size_t index, BiquadCoefficients<SampleType> coeffs) noexcept -> void {
            m_equalCoeffs = false;
            const auto [a0, a1, a2, b0, b1, b2] = coeffs;
            m_a0[index] = a0 / b0;
            m_a1[index] = a1 / b0;
            m_a2[index] = a2 / b0;
            m_b1[index] = b1 / b0;
            m_b2[index] = b2 / b0;
        }

        /**
         * Processes all samples in x through their respective biquads, and overwrites the values in x
         * @param x An array-like containing N samples to be filtered.
         */
        auto operator()(std::span<SampleType, N> x) noexcept -> void {
            constexpr static auto sizeBytes = sizeof(SampleType) * N;
            std::memcpy(m_working.data(), x.data(), sizeBytes);
            for (size_t i = 0; i < m_vecSize; i += m_simdSize) {
                const auto& a0 = xsimd::load_aligned(&m_a0[i]);
                const auto& x0 = xsimd::load_aligned(&m_working[i]);
                const auto a0x0 = a0 * x0;
                const auto& a1 = xsimd::load_aligned(&m_a1[i]);
                const auto& x1 = xsimd::load_aligned(&m_x1[i]);
                const auto a1x1 = a1 * x1;
                const auto& a2 = xsimd::load_aligned(&m_a2[i]);
                const auto& x2 = xsimd::load_aligned(&m_x2[i]);
                const auto a2x2 = a2 * x2;
                const auto& b1 = xsimd::load_aligned(&m_b1[i]);
                const auto& y1 = xsimd::load_aligned(&m_y1[i]);
                const auto b1y1 = b1 * y1;
                const auto& b2 = xsimd::load_aligned(&m_b2[i]);
                const auto& y2 = xsimd::load_aligned(&m_y2[i]);
                const auto b2y2 = b2 * y2;
                const auto res = a0x0 + a1x1 + a2x2 - b1y1 - b2y2;
                x1.store_aligned(&m_x2[i]);
                x0.store_aligned(&m_x1[i]);
                y1.store_aligned(&m_y2[i]);
                res.store_aligned(&m_y1[i]);
            }

            for (size_t i = m_vecSize; i < N; ++i) {
                const auto res = (m_a0[i] * m_working[i]) + (m_a1[i] * m_x1[i]) + (m_a2[i] * m_x2[i]) - (m_b1[i] * m_y1[i]) - (m_b2[i] * m_y2[i]);
                m_x2[i] = m_x1[i];
                m_x1[i] = m_working[i];
                m_y2[i] = m_y1[i];
                m_y1[i] = res;
            }
            std::memcpy(x.data(), m_y1.data(), sizeBytes);
        }

        /**
         * Zeroes all internal state (except coefficients).
         */
        auto reset() noexcept -> void {
            auto batch = xsimd::broadcast(0.0);
            for (size_t i = 0; i < m_vecSize; i += m_simdSize) {
                batch.store_aligned(&m_working[i]);
                batch.store_aligned(&m_x1[i]);
                batch.store_aligned(&m_x2[i]);
                batch.store_aligned(&m_y1[i]);
                batch.store_aligned(&m_y2[i]);
            }
            for (size_t i = m_vecSize; i < N; ++i) {
                m_working[i] = 0.0;
                m_x1[i] = 0.0;
                m_x2[i] = 0.0;
                m_y1[i] = 0.0;
                m_y2[i] = 0.0;
            }
        }

    private:
        constexpr static auto m_simdSize = xsimd::simd_type<SampleType>::size;
        constexpr static auto m_vecSize = N - N % m_simdSize;
        bool m_equalCoeffs{ false };
        std::vector<SampleType, xsimd::aligned_allocator<SampleType>> m_working;
        std::vector<SampleType, xsimd::aligned_allocator<SampleType>> m_a0, m_a1, m_a2, m_b1, m_b2;
        std::vector<SampleType, xsimd::aligned_allocator<SampleType>> m_x1, m_x2, m_y1, m_y2;
    };


} // namespace marvin::dsp::filters
#endif // MARVIN_SIMDBIQUAD_H
