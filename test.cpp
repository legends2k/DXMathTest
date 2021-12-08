#include <iostream>

#if defined(__clang__)
#  define COMPILER_CLANG
#elif defined(__GNUG__)
#  define COMPILER_GCC
#elif defined(_MSC_VER)
#  define COMPILER_MSVC
#endif

// avoid sal.h symbols conflicting with GCC’s libstdc++ headers
#ifdef COMPILER_GCC
#  include <algorithm>
#  include <utility>
#endif

#include <DirectXMath.h>
// all DirectX Math symbols start with ‘XM’ prefix
using namespace DirectX;

int main() {
  XMVECTOR vFive = {1.0f, 1.0f};
  XMVECTOR vQuat = XMQuaternionRotationAxis(XMVECTOR{0, 0, 1, 0}, XM_PIDIV2);
  auto mRot = XMMatrixRotationQuaternion(vQuat);
  std::cout << vFive[0] << '\t'
            << vFive[1] << '\t'
            << vFive[2] << '\t'
            << vFive[3] << '\n';
  vFive = XMVector4Transform(vFive, mRot);
  std::cout << vFive[0] << '\t'
            << vFive[1] << '\t'
            << vFive[2] << '\t'
            << vFive[3] << '\n';
}
