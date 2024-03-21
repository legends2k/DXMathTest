# DirectXMath Seed

[DirectXMath][] is an open-source 3D math library that’s usable outside Windows and Direct3D too.  This project uses CMake to build a toy program using DirectXMath.  It uses CMake to:

* Fetch DirectXMath
* Fetch `sal.h` if missing
  - An internal, header-only dependency of DirectXMath headers

If there’re conflicts due to the downloaded `sal.h` with projects like [stb][] when compiling with GCC and Clang use [NoSAL][].

Tested on

* Linux + GCC
* macOS + Clang
* Windows + MSVC
* Windows + MinGW ([MSYS2][])

# Build + Run

``` sh
# generate build files
cmake -B build

# build and run
cmake --build build -t run
```

Additionally `-G` can be used to choose the generator in the generate step e.g. `cmake -B build -G Ninja`.

[DirectXMath]: https://github.com/Microsoft/DirectXMath
[NoSAL]: https://github.com/legends2k/NoSAL
[sal]: https://github.com/dotnet/corert/blob/master/src/Native/inc/unix/sal.h
[stb]: https://github.com/nothings/stb/
[MSYS2]: https://www.msys2.org/
