# Use DirectX Math with CMake

[DirectX Math][] is an open-source 3D math library usable outside Windows and Direct3D.  This project uses CMake to build a toy program using DirectX Math.  It tackles the following using CMake:

* Fetch DirectX Math
* Fetch [NoSAL][] for `sal.h`; a dependency of DirectX Math
* Add NoSAL as a dependency on `DirectX Math`

Opted out of using the [original `sal.h`][sal] since it conflicts with other projects like [stb][] when compiling with GCC and Clang.

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

[DirectX Math]: https://github.com/Microsoft/DirectXMath
[NoSAL]: https://github.com/legends2k/NoSAL
[sal]: https://github.com/dotnet/corert/blob/master/src/Native/inc/unix/sal.h
[stb]: https://github.com/nothings/stb/
[MSYS2]: https://www.msys2.org/
