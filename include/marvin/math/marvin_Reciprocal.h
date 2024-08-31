// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef RITE_OF_SPRING_MARVIN_RECIPROCAL_H
#define RITE_OF_SPRING_MARVIN_RECIPROCAL_H
#include <marvin/library/marvin_Concepts.h>
namespace marvin::math {
    /**
     * \brief Convenience POD struct for representing a range used by `marvin::math::ReciprocalRange`.
     */
    template <FloatType T>
    struct ReciprocalRange {
        /**
         * The start of the range.
         */
        T min;

        /**
         * The centre of the range.
         */
        T centre;

        /**
         * The end of the range.
         */
        T max;
    };

    /**
     * \brief Class to handle mapping from one range to another (warped) range, based on the reciprocal function 1/x.
     *
     * Shamelessly lifted (with permission) from Signalsmith's Reciprocal class from his dsp library - all credit here to Signalsmith Audio.
     */
    template <FloatType T>
    class Reciprocal final {
    public:
        /**
         * Constructs a Reciprocal, which will map from `src` to `dest`.
         * \param src The original range.
         * \param dest The new range to map to.
         */
        Reciprocal(ReciprocalRange<T> src, ReciprocalRange<T> dest);
        /**
         * Constructs a Reciprocal, which will map from `src` to `dest`. This overload will assume a `src` range of {0, 0.5, 1}.
         * \param dest The new range to map to.
         */
        explicit Reciprocal(ReciprocalRange<T> dest);
        /**
         * Performs the mapping from `src` to `dest` on the given value.
         * \param toMap The value (in the range `src`) to remap.
         */
        T operator()(T toMap) noexcept;

    private:
        T m_a, m_b, m_c, m_d;
    };
} // namespace marvin::math

#endif // RITE_OF_SPRING_MARVIN_RECIPROCAL_H
