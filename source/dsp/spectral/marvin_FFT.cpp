// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/dsp/spectral/marvin_FFT.h"
#include "marvin/library/marvin_Literals.h"
#include "marvin/math/marvin_Math.h"
#include <memory>
#include <cmath>
#include <cassert>
#include <vector>
#include <span>
#include <complex>
#if defined(MARVIN_MACOS)
#include <Accelerate/Accelerate.h>
#endif
#if defined(MARVIN_HAS_IPP)
#include <ippcore.h>
#include <ipps.h>
#include <ippcore_l.h>
#include <ipptypes.h>
#endif

namespace marvin::dsp::spectral {
    template <FloatType SampleType>
    class ImplBase {
    public:
        explicit ImplBase(size_t order) : m_order(order), m_n(static_cast<size_t>(std::pow(2, static_cast<int>(order)))) {
        }

        virtual ~ImplBase() noexcept = default;
        virtual std::span<std::complex<SampleType>> performForwardTransform(std::span<SampleType> source) = 0;
        virtual void performForwardTransform(std::span<SampleType> source, std::span<std::complex<SampleType>> dest) = 0;
        virtual std::span<SampleType> performInverseTransform(std::span<std::complex<SampleType>> source) = 0;
        virtual void performInverseTransform(std::span<std::complex<SampleType>> source, std::span<SampleType> dest) = 0;
        [[nodiscard]] virtual EngineType getEngineType() const noexcept = 0;
        [[nodiscard]] size_t nfft() const noexcept {
            return m_n;
        }

    protected:
        const size_t m_order;
        const size_t m_n;
    };
#if defined(MARVIN_MACOS)

    template <FloatType SampleType>
    class FFT<SampleType>::Impl final : public ImplBase<SampleType> {
    public:
        explicit Impl(size_t order) : ImplBase<SampleType>(order),
                                      m_inverseScalingFactor(static_cast<SampleType>(1.0) / static_cast<SampleType>(this->m_n)) {
            if constexpr (std::is_same_v<SampleType, float>) {
                m_setup = vDSP_create_fftsetup(this->m_order, kFFTRadix2);
            } else {
                m_setup = vDSP_create_fftsetupD(this->m_order, kFFTRadix2);
            }
            m_fwdBuff.realp = new SampleType[this->m_n / 2];
            m_fwdBuff.imagp = new SampleType[this->m_n / 2];
            m_invBuff.realp = new SampleType[this->m_n / 2];
            m_invBuff.imagp = new SampleType[this->m_n / 2];
            m_forwardInternal.resize(this->m_n + 2);
            m_inverseInternal.resize(this->m_n);
            std::fill(m_forwardInternal.begin(), m_forwardInternal.end(), static_cast<SampleType>(0.0));
            std::fill(m_inverseInternal.begin(), m_inverseInternal.end(), static_cast<SampleType>(0.0));
        }

        ~Impl() noexcept override {
            if (m_setup) {
                if constexpr (std::is_same_v<SampleType, float>) {
                    vDSP_destroy_fftsetup(m_setup);
                } else {
                    vDSP_destroy_fftsetupD(m_setup);
                }
            }
            if (m_fwdBuff.realp) {
                delete[] m_fwdBuff.realp;
            }
            if (m_fwdBuff.imagp) {
                delete[] m_fwdBuff.imagp;
            }
            if (m_invBuff.realp) {
                delete[] m_invBuff.realp;
            }
            if (m_invBuff.imagp) {
                delete[] m_invBuff.imagp;
            }
        }

