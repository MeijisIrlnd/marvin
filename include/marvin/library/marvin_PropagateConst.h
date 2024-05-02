// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef SLMADSP_PROPAGATECONST_H
#define SLMADSP_PROPAGATECONST_H
#include "marvin/library/marvin_Concepts.h"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace marvin::library {
    /**
        \brief A compiler-support agnostic implementation of `std::propagate_const`

        The *raison d'etre* being that certain popular compilers (*cough* Apple-Clang *cough*) are horrifically slow at implementing stdlib features, and `std::propagate_const` is pretty desirable for anything using a `PImpl` pattern.
        <br>For more information about what `std::propagate_const` actually *does*, see here: https://en.cppreference.com/w/cpp/experimental/propagate_const
    */
    template <typename T>
    requires SmartPointerType<T> || std::is_pointer_v<T>
    class PropagateConst {
        using element_type = typename std::pointer_traits<T>::element_type;
        template <class U>
        struct is_specialisation : std::false_type {};
        template <class U>
        struct is_specialisation<PropagateConst<U>> : std::true_type {};

        static constexpr bool isConvertibleOrPointer = requires(T a) {
            std::is_pointer_v<T> || std::is_convertible_v<T, element_type*>;
        };

    public:
        /**
            Constructs a PropagateConst, default initialising the internal pointer variable.
        */
        constexpr PropagateConst() = default;
        /**
            Explicitly defaulted move constructor that move constructs the internal pointer variable with `p`
            \param p Another PropagateConst object to move from.
        */
        constexpr PropagateConst(PropagateConst&& p) = default;

        /**
            Initialises the internal pointer variable as if from the expression `std::move(pu.t_)`. Implicit if `std::is_convertible_v<U, T>`
            \param pu Another PropagateConst object of a different specialization to move from.
        */
        template <class U>
        requires std::is_constructible<T, U>::value && std::is_convertible<U, T>::value
        constexpr PropagateConst(PropagateConst<U>&& pu) : m_underlying(std::move(pu)) {
        }

        /**
            Initialises the internal pointer variable as if from the expression `std::move(pu.t_)`. Explicit if `!std::is_convertible_v<U, T>`
            \param pu Another PropagateConst object of a different specialization to move from.
        */
        template <class U>
        requires std::is_constructible<T, U>::value && (!std::is_convertible<U, T>::value)
        explicit constexpr PropagateConst(PropagateConst<U>&& pu) : m_underlying(std::move(pu)) {
        }

        /**
            Initialises the internal pointer variable with the expression `std::forward<U>(u)`. Implicit if `std::is_convertible_v<U, T>`.
            \param u Another object to initialise the internal pointer variable with.
        */
        template <class U>
        requires std::is_constructible<T, U>::value && (!is_specialisation<std::decay_t<U>>::value)
        constexpr PropagateConst(U&& u) : m_underlying(std::forward<U>(u)) {
        }

        /**
            Initialises the internal pointer variable with the expression `std::forward<U>(u)`. Explicit if `!std::is_convertible_v<U, T>`.
            \param u Another object to initialise the internal pointer variable with.
        */
        template <class U>
        requires std::is_constructible<T, U>::value && (!is_specialisation<std::decay_t<U>>::value) && (!std::is_convertible<U, T>::value)
        explicit constexpr PropagateConst(U&& u) : m_underlying(std::forward<U>(u)) {
        }

        /**
            Explicitly defaulted move-assign that move-assigns the internal pointer variable from `other`'s internal pointer variable.
        */
        constexpr PropagateConst& operator=(PropagateConst&&) = default;


        /**
            Assigns a `std::move` of `pu`'s internal pointer variable to this instance's internal pointer variable.
            \param pu Another PropagateConst object of a different specialization to move from.
            \return `*this`.
        */
        template <class U>
        requires std::is_convertible<U, T>::value
        constexpr PropagateConst& operator=(PropagateConst<U>&& pu) {
            m_underlying = std::move(pu);
            return *this;
        }

        /**
            Assigns a `std::forward` of `u` to the internal pointer variable.
            \param u Another object to assign to the internal pointer variable.
            \return `*this`.
        */
        template <class U>
        requires std::is_convertible<U, T>::value && (!is_specialisation<std::decay_t<U>>::value)
        constexpr PropagateConst& operator=(U&& u) {
            m_underlying = std::forward<U>(u);
            return *this;
        }

        /**
            Deleted - non copyable.
        */
        PropagateConst& operator=(const PropagateConst& other) = delete;

        /**
           Swaps the internal pointer variable with that of pt. UB if lvalues of T do not satisfy swappable.
           \param pt Another PropagateConst object to swap with.
        */
        constexpr void swap(PropagateConst& pt) noexcept {
            std::swap(m_underlying, pt.m_underlying);
        }

        /**
            Returns a pointer to the object pointed to by the wrapped internal pointer-like variable.
            \return If T is an object pointer type, then the internal pointer variable. Otherwise, a `.get()` call on the internal pointer variable.
        */
        constexpr element_type* get() {
            if constexpr (isSmartPointer) {
                return m_underlying.get();
            } else {
                return m_underlying;
            }
        }


        /**
            Returns a pointer to the object pointed to by the wrapped internal pointer-like variable.
            \return If T is an object pointer type, then the internal pointer variable. Otherwise, a `.get()` call on the internal pointer variable.
        */
        constexpr const element_type* get() const {
            if constexpr (isSmartPointer) {
                return m_underlying.get();
            } else {
                return m_underlying;
            }
        }


        /**
            Checks whether the internal pointer variable is null.
            \return `true` if the internal pointer variable is not null, otherwise `false`.
        */
        constexpr explicit operator bool() const {
            return m_underlying != nullptr;
        }


        /**
            Dereferences the internal pointer variable - undefined behaviour if its null!
            \return The value stored at the internal pointer variable.
        */
        constexpr element_type& operator*() {
            return *get();
        }

        /**
            Dereferences the internal pointer variable - undefined behaviour if its null!
            \return The value stored at the internal pointer variable.
        */
        constexpr const element_type& operator*() const {
            return *get();
        }

        /**
            Provides access into the internal pointer variable - undefined behaviour if its null!
            \return A pointer to the object pointed to by the internal pointer variable.
        */
        constexpr element_type* operator->() {
            return get();
        }

        /**
            Provides access into the internal pointer variable - undefined behaviour if its null!
            \return A pointer to the object pointed to by the internal pointer variable.
        */
        constexpr const element_type* operator->() const {
            return get();
        }

        /**
            Provides implicit conversion to `element_type*`
            \returns A pointer to the object pointed to by the internal pointer variable.
        */
        constexpr operator element_type*()
        requires isConvertibleOrPointer
        {
            return get();
        }

        /**
            Provides implicit conversion to `const element_type*`
            \returns A pointer to the object pointed to by the internal pointer variable.
        */
        constexpr operator const element_type*()
        requires isConvertibleOrPointer
        {
            return get();
        }


    private:
        static constexpr bool isSmartPointer = requires(T a) {
            a.get(); // Require that .get is a valid thing..
        };


        template <class U>
        friend constexpr bool operator==(const PropagateConst<U>&, std::nullptr_t);
        template <class U>
        friend constexpr bool operator==(std::nullptr_t, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator!=(const PropagateConst<U>&, std::nullptr_t);
        template <class U>
        friend constexpr bool operator!=(std::nullptr_t, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator==(const PropagateConst<T>&, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator!=(const PropagateConst<T>&, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator<(const PropagateConst<T>&, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator>(const PropagateConst<T>&, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator<=(const PropagateConst<T>&, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator>=(const PropagateConst<T>&, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator==(const PropagateConst<T>&, const U&);
        template <class U>
        friend constexpr bool operator!=(const PropagateConst<T>&, const U&);
        template <class U>
        friend constexpr bool operator==(const T&, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator!=(const T&, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator<(const PropagateConst<T>&, const U&);
        template <class U>
        friend constexpr bool operator>(const PropagateConst<T>&, const U&);
        template <class U>
        friend constexpr bool operator<=(const PropagateConst<T>&, const U&);
        template <class U>
        friend constexpr bool operator>=(const PropagateConst<T>&, const U&);
        template <class U>
        friend constexpr bool operator<(const T&, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator>(const T&, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator<=(const T&, const PropagateConst<U>&);
        template <class U>
        friend constexpr bool operator>=(const T&, const PropagateConst<U>&);
        T m_underlying;
    };

    template <class T>
    constexpr bool operator==(const PropagateConst<T>& pt, std::nullptr_t) {
        return pt.m_underlying == nullptr;
    }

    template <class T>
    constexpr bool operator==(std::nullptr_t, const PropagateConst<T>& pt) {
        return nullptr == pt.m_underlying;
    }
    template <class T>
    constexpr bool operator!=(const PropagateConst<T>& pt, std::nullptr_t) {
        return pt.m_underlying != nullptr;
    }

    template <class T>
    constexpr bool operator!=(std::nullptr_t, const PropagateConst<T>& pt) {
        return nullptr != pt.m_underlying;
    }

    template <class T, class U>
    constexpr bool operator==(const PropagateConst<T>& pt, const PropagateConst<U>& pu) {
        return pt.m_underlying == pu.m_underlying;
    }

    template <class T, class U>
    constexpr bool operator!=(const PropagateConst<T>& pt, const PropagateConst<U>& pu) {
        return pt.m_underlying != pu.m_underlying;
    }

    template <class T, class U>
    constexpr bool operator<(const PropagateConst<T>& pt, const PropagateConst<U>& pu) {
        return pt.m_underlying < pu.m_underlying;
    }

    template <class T, class U>
    constexpr bool operator>(const PropagateConst<T>& pt, const PropagateConst<U>& pu) {
        return pt.m_underlying > pu.m_underlying;
    }

    template <class T, class U>
    constexpr bool operator<=(const PropagateConst<T>& pt, const PropagateConst<U>& pu) {
        return pt.m_underlying <= pu.m_underlying;
    }

    template <class T, class U>
    constexpr bool operator>=(const PropagateConst<T>& pt, const PropagateConst<U>& pu) {
        return pt.m_underlying >= pu.m_underlying;
    }

    template <class T, class U>
    constexpr bool operator==(const PropagateConst<T>& pt, const U& u) {
        return pt.m_underlying == u;
    }

    template <class T, class U>
    constexpr bool operator!=(const PropagateConst<T>& pt, const U& u) {
        return pt.m_underlying != u;
    }

    template <class T, class U>
    constexpr bool operator==(const T& t, const PropagateConst<U>& pu) {
        return t == pu.m_underlying;
    }

    template <class T, class U>
    constexpr bool operator!=(const T& t, const PropagateConst<U>& pu) {
        return t != pu.m_underlying;
    }

    template <class T, class U>
    constexpr bool operator<(const PropagateConst<T>& pt, const U& u) {
        return pt.m_underlying < u;
    }

    template <class T, class U>
    constexpr bool operator>(const PropagateConst<T>& pt, const U& u) {
        return pt.m_underlying > u;
    }

    template <class T, class U>
    constexpr bool operator<=(const PropagateConst<T>& pt, const U& u) {
        return pt.m_underlying <= u;
    }

    template <class T, class U>
    constexpr bool operator>=(const PropagateConst<T>& pt, const U& u) {
        return pt.m_underlying >= u;
    }

    template <class T, class U>
    constexpr bool operator<(const T& t, const PropagateConst<U>& pu) {
        return t < pu.m_underlying;
    }

    template <class T, class U>
    constexpr bool operator>(const T& t, const PropagateConst<U>& pu) {
        return t > pu.m_underlying;
    }

    template <class T, class U>
    constexpr bool operator<=(const T& t, const PropagateConst<U>& pu) {
        return t <= pu.m_underlying;
    }

    template <class T, class U>
    constexpr bool operator>=(const T& t, const PropagateConst<U>& pu) {
        return t >= pu.m_underlying;
    }

} // namespace marvin::library
#endif
