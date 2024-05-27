// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_STRIDED_VIEW
#define MARVIN_STRIDED_VIEW
#include <type_traits>
#include <iterator>
#include <span>
#include <vector>
namespace marvin::containers {
    /**
        \brief Non owning view into an array-like, with a configurable step size.

        Essentially exists because std::ranges::views isn't implemented (or at least isn't widespread) on Apple Clang at the time of writing.
        A `Stride` of 2 would mean iterating over the `StrideView` would return every second value. Stride <b>cannot</b> == 0.<br>
        Does <b>not</b> require Stride to be even.
        Usage Example:

        ```cpp
                std::vector<int> vec{ 0, 1, 2, 3, 4, 5, 6, 7 };
                marvin::utils::StrideView<int, 2> evenView{ vec };
                for(const auto& el : evenView) {
                    std::cout << el << "\n";
                }
                => [0, 2, 4, 6]

                auto* offsetPtr = vec.data() + 1;
                const auto size = vec.size() - 1;
                marvin::containers::StrideView<int, 2> oddView{ offsetPtr, size };
                for(const auto& el : oddView) {
                    std::cout << el << "\n";
                }
                => [1, 3, 5, 7]

            ```


    */
    template <typename T, size_t Stride>
    requires(Stride > 0)
    class StrideView final {
    public:
        /**
            \brief Custom iterator for StrideView.
        */
        class iterator final {
        public:
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::forward_iterator_tag;

            /**
                Default constructor doesn't make sense - would just be a nullptr waiting to happen.
            */
            iterator() = delete;

            /**
                Creates an iterator from a T*. Implicitly constructable.
                \param ptr A pointer to construct the iterator from.
            */
            iterator(pointer ptr) : m_internal(ptr) {
            }

            /**
                Checks the validity of the underlying pointer.
                \return true if the underlying pointer is not null, false otherwise.
            */
            operator bool() const {
                return m_internal;
            }

            /**
                Dereferences the underlying pointer. Does <b>not</b> check for null.
                \return A reference to the value stored at the underlying pointer.
            */
            reference operator*() const {
                return *m_internal;
            }

            /**
                Retrieves the underlying pointer.
                \return The pointer wrapped by this iterator.
            */
            pointer operator->() const {
                return m_internal;
            }

            /**
                Pre increments the underlying pointer, by `Stride` steps.
                \return An iterator to the incremented underlying pointer (*this).
            */
            iterator& operator++() {
                m_internal += Stride;
                return *this;
            }

            /**
                Post increments the underlying pointer, by `Stride` steps.
                \return An iterator to the previous state of the internal pointer, before the increment.
            */
            iterator operator++(int) {
                auto temp = *this;
                m_internal += Stride;
                return temp;
            }

            /**
                Pre decrements the underlying pointer, by `Stride` steps.
                \return An iterator to the decremented underlying pointer (*this).
            */
            iterator& operator--() {
                m_internal -= Stride;
                return *this;
            }

            /**
                Post decrements the underlying pointer, by `Stride` steps.
                \return An iterator to the previous state of the internal pointer, before the decrement.
            */
            iterator operator--(int) {
                auto temp = *this;
                m_internal -= Stride;
                return temp;
            }

            /**
                Adds `n * Stride` to the underlying pointer.
                \param n The number of steps to add to the underlying pointer.
                \return An iterator to the modified underlying pointer (*this).
            */
            iterator& operator+=(difference_type n) {
                const auto stridedN = Stride * n;
                m_internal += stridedN;
                return *this;
            }

            /**
                Subtracts `n * Stride` from the underlying pointer.
                \param n The number of steps to subtract from the underlying pointer.
                \return An iterator to the modified underlying pointer (*this).
            */
            iterator& operator-=(difference_type n) {
                const auto stridedN = Stride * n;
                m_internal -= stridedN;
                return *this;
            }

            /**
                Equality operator for two iterators.
                \param lhs A const ref to an iterator.
                \param rhs A const ref to an iterator.
                \return true if the underlying pointers are equal, false otherwise.
            */
            friend bool operator==(const iterator& lhs, const iterator& rhs) {
                return lhs.m_internal == rhs.m_internal;
            }

            /**
                Inequality operator for two iterators.
                \param lhs A const ref to an iterator.
                \param rhs A const ref to an iterator.
                \return true if the underlying pointers are <b>not</b> equal, false otherwise.
            */
            friend bool operator!=(const iterator& lhs, const iterator& rhs) {
                return !(operator==(lhs, rhs));
            }

