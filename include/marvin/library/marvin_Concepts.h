#ifndef MARVIN_CONCEPTS_H
#define MARVIN_CONCEPTS_H
#include <type_traits>
template <class T>
concept FloatType = requires {
    std::is_floating_point_v<T>;
};

template <class T>
concept NumericType = requires {
    std::is_integral_v<T> ||
        std::is_floating_point_v<T>;
};

template <class T>
concept SmartPointerType = requires(T a) {
    a.get();
    a.reset();
    a.operator*();
    a.operator->();
};
#endif