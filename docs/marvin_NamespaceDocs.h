/**
    \brief A mostly harmless top-level namespace.
*/
namespace marvin {

    /**
        \brief Views, wrappers and custom containers.
    */
    namespace containers {
        /**
           \brief First in, first out queues.
        */
        namespace fifos {}
    } // namespace containers

    /**
        \brief DSP helper and utility functions.
    */
    namespace dsp {

        /**
            \brief Digital filter functions and classes.
        */
        namespace filters {

            /**
                \brief Implementations of Robert Brinstow Johnson's RBJ Cookbook formulae.
            */
            namespace rbj {}
        } // namespace filters
        /**
            \brief Oscillator functions and classes..
        */
        namespace oscillators {}
    } // namespace dsp
    /**
        \brief Standard library-esque utilities.
    */
    namespace library {}
    /**
        \brief Math helper functions and classes.
    */
    namespace math {

        /**
            \brief Collection of basic arithmetic operations on vectors, SIMD accelerated where possible.

            On macOS this will use Accelerate's vDSP library for SIMD intrinsics. On Windows, Marvin has an optional
            dependency on Intel's IPP library. If it's found by CMake, these functions will use the IPP implementations of
            these functions. If it's *not* found, it will use the fallback implementations, which are simple for loops. <br>
            If you're struggling to get IPP installed, Sudara has a great [blog post](https://melatonin.dev/blog/using-intel-performance-primitives-ipp-with-juce-and-cmake/)
            detailing the hoops you need to jump through to get it up and running. <br>

            Note that even when using SIMD,
            it's not guaranteed to be faster than what the compiler might generate with a for loop - I'd
            recommend benchmarking on the platform you're targeting with the kind of data you'll be using, and
            choosing whether to use these functions accordingly.
        */
        namespace vecops {}
    } // namespace math
    /**
        \brief Utility helper functions and classes.
    */
    namespace utils {}
} // namespace marvin