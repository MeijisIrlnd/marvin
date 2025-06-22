// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================
#ifndef MARVIN_SIMDOPS_H
#define MARVIN_SIMDOPS_H
#include <marvin/library/marvin_Concepts.h>
#include <marvin/library/marvin_Literals.h>
#include <type_traits>
#include <cstring>
#include <cassert>
namespace marvin::math::vecops {


    // =============================================================================================
    // Out of place
    /**
        Adds the values of `rhs` to the values of `lhs`, and
        stores the result in `dest`.

        @tparam T float or double.
        @param lhs A raw pointer to the lhs array-like.
        @param rhs A raw pointer to the rhs array-like.
        @param dest A raw pointer to the dest array-like
        @param size The number of elements in `lhs`, `rhs` and `dest`.
    */
    template <FloatType T>
    auto add(const T* lhs, const T* rhs, T* dest, size_t size) noexcept -> void;

    /**
     * Adds `scalar` to the values of `arr`, and stores the result in `dest`.
     * @tparam T float or double.
     * @param arr A raw pointer to the array-like to operate on
     * @param scalar The value to add to the values in `arr`
     * @param dest A raw pointer to the destination array-like
     * @param size The number of elements in `arr` and `dest`.
     */
    template <FloatType T>
    auto add(const T* arr, T scalar, T* dest, size_t size) noexcept -> void;

    /**
     * Adds the values of `rhs` to `lhs`, and stores the result in `dest`.
     * @tparam T Any stl container of type either float or double, satisfying `ArrayLike`.
     * @param lhs The lhs array-like
     * @param rhs The rhs array-like
     * @param dest The destination array-like
     */
    template <FloatArrayLike T>
    auto add(const T& lhs, const T& rhs, T& dest) noexcept -> void {
        const auto size = lhs.size();
        assert(rhs.size() == size && dest.size() == size);
        add(lhs.data(), rhs.data(), dest.data(), size);
    }

    /**
     * Adds the value of `scalar` to the values in `arr`, and stores the result in `dest`.
     *
     * @tparam T Any stl container of type either float or double, satisfying `ArrayLike`.
     * @param arr The array-like to operate on.
     * @param scalar The value to add to the values in `arr`
     * @param dest The destination array-like
     */
    template <FloatArrayLike T>
    auto add(const T& arr, typename T::value_type scalar, T& dest) noexcept -> void {
        const auto size = arr.size();
        assert(dest.size() == size);
        add(arr.data(), scalar, dest.data(), size);
    }

    // In place

    /**
        Adds the values of `rhs` to the values of `lhs`, and
        stores the result in `lhs`.
        \param lhs A raw pointer to the dest array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void add(T* lhs, const T* rhs, size_t size) noexcept;
    /**
        Adds the value of `scalar` to the values of `lhs`,
        and stores the result in `lhs`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to add to each element of `arr`.
        \param size The number of elements in `arr`.
    */
    template <FloatType T>
    void add(T* arr, T scalar, size_t size) noexcept;
    /**
        Adds the values of `rhs` to the values of `lhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <FloatArrayLike T>
    void add(T& lhs, T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        add(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Adds the value of `scalar` to the values of `lhs`,
        and stores the result in `lhs`.
        \param arr The destination array-like.
        \param scalar The value to add to each element of `arr`.
    */
    template <FloatArrayLike T>
    void add(T& arr, typename T::value_type scalar) noexcept {
        add(arr.data(), scalar, arr.size());
    }

    // =============================================================================================
    /**
        Subtracts the values of `rhs` from the values of `lhs`, and
        stores the result in `dest`.

        @tparam T float or double.
        @param lhs A raw pointer to the lhs array-like.
        @param rhs A raw pointer to the rhs array-like.
        @param dest A raw pointer to the dest array-like
        @param size The number of elements in `lhs`, `rhs` and `dest`.
    */
    template <FloatType T>
    auto subtract(const T* lhs, const T* rhs, T* dest, size_t size) noexcept -> void;

    /**
     * Subtracts `scalar` from the values of `arr`, and stores the result in `dest`.
     * @tparam T float or double.
     * @param arr A raw pointer to the array-like to operate on
     * @param scalar The value to subtract from the values in `arr`
     * @param dest A raw pointer to the destination array-like
     * @param size The number of elements in `arr` and `dest`.
     */
    template <FloatType T>
    auto subtract(const T* arr, T scalar, T* dest, size_t size) noexcept -> void;

