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

layout(location = 0) in highp vec3 position;
layout(location = 1) in highp vec2 attr;

layout(location = 0) out highp vec2 texcoord;

layout(push_constant, column_major) uniform _unused_name_uniform_buffer
{
        highp mat4x4 VP;
        highp mat4x4 M;
};

void main()
{
        //1.01 depth clip
        vec4 position = vec4(position, 1.01f); //VP * M2 * vec4(position, 1.0f);

        gl_Position = position;

        texcoord = attr;
}