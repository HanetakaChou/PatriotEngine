#version 310 es

/*
 * Copyright (c) 2015-2016 The Khronos Group Inc.
 * Copyright (c) 2015-2016 Valve Corporation
 * Copyright (c) 2015-2016 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Vertex shader used by Cube demo.
 */

layout(push_constant, column_major) uniform _unused_name_ubuf2
{
        highp mat4x4 VP;
};

layout(set = 0, binding = 0, column_major) uniform _unused_name_ubuf
{
        highp mat4x4 M;
};

layout(location = 0) in highp vec3 position;
layout(location = 1) in highp vec2 attr;

layout(location = 0) out highp vec2 texcoord;

void main()
{
        gl_Position = VP * M * vec4(position, 1.0f);
        texcoord = attr;
}
