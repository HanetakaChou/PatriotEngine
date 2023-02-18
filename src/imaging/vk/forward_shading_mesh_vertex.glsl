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

layout(location = 0) in highp vec3 in_position_model_space;
layout(location = 1) in highp vec3 in_normal;
layout(location = 2) in highp vec3 in_tangent;
layout(location = 3) in highp vec2 in_uv;

layout(location = 0) out highp vec3 out_position_world_space;
layout(location = 1) out highp vec3 out_normal_world_space;
layout(location = 2) out highp vec2 out_uv;

layout(push_constant, column_major) uniform _unused_name_uniform_buffer
{
        highp mat4x4 projection_view_matrix;
        highp mat4x4 model_matrix;
};


void main()
{
        // Input
        highp vec3 position_model_space = in_position_model_space;

        // 1.01 depth clip
        highp vec3 position_world_space = (model_matrix * vec4(position_model_space, 1.0)).xyz;
        highp vec4 position_clip_space = projection_view_matrix * vec4(position_world_space, 1.0); 

        // TODO: normal matrix
        highp mat3x3 tangent_matrix = mat3x3(model_matrix[0].xyz, model_matrix[1].xyz, model_matrix[2].xyz);
        highp vec3 normal_world_space = normalize(tangent_matrix * in_normal);

        // Output
        gl_Position = position_clip_space;
        out_position_world_space = position_world_space;
        out_normal_world_space = normal_world_space;
        out_uv = in_uv;
}