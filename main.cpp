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

std::ostream& operator<<(std::ostream &os, XMMATRIX const &m) {
  for (auto i = 0; i < 4; ++i) {
    os << m.r[i] << '\n';
  }
  return os;
}

std::ostream& operator<<(std::ostream &os, XMFLOAT3X4A const &m) {
  for (auto i = 0; i < 3; ++i) {
    os << XMLoadFloat4A(reinterpret_cast<const XMFLOAT4A*>(&m.m[i])) << '\n';
  }
  return os;
}

// Returns vector orthogonal to |V|; undefined if V.w ≠ 0 (isn’t a vector)
// Ouput isn’t necessarily normalized.
XMVECTOR ortho(XMVECTOR V) {
  const auto Vabs = XMVectorAbs(V);
  const auto Vx = XMVectorSplatX(Vabs);
  const auto Vz = XMVectorSplatZ(Vabs);
  const auto control = XMVectorGreater(Vx, Vz);
  // use w for 0 and Xor to flip sign of a component
  // https://stackoverflow.com/a/5213367/183120
  // R1 = <0, V.z, -V.y, 0>
  auto S = XMVectorSetInt(0, 0, 0x80000000, 0);
  const auto R1 = XMVectorXorInt(XMVectorSwizzle<XM_SWIZZLE_W,
                                                 XM_SWIZZLE_Z,
                                                 XM_SWIZZLE_Y,
                                                 XM_SWIZZLE_W>(V),
                                 S);
  // R2 = <V.y, -V.x, 0, 0>
  S = XMVectorRotateLeft<1>(S);
  const auto R2 = XMVectorXorInt(XMVectorSwizzle<XM_SWIZZLE_Y,
                                                 XM_SWIZZLE_X,
                                                 XM_SWIZZLE_W,
                                                 XM_SWIZZLE_W>(V),
                                 S);
  return XMVectorSelect(R1, R2, control);
}

XMVECTOR ortho_branchless(XMVECTOR V) {
  const auto V0 = XMVectorSwizzle<XM_SWIZZLE_X,
                                  XM_SWIZZLE_X,
                                  XM_SWIZZLE_X,
                                  XM_SWIZZLE_X>(V);
  const auto T1 = XMVectorAdd(XMVectorAbs(V0), XMVectorReplicate(0.5));
  const auto T2 = XMVectorFloor(T1);
  const auto F = XMVectorSubtract(T1, T2);
  const auto FV = XMVectorMultiply(F, V);         //fv[0],fv[1],fv[2],fv[3]
  const auto R1 = XMVectorPermute<XM_PERMUTE_0Y,  // v[1],fv[2], v[1], X
                                  XM_PERMUTE_1Z,
                                  XM_PERMUTE_1Y,
                                  XM_PERMUTE_0W>(V, FV);
  const auto NN11 = XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f);
  const auto R2 = XMVectorMultiply(NN11, R1);
  const auto ZV0ZZ = XMVectorMultiply(
    XMVectorSetBinaryConstant(0, 1, 0, 0), V0);
  return XMVectorAdd(R2, ZV0ZZ);
}

// Returns unit quaternion rotating unit vectors |V1| into |V2|.
// Requires V1.w and V2.w equal 0.
XMVECTOR quat_from(XMVECTOR V1, XMVECTOR V2) {
  const XMVECTOR cos_theta = XMVector4Dot(V1, V2);
  const auto near_one = XMVectorSubtract(XMVectorSplatOne(),
                                         XMVectorSplatEpsilon());
  const XMVECTOR ctrl1 = XMVectorLess(cos_theta, XMVectorNegate(near_one));
  const XMVECTOR ctrl2 = XMVectorGreaterOrEqual(cos_theta, near_one);
  const auto R1 = XMVectorSelect(XMVectorAdd(XMVectorSplatOne(), cos_theta),
                                 XMVector3Cross(V1, V2),
                                 g_XMSelect1110.v);
  const auto R2 = ortho(V1);
  const auto Q = XMQuaternionNormalize(XMVectorSelect(R1, R2, ctrl1));
  return XMVectorSelect(Q, XMQuaternionIdentity(), ctrl2);
}

XMVECTOR quat_from_branched(XMVECTOR V1, XMVECTOR V2) {
  const XMVECTOR cos_theta = XMVector4Dot(V1, V2);
  const auto near_one = XMVectorSubtract(XMVectorSplatOne(),
                                         XMVectorSplatEpsilon());
  const XMVECTOR ctrl2 = XMVectorGreaterOrEqual(cos_theta, near_one);
  const auto R = XMVector4Less(cos_theta, XMVectorNegate(near_one)) ?
    ortho(V1) : XMVectorSelect(XMVectorAdd(XMVectorSplatOne(), cos_theta),
                               XMVector3Cross(V1, V2),
                               g_XMSelect1110.v);
  return XMVectorSelect(XMQuaternionNormalize(R),
                        XMQuaternionIdentity(),
                        ctrl2);
}

