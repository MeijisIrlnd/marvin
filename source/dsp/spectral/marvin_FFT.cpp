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
#include "marvin/library/marvin_Concepts.h"
#include "marvin/library/marvin_Literals.h"
#include "marvin/math/marvin_Math.h"
#include "marvin/math/marvin_VecOps.h"
#include <memory>
#include <cmath>
#include <cassert>
#include <type_traits>
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
    template <RealOrComplexFloatType SampleType>
    class ImplBase {
    public:
        using ValueType = typename getValueType<SampleType>::ValueType;
        explicit ImplBase(size_t order) : m_order(order), m_n(static_cast<size_t>(std::pow(2, static_cast<int>(order)))) {
        }

        virtual ~ImplBase() noexcept = default;
        virtual void forward(std::span<SampleType> source, std::span<std::complex<ValueType>> dest) = 0;
        virtual std::span<std::complex<ValueType>> forward(std::span<SampleType> source) = 0;
        virtual void inverse(std::span<std::complex<ValueType>> source, std::span<SampleType> dest) = 0;
        virtual std::span<SampleType> inverse(std::span<std::complex<ValueType>> source) = 0;
        [[nodiscard]] virtual EngineType getEngineType() const noexcept = 0;
        [[nodiscard]] size_t nfft() const noexcept {
            return m_n;
        }

    protected:
        const size_t m_order;
        const size_t m_n;
    };
