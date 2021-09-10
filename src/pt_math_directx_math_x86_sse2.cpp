//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include <pt_math.h>

#if defined(PT_X64) || defined(PT_X86)

#if defined(PT_POSIX)
//#include "sal.h"
#define _In_
#define _In_reads_(expression)
#define _In_reads_bytes_(expression)
#define _Out_
#define _Out_opt_
#define _Out_writes_(expression)
#define _Out_writes_bytes_(expression)
#define _Success_(expression)
#define _Use_decl_annotations_
#define _Analysis_assume_(expression)
#endif

//https://github.com/microsoft/DirectXMath/tree/83634c742a85d1027765af53fbe79506fd72e0c3
#include <DirectXMath.h>

#ifdef _XM_AVX2_INTRINSICS_
#error AVX2 supported
#endif

#ifdef _XM_AVX_INTRINSICS_
#error AVX supported
#endif

#ifndef _XM_SSE_INTRINSICS_
#error SSE not supported
#endif

#include <DirectXCollision.h>

static_assert(sizeof(pt_math_vec3) == sizeof(DirectX::XMFLOAT3), "");
static_assert(alignof(pt_math_vec3) == alignof(DirectX::XMFLOAT3), "");
static_assert(sizeof(pt_math_vec4) == sizeof(DirectX::XMFLOAT4), "");
static_assert(alignof(pt_math_vec4) == alignof(DirectX::XMFLOAT4), "");
static_assert(sizeof(pt_math_simd_vec) == sizeof(DirectX::XMVECTOR), "");
static_assert(alignof(pt_math_simd_vec) == alignof(DirectX::XMVECTOR), "");
static_assert(sizeof(pt_math_mat4x4) == sizeof(DirectX::XMFLOAT4X4), "");
static_assert(alignof(pt_math_mat4x4) == alignof(DirectX::XMFLOAT4X4), "");
static_assert(sizeof(pt_math_alignas16_mat4x4) == sizeof(DirectX::XMFLOAT4X4A), "");
static_assert(alignof(pt_math_alignas16_mat4x4) == alignof(DirectX::XMFLOAT4X4A), "");
static_assert(sizeof(pt_math_simd_mat) == sizeof(DirectX::XMMATRIX), "");
static_assert(alignof(pt_math_simd_mat) == alignof(DirectX::XMMATRIX), "");

static_assert(sizeof(pt_math_bounding_sphere) == sizeof(DirectX::BoundingSphere), "");
static_assert(alignof(pt_math_bounding_sphere) == alignof(DirectX::BoundingSphere), "");
static_assert(sizeof(pt_math_bounding_sphere::m_center) == sizeof(DirectX::BoundingSphere::Center), "");
static_assert(offsetof(pt_math_bounding_sphere, m_center) == offsetof(DirectX::BoundingSphere, Center), "");
static_assert(sizeof(pt_math_bounding_sphere::m_radius) == sizeof(DirectX::BoundingSphere::Radius), "");
static_assert(offsetof(pt_math_bounding_sphere, m_radius) == offsetof(DirectX::BoundingSphere, Radius), "");

static_assert(sizeof(pt_math_bounding_box) == sizeof(DirectX::BoundingBox), "");
static_assert(alignof(pt_math_bounding_box) == alignof(DirectX::BoundingBox), "");
static_assert(sizeof(pt_math_bounding_box::m_center) == sizeof(DirectX::BoundingBox::Center), "");
static_assert(offsetof(pt_math_bounding_box, m_center) == offsetof(DirectX::BoundingBox, Center), "");
static_assert(sizeof(pt_math_bounding_box::m_extents) == sizeof(DirectX::BoundingBox::Extents), "");
static_assert(offsetof(pt_math_bounding_box, m_extents) == offsetof(DirectX::BoundingBox, Extents), "");