    /**
     * Subtracts the values of `rhs` from  the values in`lhs`, and stores the result in `dest`.
     * @tparam T Any stl container of type either float or double, satisfying `ArrayLike`.
     * @param lhs The lhs array-like
     * @param rhs The rhs array-like
     * @param dest The destination array-like
     */
    template <FloatArrayLike T>
    auto subtract(const T& lhs, const T& rhs, T& dest) noexcept -> void {
        const auto size = lhs.size();
        assert(rhs.size() == size && dest.size() == size);
        subtract(lhs.data(), rhs.data(), dest.data(), size);
    }

    /**
     * Subtracts the value of `scalar` from the values in `arr`, and stores the result in `dest`.
     *
     * @tparam T Any stl container of type either float or double, satisfying `ArrayLike`.
     * @param arr The array-like to operate on.
     * @param scalar The value to subtract from the values in `arr`
     * @param dest The destination array-like
     */
    template <FloatArrayLike T>
    auto subtract(const T& arr, typename T::value_type scalar, T& dest) noexcept -> void {
        const auto size = arr.size();
        assert(dest.size() == size);
        subtract(arr.data(), scalar, dest.data(), size);
    }

    /**
        Subtracts the values of `rhs` from the values of `lhs`, and
        stores the result in `lhs`.
        \param lhs A raw pointer to the dest array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void subtract(T* lhs, const T* rhs, size_t size) noexcept;
    /**
        Subtracts the value of `scalar` from the values of `lhs`, and
        stores the result in `lhs`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to subtract from each element in `arr`.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void subtract(T* arr, T scalar, size_t size) noexcept;

    /**
        Subtracts the values of `rhs` from the values of `lhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <FloatArrayLike T>
    requires std::is_floating_point_v<typename T::value_type>
    void subtract(T& lhs, const T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        subtract(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Subtracts the value of `scalar` from the values of `lhs`, and
        stores the result in `lhs`.
        \param arr The destination array-like.
        \param scalar The value to subtract from each element in `arr`.
    */
    template <FloatArrayLike T>
    void subtract(T& arr, typename T::value_type scalar) noexcept {
        subtract(arr.data(), scalar, arr.size());
    }

    /**
        Multiplies the values of `rhs` by the values of `lhs`, and
        stores the result in `dest`.

        @tparam T float or double.
        @param lhs A raw pointer to the lhs array-like.
        @param rhs A raw pointer to the rhs array-like.
        @param dest A raw pointer to the dest array-like
        @param size The number of elements in `lhs`, `rhs` and `dest`.
    */
    template <FloatType T>
    auto multiply(const T* lhs, const T* rhs, T* dest, size_t size) noexcept -> void;

    /**
     * Multiples the values of `arr` by `scalar`, and stores the result in `dest`.
     *
     * @tparam T Any stl container of type either float or double, satisfying `ArrayLike`.
     * @param arr A raw pointer to the array-like to operate on.
     * @param scalar The value to multiply the values in `arr` by.
     * @param dest A raw pointer to the destination array-like
     * @param size The size of `arr`, and `dest`.
     */
    template <FloatType T>
    auto multiply(const T* arr, T scalar, T* dest, size_t size) noexcept -> void;

    /**
     * Multiplies the values of `rhs` by the values of `lhs`, and stores the result in `dest`.
     * @tparam T Any stl container of type either float or double, satisfying `ArrayLike`.
     * @param lhs The lhs array-like
     * @param rhs The rhs array-like
     * @param dest The destination array-like
     */
    template <FloatArrayLike T>
    auto multiply(const T& lhs, const T& rhs, T& dest) noexcept -> void {
        const auto size = lhs.size();
        assert(rhs.size() == size && dest.size() == size);
        multiply(lhs.data(), rhs.data(), dest.data(), size);
    }

    /**
     * Multiplies the values of `arr` by `scalar`, and stores the result in `dest`.
     *
     * @tparam T Any stl container of type either float or double, satisfying `ArrayLike`.
     * @param arr The array-like to operate on.
     * @param scalar The value to multiply the values in `arr` by.
     * @param dest The destination array-like.
     */
    template <FloatArrayLike T>
    auto multiply(const T& arr, typename T::value_type scalar, T& dest) noexcept -> void {
        const auto size = arr.size();
        assert(dest.size() == size);
        multiply(arr.data(), scalar, dest.data(), size);
    }