            /**
                Less than operator for two iterators.
                \param lhs A const ref to an iterator.
                \param rhs A const ref to an iterator.
                \return true if lhs' underlying pointer is less than rhs' underlying pointer, false otherwise.
            */
            friend bool operator<(const iterator& lhs, const iterator& rhs) {
                return lhs.m_internal < rhs.m_internal;
            }

            /**
                Greater than operator for two iterators.
                \param lhs A const ref to an iterator.
                \param rhs A const ref to an iterator.
                \return true if lhs' underlying pointer is greater than rhs' underlying pointer, false otherwise.
            */
            friend bool operator>(const iterator& lhs, const iterator& rhs) {
                return lhs.m_internal > rhs.m_internal;
            }

            /**
                Less than or equal to operator for two iterators.
                \param lhs A const ref to an iterator.
                \param rhs A const ref to an iterator.
                \return true if lhs' underlying pointer is less than or equal to rhs' underlying pointer, false otherwise.
            */
            friend bool operator<=(const iterator& lhs, const iterator& rhs) {
                return lhs.m_internal <= rhs.m_internal;
            }

            /**
                Greater than or equal to operator for two iterators.
                \param lhs A const ref to an iterator.
                \param rhs A const ref to an iterator.
                \return true if lhs' underlying pointer is greater than or equal to rhs' underlying pointer, false otherwise.
            */
            friend bool operator>=(const iterator& lhs, const iterator& rhs) {
                return lhs.m_internal >= rhs.m_internal;
            }

            /**
                Addition operator for an iterator and a difference_type.
                \param iter An iterator to modify
                \param n A scalar to add to the underlying pointer of `iter`
                \return A modified copy of `iter`, increased by `n`.
            */
            friend iterator operator+(const iterator& iter, difference_type n) {
                iterator temp = iter;
                temp += n;
                return temp;
            }

            /**
                Addition operator for a difference_type and an iterator.
                \param n A scalar to add to the underlying pointer of `iter`.
                \param iter An iterator to modify.
                \return A modified copy of `iter`, increased by `n`.
            */
            friend iterator operator+(difference_type n, const iterator& iter) {
                return iter + n;
            }

            /**
                Subtraction operator for an iterator and a difference type.
                \param iter An iterator to modify.
                \param n A scalar to subtract from `iter`.
                \return A modified copy of `iter`, decreased by `n`.
            */
            friend iterator operator-(const iterator& iter, difference_type n) {
                iterator temp = iter;
                temp -= n;
                return temp;
            }

            /**
                Subtraction operator for two iterators.
                \param lhs A const iterator ref.
                \param rhs A const iterator ref.
                \return The internal pointer of rhs subtracted from the internal pointer of rhs.
            */
            friend difference_type operator-(const iterator& lhs, const iterator& rhs) {
                return lhs.m_internal - rhs.m_internal;
            }

        private:
            T* m_internal{ nullptr };
        };

        /**
            \brief Custom const_iterator for a StrideView.
        */
        class const_iterator {
        public:
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::forward_iterator_tag;

            /**
                Default constructor doesn't make sense - would just be a nullptr waiting to happen.
            */
            const_iterator() = delete;

            /**
                Creates a const_iterator from a T*. Implicitly constructable.
                \param ptr A pointer to construct the const_iterator from.
            */
            const_iterator(pointer ptr) : m_internal(ptr) {
            }

            /**
                Checks the validity of the underlying pointer.
                \return true if the underlying pointer is not null, false otherwise.
            */
            operator bool() const {
                return m_internal;
            }

            /**
                Dereferences the underlying pointer. Does <b>not</b> check for null.
                \return A const reference to the value stored at the underlying pointer.
            */
            const T& operator*() const {
                return *m_internal;
            }

            /**
                Retrieves the underlying pointer.
                \return The pointer (to const) wrapped by this const_iterator.
            */
            const T* operator->() const {
                return m_internal;
            }

            /**
                Pre increments the underlying pointer, by `Stride` steps.
                \return A const_iterator to the incremented underlying pointer (*this).
            */
            const_iterator& operator++() {
                m_internal += Stride;
                return *this;
            }

            /**
                Post increments the underlying pointer, by `Stride` steps.
                \return A const_iterator to the previous state of the internal pointer, before the increment.
            */
            const_iterator operator++(int) {
                auto temp = *this;
                m_internal += Stride;
                return temp;
            }