inline DirectX::XMFLOAT3 *unwrap(pt_math_vec3 *vec3) { return reinterpret_cast<DirectX::XMFLOAT3 *>(vec3); }
inline DirectX::XMFLOAT3 const *unwrap(pt_math_vec3 const *vec3) { return reinterpret_cast<DirectX::XMFLOAT3 const *>(vec3); }
inline DirectX::XMFLOAT3A *unwrap(pt_math_alignas16_vec3 *vec3) { return reinterpret_cast<DirectX::XMFLOAT3A *>(vec3); }
inline DirectX::XMFLOAT3A const *unwrap(pt_math_alignas16_vec3 const *vec3) { return reinterpret_cast<DirectX::XMFLOAT3A const *>(vec3); }
inline DirectX::XMFLOAT4 *unwrap(pt_math_vec4 *vec4) { return reinterpret_cast<DirectX::XMFLOAT4 *>(vec4); }
inline DirectX::XMFLOAT4 const *unwrap(pt_math_vec4 const *vec4) { return reinterpret_cast<DirectX::XMFLOAT4 const *>(vec4); }
inline DirectX::XMFLOAT4A *unwrap(pt_math_alignas16_vec4 *vec4) { return reinterpret_cast<DirectX::XMFLOAT4A *>(vec4); }
inline DirectX::XMFLOAT4A const *unwrap(pt_math_alignas16_vec4 const *vec4) { return reinterpret_cast<DirectX::XMFLOAT4A const *>(vec4); }
inline DirectX::XMVECTOR unwrap(pt_math_simd_vec simd_vec) { return (*reinterpret_cast<DirectX::XMVECTOR *>(&simd_vec)); }
inline DirectX::XMVECTOR *unwrap(pt_math_simd_vec *simd_vec) { return reinterpret_cast<DirectX::XMVECTOR *>(simd_vec); }
inline pt_math_simd_vec wrap(DirectX::XMVECTOR simd_vec) { return (*reinterpret_cast<pt_math_simd_vec *>(&simd_vec)); }
inline DirectX::XMFLOAT4X4 *unwrap(pt_math_mat4x4 *mat4x4) { return reinterpret_cast<DirectX::XMFLOAT4X4 *>(mat4x4); }
inline DirectX::XMFLOAT4X4 const *unwrap(pt_math_mat4x4 const *mat4x4) { return reinterpret_cast<DirectX::XMFLOAT4X4 const *>(mat4x4); }
inline DirectX::XMFLOAT4X4A *unwrap(pt_math_alignas16_mat4x4 *alignas16_mat4x4) { return reinterpret_cast<DirectX::XMFLOAT4X4A *>(alignas16_mat4x4); }
inline DirectX::XMFLOAT4X4A const *unwrap(pt_math_alignas16_mat4x4 const *alignas16_mat4x4) { return reinterpret_cast<DirectX::XMFLOAT4X4A const *>(alignas16_mat4x4); }
inline DirectX::XMMATRIX unwrap(pt_math_simd_mat simd_mat) { return (*reinterpret_cast<DirectX::XMMATRIX *>(&simd_mat)); }
inline pt_math_simd_mat wrap(DirectX::XMMATRIX simd_mat) { return (*reinterpret_cast<pt_math_simd_mat *>(&simd_mat)); }

inline DirectX::BoundingSphere const *unwrap(pt_math_bounding_sphere const *bounding_sphere) { return reinterpret_cast<DirectX::BoundingSphere const *>(bounding_sphere); }
inline pt_math_bounding_sphere wrap(DirectX::BoundingSphere bounding_sphere) { return (*reinterpret_cast<pt_math_bounding_sphere *>(&bounding_sphere)); }

