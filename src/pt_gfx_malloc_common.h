/*
 * Copyright (C) YuqiaoZhang
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

#ifndef _PT_GFX_MALLOC_COMMON_H_
#define _PT_GFX_MALLOC_COMMON_H_ 1

#include "pt_common.h"
#include "pt_gfx_common.h"

class gfx_malloc_common
{
    enum //gfx_malloc_usage_t
    {
        PT_GFX_MALLOC_USAGE_UNKNOWN,
        PT_GFX_MALLOC_USAGE_STAGING_BUFFER,
        PT_GFX_MALLOC_USAGE_CONSTANT_BUFFER,
        //PT_GFX_MALLOC_USAGE_DYNAMIC_CONSTANT_BUFFER,
        //PT_GFX_MALLOC_USAGE_IMMUTABLE_CONSTANT_BUFFER,
        PT_GFX_MALLOC_USAGE_VERTEX_INDEX_BUFFER, //Position Vertex Buffer + Varying Vertex Buffer //IDVS(Index Driven Vertex Shading) 1. Arm® Mali™ GPU Best Practices Developer Guide / 4.4 Attribute layout 2. Real-Time Rendering Fourth Edition / 23.10.1 Case Study: ARM Mali G71 Bifrost / Figure 23.22
        //PT_GFX_MALLOC_USAGE_VERTEX_BUFFER,
        //PT_GFX_MALLOC_USAGE_INDEX_BUFFER,
        PT_GFX_MALLOC_USAGE_RENDER_TARGET,
        PT_GFX_MALLOC_USAGE_DEPTH_STENCIL_DENY_SHADER_RESOURCE, //write depth to color buffer //to be consistant with MTL
        PT_GFX_MALLOC_USAGE_SHADER_RESOURCE
    };
};

#endif