template <size_t N>
XMVECTOR premultiply(const XMVECTOR (&R)[N], XMVECTOR V) {
  auto R0 = XMVector4Dot(R[0], V);        // X, X, X, X
  const auto R1 = XMVector4Dot(R[1], V);  // Y, Y, Y, Y
  const auto R2 = XMVector4Dot(R[2], V);  // Z, Z, Z, Z
  R0 = XMVectorShiftLeft<1>(R0, R1);        // X, X, X, Y
  R0 = XMVectorShiftLeft<1>(R0, R2);        // X, X, Y, Z
  R0 = XMVectorRotateLeft<1>(R0);           // X, Y, Z, X
  XMVectorSelect(V, R0, g_XMSelect1110);    // X, Y, Z, W
  return R0;
}

XMFLOAT3X4A rigid_inv(XMFLOAT3X4A MT) {
  // X = | R  0 |   X' = |  R'  0 |   Storage = |R  (-tR')'|
  //     | t  1 |        |-tR'  1 |             |0     1   |
  auto M = XMLoadFloat3x4A(&MT);
  const auto T = premultiply(M.r, XMVectorNegate(M.r[3]));
  M.r[0] = XMVectorPermute<XM_PERMUTE_0X,
                           XM_PERMUTE_0Y,
                           XM_PERMUTE_0Z,
                           XM_PERMUTE_1X>(M.r[0], T);
  M.r[1] = XMVectorPermute<XM_PERMUTE_0X,
                           XM_PERMUTE_0Y,
                           XM_PERMUTE_0Z,
                           XM_PERMUTE_1Y>(M.r[1], T);
  M.r[2] = XMVectorPermute<XM_PERMUTE_0X,
                           XM_PERMUTE_0Y,
                           XM_PERMUTE_0Z,
                           XM_PERMUTE_1Z>(M.r[2], T);
  return XMFLOAT3X4A(reinterpret_cast<float*>(&M));
}

int main() {
  const XMMATRIX cam = XMMatrixLookAtRH(XMVECTOR{15.0f,-15.0f, 15.0f, 0.0f},
                                        XMVECTOR{},
                                        XMVECTOR{0.0f, 0.0f, 1.0f, 0.0f});
  XMVECTOR det;
  const auto camInv = XMMatrixInverse(&det, cam);
  std::cout << "Camera (4x4)\n" << cam << '\n'
            << "Camera Inv (4x4)\n" << camInv << '\n';

  // test rigid_inv
  XMFLOAT3X4A cam_3x4;
  XMStoreFloat3x4(&cam_3x4, cam);
  XMFLOAT3X4A cam_inv_3x4 = rigid_inv(cam_3x4);
  std::cout << "Camera (3x4)\n" << cam_3x4 << '\n'
            << "Camera Inv (3x4)\n" << cam_inv_3x4 << '\n';

  // test ortho and ortho_branchless
  XMVECTOR v = {0.70710678f, 1.70710678f, 0.70710678f, 0};
  std::cout << "Vector: " << v << '\n'
            << "Ortho: " << ortho(v) << '\n'
            << "Dot product: " << XMVector4Dot(v, ortho(v))
            << '\n';
  std::cout << "Vector: " << v << '\n'
            << "Ortho: " << ortho_branchless(v) << '\n'
            << "Dot product: " << XMVector4Dot(v, ortho(v))
            << "\n\n";

  // test quat_from; no test for quat_from_branched
  XMVECTOR q1 = quat_from(XMVector4Normalize(XMVECTOR{1.0f, 2.0f, 1.0f, 0.0f}),
                          XMVector4Normalize(XMVECTOR{3.0f, 6.0f, 3.0f, 0.0f}));
  std::cout << "Some quaternions\n" << q1 << '\n';
  XMVECTOR q2 = quat_from(XMVector4Normalize(XMVECTOR{ 1.0f, 2.0f, 1.0f, 0.0f}),
                          XMVector4Normalize(XMVECTOR{-1.0f,-2.0f,-1.0f, 0.0f}));
  std::cout << q2 << '\n';
  XMVECTOR q3 = quat_from(XMVector4Normalize(XMVECTOR{ 1.0f, 1.0f, 0.0f, 0.0f}),
                          XMVector4Normalize(XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f}));
  std::cout << q3 << "\n\n";

  // Make a quaternion from axis-angle and make a matrix from quaternion.
  XMVECTOR vQuat = XMQuaternionRotationAxis(XMVECTOR{0, 0, 1, 0}, XM_PIDIV2);
  auto mRot = XMMatrixRotationQuaternion(vQuat);
  // Rotate a vector
  std::cout << "Vector: " << v << '\n';
  v = XMVector4Transform(v, mRot);
  std::cout << "Rot(Vector, Z, 90°)" << v << '\n';

  // DXM uses row-vector convention (successive transforms post-multiply),
  // row-major storage and row-major notation; row 3 houses translation.
  auto mMov = XMMatrixTranslation(10, 20, 30);
  std::cout << mMov.r[3] << '\n';
  XMFLOAT4X4 mV;
  XMStoreFloat4x4(&mV, mMov);
  std::cout << mV.m[3][0] << '\n';
}
