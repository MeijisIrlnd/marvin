![tests](https://github.com/MeijisIrlnd/marvin/actions/workflows/unit-test.yml/badge.svg)
# marvin: a mostly harmless dsp library
Marvin is a framework-agnostic, raw-c++ dsp helper library, used internally at SLM Audio and Kalide Systems. It's still a work in progress, and test coverage is woefully incomplete, so use at your own peril for the forseeable future!

## Installation and Usage
Marvin is built with CMake, and can be consumed either via `find_package` for a system-wide installation, via `FetchContent` (or `CPM`) as a per-project dependency, or via a github submodule. <br>
Regardless of how it's installed, usage in your CMake project remains the same:
```cmake
target_link_libraries(<YourTarget> PRIVATE slma::marvin)
```
> **A note about SIMD**<br>
Marvin contains a `vecops` namespace, which performs vector arithmetic with floating point values. Where possible, this will use SIMD intrinsics to optimize those operations. <br>
On macOS, marvin will use the Accelerate framework's vDSP library, no additional installs needed. <br>
On Windows, it's more involved - marvin will look for Intel's IPP library at configure time. If it finds it, the `vecops` functions will use IPP implementations for the operations. If it doesn't, it will use the fallback implementations (simple for loops).<br> 
If you're struggling to get IPP installed on your system, [sudara](https://github.com/sudara) has a fantastic [blog post](https://melatonin.dev/blog/using-intel-performance-primitives-ipp-with-juce-and-cmake/) on the subject.<br>RE Linux, investigating what to use there is on the todo list.
<br><br>
It's also worth noting that SIMD is *not* guaranteed to be faster than what the optimizer might be able to do with regular for-loops. We'd recommend benchmarking our implementations against a fallback on the platform you're targeting, with the kind of data you're planning to use, and basing your decision on that!
### System-Wide
To build and install marvin system-wide, for use with `find_package`: 
```sh
mkdir build
cd build
cmake ..
cmake --build . --config Release --target install
```

### FetchContent
To build marvin as a dependency for your project with a package manager: 
```cmake
include(FetchContent)
FetchContent_Declare(
    marvin
    GIT_REPOSITORY https://github.com/MeijisIrlnd/marvin
    GIT_TAG main
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(marvin)
```
### As a submodule
Assuming you've added marvin as a github submodule, in `<YourProjectRoot>/modules/marvin`:
```cmake
add_subdirectory(modules/marvin)
```
## Documentation
Documentation is generated via doxygen, and is hosted on github pages at https://meijisirlnd.github.io/marvin 

### Building the documentation
If you'd like to build the documentation locally, first ensure doxygen is installed, and on your include path. You can then run
```sh
mkdir build
cd build
cmake ..
cmake --build . --target marvin_docs
```
to generate the documentation locally.
## Building the tests
As previously mentioned, test coverage is a work-in-progress. However, to build the tests: 
```cmake
mkdir build
cd build 
cmake ..
cmake --build . --target marvin-tests
```

## Dependencies 
While dependencies are kept to a minimum, marvin depends on a few libraries, which will be consumed via FetchContent when the library is configured. 
- [aff3ct/mipp](https://github.com/aff3ct/MIPP), for hassle free SIMD ops
- [catchorg/Catch2](https://github.com/catchorg/Catch2), for unit testing
- [fmtlib/fmt](https://github.com/fmtlib/fmt), for nice string interpolation
- [cameron314/readerwriterqueue](https://github.com/cameron314/readerwriterqueue), an incredible thead + rt safe fifo

Additionally, special thanks to [Ryan](https://github.com/ryanjeffares) and [Ben](https://github.com/benthevining) for their advice, check out what they're working on!

## Contributing
Contributions are more than appreciated - if you'd like to open a PR with bugfixes or features, please open an issue, and link to the issue your PR solves in the PR's description. I'd also ask that you compile with `/W4 /WX` with MSVC, or `-Wall -Wextra -Werror` on GNU-like compilers, and that you use the included `.clang-format` file to keep the styling consistent. Other than that, have at it!