inline DirectX::BoundingBox const *unwrap(pt_math_bounding_box const *bounding_box) { return reinterpret_cast<DirectX::BoundingBox const *>(bounding_box); }
inline pt_math_bounding_box wrap(DirectX::BoundingBox bounding_box) { return (*reinterpret_cast<pt_math_bounding_box *>(&bounding_box)); }

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_vec3(pt_math_vec3 const *source)
{
    return wrap(DirectX::XMLoadFloat3(unwrap(source)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_alignas16_vec3(pt_math_alignas16_vec3 const *source)
{
    return wrap(DirectX::XMLoadFloat3A(unwrap(source)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_vec4(pt_math_vec4 const *source)
{
    return wrap(DirectX::XMLoadFloat4(unwrap(source)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_alignas16_vec4(pt_math_alignas16_vec4 const *source)
{
    return wrap(DirectX::XMLoadFloat4A(unwrap(source)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_alignas16_ivec3(pt_math_alignas16_ivec3 const *source)
{
    return wrap(DirectX::XMLoadInt3A(reinterpret_cast<uint32_t const *>(source->v)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_zero()
{
    return wrap(DirectX::XMVectorZero());
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_replicate(float value)
{
    return wrap(DirectX::XMVectorReplicate(value));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec3_transform(pt_math_simd_vec v, pt_math_simd_mat m)
{
    return wrap(DirectX::XMVector3Transform(unwrap(v), unwrap(m)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec3_transform_coord(pt_math_simd_vec v, pt_math_simd_mat m)
{
    return wrap(DirectX::XMVector3TransformCoord(unwrap(v), unwrap(m)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec3_normalize(pt_math_simd_vec v)
{
    return wrap(DirectX::XMVector3Normalize(unwrap(v)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec3_dot(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return wrap(DirectX::XMVector3Dot(unwrap(v1), unwrap(v2)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec4_dot(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return wrap(DirectX::XMVector4Dot(unwrap(v1), unwrap(v2)));
}

float PT_VECTORCALL directx_math_x86_sse2_vec_get_x(pt_math_simd_vec v)
{
    return DirectX::XMVectorGetX(unwrap(v));
}

float PT_VECTORCALL directx_math_x86_sse2_vec_get_y(pt_math_simd_vec v)
{
    return DirectX::XMVectorGetY(unwrap(v));
}

float PT_VECTORCALL directx_math_x86_sse2_vec_get_z(pt_math_simd_vec v)
{
    return DirectX::XMVectorGetZ(unwrap(v));
}

float PT_VECTORCALL directx_math_x86_sse2_vec_get_w(pt_math_simd_vec v)
{
    return DirectX::XMVectorGetW(unwrap(v));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_abs(pt_math_simd_vec v)
{
    return wrap(DirectX::XMVectorAbs(unwrap(v)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_splat_w(pt_math_simd_vec v)
{
    return wrap(DirectX::XMVectorSplatW(unwrap(v)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_convert_int_to_float(pt_math_simd_vec vint, uint32_t div_exponent)
{
    return wrap(DirectX::XMConvertVectorIntToFloat(unwrap(vint), div_exponent));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_convert_float_to_int(pt_math_simd_vec vfloat, uint32_t div_exponent)
{
    return wrap(DirectX::XMConvertVectorFloatToInt(unwrap(vfloat), div_exponent));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_add(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return wrap(DirectX::XMVectorAdd(unwrap(v1), unwrap(v2)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_min(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return wrap(DirectX::XMVectorMin(unwrap(v1), unwrap(v2)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_subtract(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return wrap(DirectX::XMVectorSubtract(unwrap(v1), unwrap(v2)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_multiply(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return wrap(DirectX::XMVectorMultiply(unwrap(v1), unwrap(v2)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_divide(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return wrap(DirectX::XMVectorDivide(unwrap(v1), unwrap(v2)));
}

void PT_VECTORCALL directx_math_x86_sse2_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v)
{
    return DirectX::XMStoreFloat3(unwrap(source), unwrap(v));
}

void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_vec3(pt_math_alignas16_vec3 *source, pt_math_simd_vec v)
{
    return DirectX::XMStoreFloat3A(unwrap(source), unwrap(v));
}

void PT_VECTORCALL directx_math_x86_sse2_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v)
{
    return DirectX::XMStoreFloat4(unwrap(source), unwrap(v));
}

void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_vec4(pt_math_alignas16_vec4 *source, pt_math_simd_vec v)
{
    return DirectX::XMStoreFloat4A(unwrap(source), unwrap(v));
}

void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_ivec3(pt_math_alignas16_ivec3 *destination, pt_math_simd_vec v)
{
    return DirectX::XMStoreInt3A(reinterpret_cast<uint32_t *>(destination->v), unwrap(v));
}

pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source)
{
    return wrap(DirectX::XMLoadFloat4x4A(unwrap(source)));
}

pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_load_mat4x4(pt_math_mat4x4 const *source)
{
    return wrap(DirectX::XMLoadFloat4x4(unwrap(source)));
}

pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_identity()
{
    return wrap(DirectX::XMMatrixIdentity());
}

pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2)
{
    return wrap(DirectX::XMMatrixMultiply(unwrap(m1), unwrap(m2)));
}

pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_rotation_y(float angle)
{
    return wrap(DirectX::XMMatrixRotationY(angle));
}

pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction)
{
    return wrap(DirectX::XMMatrixLookToRH(unwrap(eye_position), unwrap(eye_direction), unwrap(up_direction)));
}

pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_perspective_fov_rh(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
{
    // [Reversed-Z](https://developer.nvidia.com/content/depth-precision-visualized)
    //
    // _  0  0  0
    // 0  _  0  0
    // 0  0  b -1
    // 0  0  a  0
    //
    // _  0  0  0
    // 0  _  0  0
    // 0  0 zb  -z
    // 0  0  a
    //
    // z' = -b - a/z
    //
    // Standard
    // 0 = -b + a/nearz // z=-nearz
    // 1 = -b + a/farz  // z=-farz
    // a = farz*nearz/(nearz - farz)
    // b = farz/(nearz - farz)
    //
    // Reversed-Z
    // 1 = -b + a/nearz // z=-nearz
    // 0 = -b + a/farz  // z=-farz
    // a = farz*nearz/(farz - nearz)
    // b = nearz/(farz - nearz)

    // __m128 _mm_shuffle_ps(__m128 lo,__m128 hi, _MM_SHUFFLE(hi3,hi2,lo1,lo0))
    // Interleave inputs into low 2 floats and high 2 floats of output. Basically
    // out[0]=lo[lo0];
    // out[1]=lo[lo1];
    // out[2]=hi[hi2];
    // out[3]=hi[hi3];

    // DirectX::XMMatrixPerspectiveFovRH
    float SinFov;
    float CosFov;
    DirectX::XMScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);
    // Note: This is recorded on the stack
    float Height = CosFov / SinFov;
    DirectX::XMVECTOR rMem = {
        Height / AspectRatio,
        Height,
        NearZ / (FarZ - NearZ),
        (FarZ / (FarZ - NearZ)) * NearZ};
    // Copy from memory to SSE register
    DirectX::XMVECTOR vValues = rMem;
    DirectX::XMVECTOR vTemp = _mm_setzero_ps();
    // Copy x only
    vTemp = _mm_move_ss(vTemp, vValues);
    // CosFov / SinFov,0,0,0
    DirectX::XMMATRIX M;
    M.r[0] = vTemp;
    // 0,Height / AspectRatio,0,0
    vTemp = vValues;
    vTemp = _mm_and_ps(vTemp, DirectX::g_XMMaskY);
    M.r[1] = vTemp;
    // x=b,y=a,0,-1.0f
    vTemp = _mm_setzero_ps();
    vValues = _mm_shuffle_ps(vValues, DirectX::g_XMNegIdentityR3, _MM_SHUFFLE(3, 2, 3, 2));
    // 0,0,b,-1.0f
    vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 0, 0, 0));
    M.r[2] = vTemp;
    // 0,0,a,0.0f
    vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 1, 0, 0));
    M.r[3] = vTemp;
    return wrap(M);
}

pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_transpose(pt_math_simd_mat m)
{
    return wrap(DirectX::XMMatrixTranspose(unwrap(m)));
}

pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_inverse(pt_math_simd_vec *determinant, pt_math_simd_mat m)
{
    return wrap(DirectX::XMMatrixInverse(unwrap(determinant), unwrap(m)));
}

void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m)
{
    return DirectX::XMStoreFloat4x4A(unwrap(destination), unwrap(m));
}

void PT_VECTORCALL directx_math_x86_sse2_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m)
{
    return DirectX::XMStoreFloat4x4(unwrap(destination), unwrap(m));
}

pt_math_bounding_sphere PT_VECTORCALL directx_math_x86_sse2_bounding_sphere_create(pt_math_vec3 const *center, float radius)
{
    DirectX::BoundingSphere bounding_sphere(*unwrap(center), radius);
    return wrap(bounding_sphere);
}

bool PT_VECTORCALL directx_math_x86_sse2_bounding_sphere_intersect_ray(pt_math_bounding_sphere const *bounding_sphere, pt_math_simd_vec origin, pt_math_simd_vec direction, float *dist)
{
    return unwrap(bounding_sphere)->Intersects(unwrap(origin), unwrap(direction), *dist);
}

pt_math_bounding_box PT_VECTORCALL directx_math_x86_sse2_bounding_box_create_from_sphere(pt_math_bounding_sphere const *bounding_sphere)
{
    DirectX::BoundingBox bounding_box;
    DirectX::BoundingBox::CreateFromSphere(bounding_box, *unwrap(bounding_sphere));
    return wrap(bounding_box);
}

pt_math_bounding_box PT_VECTORCALL directx_math_x86_sse2_bounding_box_create_merged(pt_math_bounding_box const *b1, pt_math_bounding_box const *b2)
{
    DirectX::BoundingBox bounding_box;
    DirectX::BoundingBox::CreateMerged(bounding_box, *unwrap(b1), *unwrap(b2));
    return wrap(bounding_box);
}

pt_math_bounding_box PT_VECTORCALL directx_math_x86_sse2_bounding_box_create_from_points(size_t count, pt_math_vec3 const *points, size_t stride)
{
    DirectX::BoundingBox bounding_box;
    DirectX::BoundingBox::CreateFromPoints(bounding_box, count, unwrap(points), stride);
    return wrap(bounding_box);
}

#endif
