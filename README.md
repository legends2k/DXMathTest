# Use DirectX Math with CMake

[DirectX Math][] is an open-source 3D math library usable outside Windows and Direct3D.  This project uses CMake to build a toy program using DirectX Math.  It tackles the following using CMake:

* Fetch DirectX Math
* Fetch `sal.h` on non-Windows machines; a dependency of DirectX Math
* Make an interface library out of `sal.h`
* Add `sal` as a dependency on `DirectX Math`

Additionally, in the source, it includes some headers _before_ including `DirectXMath.h` to avoid `sal.h`’s symbols from conflicting with GCC’s standard library headers.  This was called out in DirectX Math's documentation but I didn't actually find any such conflicts.

Tested on

* Linux + GCC
* macOS + Clang

# Build + Run

```
cmake -B build
cmake --build build
./build/DXMTest
```


[DirectX Math]: https://github.com/Microsoft/DirectXMath
