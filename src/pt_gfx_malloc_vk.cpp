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

#include <pt_common.h>
#include <pt_mcrt_intrin.h>
#include <pt_gfx_common.h>
#include "pt_gfx_malloc.h"
#include "pt_gfx_malloc_vk.h"
#include "pt_gfx_connection_vk.h"
#include <vulkan/vulkan.h>
#include <assert.h>
#include <new>

static inline uint32_t __internal_find_lowest_memory_type_index(struct VkPhysicalDeviceMemoryProperties const *physical_device_memory_properties, uint32_t memory_requirements_memory_type_bits, VkMemoryPropertyFlags required_property_flags)
{
    uint32_t memory_type_count = physical_device_memory_properties->memoryTypeCount;
    assert(VK_MAX_MEMORY_TYPES >= memory_type_count);
    for (uint32_t memory_type_index = 0; memory_type_index < memory_type_count; ++memory_type_index)
    {
        uint32_t memory_type_bits = (1U << memory_type_index);
        bool is_required_memory_type = ((memory_requirements_memory_type_bits & memory_type_bits) != 0) ? true : false;
        VkMemoryPropertyFlags property_flags = physical_device_memory_properties->memoryTypes[memory_type_index].propertyFlags;
        bool has_required_property_flags = ((property_flags & required_property_flags) == required_property_flags) ? true : false;

        if (is_required_memory_type && has_required_property_flags)
        {
            return memory_type_index;
        }
    }

    return VK_MAX_MEMORY_TYPES;
}

static inline uint32_t __internal_find_lowest_memory_type_index(struct VkPhysicalDeviceMemoryProperties const *physical_device_memory_properties, uint32_t memory_requirements_memory_type_bits, VkMemoryPropertyFlags required_property_flags, VkMemoryPropertyFlags preferred_property_flags)
{
    VkMemoryPropertyFlags optimal_property_flags = (required_property_flags | preferred_property_flags);
    uint32_t memory_type_index = __internal_find_lowest_memory_type_index(physical_device_memory_properties, memory_requirements_memory_type_bits, optimal_property_flags);
    if (VK_MAX_MEMORY_TYPES != memory_type_index)
    {
        assert(VK_MAX_MEMORY_TYPES > memory_type_index);
        return memory_type_index;
    }
    else
    {
        return __internal_find_lowest_memory_type_index(physical_device_memory_properties, memory_requirements_memory_type_bits, required_property_flags);
    }
}

static inline VkDeviceSize __internal_calc_preferred_block_size(struct VkPhysicalDeviceMemoryProperties const *physical_device_memory_properties, uint32_t memory_index)
{
    assert(VK_MAX_MEMORY_TYPES > memory_index);
    assert(physical_device_memory_properties->memoryTypeCount > memory_index);
    uint32_t heap_index = physical_device_memory_properties->memoryTypes[memory_index].heapIndex;
    assert(VK_MAX_MEMORY_TYPES > heap_index);
    assert(physical_device_memory_properties->memoryHeapCount > heap_index);
    VkDeviceSize heap_size = physical_device_memory_properties->memoryHeaps[heap_index].size;

    // [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
    //
    // Copyright (c) 2017-2019 Advanced Micro Devices, Inc. All rights reserved.
    //
    // Permission is hereby granted, free of charge, to any person obtaining a copy
    // of this software and associated documentation files (the "Software"), to deal
    // in the Software without restriction, including without limitation the rights
    // to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    // copies of the Software, and to permit persons to whom the Software is
    // furnished to do so, subject to the following conditions:
    //
    // The above copyright notice and this permission notice shall be included in
    // all copies or substantial portions of the Software.
    //
    // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    // IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    // FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
    // AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    // LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    // OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    // THE SOFTWARE.
    //
    //
    /** @file src/vk_mem_alloc.inl **/
    //
    // VmaAllocator_T::CalcPreferredBlockSize

    /// Default size of a block allocated as single VkDeviceMemory from a "large" heap.
    static VkDeviceSize const VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE = (256ULL * 1024ULL * 1024ULL);

    /// Maximum size of a memory heap in Vulkan to consider it "small".
    static VkDeviceSize const VMA_SMALL_HEAP_MAX_SIZE = (1024ULL * 1024ULL * 1024ULL);

    bool is_small_heap = (heap_size <= VMA_SMALL_HEAP_MAX_SIZE);
    VkDeviceSize preferred_block_size = mcrt_intrin_round_up(is_small_heap ? (heap_size / 8) : VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE, static_cast<VkDeviceSize>(32));
    return preferred_block_size;
}

gfx_malloc_vk::gfx_malloc_vk()
{
}

