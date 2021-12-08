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

include(FetchContent)
FetchContent_Declare(
  sal
  URL https://raw.githubusercontent.com/dotnet/corert/master/src/Native/inc/unix/sal.h
  URL_HASH MD5=08728f6e65b817294b8ecbb9056d3fe8
  DOWNLOAD_NO_EXTRACT TRUE
  DOWNLOAD_NO_PROGRESS TRUE
)
FetchContent_GetProperties(sal)
if (NOT sal_POPULATED)
  FetchContent_Populate(sal)
  add_library(sal INTERFACE)
  include(GNUInstallDirs)
  target_include_directories(sal INTERFACE
    $<BUILD_INTERFACE:${sal_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  install(TARGETS sal
    EXPORT salTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  install(FILES sal.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  install(EXPORT salTargets
    FILE salTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sal)
endif ()

FetchContent_Declare(
  DirectXMath
  GIT_REPOSITORY https://github.com/Microsoft/DirectXMath
  GIT_TAG        0ca5e0b89ac740eea850760f54211a8609df7265
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(DirectXMath)
# disable XMVECTOR operator overloads; unsupported by GCC and Clang
target_compile_definitions(DirectXMath INTERFACE
  _XM_NO_XMVECTOR_OVERLOADS_)
target_link_libraries(DirectXMath INTERFACE sal)

target_link_libraries(${PROJECT_NAME} PRIVATE DirectXMath)
