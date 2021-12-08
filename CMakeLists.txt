cmake_minimum_required(VERSION 3.16)
project(tesst VERSION 0.1)

# Set a default build type if none was specified
set(default_build_type "Debug")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug"
    "Release" "RelWithDebInfo")
endif()

add_executable(${PROJECT_NAME} "test.cpp")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Release,RelWithDebInfo>:-Os>)

file(DOWNLOAD
  https://raw.githubusercontent.com/dotnet/corert/master/src/Native/inc/unix/sal.h
  "${CMAKE_BINARY_DIR}/_deps/sal-src/sal.h"
  EXPECTED_HASH MD5=08728f6e65b817294b8ecbb9056d3fe8
  STATUS SAL_DOWNLOAD_STATUS)
list(GET SAL_DOWNLOAD_STATUS 0 SAL_STATUS_CODE)
if (NOT SAL_STATUS_CODE EQUAL 0)
  message(FATAL_ERROR "Failed to download sal.h for DirectX Math library")
endif ()

include(FetchContent)
FetchContent_Declare(
  DirectXMath
  GIT_REPOSITORY https://github.com/Microsoft/DirectXMath
  GIT_TAG        0ca5e0b89ac740eea850760f54211a8609df7265
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(DirectXMath)
target_include_directories(DirectXMath INTERFACE
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/_deps/sal-src>
  $<INSTALL_INTERFACE:include>)
# disable XMVECTOR operator overloads; unsupported by GCC and Clang
target_compile_definitions(DirectXMath INTERFACE
  _XM_NO_XMVECTOR_OVERLOADS_)

target_link_libraries(${PROJECT_NAME} PRIVATE DirectXMath)