bool gfx_malloc_vk::init(class gfx_api_vk *api_vk)
{
    m_api_vk = api_vk;

    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    m_api_vk->get_physical_device_memory_properties(&physical_device_memory_properties);

    // vulkaninfo
    // https://github.com/KhronosGroup/Vulkan-Tools/tree/master/vulkaninfo/vulkaninfo/vulkaninfo.h
    // GetImageCreateInfo
    // FillImageTypeSupport
    // https://github.com/KhronosGroup/Vulkan-Tools/tree/master/vulkaninfo/vulkaninfo.cpp
    // GpuDumpMemoryProps //"usable for"

    // https://www.khronos.org/registry/vulkan/specs/1.0/html/chap13.html#VkMemoryRequirements
    // If buffer is a VkBuffer not created with the VK_BUFFER_CREATE_SPARSE_BINDING_BIT bit set, or if image is linear image,
    // then the memoryTypeBits member always contains at least one bit set corresponding to a VkMemoryType with a
    // propertyFlags that has both the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit and the
    // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit set. In other words, mappable coherent memory can always be attached to
    // these objects.

    // https://www.khronos.org/registry/vulkan/specs/1.0/html/chap13.html#VkMemoryRequirements
    // The memoryTypeBits member is identical for all VkBuffer objects created with the same value for the flags and usage
    // members in the VkBufferCreateInfo structure passed to vkCreateBuffer. Further, if usage1 and usage2 of type
    // VkBufferUsageFlags are such that the bits set in usage2 are a subset of the bits set in usage1, and they have the same flags,
    // then the bits set in memoryTypeBits returned for usage1 must be a subset of the bits set in memoryTypeBits returned for
    // usage2, for all values of flags.

    // constant buffer
    uint32_t uniform_buffer_memory_index = VK_MAX_MEMORY_TYPES;
    // NVIDIA Driver 128 MB
    // \[Gruen 2015\] [Holger Gruen. "Constant Buffers without Constant Pain." NVIDIA GameWorks Blog 2015.](https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0)
    // AMD Special Pool 256MB
    // \[Sawicki 2018\] [Adam Sawicki. "Memory Management in Vulkan and DX12." GDC 2018.](https://gpuopen.com/events/gdc-2018-presentations)
    VkDeviceSize uniform_buffer_size = (224ULL * 1024ULL * 1024ULL); // 224MB
    this->m_uniform_buffer = VK_NULL_HANDLE;
    this->m_uniform_buffer_device_memory = VK_NULL_HANDLE;
    this->m_uniform_buffer_device_memory_pointer = NULL;
    {
        uint32_t memory_requirements_memory_type_bits = 0U;
        {
            struct VkBufferCreateInfo buffer_create_info_uniform;
            buffer_create_info_uniform.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info_uniform.pNext = NULL;
            buffer_create_info_uniform.flags = 0U;
            buffer_create_info_uniform.size = 8U;
            buffer_create_info_uniform.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            buffer_create_info_uniform.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            buffer_create_info_uniform.queueFamilyIndexCount = 0U;
            buffer_create_info_uniform.pQueueFamilyIndices = NULL;

            VkBuffer dummy_buf;
            VkResult res_create_buffer = m_api_vk->create_buffer(&buffer_create_info_uniform, &dummy_buf);
            assert(VK_SUCCESS == res_create_buffer);

            struct VkMemoryRequirements mem_req;
            m_api_vk->get_buffer_memory_requirements(dummy_buf, &mem_req);

            memory_requirements_memory_type_bits = mem_req.memoryTypeBits;

            m_api_vk->destroy_buffer(dummy_buf);
        }

        for (uint32_t memory_type_index;
             (0U != memory_requirements_memory_type_bits) &&
             // VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
             // We use the "AMD Special Pool" for uniform buffer
             (VK_MAX_MEMORY_TYPES != (memory_type_index = __internal_find_lowest_memory_type_index(&physical_device_memory_properties, memory_requirements_memory_type_bits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
             memory_requirements_memory_type_bits ^= (1U << memory_type_index))
        {
            assert(VK_MAX_MEMORY_TYPES > memory_type_index);

            VkResult res_allocate_memory;
            {
                VkMemoryAllocateInfo memory_allocate_info;
                memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memory_allocate_info.pNext = NULL;
                memory_allocate_info.allocationSize = uniform_buffer_size;
                memory_allocate_info.memoryTypeIndex = memory_type_index;

                res_allocate_memory = m_api_vk->allocate_memory(&memory_allocate_info, &this->m_uniform_buffer_device_memory);
            }
            assert(VK_SUCCESS == res_allocate_memory || VK_ERROR_OUT_OF_HOST_MEMORY == res_allocate_memory || VK_ERROR_OUT_OF_DEVICE_MEMORY == res_allocate_memory);

            if (VK_SUCCESS == res_allocate_memory)
            {
                VkDeviceSize heap_size_budget = physical_device_memory_properties.memoryHeaps[memory_type_index].size;
                // The application is not alone and there may be other applications which interact with the Vulkan as well.
                // The allocation may success even if the budget has been exceeded. However, this may result in performance issue.
                assert(uniform_buffer_size <= heap_size_budget);

                {
                    VkResult res_map_memory = this->m_api_vk->map_memory(this->m_uniform_buffer_device_memory, 0U, uniform_buffer_size, 0U, &this->m_uniform_buffer_device_memory_pointer);
                    assert(VK_SUCCESS == res_map_memory);
                }

                {
                    struct VkBufferCreateInfo buffer_create_info_uniform;
                    buffer_create_info_uniform.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                    buffer_create_info_uniform.pNext = NULL;
                    buffer_create_info_uniform.flags = 0U;
                    buffer_create_info_uniform.size = uniform_buffer_size;
                    buffer_create_info_uniform.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                    buffer_create_info_uniform.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    buffer_create_info_uniform.queueFamilyIndexCount = 0U;
                    buffer_create_info_uniform.pQueueFamilyIndices = NULL;

                    VkResult res_create_buffer = m_api_vk->create_buffer(&buffer_create_info_uniform, &this->m_uniform_buffer);
                    assert(VK_SUCCESS == res_create_buffer);
                }

#ifndef NDEBUG
                {
                    struct VkMemoryRequirements mem_req;
                    m_api_vk->get_buffer_memory_requirements(m_uniform_buffer, &mem_req);
                    assert(0 != (mem_req.memoryTypeBits & (1U << memory_type_index)));
                }
#endif

                {
                    VkResult res_bind_buffer_memory = this->m_api_vk->bind_buffer_memory(this->m_uniform_buffer, this->m_uniform_buffer_device_memory, 0U);
                    assert(VK_SUCCESS == res_bind_buffer_memory);
                }

                uniform_buffer_memory_index = memory_type_index;
                break;
            }
        }
    }
    if (VK_NULL_HANDLE == this->m_uniform_buffer || VK_NULL_HANDLE == this->m_uniform_buffer_device_memory || NULL == this->m_uniform_buffer_device_memory_pointer)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > uniform_buffer_memory_index);
    this->gfx_malloc::uniform_buffer_init(uniform_buffer_size);

    // staging buffer
    uint32_t transfer_src_memory_index = VK_MAX_MEMORY_TYPES;
    VkDeviceSize transfer_src_buffer_size = (512ULL * 1024ULL * 1024ULL);
    this->m_transfer_src_buffer = VK_NULL_HANDLE;
    this->m_transfer_src_buffer_device_memory = VK_NULL_HANDLE;
    this->m_transfer_src_buffer_device_memory_pointer = NULL;
    {
        uint32_t memory_requirements_memory_type_bits = 0U;
        {
            struct VkBufferCreateInfo buffer_create_info_transfer_src;
            buffer_create_info_transfer_src.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info_transfer_src.pNext = NULL;
            buffer_create_info_transfer_src.flags = 0U;
            buffer_create_info_transfer_src.size = 8U;
            buffer_create_info_transfer_src.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            buffer_create_info_transfer_src.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            buffer_create_info_transfer_src.queueFamilyIndexCount = 0U;
            buffer_create_info_transfer_src.pQueueFamilyIndices = NULL;

            VkBuffer dummy_buf;
            VkResult vk_res = m_api_vk->create_buffer(&buffer_create_info_transfer_src, &dummy_buf);
            assert(VK_SUCCESS == vk_res);

            struct VkMemoryRequirements mem_req;
            m_api_vk->get_buffer_memory_requirements(dummy_buf, &mem_req);

            memory_requirements_memory_type_bits = mem_req.memoryTypeBits;

            m_api_vk->destroy_buffer(dummy_buf);
        }

        for (uint32_t memory_type_index;
             (0U != memory_requirements_memory_type_bits) &&
             // VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
             // We leave the "AMD Special Pool" for uniform buffer
             (VK_MAX_MEMORY_TYPES != (memory_type_index = __internal_find_lowest_memory_type_index(&physical_device_memory_properties, memory_requirements_memory_type_bits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)));
             memory_requirements_memory_type_bits ^= (1U << memory_type_index))
        {
            assert(VK_MAX_MEMORY_TYPES > memory_type_index);

            VkResult res_allocate_memory;
            {
                VkMemoryAllocateInfo memory_allocate_info;
                memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memory_allocate_info.pNext = NULL;
                memory_allocate_info.allocationSize = transfer_src_buffer_size;
                memory_allocate_info.memoryTypeIndex = memory_type_index;

                res_allocate_memory = m_api_vk->allocate_memory(&memory_allocate_info, &this->m_transfer_src_buffer_device_memory);
            }
            assert(VK_SUCCESS == res_allocate_memory || VK_ERROR_OUT_OF_HOST_MEMORY == res_allocate_memory || VK_ERROR_OUT_OF_DEVICE_MEMORY == res_allocate_memory);

            if (VK_SUCCESS == res_allocate_memory)
            {
                // We allocate the contant buffer first to ensure that the "AMD Special Pool" is occupied by the the contant buffer
                VkDeviceSize heap_size_budget = (memory_type_index != uniform_buffer_memory_index) ? (physical_device_memory_properties.memoryHeaps[memory_type_index].size) : (physical_device_memory_properties.memoryHeaps[memory_type_index].size - uniform_buffer_size);
                // The application is not alone and there may be other applications which interact with the Vulkan as well.
                // The allocation may success even if the budget has been exceeded. However, this may result in performance issue.
                assert(transfer_src_buffer_size <= heap_size_budget);

                {
                    VkResult res_map_memory = this->m_api_vk->map_memory(this->m_transfer_src_buffer_device_memory, 0U, transfer_src_buffer_size, 0U, &this->m_transfer_src_buffer_device_memory_pointer);
                    assert(VK_SUCCESS == res_map_memory);
                }

                {
                    struct VkBufferCreateInfo buffer_create_info_uniform;
                    buffer_create_info_uniform.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                    buffer_create_info_uniform.pNext = NULL;
                    buffer_create_info_uniform.flags = 0U;
                    buffer_create_info_uniform.size = transfer_src_buffer_size;
                    buffer_create_info_uniform.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                    buffer_create_info_uniform.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    buffer_create_info_uniform.queueFamilyIndexCount = 0U;
                    buffer_create_info_uniform.pQueueFamilyIndices = NULL;

                    VkResult res_create_buffer = m_api_vk->create_buffer(&buffer_create_info_uniform, &this->m_transfer_src_buffer);
                    assert(VK_SUCCESS == res_create_buffer);
                }

#ifndef NDEBUG
                {
                    struct VkMemoryRequirements mem_req;
                    m_api_vk->get_buffer_memory_requirements(m_transfer_src_buffer, &mem_req);
                    assert(0 != (mem_req.memoryTypeBits & (1U << memory_type_index)));
                }
#endif

                {
                    VkResult res_bind_buffer_memory = this->m_api_vk->bind_buffer_memory(this->m_transfer_src_buffer, this->m_transfer_src_buffer_device_memory, 0U);
                    assert(VK_SUCCESS == res_bind_buffer_memory);
                }

                transfer_src_memory_index = memory_type_index;
                break;
            }
        }
    }
    if (VK_NULL_HANDLE == this->m_transfer_src_buffer || VK_NULL_HANDLE == this->m_transfer_src_buffer_device_memory || NULL == this->m_transfer_src_buffer_device_memory_pointer)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > transfer_src_memory_index);
    this->gfx_malloc::transfer_src_buffer_init(transfer_src_buffer_size);

    m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_memory_index = VK_MAX_MEMORY_TYPES;
    {
        uint32_t memory_requirements_memory_type_bits_vertex_buffer = 0U;
        {
            struct VkBufferCreateInfo buffer_create_info_transfer_dst_and_vertex_buffer;
            buffer_create_info_transfer_dst_and_vertex_buffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info_transfer_dst_and_vertex_buffer.pNext = NULL;
            buffer_create_info_transfer_dst_and_vertex_buffer.flags = 0U;
            buffer_create_info_transfer_dst_and_vertex_buffer.size = 8U;

            buffer_create_info_transfer_dst_and_vertex_buffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            buffer_create_info_transfer_dst_and_vertex_buffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            buffer_create_info_transfer_dst_and_vertex_buffer.queueFamilyIndexCount = 0U;
            buffer_create_info_transfer_dst_and_vertex_buffer.pQueueFamilyIndices = NULL;

            VkBuffer dummy_buf;
            VkResult vk_res = m_api_vk->create_buffer(&buffer_create_info_transfer_dst_and_vertex_buffer, &dummy_buf);
            assert(VK_SUCCESS == vk_res);

            struct VkMemoryRequirements mem_req;
            m_api_vk->get_buffer_memory_requirements(dummy_buf, &mem_req);

            memory_requirements_memory_type_bits_vertex_buffer = mem_req.memoryTypeBits;

            m_api_vk->destroy_buffer(dummy_buf);
        }

        uint32_t memory_requirements_memory_type_bits_index_buffer = 0U;
        {
            struct VkBufferCreateInfo buffer_create_info_transfer_dst_and_index_buffer;
            buffer_create_info_transfer_dst_and_index_buffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info_transfer_dst_and_index_buffer.pNext = NULL;
            buffer_create_info_transfer_dst_and_index_buffer.flags = 0U;
            buffer_create_info_transfer_dst_and_index_buffer.size = 8U;
            buffer_create_info_transfer_dst_and_index_buffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            buffer_create_info_transfer_dst_and_index_buffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            buffer_create_info_transfer_dst_and_index_buffer.queueFamilyIndexCount = 0U;
            buffer_create_info_transfer_dst_and_index_buffer.pQueueFamilyIndices = NULL;

            VkBuffer dummy_buf;
            VkResult vk_res = m_api_vk->create_buffer(&buffer_create_info_transfer_dst_and_index_buffer, &dummy_buf);
            assert(VK_SUCCESS == vk_res);

            struct VkMemoryRequirements mem_req;
            m_api_vk->get_buffer_memory_requirements(dummy_buf, &mem_req);

            memory_requirements_memory_type_bits_index_buffer = mem_req.memoryTypeBits;

            m_api_vk->destroy_buffer(dummy_buf);
        }

        uint32_t memory_requirements_memory_type_bits = (memory_requirements_memory_type_bits_vertex_buffer & memory_requirements_memory_type_bits_index_buffer);

        for (uint32_t memory_type_index;
             (0U != memory_requirements_memory_type_bits) &&
             // NOT HOST_VISIBLE
             // The UMA driver may compress the buffer/texture to boost performance
             (VK_MAX_MEMORY_TYPES != (memory_type_index = __internal_find_lowest_memory_type_index(&physical_device_memory_properties, memory_requirements_memory_type_bits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
             memory_requirements_memory_type_bits ^= (1U << memory_type_index))
        {
            assert(VK_MAX_MEMORY_TYPES > memory_type_index);

            // The lower index indicates the more performance
            m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_memory_index = memory_type_index;
            break;
        }
    }
    if (VK_MAX_MEMORY_TYPES == m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_memory_index)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_memory_index);

    this->m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_size = __internal_calc_preferred_block_size(&physical_device_memory_properties, m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_memory_index);
    this->gfx_malloc::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_init(this->m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_size);

    // https://www.khronos.org/registry/vulkan/specs/1.0/html/chap13.html#VkMemoryRequirements
    // For images created with a color format, the memoryTypeBits member is identical for all VkImage objects created with the
    // same combination of values for the tiling member, the VK_IMAGE_CREATE_SPARSE_BINDING_BIT bit of the flags member, and
    // the VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT of the usage member in the VkImageCreateInfo structure passed to
    // vkCreateImage.

    uint32_t color_format_regular_optimal_memory_index = VK_MAX_MEMORY_TYPES;
    {
        uint32_t memory_requirements_memory_type_bits = 0U;
        {
            enum VkFormat color_format = VK_FORMAT_R8G8B8A8_UNORM;

            struct VkFormatProperties format_properties;
            m_api_vk->get_physical_device_format_properties(color_format, &format_properties);
            assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
            assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT);
            assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
            assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT);

            struct VkImageCreateInfo image_create_info_regular_tiling_optimal;
            image_create_info_regular_tiling_optimal.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_create_info_regular_tiling_optimal.pNext = NULL;
            image_create_info_regular_tiling_optimal.flags = 0U;
            image_create_info_regular_tiling_optimal.imageType = VK_IMAGE_TYPE_2D;
            image_create_info_regular_tiling_optimal.format = color_format;
            image_create_info_regular_tiling_optimal.extent.width = 8U;
            image_create_info_regular_tiling_optimal.extent.height = 8U;
            image_create_info_regular_tiling_optimal.extent.depth = 1U;
            image_create_info_regular_tiling_optimal.mipLevels = 1U;
            image_create_info_regular_tiling_optimal.arrayLayers = 1U;
            image_create_info_regular_tiling_optimal.samples = VK_SAMPLE_COUNT_1_BIT;
            image_create_info_regular_tiling_optimal.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_create_info_regular_tiling_optimal.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            image_create_info_regular_tiling_optimal.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            image_create_info_regular_tiling_optimal.queueFamilyIndexCount = 0U;
            image_create_info_regular_tiling_optimal.pQueueFamilyIndices = NULL;
            image_create_info_regular_tiling_optimal.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VkImage dummy_img;
            VkResult vk_res = m_api_vk->create_image(&image_create_info_regular_tiling_optimal, &dummy_img);
            assert(VK_SUCCESS == vk_res);

            struct VkMemoryRequirements mem_req;
            m_api_vk->get_image_memory_requirements(dummy_img, &mem_req);

            memory_requirements_memory_type_bits = mem_req.memoryTypeBits;

            m_api_vk->destroy_image(dummy_img);
        }

        for (uint32_t memory_type_index;
             (0U != memory_requirements_memory_type_bits) &&

             (VK_MAX_MEMORY_TYPES != (memory_type_index = __internal_find_lowest_memory_type_index(&physical_device_memory_properties, memory_requirements_memory_type_bits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
             memory_requirements_memory_type_bits ^= (1U << memory_type_index))
        {
            assert(VK_MAX_MEMORY_TYPES > memory_type_index);

            // The lower index indicates the more performance
            color_format_regular_optimal_memory_index = memory_type_index;
            break;
        }
    }
    if (VK_MAX_MEMORY_TYPES == color_format_regular_optimal_memory_index)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > color_format_regular_optimal_memory_index);

    uint32_t color_format_transient_tiling_optimal_memory_index = VK_MAX_MEMORY_TYPES;
    {
        uint32_t memory_requirements_memory_type_bits = 0U;
        {
            enum VkFormat color_format = VK_FORMAT_R8G8B8A8_UNORM;

            struct VkFormatProperties format_properties;
            m_api_vk->get_physical_device_format_properties(color_format, &format_properties);
            assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT); //INPUT_ATTACHMENT
            assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT);

            struct VkImageCreateInfo image_create_info_transient_tiling_optimal;
            image_create_info_transient_tiling_optimal.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_create_info_transient_tiling_optimal.pNext = NULL;
            image_create_info_transient_tiling_optimal.flags = 0U;
            image_create_info_transient_tiling_optimal.imageType = VK_IMAGE_TYPE_2D;
            image_create_info_transient_tiling_optimal.format = color_format;
            image_create_info_transient_tiling_optimal.extent.width = 8U;
            image_create_info_transient_tiling_optimal.extent.height = 8U;
            image_create_info_transient_tiling_optimal.extent.depth = 1U;
            image_create_info_transient_tiling_optimal.mipLevels = 1U;
            image_create_info_transient_tiling_optimal.arrayLayers = 1U;
            image_create_info_transient_tiling_optimal.samples = VK_SAMPLE_COUNT_1_BIT;
            image_create_info_transient_tiling_optimal.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_create_info_transient_tiling_optimal.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
            image_create_info_transient_tiling_optimal.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            image_create_info_transient_tiling_optimal.queueFamilyIndexCount = 0U;
            image_create_info_transient_tiling_optimal.pQueueFamilyIndices = NULL;
            image_create_info_transient_tiling_optimal.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VkImage dummy_img;
            VkResult vk_res = m_api_vk->create_image(&image_create_info_transient_tiling_optimal, &dummy_img);
            assert(VK_SUCCESS == vk_res);

            struct VkMemoryRequirements mem_req;
            m_api_vk->get_image_memory_requirements(dummy_img, &mem_req);

            memory_requirements_memory_type_bits = mem_req.memoryTypeBits;

            m_api_vk->destroy_image(dummy_img);
        }

        for (uint32_t memory_type_index;
             (0U != memory_requirements_memory_type_bits) &&

             (VK_MAX_MEMORY_TYPES != (memory_type_index = __internal_find_lowest_memory_type_index(&physical_device_memory_properties, memory_requirements_memory_type_bits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)));
             memory_requirements_memory_type_bits ^= (1U << memory_type_index))
        {
            assert(VK_MAX_MEMORY_TYPES > memory_type_index);

            // The lower index indicates the more performance
            color_format_transient_tiling_optimal_memory_index = memory_type_index;
            break;
        }
    }
    if (VK_MAX_MEMORY_TYPES == color_format_transient_tiling_optimal_memory_index)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > color_format_transient_tiling_optimal_memory_index);

    this->m_color_attachment_and_input_attachment_and_transient_attachment_memory_index = color_format_transient_tiling_optimal_memory_index;
    this->m_color_attachment_and_sampled_image_memory_index = color_format_regular_optimal_memory_index;
    this->m_transfer_dst_and_sampled_image_memory_index = color_format_regular_optimal_memory_index;
    this->m_transfer_dst_and_sampled_image_page_size = __internal_calc_preferred_block_size(&physical_device_memory_properties, m_transfer_dst_and_sampled_image_memory_index);
    this->gfx_malloc::transfer_dst_and_sampled_image_init(this->m_transfer_dst_and_sampled_image_page_size);

    // https://www.khronos.org/registry/vulkan/specs/1.0/html/chap13.html#VkMemoryRequirements
    // For images created with a depth/stencil format, the memoryTypeBits member is identical for all VkImage objects created with the same
    // combination of values for the format member, the tiling member, the VK_IMAGE_CREATE_SPARSE_BINDING_BIT bit of the flags member, and
    // the VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT of the usage member in the VkImageCreateInfo structure passed to vkCreateImage.

    // https://www.khronos.org/registry/vulkan/specs/1.0/html/chap34.html#features-required-format-support
    // VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT feature must be supported for at least one of
    // VK_FORMAT_X8_D24_UNORM_PACK32 and VK_FORMAT_D32_SFLOAT, and must be supported for at least one of
    // VK_FORMAT_D24_UNORM_S8_UINT and VK_FORMAT_D32_SFLOAT_S8_UINT.

    this->m_format_depth = VK_FORMAT_UNDEFINED;
    this->m_format_depth_stencil = VK_FORMAT_UNDEFINED;
    this->m_depth_stencil_attachment_and_transient_attachment_memory_index = VK_MAX_MEMORY_TYPES;
    {
        struct VkFormatProperties format_properties;

        this->m_format_depth = VK_FORMAT_D32_SFLOAT;
        this->m_api_vk->get_physical_device_format_properties(this->m_format_depth, &format_properties);
        if (0 == (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
        {
            this->m_format_depth = VK_FORMAT_X8_D24_UNORM_PACK32;
            this->m_api_vk->get_physical_device_format_properties(this->m_format_depth, &format_properties);
            assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        }

        this->m_format_depth_stencil = VK_FORMAT_D32_SFLOAT_S8_UINT;
        this->m_api_vk->get_physical_device_format_properties(this->m_format_depth_stencil, &format_properties);
        if (0 == (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
        {
            this->m_format_depth_stencil = VK_FORMAT_D24_UNORM_S8_UINT;
            this->m_api_vk->get_physical_device_format_properties(this->m_format_depth_stencil, &format_properties);
            assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        }

        uint32_t memory_requirements_memory_type_bits = 0U;
        {
            struct VkImageCreateInfo image_create_info_depth_stencil_transient_tiling_optimal;
            image_create_info_depth_stencil_transient_tiling_optimal.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_create_info_depth_stencil_transient_tiling_optimal.pNext = NULL;
            image_create_info_depth_stencil_transient_tiling_optimal.flags = 0U;
            image_create_info_depth_stencil_transient_tiling_optimal.imageType = VK_IMAGE_TYPE_2D;
            image_create_info_depth_stencil_transient_tiling_optimal.format = m_format_depth_stencil;
            image_create_info_depth_stencil_transient_tiling_optimal.extent.width = 8U;
            image_create_info_depth_stencil_transient_tiling_optimal.extent.height = 8U;
            image_create_info_depth_stencil_transient_tiling_optimal.extent.depth = 1U;
            image_create_info_depth_stencil_transient_tiling_optimal.mipLevels = 1U;
            image_create_info_depth_stencil_transient_tiling_optimal.arrayLayers = 1U;
            image_create_info_depth_stencil_transient_tiling_optimal.samples = VK_SAMPLE_COUNT_1_BIT;
            image_create_info_depth_stencil_transient_tiling_optimal.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_create_info_depth_stencil_transient_tiling_optimal.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
            image_create_info_depth_stencil_transient_tiling_optimal.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            image_create_info_depth_stencil_transient_tiling_optimal.queueFamilyIndexCount = 0U;
            image_create_info_depth_stencil_transient_tiling_optimal.pQueueFamilyIndices = NULL;
            image_create_info_depth_stencil_transient_tiling_optimal.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VkImage dummy_img;
            VkResult vk_res = m_api_vk->create_image(&image_create_info_depth_stencil_transient_tiling_optimal, &dummy_img);
            assert(VK_SUCCESS == vk_res);

            struct VkMemoryRequirements mem_req;
            m_api_vk->get_image_memory_requirements(dummy_img, &mem_req);

            memory_requirements_memory_type_bits = mem_req.memoryTypeBits;

            m_api_vk->destroy_image(dummy_img);
        }

        for (uint32_t memory_type_index;
             (0U != memory_requirements_memory_type_bits) &&

             (VK_MAX_MEMORY_TYPES != (memory_type_index = __internal_find_lowest_memory_type_index(&physical_device_memory_properties, memory_requirements_memory_type_bits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)));
             memory_requirements_memory_type_bits ^= (1U << memory_type_index))
        {
            assert(VK_MAX_MEMORY_TYPES > memory_type_index);

            // The lower index indicates the more performance
            this->m_depth_stencil_attachment_and_transient_attachment_memory_index = memory_type_index;
            break;
        }
    }
    if (VK_MAX_MEMORY_TYPES == this->m_depth_stencil_attachment_and_transient_attachment_memory_index)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > this->m_depth_stencil_attachment_and_transient_attachment_memory_index);

    return true;
}

gfx_malloc_vk::~gfx_malloc_vk()
{
    return;
}

void *gfx_malloc_vk::transfer_src_buffer_pointer()
{
    return this->m_transfer_src_buffer_device_memory_pointer;
}

void gfx_malloc_vk::transfer_src_buffer_lock()
{
    return this->gfx_malloc::transfer_src_buffer_lock();
}

uint64_t gfx_malloc_vk::transfer_src_buffer_offset()
{
    return this->gfx_malloc::transfer_src_buffer_offset();
}

bool gfx_malloc_vk::transfer_src_buffer_validate_offset(uint64_t size)
{
    return this->gfx_malloc::transfer_src_buffer_validate_offset(size);
}

bool gfx_malloc_vk::transfer_src_buffer_alloc(uint64_t size, uint64_t *out_offset)
{
    return this->gfx_malloc::transfer_src_buffer_alloc(size, out_offset);
}

void gfx_malloc_vk::transfer_src_buffer_free(uint64_t offset, uint64_t size)
{
    return this->gfx_malloc::transfer_src_buffer_free(offset, size);
}

void gfx_malloc_vk::transfer_src_buffer_unlock()
{
    return this->gfx_malloc::transfer_src_buffer_unlock();
}

void *gfx_malloc_vk::uniform_buffer_pointer()
{
    return this->m_uniform_buffer_device_memory_pointer;
}

void gfx_malloc_vk::uniform_buffer_lock()
{
    return this->gfx_malloc::uniform_buffer_lock();
}

bool gfx_malloc_vk::uniform_buffer_alloc(uint64_t size, uint64_t *out_offset)
{
    return this->gfx_malloc::uniform_buffer_alloc(size, out_offset);
}

void gfx_malloc_vk::uniform_buffer_free(uint64_t offset, uint64_t size)
{
    return this->gfx_malloc::uniform_buffer_free(offset, size);
}

void gfx_malloc_vk::uniform_buffer_unlock()
{
    return this->gfx_malloc::uniform_buffer_unlock();
}

uint64_t gfx_malloc_vk::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_new_pages(void *_malloc_vk)
{
    class gfx_malloc_vk *malloc_vk = static_cast<class gfx_malloc_vk *>(_malloc_vk);

    VkDeviceMemory device_memory;
    VkResult res_allocate_memory;
    {
        VkMemoryAllocateInfo memory_allocate_info;
        memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocate_info.pNext = NULL;
        memory_allocate_info.allocationSize = malloc_vk->m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_size;
        memory_allocate_info.memoryTypeIndex = malloc_vk->m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_memory_index;
        res_allocate_memory = malloc_vk->m_api_vk->allocate_memory(&memory_allocate_info, &device_memory);
    }
    assert(VK_SUCCESS == res_allocate_memory || VK_ERROR_OUT_OF_HOST_MEMORY == res_allocate_memory || VK_ERROR_OUT_OF_DEVICE_MEMORY == res_allocate_memory);

    if (VK_SUCCESS == res_allocate_memory)
    {
        static_assert(sizeof(VkDeviceMemory) == sizeof(uint64_t), "");
        return (uint64_t)device_memory;
    }
    else
    {
        return PAGE_MEMORY_HANDLE_POISON;
    }
}

void gfx_malloc_vk::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_free_pages(uint64_t page_memory_handle, void *_malloc_vk)
{
    class gfx_malloc_vk *malloc_vk = static_cast<class gfx_malloc_vk *>(_malloc_vk);
    static_assert(sizeof(VkDeviceMemory) == sizeof(uint64_t), "");
    malloc_vk->m_api_vk->free_memory((VkDeviceMemory)page_memory_handle);
    return;
}

uint64_t gfx_malloc_vk::transfer_dst_and_sampled_image_slob_new_pages(void *_malloc_vk)
{
    class gfx_malloc_vk *malloc_vk = static_cast<class gfx_malloc_vk *>(_malloc_vk);

    VkDeviceMemory device_memory;
    VkResult res_allocate_memory;
    {
        VkMemoryAllocateInfo memory_allocate_info;
        memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocate_info.pNext = NULL;
        memory_allocate_info.allocationSize = malloc_vk->m_transfer_dst_and_sampled_image_page_size;
        memory_allocate_info.memoryTypeIndex = malloc_vk->m_transfer_dst_and_sampled_image_memory_index;
        res_allocate_memory = malloc_vk->m_api_vk->allocate_memory(&memory_allocate_info, &device_memory);
    }
    assert(VK_SUCCESS == res_allocate_memory || VK_ERROR_OUT_OF_HOST_MEMORY == res_allocate_memory || VK_ERROR_OUT_OF_DEVICE_MEMORY == res_allocate_memory);

    if (VK_SUCCESS == res_allocate_memory)
    {
        static_assert(sizeof(VkDeviceMemory) == sizeof(uint64_t), "");
        return (uint64_t)device_memory;
    }
    else
    {
        return PAGE_MEMORY_HANDLE_POISON;
    }
}

void gfx_malloc_vk::transfer_dst_and_sampled_image_slob_free_pages(uint64_t page_memory_handle, void *_malloc_vk)
{
    class gfx_malloc_vk *malloc_vk = static_cast<class gfx_malloc_vk *>(_malloc_vk);
    static_assert(sizeof(VkDeviceMemory) == sizeof(uint64_t), "");
    malloc_vk->m_api_vk->free_memory((VkDeviceMemory)page_memory_handle);
    return;
}

VkDeviceMemory gfx_malloc_vk::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(VkMemoryRequirements const *memory_requirements, void **out_page_handle, uint64_t *out_offset, uint64_t *out_size)
{
    assert(((1U << m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_memory_index) & memory_requirements->memoryTypeBits) != 0);

    void *page_handle;
    uint64_t page_memory_handle = this->gfx_malloc::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(
        memory_requirements->size, memory_requirements->alignment,
        transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_new_pages, this, &page_handle, out_offset);

    if (PAGE_MEMORY_HANDLE_POISON != page_memory_handle)
    {
        (*out_page_handle) = page_handle;
        (*out_size) = memory_requirements->size;
        return ((VkDeviceMemory)page_memory_handle);
    }
    else
    {
        (*out_page_handle) = NULL;
        (*out_size) = 0U;
        return VK_NULL_HANDLE;
    }
}

void gfx_malloc_vk::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(void *page_handle, uint64_t offset, uint64_t size, VkDeviceMemory device_memory)
{
    return this->gfx_malloc::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(page_handle, offset, size, (uint64_t)device_memory, transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_free_pages, this);
}

VkDeviceMemory gfx_malloc_vk::transfer_dst_and_sampled_image_alloc(VkMemoryRequirements const *memory_requirements, void **out_page_handle, uint64_t *out_offset, uint64_t *out_size)
{
    assert(((1U << m_transfer_dst_and_sampled_image_memory_index) & memory_requirements->memoryTypeBits) != 0);

    void *page_handle;
    uint64_t page_memory_handle = this->gfx_malloc::transfer_dst_and_sampled_image_alloc(
        memory_requirements->size, memory_requirements->alignment,
        transfer_dst_and_sampled_image_slob_new_pages, this, &page_handle, out_offset);

    if (PAGE_MEMORY_HANDLE_POISON != page_memory_handle)
    {
        (*out_page_handle) = page_handle;
        (*out_size) = memory_requirements->size;
        return ((VkDeviceMemory)page_memory_handle);
    }
    else
    {
        (*out_page_handle) = NULL;
        (*out_size) = 0U;
        return VK_NULL_HANDLE;
    }
}

void gfx_malloc_vk::transfer_dst_and_sampled_image_free(void *page_handle, uint64_t offset, uint64_t size, VkDeviceMemory device_memory)
{
    return this->gfx_malloc::transfer_dst_and_sampled_image_free(page_handle, offset, size, (uint64_t)device_memory, transfer_dst_and_sampled_image_slob_free_pages, this);
}