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

#ifndef _GFX_MEM_ALLOC_H_
#define _GFX_MEM_ALLOC_H_ 1

#include "pt_common.h"
#include "pt_gfx_common.h"

typedef enum GFX_MEM_ALLOC_USAGE
{
    GFX_MEM_ALLOC_USAGE_UNKNOWN,
    GFX_MEM_ALLOC_USAGE_STAGING_BUFFER,
    GFX_MEM_ALLOC_USAGE_CONSTANT_BUFFER,
    //GFX_MEM_ALLOC_USAGE_DYNAMIC_CONSTANT_BUFFER,
    //GFX_MEM_ALLOC_USAGE_IMMUTABLE_CONSTANT_BUFFER,
    GFX_MEM_ALLOC_USAGE_VERTEX_INDEX_BUFFER, //Position Vertex Buffer + Varying Vertex Buffer //IDVS(Index Driven Vertex Shading) 1. Arm® Mali™ GPU Best Practices Developer Guide / 4.4 Attribute layout 2. Real-Time Rendering Fourth Edition / 23.10.1 Case Study: ARM Mali G71 Bifrost / Figure 23.22
    //GFX_MEM_ALLOC_USAGE_VERTEX_BUFFER,
    //GFX_MEM_ALLOC_USAGE_INDEX_BUFFER,
    GFX_MEM_ALLOC_USAGE_RENDER_TARGET,
    GFX_MEM_ALLOC_USAGE_DEPTH_STENCIL_DENY_SHADER_RESOURCE, //write depth to color buffer //to be consistant with MTL
    GFX_MEM_ALLOC_USAGE_SHADER_RESOURCE
} GFX_MEM_ALLOC_USAGE;

#endif
