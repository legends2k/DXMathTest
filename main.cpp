#include <iostream>

// Note: Order matters.  Clang also defines __GNUG__!!
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
  XMVECTOR v = {1.0f, 1.0f};
  XMVECTOR vQuat = XMQuaternionRotationAxis(XMVECTOR{0, 0, 1, 0}, XM_PIDIV2);
  auto mRot = XMMatrixRotationQuaternion(vQuat);
  std::cout << v[0] << '\t' << v[1] << '\t' << v[2] << '\t' << v[3] << '\n';
  v = XMVector4Transform(v, mRot);
  std::cout << v[0] << '\t' << v[1] << '\t' << v[2] << '\t' << v[3] << '\n';
}
