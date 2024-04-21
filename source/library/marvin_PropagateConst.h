#ifndef SLMADSP_PROPAGATECONST_H
#define SLMADSP_PROPAGATECONST_H
#include "audio/utils/marvin_Concepts.h"
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace Audio {
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
        constexpr PropagateConst() = default;
        constexpr PropagateConst(PropagateConst&& p) = default;

        template <class U>
        requires std::is_constructible<T, U>::value && std::is_convertible<U, T>::value
        constexpr PropagateConst(PropagateConst<U>&& pu) : m_underlying(std::move(pu)) {
        }

        template <class U>
        requires std::is_constructible<T, U>::value && (!std::is_convertible<U, T>::value)
        explicit constexpr PropagateConst(PropagateConst<U>&& pu) : m_underlying(std::move(pu)) {
        }

        template <class U>
        requires std::is_constructible<T, U>::value && (!is_specialisation<std::decay_t<U>>::value)
        constexpr PropagateConst(U&& u) : m_underlying(std::forward<U>(u)) {
        }

        template <class U>
        requires std::is_constructible<T, U>::value && (!is_specialisation<std::decay_t<U>>::value) && (!std::is_convertible<U, T>::value)
        explicit constexpr PropagateConst(U&& u) : m_underlying(std::forward<U>(u)) {
        }

        constexpr PropagateConst& operator=(PropagateConst&&) = default;

        template <class U>
        requires std::is_convertible<U, T>::value
        constexpr PropagateConst& operator=(PropagateConst<U>&& pu) {
            m_underlying = std::move(pu);
            return *this;
        }

        template <class U>
        requires std::is_convertible<U, T>::value && (!is_specialisation<std::decay_t<U>>::value)
        constexpr PropagateConst& operator=(U&& u) {
            m_underlying = std::forward<U>(u);
            return *this;
        }

        PropagateConst& operator=(const PropagateConst& other) = delete;

        constexpr void swap(PropagateConst& pt) noexcept {
            std::swap(m_underlying, pt.m_underlying);
        }

        constexpr element_type* get() {
            if constexpr (isSmartPointer) {
                return m_underlying.get();
            } else {
                return m_underlying;
            }
        }


        constexpr const element_type* get() const {
            if constexpr (isSmartPointer) {
                return m_underlying.get();
            } else {
                return m_underlying;
            }
        }


        constexpr explicit operator bool() const {
            return m_underlying != nullptr;
        }


        constexpr element_type& operator*() {
            return *get();
        }

        constexpr const element_type& operator*() const {
            return *get();
        }

        constexpr element_type* operator->() {
            return get();
        }

        constexpr const element_type* operator->() const {
            return get();
        }

        constexpr operator element_type*()
        requires isConvertibleOrPointer
        {
            return get();
        }

        constexpr operator const element_type*()
        requires isConvertibleOrPointer
        {
            return get();
        }


    private:
        static constexpr bool isSmartPointer = requires(T a) {
            a.get(); // Require that .get is a valid thing..
        };


        friend constexpr bool operator==(const PropagateConst<T>&, std::nullptr_t);
        friend constexpr bool operator==(std::nullptr_t, const PropagateConst<T>&);
        friend constexpr bool operator!=(const PropagateConst<T>&, std::nullptr_t);
        friend constexpr bool operator!=(std::nullptr_t, const PropagateConst<T>&);
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

} // namespace Audio
#endif