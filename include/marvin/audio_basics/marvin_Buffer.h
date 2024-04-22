// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_BUFFER_H
#define MARVIN_BUFFER_H
#include "marvin/library/marvin_Concepts.h"
namespace marvin::audiobasics {
    template <FloatType SampleType>
    struct Buffer {
        SampleType** in;
        SampleType** out;
        int numChannels;
        int numSamples;
    };
} // namespace marvin::audiobasics
#endif
