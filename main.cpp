#include <iostream>

#include <DirectXMath.h>
// all DirectX Math symbols start with ‘XM’ prefix
using namespace DirectX;

std::ostream& operator<<(std::ostream &os, XMVECTOR const &v) {
  return os << XMVectorGetX(v) << '\t'
            << XMVectorGetY(v) << '\t'
            << XMVectorGetZ(v) << '\t'
            << XMVectorGetW(v);
}

int main() {
  XMVECTOR v = {1.0f, 1.0f};
  XMVECTOR vQuat = XMQuaternionRotationAxis(XMVECTOR{0, 0, 1, 0}, XM_PIDIV2);
  auto mRot = XMMatrixRotationQuaternion(vQuat);
  std::cout << v << '\n';
  v = XMVector4Transform(v, mRot);
  std::cout << v << '\n';
}