    /**
        Multiplies the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`.
        \param lhs A raw pointer to the dest array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void multiply(T* lhs, const T* rhs, size_t size) noexcept;

    /**
        Multiplies the values of `lhs` by value of `scalar`, and
        stores the result in `lhs`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to multiply each element in `arr` by.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void multiply(T* arr, T scalar, size_t size) noexcept;

    /**
        Multiplies the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <FloatArrayLike T>
    void multiply(T& lhs, const T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        multiply(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Multiplies the values of `lhs` by the value of `scalar`, and
        stores the result in `lhs`.
        \param arr The destination array-like.
        \param scalar The value to multiply each element in `arr` by.
    */
    template <FloatArrayLike T>
    void multiply(T& arr, typename T::value_type scalar) noexcept {
        multiply(arr.data(), scalar, arr.size());
    }

    /**
        Divides the values of `lhs` by the values of `rhs`, and
        stores the result in `dest`.

        @tparam T float or double.
        @param lhs A raw pointer to the lhs array-like.
        @param rhs A raw pointer to the rhs array-like.
        @param dest A raw pointer to the dest array-like
        @param size The number of elements in `lhs`, `rhs` and `dest`.
    */
    template <FloatType T>
    auto divide(const T* lhs, const T* rhs, T* dest, size_t size) noexcept -> void;

    /**
     * Divides the values of `arr` by `scalar`, and stores the result in `dest`.
     *
     * @tparam T Any stl container of type either float or double, satisfying `ArrayLike`.
     * @param arr A raw pointer to the array-like to operate on.
     * @param scalar The value to divide the values in `arr` by.
     * @param dest A raw pointer to the destination array-like
     * @param size The size of `arr` and `dest`
     */
    template <FloatType T>
    auto divide(const T* arr, T scalar, T* dest, size_t size) noexcept -> void;

    /**
     * Divides the values of `lhs` by the values of `rhs`, and stores the result in `dest`.
     * @tparam T Any stl container of type either float or double, satisfying `ArrayLike`.
     * @param lhs The lhs array-like
     * @param rhs The rhs array-like
     * @param dest The destination array-like
     */
    template <FloatArrayLike T>
    auto divide(const T& lhs, const T& rhs, T& dest) noexcept -> void {
        const auto size = lhs.size();
        assert(rhs.size() == size && dest.size() == size);
        divide(lhs.data(), rhs.data(), dest.data(), size);
    }

    /**
     * Divides the values of `arr` by `scalar`, and stores the result in `dest`.
     *
     * @tparam T Any stl container of type either float or double, satisfying `ArrayLike`.
     * @param arr The array-like to operate on.
     * @param scalar The value to divide the values in `arr` by.
     * @param dest The destination array-like.
     */
    template <FloatArrayLike T>
    auto divide(const T& arr, typename T::value_type scalar, T& dest) noexcept -> void {
        const auto size = arr.size();
        assert(dest.size() == size);
        divide(arr.data(), scalar, dest.data(), size);
    }


    /**
        Divides the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`.
        \param lhs A raw pointer to the dest array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void divide(T* lhs, const T* rhs, size_t size) noexcept;

    /**
        Divides the values of `lhs` by the value of `scalar`, and
        stores the result in `lhs`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to divide each element in `arr` by.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void divide(T* arr, T scalar, size_t size) noexcept;

    /**
        Divides the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <FloatArrayLike T>
    void divide(T& lhs, const T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        divide(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Divides the values of `lhs` by the value of `scalar`, and
        stores the result in `lhs`.
        \param arr The destination array-like.
        \param scalar The value to divide each element in `arr` by.
    */
    template <FloatArrayLike T>
    void divide(T& arr, typename T::value_type scalar) noexcept {
        divide(arr.data(), scalar, arr.size());
    }

    /**
        Copies the contents of rhs into lhs.
        \param lhs A raw pointer to the destination array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements to copy.
    */
    template <NumericType T>
    void copy(T* lhs, const T* rhs, size_t size) noexcept {
        std::memcpy(lhs, rhs, sizeof(T) * size);
    }


    /**
        Copies the contents of rhs into lhs.
        `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <ArrayLike T>
    void copy(T& lhs, const T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        copy(lhs.data(), rhs.data(), lhs.size());
    }
} // namespace marvin::math::vecops
#endif