            /**
                Pre decrements the underlying pointer, by `Stride` steps.
                \return A const_iterator to the decremented underlying pointer (*this).
            */
            const_iterator& operator--() {
                m_internal -= Stride;
                return *this;
            }

            /**
                Post decrements the underlying pointer, by `Stride` steps.
                \return A const_iterator to the previous state of the internal pointer, before the decrement.
            */
            const_iterator operator--(int) {
                auto temp = *this;
                m_internal -= Stride;
                return *this;
            }

            /**
                Adds `n * Stride` to the underlying pointer.
                \param n The number of steps to add to the underlying pointer.
                \return A const_iterator to the modified underlying pointer (*this).
            */
            const_iterator& operator+=(difference_type n) {
                const auto stridedDiff = n * Stride;
                m_internal += stridedDiff;
                return *this;
            }

            /**
                Subtracts `n * Stride` from the underlying pointer.
                \param n The number of steps to subtract from the underlying pointer.
                \return A const_iterator to the modified underlying pointer (*this).
            */
            const_iterator& operator-=(difference_type n) {
                const auto stridedDiff = n * Stride;
                m_internal -= stridedDiff;
                return *this;
            }

            /**
                Equality operator for two const_iterators.
                \param lhs A const ref to an const_iterator.
                \param rhs A const ref to an const_iterator.
                \return true if the underlying pointers are equal, false otherwise.
            */
            friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) {
                return lhs.m_internal == rhs.m_internal;
            }

            /**
                Inequality operator for two const_iterators.
                \param lhs A const ref to an const_iterator.
                \param rhs A const ref to an const_iterator.
                \return true if the underlying pointers are <b>not</b> equal, false otherwise.
            */
            friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) {
                return lhs.m_internal != rhs.m_internal;
            }

            /**
                Less than operator for two const_iterators.
                \param lhs A const ref to an const_iterator.
                \param rhs A const ref to an const_iterator.
                \return true if lhs' underlying pointer is less than rhs' underlying pointer, false otherwise.
            */
            friend bool operator<(const const_iterator& lhs, const const_iterator& rhs) {
                return lhs.m_internal < rhs.m_internal;
            }

            /**
                Greater than operator for two const_iterators.
                \param lhs A const ref to an const_iterator.
                \param rhs A const ref to an const_iterator.
                \return true if lhs' underlying pointer is greater than rhs' underlying pointer, false otherwise.
            */
            friend bool operator>(const const_iterator& lhs, const const_iterator& rhs) {
                return lhs.m_internal > rhs.m_internal;
            }

            /**
                Less than or equal to operator for two const_iterators.
                \param lhs A const ref to an const_iterator.
                \param rhs A const ref to an const_iterator.
                \return true if lhs' underlying pointer is less than or equal to rhs' underlying pointer, false otherwise.
            */
            friend bool operator<=(const const_iterator& lhs, const const_iterator& rhs) {
                return lhs.m_internal <= rhs.m_internal;
            }

            /**
                Greater than or equal to operator for two const_iterators.
                \param lhs A const ref to an const_iterator.
                \param rhs A const ref to an const_iterator.
                \return true if lhs' underlying pointer is greater than or equal to rhs' underlying pointer, false otherwise.
            */
            friend bool operator>=(const const_iterator& lhs, const const_iterator& rhs) {
                return lhs.m_internal >= rhs.m_internal;
            }

            /**
                Addition operator for an const_iterator and a difference_type.
                \param iter An const_iterator to modify
                \param n A scalar to add to the underlying pointer of `iter`
                \return A modified copy of `iter`, increased by `n`.
            */
            friend const_iterator operator+(const const_iterator& iter, difference_type n) {
                const_iterator temp = iter;
                temp += n;
                return temp;
            }

            /**
                Subtraction operator for an const_iterator and a difference type.
                \param iter An const_iterator to modify.
                \param n A scalar to subtract from `iter`.
                \return A modified copy of `iter`, decreased by `n`.
            */
            friend const_iterator operator+(difference_type n, const const_iterator& iter) {
                return iter + n;
            }

            /**
                Subtraction operator for an const_iterator and a difference type.
                \param iter An const_iterator to modify.
                \param n A scalar to subtract from `iter`.
                \return A modified copy of `iter`, decreased by `n`.
            */
            friend const_iterator operator-(const const_iterator& iter, difference_type n) {
                const_iterator temp = iter;
                temp -= n;
                return temp;
            }