#if defined(MARVIN_MACOS) && !defined(MARVIN_FORCE_FALLBACK_FFT)

    template <RealOrComplexFloatType SampleType>
    class FFT<SampleType>::Impl final : public ImplBase<SampleType> {
    public:
        explicit Impl(size_t order) : ImplBase<SampleType>(order) {
            if constexpr (std::is_same_v<ValueType, float>) {
                m_setup = vDSP_create_fftsetup(this->m_order, kFFTRadix2);
            } else {
                m_setup = vDSP_create_fftsetupD(this->m_order, kFFTRadix2);
            }
            const auto Size = ComplexFloatType<SampleType> ? this->m_n : this->m_n / 2;
            m_fwdBuff.realp = new ValueType[Size];
            m_fwdBuff.imagp = new ValueType[Size];
            m_invBuff.realp = new ValueType[Size];
            m_invBuff.imagp = new ValueType[Size];

            if constexpr (ComplexFloatType<SampleType>) {
                m_forwardInternal.resize(Size);
            } else {
                m_forwardInternal.resize(Size + 1);
            }
            m_inverseInternal.resize(this->m_n);
            std::fill(m_forwardInternal.begin(), m_forwardInternal.end(), static_cast<SampleType>(0.0));
            std::fill(m_inverseInternal.begin(), m_inverseInternal.end(), static_cast<SampleType>(0.0));
        }

        ~Impl() noexcept override {
            if (m_setup) {
                if constexpr (std::same_as<ValueType, float>) {
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

        void forward(std::span<SampleType> source, std::span<std::complex<ValueType>> dest) override {
            // https://developer.apple.com/documentation/accelerate/data_packing_for_fourier_transforms
            if constexpr (ComplexFloatType<SampleType>) {
                if constexpr (std::same_as<ValueType, float>) {
                    vDSP_ctoz(reinterpret_cast<DSPComplex*>(source.data()), 2, &m_fwdBuff, 1, this->m_n);
                    vDSP_fft_zip(m_setup, &m_fwdBuff, 1, this->m_order, kFFTDirection_Forward);
                    std::span<ValueType> asInterleaved = math::complexViewToInterleaved<ValueType>(dest);
                    vDSP_ztoc(&m_fwdBuff, 1, (DSPComplex*)asInterleaved.data(), 2, this->m_n);
                } else {
                    vDSP_ctozD(reinterpret_cast<DSPDoubleComplex*>(source.data()), 2, &m_fwdBuff, 1, this->m_n);
                    vDSP_fft_zipD(m_setup, &m_fwdBuff, 1, this->m_order, kFFTDirection_Forward);
                    auto asInterleaved = math::complexViewToInterleaved(dest);
                    vDSP_ztocD(&m_fwdBuff, 1, (DSPDoubleComplex*)asInterleaved.data(), 2, this->m_n);
                }
            } else {
                constexpr static auto scalingFactor = static_cast<ValueType>(0.5);
                if constexpr (std::same_as<ValueType, float>) {
                    vDSP_ctoz((DSPComplex*)source.data(), 2, &m_fwdBuff, 1, this->m_n / 2);
                    vDSP_fft_zrip(m_setup, &m_fwdBuff, 1, this->m_order, kFFTDirection_Forward);
                    auto asInterleaved = math::complexViewToInterleaved(dest);
                    vDSP_ztoc(&m_fwdBuff, 1, (DSPComplex*)asInterleaved.data(), 2, this->m_n / 2);
                    vDSP_vsmul(asInterleaved.data(), 1, &scalingFactor, asInterleaved.data(), 1, this->m_n);
                } else {
                    vDSP_ctozD((DSPDoubleComplex*)source.data(), 2, &m_fwdBuff, 1, this->m_n / 2);
                    vDSP_fft_zripD(m_setup, &m_fwdBuff, 1, this->m_order, kFFTDirection_Forward);
                    auto asInterleaved = math::complexViewToInterleaved(dest);
                    vDSP_ztocD(&m_fwdBuff, 1, (DSPDoubleComplex*)asInterleaved.data(), 2, this->m_n / 2);
                    vDSP_vsmulD(asInterleaved.data(), 1, &scalingFactor, asInterleaved.data(), 1, this->m_n);
                }
                dest[dest.size() - 1] = dest[0].imag();
                dest[0] = { dest[0].real(), static_cast<ValueType>(0.0) };
            }
        }

        std::span<std::complex<ValueType>> forward(std::span<SampleType> source) override {
            forward(source, m_forwardInternal);
            return m_forwardInternal;
        }

        void inverse(std::span<std::complex<ValueType>> source, std::span<SampleType> dest) override {
            const auto scalingFactor = static_cast<ValueType>(1.0) / static_cast<ValueType>(this->m_n);
            auto asInterleaved = math::complexViewToInterleaved(source);
            if constexpr (ComplexFloatType<SampleType>) {
                auto interleavedDest = math::complexViewToInterleaved(dest);
                if constexpr (std::same_as<ValueType, float>) {
                    vDSP_ctoz((DSPComplex*)asInterleaved.data(), 2, &m_invBuff, 1, this->m_n);
                    vDSP_fft_zip(m_setup, &m_invBuff, 1, this->m_order, kFFTDirection_Inverse);
                    vDSP_ztoc(&m_invBuff, 1, (DSPComplex*)dest.data(), 2, this->m_n);
                    vDSP_vsmul(interleavedDest.data(), 1, &scalingFactor, interleavedDest.data(), 1, this->m_n * 2);
                } else {
                    vDSP_ctozD((DSPDoubleComplex*)asInterleaved.data(), 2, &m_invBuff, 1, this->m_n);
                    vDSP_fft_zipD(m_setup, &m_invBuff, 1, this->m_order, kFFTDirection_Inverse);
                    vDSP_ztocD(&m_invBuff, 1, (DSPDoubleComplex*)dest.data(), 2, this->m_n);
                    vDSP_vsmulD(interleavedDest.data(), 1, &scalingFactor, interleavedDest.data(), 1, this->m_n * 2);
                }
            } else {
                source[0] = { source[0].real(), source[source.size() - 1].real() };
                source[source.size() - 1] = static_cast<ValueType>(0.0);
                if constexpr (std::same_as<ValueType, float>) {
                    vDSP_ctoz((DSPComplex*)asInterleaved.data(), 2, &m_invBuff, 1, this->m_n / 2);
                    vDSP_fft_zrip(m_setup, &m_invBuff, 1, this->m_order, kFFTDirection_Inverse);
                    vDSP_ztoc(&m_invBuff, 1, (DSPComplex*)dest.data(), 2, this->m_n / 2);
                    vDSP_vsmul(dest.data(), 1, &scalingFactor, dest.data(), 1, this->m_n);
                } else {
                    vDSP_ctozD((DSPDoubleComplex*)asInterleaved.data(), 2, &m_invBuff, 1, this->m_n / 2);
                    vDSP_fft_zripD(m_setup, &m_invBuff, 1, this->m_order, kFFTDirection_Inverse);
                    vDSP_ztocD(&m_invBuff, 1, (DSPDoubleComplex*)dest.data(), 2, this->m_n / 2);
                    vDSP_vsmulD(dest.data(), 1, &scalingFactor, dest.data(), 1, this->m_n);
                }
            }
        }

        std::span<SampleType> inverse(std::span<std::complex<ValueType>> source) override {
            inverse(source, m_inverseInternal);
            return m_inverseInternal;
        }

        [[nodiscard]] EngineType getEngineType() const noexcept override {
            return EngineType::Accelerate_FFT;
        }

    private:
        using FFTSetupType = std::conditional_t<std::is_same_v<ValueType, float>, FFTSetup, FFTSetupD>;
        using DSPSplitBuffType = std::conditional_t<std::is_same_v<ValueType, float>, DSPSplitComplex, DSPDoubleSplitComplex>;
        FFTSetupType m_setup;
        DSPSplitBuffType m_fwdBuff;
        DSPSplitBuffType m_invBuff;
        // https://developer.apple.com/documentation/accelerate/fast_fourier_transforms/data_packing_for_fourier_transforms
        std::vector<SampleType> m_inverseInternal;
        std::vector<std::complex<ValueType>> m_forwardInternal;
    };
#elif defined(MARVIN_HAS_IPP) && !defined(MARVIN_FORCE_FALLBACK_FFT)

    template <RealOrComplexFloatType T>
    struct State {
    };

    template <FloatType T>
    struct State<T> {
        using IppsFFTSpec = std::conditional_t<std::same_as<T, float>, IppsFFTSpec_R_32f, IppsFFTSpec_R_64f>;
        using IppsBufferType = std::conditional_t<std::is_same_v<T, float>, Ipp32f, Ipp64f>;
        IppsFFTSpec* spec{ nullptr };
        Ipp8u* specBuff{ nullptr };
        Ipp8u* workBuff{ nullptr };
        IppsBufferType* fwdScratchBuff{ nullptr };
        IppsBufferType* invScratchBuff{ nullptr };
    };

    template <ComplexFloatType T>
    struct State<T> {
        using ValueType = getValueType<T>::ValueType;
        using IppsFFTSpec = std::conditional_t<std::same_as<ValueType, float>, IppsFFTSpec_C_32fc, IppsFFTSpec_C_64fc>;
        using IppsBufferType = std::conditional_t<std::same_as<ValueType, float>, Ipp32fc, Ipp64fc>;
        IppsFFTSpec* spec{ nullptr };
        Ipp8u* specBuff{ nullptr };
        Ipp8u* workBuff{ nullptr };
        IppsBufferType* fwdScratchBuff{ nullptr };
        IppsBufferType* invScratchBuff{ nullptr };
    };


    template <RealOrComplexFloatType SampleType>
    class FFT<SampleType>::Impl final : public ImplBase<SampleType> {
    public:
        explicit Impl(size_t order) : ImplBase<SampleType>(order) {
            Ipp8u* initBuffer{ nullptr };
            int fftSpecSize, fftInitBuffSize, fftWorkBuffSize;
            [[maybe_unused]] IppStatus status;
            if constexpr (std::same_as<ValueType, float>) {
                if constexpr (ComplexFloatType<SampleType>) {
                    m_state.fwdScratchBuff = ippsMalloc_32fc(static_cast<int>(this->m_n) + 2);
                    status = ippsFFTGetSize_C_32fc(static_cast<int>(this->m_order), IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, &fftSpecSize, &fftInitBuffSize, &fftWorkBuffSize);
                    assert(status == ippStsNoErr);
                    m_state.specBuff = ippsMalloc_8u(fftSpecSize);
                    m_state.spec = (IppsFFTSpec_C_32fc*)m_state.specBuff;
                    if (fftInitBuffSize != 0) {
                        initBuffer = ippsMalloc_8u(fftInitBuffSize);
                    }
                    if (fftWorkBuffSize != 0) {
                        m_state.workBuff = ippsMalloc_8u(fftWorkBuffSize);
                    }
                    status = ippsFFTInit_C_32fc(&m_state.spec, static_cast<int>(this->m_order), IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, m_state.specBuff, initBuffer);
                    assert(status == ippStsNoErr);
                    if (initBuffer) {
                        ippFree(initBuffer);
                    }
                } else {
                    m_state.fwdScratchBuff = ippsMalloc_32f(static_cast<int>(this->m_n) + 2);
                    status = ippsFFTGetSize_R_32f(static_cast<int>(this->m_order), IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, &fftSpecSize, &fftInitBuffSize, &fftWorkBuffSize);
                    assert(status == ippStsNoErr);
                    m_state.specBuff = ippsMalloc_8u(fftSpecSize);
                    m_state.spec = (IppsFFTSpec_R_32f*)m_state.specBuff;
                    if (fftInitBuffSize != 0) {
                        initBuffer = ippsMalloc_8u(fftInitBuffSize);
                    }
                    if (fftWorkBuffSize != 0) {
                        m_state.workBuff = ippsMalloc_8u(fftWorkBuffSize);
                    }
                    status = ippsFFTInit_R_32f(&m_state.spec, static_cast<int>(this->m_order), IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, m_state.specBuff, initBuffer);
                    assert(status == ippStsNoErr);
                    if (initBuffer) {
                        ippFree(initBuffer);
                    }
                }
            } else {
                if constexpr (ComplexFloatType<SampleType>) {
                    m_state.fwdScratchBuff = ippsMalloc_64fc(static_cast<int>(this->m_n) + 2);
                    status = ippsFFTGetSize_C_64fc(static_cast<int>(this->m_order), IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, &fftSpecSize, &fftInitBuffSize, &fftWorkBuffSize);
                    assert(status == ippStsNoErr);
                    m_state.specBuff = ippsMalloc_8u(fftSpecSize);
                    m_state.spec = (IppsFFTSpec_C_64fc*)m_state.specBuff;
                    if (fftInitBuffSize != 0) {
                        initBuffer = ippsMalloc_8u(fftInitBuffSize);
                    }
                    if (fftWorkBuffSize != 0) {
                        m_state.workBuff = ippsMalloc_8u(fftWorkBuffSize);
                    }
                    status = ippsFFTInit_C_64fc(&m_state.spec, static_cast<int>(this->m_order), IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, m_state.specBuff, initBuffer);
                    assert(status == ippStsNoErr);
                    if (initBuffer) {
                        ippFree(initBuffer);
                    }

                } else {
                    m_state.fwdScratchBuff = ippsMalloc_64f(static_cast<int>(this->m_n) + 2);
                    status = ippsFFTGetSize_R_64f(static_cast<int>(this->m_order), IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, &fftSpecSize, &fftInitBuffSize, &fftWorkBuffSize);
                    assert(status == ippStsNoErr);
                    m_state.specBuff = ippsMalloc_8u(fftSpecSize);
                    m_state.spec = (IppsFFTSpec_R_64f*)m_state.specBuff;
                    if (fftInitBuffSize != 0) {
                        initBuffer = ippsMalloc_8u(fftInitBuffSize);
                    }
                    if (fftWorkBuffSize != 0) {
                        m_state.workBuff = ippsMalloc_8u(fftWorkBuffSize);
                    }
                    status = ippsFFTInit_R_64f(&m_state.spec, static_cast<int>(this->m_order), IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, m_state.specBuff, initBuffer);
                    assert(status == ippStsNoErr);
                    if (initBuffer) {
                        ippFree(initBuffer);
                    }
                }
            }
        }

        ~Impl() noexcept override {
            if (m_state.workBuff) {
                ippFree(m_state.workBuff);
            }
            if (m_state.specBuff) {
                ippFree(m_state.specBuff);
            }
            if (m_state.fwdScratchBuff) {
                ippFree(m_state.fwdScratchBuff);
            }
            if (m_state.invScratchBuff) {
                ippFree(m_state.invScratchBuff);
            }
        }

        void forward(std::span<SampleType> source, std::span<std::complex<ValueType>> dest) override {
            if constexpr (ComplexFloatType<SampleType>) {
                assert(source.size() == dest.size());
                assert(dest.size() == this->m_n);
                using ComplexPointer = std::conditional_t<std::same_as<ValueType, float>, Ipp32fc*, Ipp64fc*>;
                auto* srcPtr = reinterpret_cast<ComplexPointer>(source.data());
                auto* dstPtr = reinterpret_cast<ComplexPointer>(dest.data());
                if constexpr (std::same_as<ValueType, float>) {
                    [[maybe_unused]] const auto status = ippsFFTFwd_CToC_32fc(srcPtr, dstPtr, m_state.spec, m_state.workBuff);
                    assert(status == ippStsNoErr);
                } else {
                    [[maybe_unused]] const auto status = ippsFFTFwd_CToC_64fc(srcPtr, dstPtr, m_state.spec, m_state.workBuff);
                    assert(status == ippStsNoErr);
                }
            } else {
                assert(source.size() == this->m_n);
                assert(dest.size() == (this->m_n / 2) + 1);
                auto asInterleaved = math::complexViewToInterleaved(dest);
                if constexpr (std::same_as<ValueType, float>) {
                    [[maybe_unused]] const auto status = ippsFFTFwd_RToCCS_32f(source.data(), asInterleaved.data(), m_state.spec, m_state.workBuff);
                    assert(status == ippStsNoErr);
                } else {
                    [[maybe_unused]] const auto status = ippsFFTFwd_RToCCS_64f(source.data(), asInterleaved.data(), m_state.spec, m_state.workBuff);
                    assert(status == ippStsNoErr);
                }
            }
        }

        std::span<std::complex<ValueType>> forward(std::span<SampleType> source) override {
            if constexpr (ComplexFloatType<SampleType>) {
                std::span<std::complex<ValueType>> complexView{ reinterpret_cast<std::complex<ValueType>*>(m_state.fwdScratchBuff), this->m_n };
                forward(source, complexView);
                return complexView;
            } else {
                std::span<std::complex<ValueType>> complexView{ reinterpret_cast<std::complex<ValueType>*>(m_state.fwdScratchBuff), (this->m_n / 2) + 1 };
                forward(source, complexView);
                return complexView;
            }
        }

        void inverse(std::span<std::complex<ValueType>> source, std::span<SampleType> dest) override {
            if constexpr (ComplexFloatType<SampleType>) {
                assert(source.size() == dest.size());
                assert(dest.size() == this->m_n);
                using ComplexPointer = std::conditional_t<std::same_as<ValueType, float>, Ipp32fc*, Ipp64fc*>;
                auto* srcPtr = reinterpret_cast<ComplexPointer>(source.data());
                auto* dstPtr = reinterpret_cast<ComplexPointer>(dest.data());
                if constexpr (std::same_as<ValueType, float>) {
                    [[maybe_unused]] const auto status = ippsFFTInv_CToC_32fc(srcPtr, dstPtr, m_state.spec, m_state.workBuff);
                    assert(status == ippStsNoErr);
                } else {
                    [[maybe_unused]] const auto status = ippsFFTInv_CToC_64fc(srcPtr, dstPtr, m_state.spec, m_state.workBuff);
                    assert(status == ippStsNoErr);
                }
            } else {
                assert(source.size() == (this->m_n / 2) + 1);
                assert(dest.size() == this->m_n);
                auto asInterleaved = math::complexViewToInterleaved(source);
                if constexpr (std::same_as<ValueType, float>) {
                    [[maybe_unused]] const auto status = ippsFFTInv_CCSToR_32f(asInterleaved.data(), dest.data(), m_state.spec, m_state.workBuff);
                    assert(status == ippStsNoErr);
                } else {
                    [[maybe_unused]] const auto status = ippsFFTInv_CCSToR_64f(asInterleaved.data(), dest.data(), m_state.spec, m_state.workBuff);
                    assert(status == ippStsNoErr);
                }
            }
        }

        std::span<SampleType> inverse(std::span<std::complex<ValueType>> source) override {
            std::span<SampleType> destView{ reinterpret_cast<SampleType*>(m_state.invScratchBuff), this->m_n };
            inverse(source, destView);
            return destView;
        }


        [[nodiscard]] EngineType getEngineType() const noexcept override {
            return EngineType::Ipp_FFT;
        }

    private:
        State<SampleType> m_state;
    };
#else
    template <RealOrComplexFloatType SampleType>
    class FFT<SampleType>::Impl final : public ImplBase<SampleType> {
    public:
        explicit Impl(size_t order) : ImplBase<SampleType>(order) {
            constexpr static auto twoPi = std::numbers::pi_v<ValueType> * static_cast<ValueType>(2.0);
            m_complexScratchBuff.resize(this->m_n);
            m_forwardInternalBuff.resize(this->m_n);
            m_inverseInternalBuff.resize(this->m_n);
            m_inverseComplexBuff.resize(this->m_n);
            m_workingBuff.resize(this->m_n);
            std::fill(m_complexScratchBuff.begin(), m_complexScratchBuff.end(), static_cast<SampleType>(0.0));
            for (auto i = 0_sz; i < this->m_n / 2; ++i) {
                const auto phase = -twoPi * (static_cast<ValueType>(i) / static_cast<ValueType>(this->m_n));
                m_twiddleFactors.emplace_back(std::cos(phase), std::sin(phase));
            }
        }

        ~Impl() noexcept override = default;

        void forward(std::span<SampleType> source, std::span<std::complex<ValueType>> dest) override {
            if constexpr (ComplexFloatType<SampleType>) {
                assert(source.size() == dest.size());
                assert(source.size() == this->m_n);
                butterfly<1, false>(this->m_n, source, dest, m_workingBuff);
            } else {
                assert(dest.size() == (this->m_n / 2) + 1);
                fillComplexScratch(source);
                butterfly<1, false>(this->m_n, m_complexScratchBuff, m_forwardInternalBuff, m_workingBuff);
                // Only need (N / 2) + 1 values....
                std::memcpy(dest.data(), m_forwardInternalBuff.data(), sizeof(std::complex<ValueType>) * ((this->m_n / 2) + 1));
            }
        }

        std::span<std::complex<ValueType>> forward(std::span<SampleType> source) override {
            if constexpr (ComplexFloatType<SampleType>) {
                forward(source, m_forwardInternalBuff);
                return m_forwardInternalBuff;
            } else {
                std::span<std::complex<ValueType>> destView{ m_forwardInternalBuff.data(), (this->m_n / 2) + 1 };
                forward(source, destView);
                return destView;
            }
        }

        void inverse(std::span<std::complex<ValueType>> source, std::span<SampleType> dest) override {
            const auto normalisationFactor = static_cast<ValueType>(1.0) / static_cast<ValueType>(this->m_n);
            if constexpr (ComplexFloatType<SampleType>) {
                assert(source.size() == dest.size());
                assert(dest.size() == this->m_n);
                auto interleavedDest = marvin::math::complexViewToInterleaved(dest);
                butterfly<1, true>(this->m_n, source, dest, m_workingBuff);
                math::vecops::multiply(interleavedDest.data(), normalisationFactor, this->m_n * 2);
            } else {
                assert(source.size() == (this->m_n / 2) + 1);
                assert(dest.size() == this->m_n);
                std::memcpy(m_complexScratchBuff.data(), source.data(), sizeof(std::complex<ValueType>) * source.size());
                // Now - the remaining values should be conjugated..
                const auto nyquistIdx{ this->m_n / 2 };
                for (auto i = 1_sz; i < this->m_n / 2; ++i) {
                    const auto negativeIndex = nyquistIdx + i;
                    const auto positiveIndex = nyquistIdx - i;
                    m_complexScratchBuff[negativeIndex] = std::conj(m_complexScratchBuff[positiveIndex]);
                }
                butterfly<1, true>(this->m_n, m_complexScratchBuff, m_inverseComplexBuff, m_workingBuff);
                for (auto i = 0_sz; i < this->m_n; ++i) {
                    dest[i] = m_inverseComplexBuff[i].real() * normalisationFactor;
                }
            }
        }

        std::span<SampleType> inverse(std::span<std::complex<ValueType>> source) override {
            inverse(source, m_inverseInternalBuff);
            return m_inverseInternalBuff;
        }


        [[nodiscard]] EngineType getEngineType() const noexcept override {
            return EngineType::Fallback_FFT;
        }


    private:
        void fillComplexScratch(std::span<SampleType> input) {
            for (auto i = 0_sz; i < input.size(); ++i) {
                m_complexScratchBuff[i] = input[i];
            }
        }


        template <size_t Stride, bool Inverse>
        void butterfly(size_t size, std::span<std::complex<ValueType>> input, std::span<std::complex<ValueType>> output, std::span<std::complex<ValueType>> working) {
            const auto halfSize = size / 2;
            if (size > 2) {
                butterfly<Stride * 2, Inverse>(halfSize, input, working, output);
                input = working;
            }
            for (auto i = 0_sz; i < halfSize; ++i) {
                const auto ipAOffset = 2 * i * Stride;
                const auto ipBOffset = (2 * i + 1) * Stride;
                const auto opAOffset = i * Stride;
                const auto opBOffset = (i + halfSize) * Stride;
                const auto tfStep = m_twiddleFactors.size() * 2 / size;
                for (auto j = 0_sz; j < Stride; ++j) {
                    const auto tfIndex = i * tfStep;
                    auto twiddleFactor = m_twiddleFactors[tfIndex];
                    if constexpr (Inverse) {
                        twiddleFactor = std::conj(twiddleFactor);
                    }
                    const auto a = input[ipAOffset + j];
                    const auto b = twiddleFactor * input[ipBOffset + j];
                    output[opAOffset + j] = a + b;
                    output[opBOffset + j] = a - b;
                }
            }
        }

        std::vector<std::complex<ValueType>> m_twiddleFactors;
        std::vector<std::complex<ValueType>> m_workingBuff{};
        std::vector<std::complex<ValueType>> m_complexScratchBuff{};
        std::vector<std::complex<ValueType>> m_forwardInternalBuff{};
        std::vector<SampleType> m_inverseInternalBuff{};
        std::vector<std::complex<ValueType>> m_inverseComplexBuff{};
    };
#endif

    template <RealOrComplexFloatType SampleType>
    FFT<SampleType>::FFT(size_t order) {
        m_impl = std::make_unique<FFT<SampleType>::Impl>(order);
    }

    template <RealOrComplexFloatType SampleType>
    FFT<SampleType>::~FFT() noexcept {
    }

    template <RealOrComplexFloatType SampleType>
    EngineType FFT<SampleType>::getEngineType() const noexcept {
        return m_impl->getEngineType();
    }

    template <RealOrComplexFloatType SampleType>
    size_t FFT<SampleType>::getFFTSize() const noexcept {
        return m_impl->nfft();
    }

    template <RealOrComplexFloatType SampleType>
    void FFT<SampleType>::forward(std::span<SampleType> source, std::span<std::complex<ValueType>> dest) {
        m_impl->forward(source, dest);
    }

    template <RealOrComplexFloatType SampleType>
    std::span<std::complex<typename FFT<SampleType>::ValueType>> FFT<SampleType>::forward(std::span<SampleType> source) {
        return m_impl->forward(source);
    }

    template <RealOrComplexFloatType SampleType>
    void FFT<SampleType>::inverse(std::span<std::complex<ValueType>> source, std::span<SampleType> dest) {
        m_impl->inverse(source, dest);
    }

    template <RealOrComplexFloatType SampleType>
    std::span<SampleType> FFT<SampleType>::inverse(std::span<std::complex<ValueType>> source) {
        return m_impl->inverse(source);
    }


    template class FFT<float>;
    template class FFT<std::complex<float>>;
    template class FFT<double>;
    template class FFT<std::complex<double>>;

} // namespace marvin::dsp::spectral