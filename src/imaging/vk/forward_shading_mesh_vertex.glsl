#version 310 es

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

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_control_flow_attributes : enable

#include "quaternion.glsli"

layout(location = 0) in highp vec3 in_position;
layout(location = 1) in highp vec4 in_qtangentxyz_xyz_reflection_w;
layout(location = 2) in highp vec4 in_qtangentw_x_uv_yz;

layout(location = 0) out highp vec3 out_position_world_space;
layout(location = 1) out highp vec3 out_normal_world_space;
layout(location = 2) out highp vec3 out_tangent_world_space;
layout(location = 3) out highp vec3 out_bitangent_world_space;
layout(location = 4) out highp vec2 out_uv;

layout(push_constant, column_major) uniform _unused_name_uniform_buffer
{
    highp mat4x4 projection_view_matrix;
    highp mat4x4 model_matrix;
};

void main()
{
    // Input
    highp vec3 position_model_space;
    highp vec4 qtangent;
    highp float reflection;
    highp vec2 uv;
    {
        position_model_space = in_position;

        // [Frey 2011] Ivo Frey. "Spherical Skinning withDual-Quaternions and Qtangents." SIGGRAPH 2011.
        //
        // Perhaps, we can compress the unit quaternion to three components.
        // But the precision of the vertex format is much lower. And the result "1.0 - dot(quaternion_tangent.xyz, quaternion_tangent.xyz)" can be negative.
        // Thus currently we still use four components.
        qtangent = vec4(in_qtangentxyz_xyz_reflection_w.xyz, in_qtangentw_x_uv_yz.x) * 2.0 - 1.0;

        reflection = (in_qtangentxyz_xyz_reflection_w.w > 0.0) ? 1.0 : -1.0;

        uv = in_qtangentw_x_uv_yz.yz;
    }

    highp vec3 normal_tangent_space = vec3(0.0, 0.0, 1.0);
    highp vec3 tangent_tangent_space = vec3(1.0, 0.0, 0.0);
    highp vec3 bitangent_tangent_space = vec3(0.0, 1.0, 0.0);

    // Currently, the "reflection" is the sign of "dot(b, cross(n, t))".
    // In some engines, the "reflection" is the sign of "dot(n, cross(t, b))". In this case, we should modify the "normal_tangent_space.z" instead.
    normal_tangent_space.y *= ((reflection > 0.0) ? 1.0 : -1.0);
    tangent_tangent_space.y *= ((reflection > 0.0) ? 1.0 : -1.0);
    bitangent_tangent_space.y *= ((reflection > 0.0) ? 1.0 : -1.0);

    // Compute Model Space
    highp vec3 normal_model_space = unit_quaternion_to_rotation_transform(qtangent, normal_tangent_space);
    highp vec3 tangent_model_space = unit_quaternion_to_rotation_transform(qtangent, tangent_tangent_space);
    highp vec3 bitangent_model_space = unit_quaternion_to_rotation_transform(qtangent, bitangent_tangent_space);

    // Compute World Space
    highp vec3 position_world_space = (model_matrix * vec4(position_model_space, 1.0)).xyz;

    highp mat3x3 tangent_matrix = mat3x3(model_matrix[0].xyz, model_matrix[1].xyz, model_matrix[2].xyz);
    // TODO: normal matrix
    highp vec3 normal_world_space = normalize(tangent_matrix * normal_model_space);
    highp vec3 tangent_world_space = normalize(tangent_matrix * tangent_model_space);
    highp vec3 bitangent_world_space = normalize(tangent_matrix * bitangent_model_space);

    // Compute Clip Space
    highp vec4 position_clip_space = projection_view_matrix * vec4(position_world_space, 1.0);

    // Output
    gl_Position = position_clip_space;
    out_position_world_space = position_world_space;
    // TODO: Can we interpolate the quaternion?
    // out_qtangent = qtangent;
    out_normal_world_space = normal_model_space;
    out_tangent_world_space = tangent_model_space;
    out_bitangent_world_space = bitangent_model_space;
    out_uv = uv;
}