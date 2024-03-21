cmake_minimum_required(VERSION 3.16)
project(DXMTest VERSION 0.1)

# Set a default build type if none was specified
set(default_build_type "Debug")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug"
    "Release" "RelWithDebInfo")
endif()

add_executable(${PROJECT_NAME} main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
  DirectXMath
  GIT_REPOSITORY https://github.com/Microsoft/DirectXMath
  GIT_TAG        main
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(DirectXMath)
# disable XMVECTOR operator overloads; unsupported by GCC and Clang
target_compile_definitions(DirectXMath INTERFACE
  _XM_NO_XMVECTOR_OVERLOADS_)

target_link_libraries(${PROJECT_NAME} PRIVATE DirectXMath)
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra>)

# Look for sal.h; download if missing.  This skips download on Windows (both
# with MSVC and MinGW which ships a sal.h).
find_file(SAL-HEADER sal.h)
if(${SAL-HEADER} STREQUAL "SAL-HEADER-NOTFOUND")
  # Refer target-not-Windows section of
  # https://github.com/microsoft/vcpkg/blob/master/ports/directxmath/portfile.cmake
  file(DOWNLOAD
    "https://raw.githubusercontent.com/dotnet/runtime/v8.0.1/src/coreclr/pal/inc/rt/sal.h"
    "${DirectXMath_SOURCE_DIR}/Inc/sal.h"
    EXPECTED_HASH SHA512=0f5a80b97564217db2ba3e4624cc9eb308e19cc9911dae21d983c4ab37003f4756473297ba81b386c498514cedc1ef5a3553d7002edc09aeb6a1335df973095f
  )
endif()

# convenience target for quick build and run
add_custom_target(run
  DEPENDS ${PROJECT_NAME}
  COMMAND ${PROJECT_NAME}
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  VERBATIM)
