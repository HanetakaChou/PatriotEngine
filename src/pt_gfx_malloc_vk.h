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

#ifndef _PT_GFX_MALLOC_VK_H_
#define _PT_GFX_MALLOC_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_common.h>
#include <pt_gfx_common.h>
#include <pt_mcrt_common.h>
#include "pt_gfx_malloc_common.h"
#include <vulkan/vulkan.h>
#include <deque>

//VmaBlocksOnSamePage
//VmaIsBufferImageGranularityConflict

class gfx_malloc_vk : public gfx_malloc_common
{
    VkPhysicalDeviceMemoryProperties m_physical_device_memory_properties;

    uint32_t m_uniform_buffer_memory_index;

    //uint32_t memory_index_elem[9];
    //static_assert(PT_GFX_MALLOC_USAGE_RANGE_SIZE == (sizeof(memory_index_elem) / sizeof(memory_index_elem[0])), "PT_GFX_MALLOC_USAGE_RANGE_SIZE == (sizeof(memory_index_elem) / sizeof(memory_index_elem[0]))");

    class gfx_malloc_slob_page_vk_t : public gfx_malloc_slob_page_common_t
    {
        VkDeviceMemory m_page;
    };

    std::deque<gfx_malloc_slob_page_vk_t, mcrt::scalable_allocator<gfx_malloc_slob_page_vk_t>> m_free_slob_small;
    std::deque<gfx_malloc_slob_page_vk_t, mcrt::scalable_allocator<gfx_malloc_slob_page_vk_t>> m_free_slob_medium;
    std::deque<gfx_malloc_slob_page_vk_t, mcrt::scalable_allocator<gfx_malloc_slob_page_vk_t>> m_free_slob_large;

    //uint32_t malloc_usage_to_memory_type_index(enum gfx_malloc_usage_t malloc_usage);

    uint32_t find_memory_type_index(uint32_t memory_requirements_memory_type_bits, VkMemoryPropertyFlags required_property_flags);

    uint32_t find_memory_type_index(uint32_t memory_requirements_memory_type_bits, VkMemoryPropertyFlags required_property_flags, VkMemoryPropertyFlags preferred_property_flags);

    void *alloc_uniform_buffer(size_t size) override;
};

#endif