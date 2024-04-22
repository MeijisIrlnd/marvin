#ifndef MARVIN_BUFFER_H
#define MARVIN_BUFFER_H
#include "marvin/library/marvin_Concepts.h"
namespace Audio {
    template <FloatType SampleType>
    struct Buffer {
        SampleType** in;
        SampleType** out;
        int numChannels;
        int numSamples;
    };
} // namespace Audio
#endif
