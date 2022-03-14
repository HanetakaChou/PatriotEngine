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

layout(set = 0, binding = 1) readonly buffer _unused_name_global_set_per_drawcall_binding_buffer
{
    highp mat4x4 Ms[4096];
};

layout(location = 0) in highp vec3 in_position_model_space;

layout(location = 0) out highp vec3 out_position_world_space;

void main(void)
{
    mat4x4 M = Ms[gl_InstanceIndex];

    out_position_world_space = (M * vec4(in_position_model_space, 1.0)).xyz;
}