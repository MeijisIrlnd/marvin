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
#include <ippcore.h>
#include <ipps.h>
#include <ippcore_l.h>
#include <ipptypes.h>
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
    class Impl final : public ImplBase<SampleType> {
    public:
        ~Impl() noexcept override = default;
        [[nodiscard]] EngineType getEngineType() const noexcept override {
            return EngineType::Accelerate_FFT;
        }
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