        void performForwardTransform(std::span<SampleType> source, std::span<std::complex<SampleType>> dest) override {
            if constexpr (std::is_same_v<SampleType, float>) {
                vDSP_ctoz((DSPComplex*)source.data(), 2, &m_fwdBuff, 1, this->m_n / 2);
                vDSP_fft_zrip(m_setup, &m_fwdBuff, 1, this->m_order, kFFTDirection_Forward);
                auto asInterleaved = math::complexViewToInterleaved(dest);
                vDSP_ztoc(&m_fwdBuff, 1, (DSPComplex*)asInterleaved.data(), 2, this->m_n / 2);
                vDSP_vsmul(asInterleaved.data(), 1, &m_forwardScalingFactor, asInterleaved.data(), 1, this->m_n);
            } else {
                vDSP_ctozD((DSPDoubleComplex*)source.data(), 2, &m_fwdBuff, 1, this->m_n / 2);
                vDSP_fft_zripD(m_setup, &m_fwdBuff, 1, this->m_order, kFFTDirection_Forward);
                auto asInterleaved = math::complexViewToInterleaved(dest);
                vDSP_ztocD(&m_fwdBuff, 1, (DSPDoubleComplex*)asInterleaved.data(), 2, this->m_n / 2);
                vDSP_vsmulD(asInterleaved.data(), 1, &m_forwardScalingFactor, asInterleaved.data(), 1, this->m_n);
            }
            // CCS format (Apple store DC and Nyquist at 0)
            dest[dest.size() - 1] = dest[0].imag();
            dest[0] = { dest[0].real(), static_cast<SampleType>(0.0) };
        }

        [[nodiscard]] std::span<std::complex<SampleType>> performForwardTransform(std::span<SampleType> source) override {
            auto asComplexView = math::interleavedToComplexView<SampleType>(m_forwardInternal);
            performForwardTransform(source, asComplexView);
            return asComplexView;
        }

        void performInverseTransform(std::span<std::complex<SampleType>> source, std::span<SampleType> dest) override {
            // move nyquist home :)
            source[0] = { source[0].real(), source[source.size() - 1].real() };
            source[source.size() - 1] = static_cast<SampleType>(0.0);
            auto asInterleaved = math::complexViewToInterleaved(source);
            if constexpr (std::is_same_v<SampleType, float>) {
                vDSP_ctoz((DSPComplex*)asInterleaved.data(), 2, &m_invBuff, 1, this->m_n / 2);
                vDSP_fft_zrip(m_setup, &m_invBuff, 1, this->m_order, kFFTDirection_Inverse);
                vDSP_ztoc(&m_invBuff, 1, (DSPComplex*)dest.data(), 2, this->m_n / 2);
                vDSP_vsmul(dest.data(), 1, &m_inverseScalingFactor, dest.data(), 1, this->m_n);
            } else {
                vDSP_ctozD((DSPDoubleComplex*)asInterleaved.data(), 2, &m_invBuff, 1, this->m_n / 2);
                vDSP_fft_zripD(m_setup, &m_invBuff, 1, this->m_order, kFFTDirection_Inverse);
                vDSP_ztocD(&m_invBuff, 1, (DSPDoubleComplex*)dest.data(), 2, this->m_n / 2);
                vDSP_vsmulD(dest.data(), 1, &m_inverseScalingFactor, dest.data(), 1, this->m_n);
            }
        }

        [[nodiscard]] std::span<SampleType> performInverseTransform(std::span<std::complex<SampleType>> source) override {
            performInverseTransform(source, m_inverseInternal);
            return m_inverseInternal;
        }

        [[nodiscard]] EngineType getEngineType() const noexcept override {
            return EngineType::Accelerate_FFT;
        }

    private:
        using FFTSetupType = std::conditional_t<std::is_same_v<SampleType, float>, FFTSetup, FFTSetupD>;
        using DSPSplitBuffType = std::conditional_t<std::is_same_v<SampleType, float>, DSPSplitComplex, DSPDoubleSplitComplex>;
        FFTSetupType m_setup;
        DSPSplitBuffType m_fwdBuff;
        DSPSplitBuffType m_invBuff;
        // https://developer.apple.com/documentation/accelerate/fast_fourier_transforms/data_packing_for_fourier_transforms
        constexpr static auto m_forwardScalingFactor{ static_cast<SampleType>(0.5) };
        const SampleType m_inverseScalingFactor;
        std::vector<SampleType> m_forwardInternal, m_inverseInternal;
    };
#elif defined(MARVIN_HAS_IPP)

