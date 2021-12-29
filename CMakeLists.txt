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

FetchContent_Declare(
  NoSAL
  GIT_REPOSITORY https://github.com/legends2k/NoSAL
  GIT_TAG        main
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(NoSAL)
cmake_policy(PUSH)
# Allow calling target_link_libraries on targets not authored here
cmake_policy(SET CMP0079 NEW)
target_link_libraries(DirectXMath INTERFACE NoSAL::NoSAL)
cmake_policy(POP)

target_link_libraries(${PROJECT_NAME} PRIVATE DirectXMath)

# convenience target for quick build and run
add_custom_target(run
  DEPENDS ${PROJECT_NAME}
  COMMAND ${PROJECT_NAME}
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  VERBATIM)
