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

layout(set = 0, binding = 5) uniform highp sampler2DArray point_lights_shadow;

layout(set = 1, binding = 0) uniform highp sampler2D tex;

layout(location = 0) in highp vec3 in_position_world_space;
layout(location = 1) in highp vec2 texcoord;

layout(location = 0) out highp vec4 uFragColor;

//highp vec2 ndc_to_uv(highp vec2 ndc)
//{
//   return (ndc * vec2(0.5, 0,5) + vec2(0.5, 0.5));
//}

void main()
{
#if 0
   highp float shadow_attenuation;
   {
            // world space to light view space
            // identity rotation
            // Z - Up
            // Y - Forward
            // X - Right
            highp vec3 position_view_space = in_position_world_space - point_light_position;

            highp vec3 position_spherical_function_domain = normalize(position_view_space);

            // use abs to avoid divergence
            // z > 0
            // (x_2d, y_2d, 0) + (0, 0, 1) = λ ((x_sph, y_sph, z_sph) + (0, 0, 1))
            // (x_2d, y_2d) = (x_sph, y_sph) / (z_sph + 1)
            // z < 0
            // (x_2d, y_2d, 0) + (0, 0, -1) = λ ((x_sph, y_sph, z_sph) + (0, 0, -1))
            // (x_2d, y_2d) = (x_sph, y_sph) / (-z_sph + 1)
            highp vec2 position_ndcxy = position_spherical_function_domain.xy / (abs(position_spherical_function_domain.z) + 1.0);

            // use sign to avoid divergence
            // -1.0 to 0
            // 1.0 to 1
            highp vec2 uv = ndcxy_to_uv(position_ndcxy);
            highp float layer_index = (0.5 + 0.5 * sign(position_spherical_function_domain.z)) + 2.0 * float(point_light_index);
            
            highp float closest_length = (texture(point_lights_shadow, vec3(uv, layer_index)).r + 0.000075) * point_light_radius;

            highp float current_length = length(position_view_space);

            shadow_attenuation = (closest_position >= current_length) ? 1.0:0.0;
   }
#endif

   uFragColor = texture(tex, texcoord);
}