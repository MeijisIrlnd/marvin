// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <catch2/catch_test_macros.hpp>
#include <marvin/library/marvin_Concepts.h>
#include <span>
#include <memory>
#include <array>
namespace marvin::testing {
    TEST_CASE("Assert FloatType") {
        static_assert(FloatType<float>);
        static_assert(FloatType<double>);
        static_assert(!FloatType<int>);
        static_assert(!FloatType<std::complex<float>>);
        static_assert(!FloatType<std::complex<double>>);
        static_assert(!FloatType<std::string>);
        static_assert(!FloatType<std::span<float>>);
    }

    TEST_CASE("Assert ComplexFloatType") {
        static_assert(!ComplexFloatType<float>);
        static_assert(!ComplexFloatType<double>);
        static_assert(!ComplexFloatType<int>);
        static_assert(ComplexFloatType<std::complex<float>>);
        static_assert(ComplexFloatType<std::complex<double>>);
        static_assert(!ComplexFloatType<std::string>);
        static_assert(!ComplexFloatType<std::span<float>>);
    }

    TEST_CASE("Assert RealOrComplexFloatType") {
        static_assert(RealOrComplexFloatType<float>);
        static_assert(RealOrComplexFloatType<double>);
        static_assert(!RealOrComplexFloatType<int>);
        static_assert(RealOrComplexFloatType<std::complex<float>>);
        static_assert(RealOrComplexFloatType<std::complex<double>>);
        static_assert(!RealOrComplexFloatType<std::string>);
        static_assert(!RealOrComplexFloatType<std::span<float>>);
    }

    TEST_CASE("Assert NumericType") {
        static_assert(NumericType<float>);
        static_assert(NumericType<double>);
        static_assert(NumericType<int>);
        static_assert(!NumericType<std::complex<float>>);
        static_assert(!NumericType<std::complex<double>>);
        static_assert(!NumericType<std::string>);
        static_assert(!NumericType<std::span<float>>);
    }

    TEST_CASE("Assert ArrayLike") {
        static_assert(!ArrayLike<float>);
        static_assert(!ArrayLike<double>);
        static_assert(!ArrayLike<int>);
        static_assert(!ArrayLike<std::complex<float>>);
        static_assert(!ArrayLike<std::complex<double>>);
        static_assert(!ArrayLike<std::string>);
        static_assert(ArrayLike<std::span<float>>);
        static_assert(ArrayLike<std::vector<float>>);
        static_assert(ArrayLike<std::array<int, 16>>);
    }

    TEST_CASE("Assert FloatArrayLike") {
        static_assert(!FloatArrayLike<float>);
        static_assert(!FloatArrayLike<double>);
        static_assert(!FloatArrayLike<int>);
        static_assert(!FloatArrayLike<std::complex<float>>);
        static_assert(!FloatArrayLike<std::complex<double>>);
        static_assert(!FloatArrayLike<std::string>);
        static_assert(FloatArrayLike<std::span<float>>);
        static_assert(FloatArrayLike<std::vector<float>>);
        static_assert(!FloatArrayLike<std::array<int, 16>>);
        static_assert(FloatArrayLike<std::array<double, 16>>);
    }

    TEST_CASE("Assert SmartPointerType") {
        static_assert(SmartPointerType<std::unique_ptr<float>>);
        static_assert(SmartPointerType<std::unique_ptr<double>>);
        static_assert(SmartPointerType<std::unique_ptr<char>>);
        static_assert(SmartPointerType<std::shared_ptr<float>>);
        static_assert(SmartPointerType<std::shared_ptr<double>>);
        static_assert(SmartPointerType<std::shared_ptr<char>>);
        static_assert(!SmartPointerType<double*>);
        static_assert(!SmartPointerType<float*>);
        static_assert(!SmartPointerType<char*>);
    }

    TEST_CASE("Test isPowerOfTwo") {
        static_assert(isPowerOfTwo<1>());
        static_assert(isPowerOfTwo<2>());
        static_assert(!isPowerOfTwo<3>());
        static_assert(isPowerOfTwo<4>());
        static_assert(!isPowerOfTwo<5>());
        static_assert(!isPowerOfTwo<6>());
        static_assert(!isPowerOfTwo<7>());
        static_assert(isPowerOfTwo<8>());
    }

} // namespace marvin::testing
