//
// Copyright (C) YuqiaoZhang(HanetakaChou)
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

#include "mesh_vertex_tangent.h"
#include <pt_mcrt_vector.h>
#include <array>

extern void mesh_vertex_calculate_tangents(
    size_t face_count,
    uint32_t const *indices,
    size_t vertex_count,
    pt_math_vec3 const *positions,
    pt_math_vec3 const *normals,
    pt_math_vec2 const *uvs,
    pt_math_vec4 *out_qtangents,
    float *out_reflections)
{

    // [DirectXMesh Wiki / ComputeTangentFrameImpl](https://github.com/microsoft/DirectXMesh/wiki/ComputeTangentFrame)
    // [DirectX::ComputeTangentFrameImpl](https://github.com/microsoft/DirectXMesh/blob/dec2022/DirectXMesh/DirectXMeshTangentFrame.cpp#L22)

    // floating-point addition is NOT associative
    // the "double" is used to improve precision
    mcrt_vector<std::array<double, 3U>> reduction_tangents(vertex_count);
    mcrt_vector<std::array<double, 3U>> reduction_bitangents(vertex_count);
    for (size_t vertex_index = 0U; vertex_index < vertex_count; ++vertex_index)
    {
        reduction_tangents[vertex_index][0] = 0.0;
        reduction_tangents[vertex_index][1] = 0.0;
        reduction_tangents[vertex_index][2] = 0.0;

        reduction_bitangents[vertex_index][0] = 0.0;
        reduction_bitangents[vertex_index][1] = 0.0;
        reduction_bitangents[vertex_index][2] = 0.0;
    }

    for (size_t face_index = 0U; face_index < face_count; ++face_index)
    {
        uint32_t i0 = indices[face_index * 3];
        uint32_t i1 = indices[face_index * 3 + 1];
        uint32_t i2 = indices[face_index * 3 + 2];

        pt_math_simd_vec const p0 = pt_math_load_vec3(&positions[i0]);
        pt_math_simd_vec const p1 = pt_math_load_vec3(&positions[i1]);
        pt_math_simd_vec const p2 = pt_math_load_vec3(&positions[i2]);

        double u0 = uvs[i0].x;
        double v0 = uvs[i0].y;
        double u1 = uvs[i1].x;
        double v1 = uvs[i1].y;
        double u2 = uvs[i2].x;
        double v2 = uvs[i2].y;

        // p1 - p0 = (u1 - u0) * t + (v1 - v0) * b
        // p2 - p0 = (u2 - u0) * t + (v2 - v0) * b
        //
        // [ p1 - p0 ]   [ u1 - u0  v1 - v0 ]   [ t ]
        // [ p2 - p0 ] = [ u2 - u0  v2 - v0 ] * [ b ]
        //
        // determinant =  (u1 - u0) * (v2 - v0) - (u2 - u0) - (v1 - v0)
        //
        //                                       [   v2 - v0    -(v1 - v0) ]
        // inverse_mat_v = (1.0 / determinant) * [ -(u2 - u0)     u1 - u0  ]
        //
        // [ t ] =                       [   v2 - v0    -(v1 - v0) ] * [ p1 - p0 ]
        // [ b ] = (1.0 / determinant) * [ -(u2 - u0)     u1 - u0  ] * [ p2 - p0 ]
        //
        // t = ((1.0 / determinant) *   (v2 - v0))  * [ p1 - p0 ] + ((1.0 / determinant) * (-(v1 - v0))) * [ p2 - p0 ]
        // b = ((1.0 / determinant) * (-(u2 - u0))) * [ p1 - p0 ] + ((1.0 / determinant) *   (u1 - u0))  * [ p2 - p0 ]

        double mat_uv_r0_c0 = u1 - u0;
        double mat_uv_r0_c1 = v1 - v0;
        double mat_uv_r1_c0 = u2 - u0;
        double mat_uv_r1_c1 = v2 - v0;

        double determinant = mat_uv_r0_c0 * mat_uv_r1_c1 - mat_uv_r1_c0 * mat_uv_r0_c1;

        double reciprocal_determinant = (((determinant < (-DBL_EPSILON)) || (determinant > DBL_EPSILON)) ? (1.0 / determinant) : 1.0);

        double inverse_mat_uv_r0_c0 = reciprocal_determinant * mat_uv_r1_c1;
        double inverse_mat_uv_r0_c1 = reciprocal_determinant * (-mat_uv_r0_c1);
        double inverse_mat_uv_r1_c0 = reciprocal_determinant * (-mat_uv_r1_c0);
        double inverse_mat_uv_r1_c1 = reciprocal_determinant * mat_uv_r0_c0;

        pt_math_simd_vec p0_to_p1 = pt_math_vec_subtract(p1, p0);
        pt_math_simd_vec p0_to_p2 = pt_math_vec_subtract(p2, p0);

        pt_math_alignas16_vec3 tangent;
        pt_math_store_alignas16_vec3(&tangent, pt_math_vec_add(pt_math_vec_scale(p0_to_p1, static_cast<float>(inverse_mat_uv_r0_c0)), pt_math_vec_scale(p0_to_p2, static_cast<float>(inverse_mat_uv_r0_c1))));
        pt_math_alignas16_vec3 bitangent;
        pt_math_store_alignas16_vec3(&bitangent, pt_math_vec_add(pt_math_vec_scale(p0_to_p1, static_cast<float>(inverse_mat_uv_r1_c0)), pt_math_vec_scale(p0_to_p2, static_cast<float>(inverse_mat_uv_r1_c1))));

        // blend tangents of the same vertex
        reduction_tangents[i0][0] += tangent.x;
        reduction_tangents[i0][1] += tangent.y;
        reduction_tangents[i0][2] += tangent.z;
        reduction_tangents[i1][0] += tangent.x;
        reduction_tangents[i1][1] += tangent.y;
        reduction_tangents[i1][2] += tangent.z;
        reduction_tangents[i2][0] += tangent.x;
        reduction_tangents[i2][1] += tangent.y;
        reduction_tangents[i2][2] += tangent.z;
        reduction_bitangents[i0][0] += bitangent.x;
        reduction_bitangents[i0][1] += bitangent.y;
        reduction_bitangents[i0][2] += bitangent.z;
        reduction_bitangents[i1][0] += bitangent.x;
        reduction_bitangents[i1][1] += bitangent.y;
        reduction_bitangents[i1][2] += bitangent.z;
        reduction_bitangents[i2][0] += bitangent.x;
        reduction_bitangents[i2][1] += bitangent.y;
        reduction_bitangents[i2][2] += bitangent.z;
    }

    for (size_t vertex_index = 0U; vertex_index < vertex_count; ++vertex_index)
    {
        pt_math_simd_vec normal;
        pt_math_simd_vec tangent;
        pt_math_simd_vec bitangent;
        {
            // Gram–Schmidt process
            // https://en.wikipedia.org/wiki/Gram%E2%80%93Schmidt_process
            //
            // (modified) Gram-Schmidt process
            // "Equation (9.48)" of [Real Time Rendering](https://www.realtimerendering.com/)
            //
            // e1 = normalize(v1)
            // e2 = normalize(v2 - e1 * dot(v2, e1))
            // e3 = normalize(v3 - e1 * dot(v3, e1) - e2 * dot(v3, e2))

            pt_math_simd_vec v1 = pt_math_load_vec3(&normals[vertex_index]);
            pt_math_alignas16_vec3 tmp_v2(static_cast<float>(reduction_tangents[vertex_index][0]), static_cast<float>(reduction_tangents[vertex_index][1]), static_cast<float>(reduction_tangents[vertex_index][2]));
            pt_math_simd_vec v2 = pt_math_load_alignas16_vec3(&tmp_v2);
            pt_math_alignas16_vec3 tmp_v3(static_cast<float>(reduction_bitangents[vertex_index][0]), static_cast<float>(reduction_bitangents[vertex_index][1]), static_cast<float>(reduction_bitangents[vertex_index][2]));
            pt_math_simd_vec v3 = pt_math_load_alignas16_vec3(&tmp_v3);

#if 1
            // v1, v2 and v3 should be linearly independent
            {
                float length_v1 = pt_math_vec_get_x(pt_math_vec3_length(v1));
                float length_v2 = pt_math_vec_get_x(pt_math_vec3_length(v2));
                float length_v3 = pt_math_vec_get_x(pt_math_vec3_length(v3));

                // Handle degenerate vectors
                if (length_v1 < FLT_EPSILON || length_v2 < FLT_EPSILON || length_v3 < FLT_EPSILON)
                {
                    auto reset_from_two_vectors = [](pt_math_simd_vec a, pt_math_simd_vec b, pt_math_simd_vec &c) -> void
                    {
                        // Reset c from a and b
                        pt_math_simd_vec simd_c_from_cross = pt_math_vec3_cross(a, b);

                        pt_math_alignas16_vec3 c_from_cross;
                        pt_math_store_alignas16_vec3(&c_from_cross, simd_c_from_cross);

                        pt_math_alignas16_vec3 c_from_asset;
                        pt_math_store_alignas16_vec3(&c_from_asset, c);

                        // +0 and -0 are different
                        float reflect = ((static_cast<double>(c_from_asset.x) * static_cast<double>(c_from_cross.x) + static_cast<double>(c_from_asset.y) * static_cast<double>(c_from_cross.y) + static_cast<double>(c_from_asset.z) * static_cast<double>(c_from_cross.z)) >= 0.0) ? 1.0F : -1.0F;

                        c = pt_math_vec_scale(simd_c_from_cross, reflect);
                    };

                    auto reset_from_one_vector = [](pt_math_simd_vec a, pt_math_simd_vec &b, pt_math_simd_vec &c) -> void
                    {
                        // Reset both b and c from a
                        pt_math_alignas16_vec3 tmp_a;
                        pt_math_store_alignas16_vec3(&tmp_a, a);

                        pt_math_alignas16_vec3 tmp_axis;
                        if (std::abs(tmp_a.x) < std::abs(tmp_a.y))
                        {
                            if (std::abs(tmp_a.x) < std::abs(tmp_a.z))
                            {
                                tmp_axis.x = 1.0;
                                tmp_axis.y = 0.0;
                                tmp_axis.z = 0.0;
                            }
                            else
                            {
                                tmp_axis.x = 0.0;
                                tmp_axis.y = 0.0;
                                tmp_axis.z = 1.0;
                            }
                        }
                        else
                        {
                            if (std::abs(tmp_a.y) < std::abs(tmp_a.z))
                            {
                                tmp_axis.x = 0.0;
                                tmp_axis.y = 1.0;
                                tmp_axis.z = 0.0;
                            }
                            else
                            {
                                tmp_axis.x = 0.0;
                                tmp_axis.y = 0.0;
                                tmp_axis.z = 1.0;
                            }
                        }

                        pt_math_alignas16_vec3 b_from_uv;
                        pt_math_store_alignas16_vec3(&b_from_uv, b);

                        // +0 and -0 are different
                        float reflect_b = ((static_cast<double>(b_from_uv.x) * static_cast<double>(tmp_axis.x) + static_cast<double>(b_from_uv.y) * static_cast<double>(tmp_axis.y) + static_cast<double>(b_from_uv.z) * static_cast<double>(tmp_axis.z)) >= 0.0) ? 1.0F : -1.0F;

                        b = pt_math_vec_scale(pt_math_load_alignas16_vec3(&tmp_axis), reflect_b);

                        pt_math_simd_vec simd_c_from_cross = pt_math_vec3_cross(a, b);

                        pt_math_alignas16_vec3 c_from_cross;
                        pt_math_store_alignas16_vec3(&c_from_cross, simd_c_from_cross);

                        pt_math_alignas16_vec3 c_from_asset;
                        pt_math_store_alignas16_vec3(&c_from_asset, c);

                        // +0 and -0 are different
                        float reflect_c = ((static_cast<double>(c_from_asset.x) * static_cast<double>(c_from_cross.x) + static_cast<double>(c_from_asset.y) * static_cast<double>(c_from_cross.y) + static_cast<double>(c_from_asset.z) * static_cast<double>(c_from_cross.z)) >= 0.0) ? 1.0F : -1.0F;

                        c = pt_math_vec_scale(simd_c_from_cross, reflect_c);
                    };

                    if (length_v1 >= FLT_EPSILON && length_v2 >= FLT_EPSILON)
                    {
                        // Reset v3 from v1 and v2
                        reset_from_two_vectors(v1, v2, v3);
                    }
                    else if (length_v2 >= FLT_EPSILON && length_v3 >= FLT_EPSILON)
                    {
                        // Reset v1 from v2 and v3
                        reset_from_two_vectors(v2, v3, v1);
                    }
                    else if (length_v3 >= FLT_EPSILON && length_v1 >= FLT_EPSILON)
                    {
                        // Reset v2 from v3 and v1
                        reset_from_two_vectors(v3, v1, v2);
                    }
                    else if (length_v1 >= FLT_EPSILON)
                    {
                        // Reset both v2 and v3 from v1
                        reset_from_one_vector(v1, v2, v3);
                    }
                    else if (length_v2 >= FLT_EPSILON)
                    {
                        // Reset both v3 and v1 from v2
                        reset_from_one_vector(v2, v3, v1);
                    }
                    else if (length_v3 >= FLT_EPSILON)
                    {
                        // Reset both v1 and v2 from v3
                        reset_from_one_vector(v3, v1, v2);
                    }
                    else
                    {
                        pt_math_alignas16_vec3 z_axis(0.0F, 0.0F, 1.0F);
                        v1 = pt_math_load_alignas16_vec3(&z_axis);

                        pt_math_alignas16_vec3 x_axis(1.0F, 0.0F, 0.0F);
                        v2 = pt_math_load_alignas16_vec3(&x_axis);

                        pt_math_alignas16_vec3 y_axis(0.0F, 1.0F, 0.0F);
                        v3 = pt_math_load_alignas16_vec3(&y_axis);
                    }
                }
            }
#endif

            pt_math_simd_vec e1 = pt_math_vec3_normalize(v1);
            pt_math_simd_vec e2 = pt_math_vec3_normalize(pt_math_vec_subtract(v2, pt_math_vec_scale(e1, pt_math_vec_get_x(pt_math_vec3_dot(v2, e1)))));
            pt_math_simd_vec e3 = pt_math_vec3_normalize(pt_math_vec_subtract(pt_math_vec_subtract(v3, pt_math_vec_scale(e1, pt_math_vec_get_x(pt_math_vec3_dot(v3, e1)))), pt_math_vec_scale(e2, pt_math_vec_get_x(pt_math_vec3_dot(v3, e2)))));

            normal = e1;
            tangent = e2;
            bitangent = e3;
        }

        float tangent_w;
        {
            // [DirectXMesh Wiki / ComputeTangentFrameImpl](https://github.com/microsoft/DirectXMesh/wiki/ComputeTangentFrame)
            //
            // "7.5 Tangent Space" of [Lengyel 2019] [Eric Lengyel. "Foundations of Game Engine Development : Volume 2 - Rendering." Terathon Software LLC 2019.](http://foundationsofgameenginedev.com/)
            // Since the vectors are orthogonal, it is not necessary to store all three of the vectors t, b, and n for each vertex.
            // Just the normal vector and the tangent vector will always suffice, but we do need one additional bit of information.
            // The tangent frame can form either a right-handed or left-handed coordinate system, and which one is given by the sign of dot(b, cross(n, t)).
            // Calling the sign of dot(b, cross(n, t)) "tangent_w", we can reconstitute the bitangent with the cross product b = tangent_w * cross(n, t), and then only the normal and tangent need to be supplied as vertex attributes.

            pt_math_simd_vec bitangent_from_cross = pt_math_vec3_cross(normal, tangent);
            tangent_w = pt_math_vec_get_x(pt_math_vec3_dot(bitangent, bitangent_from_cross)) > 0.0F ? 1.0F : -1.0F;
        }

        out_reflections[vertex_index] = tangent_w;

        pt_math_alignas16_vec4 quaternion_tangent;
        {
            // [Frey 2011] Ivo Frey. "Spherical Skinning withDual-Quaternions and Qtangents." SIGGRAPH 2011.

            // NOTE: We should force the handness of the basis to be the same before we use the "XMMatrixDecompose" or "XMQuaternionRotationMatrix".
            // NOTE: When the "tangent_w" is negative, the result of the "XMMatrixDecompose" or "XMQuaternionRotationMatrix" may NOT be correct.
            pt_math_simd_vec bitangent_from_cross = pt_math_vec3_cross(normal, tangent);

            pt_math_simd_mat tangent_to_model_transform;
            {
                tangent_to_model_transform.r[0] = tangent;
                tangent_to_model_transform.r[1] = bitangent_from_cross;
                tangent_to_model_transform.r[2] = normal;
                pt_math_alignas16_vec4 identity_r3(0.0F, 0.0F, 0.0F, 1.0F);
                tangent_to_model_transform.r[3] = pt_math_load_alignas16_vec4(&identity_r3);
            }

#if 1
            // TODO: Can we prove this is a rotation transform?
            pt_math_simd_vec unit_quaternion_rotation_transform;
            {
                pt_math_simd_vec unused_scale;
                pt_math_simd_vec unused_translation;
                bool res_mat_decompose = pt_math_mat_decompose(&unused_scale, &unit_quaternion_rotation_transform, &unused_translation, tangent_to_model_transform);
                assert(res_mat_decompose);

                assert(pt_math_vec_get_x(pt_math_vec3_length(tangent)) < FLT_EPSILON || std::abs(pt_math_vec_get_x(unused_scale) - 1.0F) < 0.0002F);
                assert(pt_math_vec_get_x(pt_math_vec3_length(bitangent_from_cross)) < FLT_EPSILON || std::abs(pt_math_vec_get_y(unused_scale) - 1.0F) < 0.0002F);
                assert(pt_math_vec_get_x(pt_math_vec3_length(normal)) < FLT_EPSILON || std::abs(pt_math_vec_get_z(unused_scale) - 1.0F) < 0.0002F);

                assert(std::abs(pt_math_vec_get_x(unused_translation)) < 0.0002F);
                assert(std::abs(pt_math_vec_get_y(unused_translation)) < 0.0002F);
                assert(std::abs(pt_math_vec_get_z(unused_translation)) < 0.0002F);
            }

#else
            unit_quaternion_rotation_transform = pt_math_quat_rotation_mat(tangent_to_model_transform);
#endif

            // Antipodal (Sign Invariant): the unit quaternion q and -q represent the same rotation transform.
            //
            // We can use the sign of the w component of the quaternion to denote the "tangent_w".
            // NOTE: When the w component of the quaternion is close to 0, the sign (+0 or -0) may NOT be distinguished by GPU. A bias may be introduced to avoid this situation.
            //
            // Perhaps, we can compress the unit quaternion to three components.
            // But the precision of the vertex format is much lower. And the result "1.0 - dot(quaternion_tangent.xyz, quaternion_tangent.xyz)" can be negative.
            // Thus currently we still use four components.
            pt_math_store_vec4(&quaternion_tangent, unit_quaternion_rotation_transform);
        }

        out_qtangents[vertex_index] = quaternion_tangent;
    }
}
