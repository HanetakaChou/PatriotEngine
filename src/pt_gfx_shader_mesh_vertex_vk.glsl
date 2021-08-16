#version 310 es

/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

layout(location = 0) in highp vec3 position;
layout(location = 1) in highp vec2 attr;

layout(location = 0) out highp vec2 texcoord;

void main()
{
        //gl_Position = vec4(position.xy, 0.5, 1.0f); //VP * M2 * vec4(position, 1.0f);
        //texcoord = attr;
        
        if(0 == gl_VertexIndex)
        {
                gl_Position = vec4(1.0f,1.0f,0.5f,1.0f);
                texcoord = vec2(1.0f, 1.0f);
        }
        else if(1 == gl_VertexIndex)
        {
                gl_Position = vec4(1.0f,-1.0f,0.5f,1.0f);
                texcoord = vec2(1.0f, 0.0f);
        }
        else if(2 == gl_VertexIndex)
        {
                gl_Position = vec4(-1.0f,-1.0f,0.5f,1.0f);
                texcoord = vec2(0.0f, 0.0f);
        }
        else if(3 == gl_VertexIndex)
        {
                gl_Position = vec4(-1.0f,-1.0f,0.5f,1.0f);
                texcoord = vec2(0.0f, 0.0f);
        }
        else if(4 == gl_VertexIndex)
        {
                gl_Position = vec4(-1.0f,1.0f,0.5f,1.0f);
                texcoord = vec2(0.0f, 1.0f);
        }
        else if(5 == gl_VertexIndex)
        {
                gl_Position = vec4(1.0f,1.0f,0.5f,1.0f);
                texcoord = vec2(1.0f, 1.0f);
        }
}