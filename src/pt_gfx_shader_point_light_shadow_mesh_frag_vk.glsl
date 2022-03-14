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

#extension GL_EXT_geometry_shader : enable

#define MAX_POINT_LIGHT_COUNT 20

layout(set = 0, binding = 0) readonly buffer _unused_name_global_set_per_frame_binding_buffer
{
    uint point_light_count;
    uint _padding_point_light_count_0;
    uint _padding_point_light_count_1;
    uint _padding_point_light_count_2;
    highp vec4 point_lights_position_and_radius[MAX_POINT_LIGHT_COUNT];
};

layout(location = 0) in highp float in_inv_length;
// NOTE: we can't interpolate the length of "position_view_space" directly, otherwise the result is incorrect
layout(location = 1) in highp vec3 in_inv_length_position_view_space;

layout(location = 0) out highp float out_depth;

void main()
{
    // perspective correct interpolation_
    highp vec3 position_view_space = in_inv_length_position_view_space / in_inv_length;

    highp float point_light_radius = point_lights_position_and_radius[gl_Layer / 2].w;

    highp float ratio = length(position_view_space) / point_light_radius;

    // Trick: we don't write to depth, and thus we can use early depth test
    gl_FragDepth = ratio;  
    out_depth = ratio;
}