    template <FloatType SampleType>
    class FFT<SampleType>::Impl final : public ImplBase<SampleType> {
    public:
        explicit Impl(size_t order) : ImplBase<SampleType>(order) {
            Ipp8u* initBuffer{ nullptr };
            int fftSpecSize, fftInitBuffSize, fftWorkBuffSize;
            [[maybe_unused]] IppStatus status;
            if constexpr (std::is_same_v<SampleType, float>) {
                m_fwdScratchBuff = ippsMalloc_32f(this->m_n + 2);
                m_invScratchBuff = ippsMalloc_32f(this->m_n);

                status = ippsFFTGetSize_R_32f(this->m_order, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone, &fftSpecSize, &fftInitBuffSize, &fftWorkBuffSize);
                assert(status == ippStsNoErr);


                m_specBuffer = ippsMalloc_8u(fftSpecSize);
                m_fftSpec = (IppsFFTSpec_R_32f*)m_specBuffer;
                // m_fftSpec = (IppsFFTSpec_R_32f*)ippsMalloc_8u(fftSpecSize);
                // m_specBuffer = ippsMalloc_8u(fftSpecSize);
                if (fftInitBuffSize != 0) {
                    initBuffer = ippsMalloc_8u(fftInitBuffSize);
                }
                if (fftWorkBuffSize != 0) {
                    m_workBuffer = ippsMalloc_8u(fftWorkBuffSize);
                }
                // status = ippsFFTInit_R_32f(&m_fftSpec, this->m_order, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone, initBuffer, m_workBuffer);
                status = ippsFFTInit_R_32f(&m_fftSpec, this->m_order, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone, m_specBuffer, initBuffer);
                assert(status == ippStsNoErr);


            } else {
                m_fwdScratchBuff = ippsMalloc_64f(this->m_n + 2);
                m_invScratchBuff = ippsMalloc_64f(this->m_n);

                status = ippsFFTGetSize_R_64f(this->m_order, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone, &fftSpecSize, &fftInitBuffSize, &fftWorkBuffSize);
                assert(status == ippStsNoErr);
                m_specBuffer = ippsMalloc_8u(fftSpecSize);
                m_fftSpec = (IppsFFTSpec_R_64f*)m_specBuffer;
                // m_fftSpec = (IppsFFTSpec_R_64f*)ippsMalloc_8u(fftSpecSize);
                // m_specBuffer = ippsMalloc_8u(fftSpecSize);
                initBuffer = ippsMalloc_8u(fftInitBuffSize);
                m_workBuffer = ippsMalloc_8u(fftWorkBuffSize);
                // status = ippsFFTInit_R_64f(&m_fftSpec, this->m_order, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone, initBuffer, m_workBuffer);
                status = ippsFFTInit_R_64f(&m_fftSpec, this->m_order, IPP_FFT_DIV_FWD_BY_N, ippAlgHintNone, m_specBuffer, initBuffer);
                assert(status == ippStsNoErr);
            }
            if (initBuffer) {
                ippFree(initBuffer);
            }
        }

        ~Impl() noexcept override {
            if (m_workBuffer) {
                ippFree(m_workBuffer);
            }
            if (m_specBuffer) {
                ippFree(m_specBuffer);
            }
            if (m_fwdScratchBuff) {
                ippFree(m_fwdScratchBuff);
            }
            if (m_invScratchBuff) {
                ippFree(m_invScratchBuff);
            }
        }

        void performForwardTransform(std::span<SampleType> source, std::span<std::complex<SampleType>> dest) override {
            if constexpr (std::is_same_v<SampleType, float>) {
                assert(dest.size() == (this->m_n / 2) + 1);
                auto asInterleaved = math::complexViewToInterleaved(dest);
                [[maybe_unused]] auto status = ippsFFTFwd_RToCCS_32f(source.data(), asInterleaved.data(), m_fftSpec, m_workBuffer);
                assert(status == ippStsNoErr);

            } else {
                assert(dest.size() == (this->m_n / 2) + 1);
                auto asInterleaved = math::complexViewToInterleaved(dest);
                [[maybe_unused]] auto status = ippsFFTFwd_RToCCS_64f(source.data(), asInterleaved.data(), m_fftSpec, m_workBuffer);
                assert(status == ippStsNoErr);
            }
        }

