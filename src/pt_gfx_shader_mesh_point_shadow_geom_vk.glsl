#version 310 es

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

// Imagination Technologies Limited. "Dual Paraboloid Environment Mapping." Power SDK Whitepaper 2017.
// https://github.com/powervr-graphics/Native_SDK/blob/R17.1-v4.3/Documentation/Whitepapers/Dual%20Paraboloid%20Environment%20Mapping.Whitepaper.pdf

#extension GL_EXT_geometry_shader : enable

#define MAX_POINT_LIGHT_COUNT 20

layout(set = 0, binding = 0) readonly buffer _unused_name_global_set_per_frame_binding_buffer
{
    uint point_light_count;
    uint _padding_point_light_count_0;
    uint _padding_point_light_count_1;
    uint _padding_point_light_count_2;
    vec4 point_lights_position_and_radius[MAX_POINT_LIGHT_COUNT];
};

layout(triangles) in;
layout(triangle_strip, max_vertices = 60) out; // 60 = 3 * MAX_POINT_LIGHT_COUNT

layout(location = 0) in vec3 in_positions_world_space[];

void main()
{
    for (highp int point_light_index = 0; point_light_index < int(point_light_count) && point_light_index < MAX_POINT_LIGHT_COUNT; ++point_light_index)
    {
        vec3 point_light_position = point_lights_position_and_radius[point_light_index].xyz;
        float point_light_radius = point_lights_position_and_radius[point_light_index].w;

        for (highp int vertex_index = 0; vertex_index < 3; ++vertex_index)
        {
            vec3 position_world_space = in_positions_world_space[vertex_index];

            // world space to light view space
            // identity rotation
            // Z - Up
            // Y - Forward
            // X - Right
            vec3 position_view_space = position_world_space - point_light_position;

            vec3 position_spherical_function_domain = normalize(position_view_space);

            // use abs to avoid divergence
            // z > 0
            // (x_2d, y_2d, 0) + (0, 0, 1) = λ ((x_sph, y_sph, z_sph) + (0, 0, 1))
            // (x_2d, y_2d) = (x_sph, y_sph) / (z_sph + 1)
            // z < 0
            // (x_2d, y_2d, 0) + (0, 0, -1) = λ ((x_sph, y_sph, z_sph) + (0, 0, -1))
            // (x_2d, y_2d) = (x_sph, y_sph) / (-z_sph + 1)
            vec4 position_clip;
            position_clip.xy = position_spherical_function_domain.xy;
            position_clip.w = abs(position_spherical_function_domain.z) + 1.0;
            position_clip.z = length(position_view_space) * position_clip.w / point_light_radius;
            gl_Position = position_clip;

            // use sign to avoid divergence
            // -0.5 to 0
            // 1.5 to 1
            gl_Layer = int(sign(position_spherical_function_domain.z) + 0.5) + 2 * point_light_index;
            EmitVertex();
        }
        EndPrimitive();
    }
}
