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

#ifndef _PT_GFX_MALLOC_COMMON_H_
#define _PT_GFX_MALLOC_COMMON_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_common.h>
#include <pt_gfx_common.h>
#include <pt_mcrt_common.h>
#include <pt_mcrt_scalable_allocator.h>
#include <deque>

class gfx_malloc_common
{
protected:
    /*
    enum gfx_malloc_usage_t //routed into memory index //may be the same index
    {
        PT_GFX_MALLOC_USAGE_UNKNOWN,
        PT_GFX_MALLOC_USAGE_TRANSFER_SRC_BUFFER, //staging buffer
        PT_GFX_MALLOC_USAGE_UNIFORM_BUFFER,
        //PT_GFX_MALLOC_USAGE_DYNAMIC_CONSTANT_BUFFER, //bind /unbind //switch between diffirent constant buffer //cost
        //PT_GFX_MALLOC_USAGE_IMMUTABLE_CONSTANT_BUFFER,
        PT_GFX_MALLOC_USAGE_TRANSFER_DST_AND_VERTEX_BUFFER, //Position Vertex Buffer + Varying Vertex Buffer //IDVS(Index Driven Vertex Shading) 1. Arm® Mali™ GPU Best Practices Developer Guide / 4.4 Attribute layout 2. Real-Time Rendering Fourth Edition / 23.10.1 Case Study: ARM Mali G71 Bifrost / Figure 23.22
        PT_GFX_MALLOC_USAGE_TRANSFER_DST_AND_INDEX_BUFFER,
        PT_GFX_MALLOC_USAGE_COLOR_ATTACHMENT_AND_INPUT_ATTACHMENT_AND_TRANSIENT_ATTACHMENT,
        PT_GFX_MALLOC_USAGE_COLOR_ATTACHMENT_AND_SAMPLED_IMAGE,
        PT_GFX_MALLOC_USAGE_DEPTH_STENCIL_ATTACHMENT, //DENY_SAMPLED_IMAGE //write depth to color buffer //to be consistant with MTL
        PT_GFX_MALLOC_USAGE_TRANSFER_DST_AND_SAMPLED_IMAGE,
        PT_GFX_MALLOC_USAGE_RANGE_SIZE
    };
    */

    class gfx_malloc_slob_block_t
    {
        uint64_t offset;
        uint64_t size;
    };

    class gfx_malloc_slob_page_common_t
    {
        std::deque<gfx_malloc_slob_block_t, mcrt::scalable_allocator<gfx_malloc_slob_block_t>> m_free;
    };

    static void *slob_page_alloc(class gfx_malloc_slob_page_common_t *sp, uint64_t size, uint64_t align);
private:
    //malloc_usage_to_memory_index

public:
    virtual void *alloc_uniform_buffer(size_t size) = 0;
};

#endif
