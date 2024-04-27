# marvin: a mostly harmless dsp library
Marvin is a framework-agnostic, raw-c++ dsp helper library, used internally at SLM Audio and Kalide Systems. It's still a work in progress, and both the documentation and test coverage are incomplete, so use at your own peril for the forseeable future!

## Installation and Usage
Marvin is built with CMake, and can be consumed either via `find_package` for a system-wide installation, via `FetchContent` (or `CPM`) as a per-project dependency, or via a github submodule. <br>
Regardless of how it's installed, usage in your CMake project remains the same:
```cmake
target_link_libraries(<YourTarget> PRIVATE marvin::marvin)
```

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


