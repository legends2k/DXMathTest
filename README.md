# Use DirectX Math with CMake

[DirectX Math][] is an open-source 3D math library usable outside Windows and Direct3D.  This project uses CMake to build a toy program using DirectX Math.  It tackles the following using CMake:

* Fetch DirectX Math
* Fetch `sal.h` on non-Windows machines; a dependency of DirectX Math
* Make an interface library out of `sal.h`
* Add `sal` as a dependency on `DirectX Math`

Additionally, in the source, it includes some headers _before_ including `DirectXMath.h` to avoid `sal.h`’s symbols from conflicting with GCC’s standard library headers.  This was done as called out in DirectX Math's documentation but no such conflicts were actually found.

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
[MSYS2]: https://www.msys2.org/