        std::span<std::complex<SampleType>> performForwardTransform(std::span<SampleType> source) override {
            auto asComplexView = marvin::math::interleavedToComplexView<SampleType>({ m_fwdScratchBuff, this->m_n + 2 });
            performForwardTransform(source, asComplexView);
            return asComplexView;
        }


        void performInverseTransform(std::span<std::complex<SampleType>> source, std::span<SampleType> dest) override {
            assert(dest.size() == this->m_n);
            auto asInterleaved = marvin::math::complexViewToInterleaved(source);
            if constexpr (std::is_same_v<SampleType, float>) {
                [[maybe_unused]] auto status = ippsFFTInv_CCSToR_32f(asInterleaved.data(), dest.data(), m_fftSpec, m_workBuffer);
                assert(status == ippStsNoErr);
            } else {
                [[maybe_unused]] auto status = ippsFFTInv_CCSToR_64f(asInterleaved.data(), dest.data(), m_fftSpec, m_workBuffer);
                assert(status == ippStsNoErr);
            }
        }

        std::span<SampleType> performInverseTransform(std::span<std::complex<SampleType>> source) override {
            std::span<SampleType> res{ m_invScratchBuff, this->m_n };
            performInverseTransform(source, res);
            return { m_invScratchBuff, this->m_n };
        }


        [[nodiscard]] EngineType getEngineType() const noexcept override {
            return EngineType::Ipp_FFT;
        }

    private:
        using IppsFFTSpec = std::conditional_t<std::is_same_v<SampleType, float>, IppsFFTSpec_R_32f, IppsFFTSpec_R_64f>;
        using IppsBufferType = std::conditional_t<std::is_same_v<SampleType, float>, Ipp32f, Ipp64f>;
        using IppsComplexBufferType = std::conditional_t<std::is_same_v<SampleType, float>, Ipp32fc, Ipp64fc>;
        IppsFFTSpec* m_fftSpec{ nullptr };
        Ipp8u* m_specBuffer{ nullptr };
        Ipp8u* m_workBuffer{ nullptr };
        IppsBufferType* m_fwdScratchBuff{ nullptr };
        IppsBufferType* m_invScratchBuff{ nullptr };
    };
#else

    template <FloatType SampleType>
    class Impl final : public ImplBase<SampleType> {
    public:
        ~Impl() noexcept override = default;
        [[nodiscard]] EngineType getEngineType() const noexcept override {
            return EngineEngineType::Fallback_FFT;
        }
    };
#endif

    template <FloatType SampleType>
    FFT<SampleType>::FFT(size_t order) {
        m_impl = std::make_unique<FFT<SampleType>::Impl>(order);
    }

    template <FloatType SampleType>
    FFT<SampleType>::~FFT() noexcept {
    }

    template <FloatType SampleType>
    EngineType FFT<SampleType>::getEngineType() const noexcept {
        return m_impl->getEngineType();
    }

    template <FloatType SampleType>
    size_t FFT<SampleType>::getFFTSize() const noexcept {
        return m_impl->nfft();
    }

    template <FloatType SampleType>
    std::span<std::complex<SampleType>> FFT<SampleType>::performForwardTransform(std::span<SampleType> source) {
        return m_impl->performForwardTransform(source);
    }

    template <FloatType SampleType>
    void FFT<SampleType>::performForwardTransform(std::span<SampleType> source, std::span<std::complex<SampleType>> dest) {
        m_impl->performForwardTransform(source, dest);
    }

    template <FloatType SampleType>
    std::span<SampleType> FFT<SampleType>::performInverseTransform(std::span<std::complex<SampleType>> source) {
        return m_impl->performInverseTransform(source);
    }

    template <FloatType SampleType>
    void FFT<SampleType>::performInverseTransform(std::span<std::complex<SampleType>> source, std::span<SampleType> dest) {
        m_impl->performInverseTransform(source, dest);
    }

    template class FFT<float>;
    template class FFT<double>;

} // namespace marvin::dsp::spectral