            /**
                Subtraction operator for two const_iterators.
                \param lhs A const const_iterator ref.
                \param rhs A const const_iterator ref.
                \return The internal pointer of rhs subtracted from the internal pointer of rhs.
            */
            friend const_iterator operator-(const const_iterator& lhs, const const_iterator& rhs) {
                return lhs.m_internal - rhs.m_internal;
            }


        private:
            pointer m_internal{ nullptr };
        };


        /**
            Constructs a StrideView with a `T*` and `size`.
            \param data A T* to the data to wrap.
            \param size The number of elements to wrap.
        */
        StrideView(T* data, size_t size) : m_internalView(data, size) {
        }

        /**
            Constructs a StrideView from a start and end iterator. The passed iterators must satisfy `std::input_or_output_iterator`,
            and must have a ::value_type typedef that is the same as T.
            \param begin The start iterator.
            \param end The sentinel iterator.
        */
        template <std::input_or_output_iterator Iter>
        requires std::is_same_v<typename Iter::value_type, T>
        StrideView(Iter begin, Iter end) {
            // Apple clang don't have a constructor for spans that takes iterators yet...
            const auto size = static_cast<size_t>(std::distance(begin, end));
            const auto data = &(*begin);
            m_internalView = { data, size };
        }

        /**
            Constructs a StrideView with any array-like that can bind to `std::span`.
            \param arrayLike A span to bind to.
        */
        explicit StrideView(std::span<T> arrayLike) : m_internalView(arrayLike) {
        }

        /**
            Retrieves the number of elements in the view (with respect to `Stride`).
            \return The number of elements in the view.
        */
        [[nodiscard]] size_t size() const noexcept {
            const auto stridedSize = m_internalView.size() / Stride;
            return stridedSize;
        }

        /**
            Subscript operator. The index passed here is with respect to stride - ie if the wrapped array is `[0, 1, 2, 3]` and `Stride == 2`,
            then `operator[](1)` will return `2`.
            \param idx The index to retrieve.
            \return A reference to the element at `idx`.
        */
        T& operator[](size_t idx) {
            const auto stridedIdx = Stride * idx;
            return m_internalView[stridedIdx];
        }

        /**
            Subscript operator. The index passed here is with respect to stride - ie if the wrapped array is `[0, 1, 2, 3]` and `Stride == 2`,
            then `operator[](1)` will return `2`.
            \param idx The index to retrieve.
            \return A const reference to the element at `idx`.
        */
        const T& operator[](size_t idx) const {
            const auto stridedIdx = Stride * idx;
            return m_internalView[stridedIdx];
        }

        /**
            Creates an iterator to the first element in the view.
            \return An iterator to the beginning of the view.
        */
        iterator begin() {
            auto* ptr = &(*m_internalView.begin());
            return iterator{ ptr };
        }

        /**
            Creates a const iterator to the first element in the view.
            \return An const iterator to the beginning of the view.
        */
        const_iterator begin() const {
            auto* ptr = &(*m_internalView.begin());
            return const_iterator{ ptr };
        }

        /**
            Creates a const iterator to the first element in the view.
            \return An const iterator to the beginning of the view.
        */
        const_iterator cbegin() const {
            auto* ptr = &(*m_internalView.begin());
            return const_iterator{ ptr };
        }

        /**
            Creates an iterator to the last element in the view.
            \return An iterator to the end of the view.
        */
        iterator end() {
            auto beginIt = begin();
            const auto endIndex = static_cast<std::ptrdiff_t>(m_internalView.size() / Stride);
            auto endIt = beginIt + endIndex;
            return endIt;
        }

        /**
            Creates a const iterator to the last element in the view.
            \return A const iterator to the end of the view.
        */
        const_iterator end() const {
            auto beginIt = begin();
            const auto endIndex = static_cast<std::ptrdiff_t>(m_internalView.size() / Stride);
            auto endIt = beginIt + endIndex;
            return endIt;
        }

        /**
            Creates a const iterator to the last element in the view.
            \return A const iterator to the end of the view.
        */
        const_iterator cend() const {
            auto beginIt = cbegin();
            const auto endIndex = static_cast<std::ptrdiff_t>(m_internalView.size() / Stride);
            auto endIt = beginIt + endIndex;
            return endIt;
        }

    private:
        std::span<T> m_internalView{};
    };
} // namespace marvin::containers

#endif
