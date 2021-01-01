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

static inline uint32_t __internal_find_memory_type_index(struct VkPhysicalDeviceMemoryProperties const *physical_device_memory_properties, uint32_t memory_requirements_memory_type_bits, VkMemoryPropertyFlags required_property_flags)
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

static inline uint32_t __internal_find_memory_type_index(struct VkPhysicalDeviceMemoryProperties const *physical_device_memory_properties, uint32_t memory_requirements_memory_type_bits, VkMemoryPropertyFlags required_property_flags, VkMemoryPropertyFlags preferred_property_flags)
{
    VkMemoryPropertyFlags optimal_property_flags = (required_property_flags | preferred_property_flags);
    uint32_t memory_type_index = __internal_find_memory_type_index(physical_device_memory_properties, memory_requirements_memory_type_bits, optimal_property_flags);
    if (VK_MAX_MEMORY_TYPES != memory_type_index)
    {
        assert(VK_MAX_MEMORY_TYPES > memory_type_index);
        return memory_type_index;
    }
    else
    {
        return __internal_find_memory_type_index(physical_device_memory_properties, memory_requirements_memory_type_bits, required_property_flags);
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

    /** @file src/vk_mem_alloc.inl **/

    /// Default size of a block allocated as single VkDeviceMemory from a "large" heap.
    static VkDeviceSize const VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE = (256ULL * 1024ULL * 1024ULL);

    /// Maximum size of a memory heap in Vulkan to consider it "small".
    static VkDeviceSize const VMA_SMALL_HEAP_MAX_SIZE = (1024ULL * 1024ULL * 1024ULL);

    // VmaAllocator_T::CalcPreferredBlockSize
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

    m_transfer_src_buffer_memory_index = VK_MAX_MEMORY_TYPES;
    {
        struct VkBufferCreateInfo buffer_ci_transfer_src;
        buffer_ci_transfer_src.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_ci_transfer_src.pNext = NULL;
        buffer_ci_transfer_src.flags = 0U;
        buffer_ci_transfer_src.size = 8U;
        buffer_ci_transfer_src.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        buffer_ci_transfer_src.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        buffer_ci_transfer_src.queueFamilyIndexCount = 0U;
        buffer_ci_transfer_src.pQueueFamilyIndices = NULL;

        VkBuffer dummy_buf;
        VkResult vk_res = m_api_vk->create_buffer(&buffer_ci_transfer_src, &dummy_buf);
        assert(VK_SUCCESS == vk_res);

        struct VkMemoryRequirements mem_req;
        m_api_vk->get_buffer_memory_requirements(dummy_buf, &mem_req);
        //e.g. VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : AMD "Special pool of video memory" [Sawicki 2018] Adam Sawicki. "Memory Management in Vulkan and DX12." GDC 2018.
        m_transfer_src_buffer_memory_index = __internal_find_memory_type_index(&physical_device_memory_properties, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    if (VK_MAX_MEMORY_TYPES == m_transfer_src_buffer_memory_index)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > m_transfer_src_buffer_memory_index);

    m_uniform_buffer_memory_index = VK_MAX_MEMORY_TYPES;
    {
        struct VkBufferCreateInfo buffer_ci_uniform;
        buffer_ci_uniform.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_ci_uniform.pNext = NULL;
        buffer_ci_uniform.flags = 0U;
        buffer_ci_uniform.size = 8U;

        buffer_ci_uniform.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        buffer_ci_uniform.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        buffer_ci_uniform.queueFamilyIndexCount = 0U;
        buffer_ci_uniform.pQueueFamilyIndices = NULL;

        VkBuffer dummy_buf;
        VkResult vk_res = m_api_vk->create_buffer(&buffer_ci_uniform, &dummy_buf);
        assert(VK_SUCCESS == vk_res);

        struct VkMemoryRequirements mem_req;
        m_api_vk->get_buffer_memory_requirements(dummy_buf, &mem_req);
        //e.g. VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : AMD "Special pool of video memory" [Sawicki 2018] Adam Sawicki. "Memory Management in Vulkan and DX12." GDC 2018.
        m_uniform_buffer_memory_index = __internal_find_memory_type_index(&physical_device_memory_properties, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    if (VK_MAX_MEMORY_TYPES == m_uniform_buffer_memory_index)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > m_uniform_buffer_memory_index);

    m_transfer_dst_and_vertex_buffer_memory_index = VK_MAX_MEMORY_TYPES;
    {
        struct VkBufferCreateInfo buffer_ci_transfer_dst_and_vertex_buffer;
        buffer_ci_transfer_dst_and_vertex_buffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_ci_transfer_dst_and_vertex_buffer.pNext = NULL;
        buffer_ci_transfer_dst_and_vertex_buffer.flags = 0U;
        buffer_ci_transfer_dst_and_vertex_buffer.size = 8U;

        buffer_ci_transfer_dst_and_vertex_buffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        buffer_ci_transfer_dst_and_vertex_buffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        buffer_ci_transfer_dst_and_vertex_buffer.queueFamilyIndexCount = 0U;
        buffer_ci_transfer_dst_and_vertex_buffer.pQueueFamilyIndices = NULL;

        VkBuffer dummy_buf;
        VkResult vk_res = m_api_vk->create_buffer(&buffer_ci_transfer_dst_and_vertex_buffer, &dummy_buf);
        assert(VK_SUCCESS == vk_res);

        struct VkMemoryRequirements mem_req;
        m_api_vk->get_buffer_memory_requirements(dummy_buf, &mem_req);
        //e.g. NOT HOST_VISIBLE : the UMA driver may compress the buffer/texture to boost performance
        m_transfer_dst_and_vertex_buffer_memory_index = __internal_find_memory_type_index(&physical_device_memory_properties, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    if (VK_MAX_MEMORY_TYPES == m_transfer_dst_and_vertex_buffer_memory_index)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > m_transfer_dst_and_vertex_buffer_memory_index);

    m_transfer_dst_and_index_buffer_memory_index = VK_MAX_MEMORY_TYPES;
    {
        struct VkBufferCreateInfo buffer_ci_transfer_dst_and_index_buffer;
        buffer_ci_transfer_dst_and_index_buffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_ci_transfer_dst_and_index_buffer.pNext = NULL;
        buffer_ci_transfer_dst_and_index_buffer.flags = 0U;
        buffer_ci_transfer_dst_and_index_buffer.size = 8U;
        buffer_ci_transfer_dst_and_index_buffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        buffer_ci_transfer_dst_and_index_buffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        buffer_ci_transfer_dst_and_index_buffer.queueFamilyIndexCount = 0U;
        buffer_ci_transfer_dst_and_index_buffer.pQueueFamilyIndices = NULL;

        VkBuffer dummy_buf;
        VkResult vk_res = m_api_vk->create_buffer(&buffer_ci_transfer_dst_and_index_buffer, &dummy_buf);
        assert(VK_SUCCESS == vk_res);

        struct VkMemoryRequirements mem_req;
        m_api_vk->get_buffer_memory_requirements(dummy_buf, &mem_req);
        //e.g. NOT HOST_VISIBLE : the UMA driver may compress the buffer/texture to boost performance
        m_transfer_dst_and_index_buffer_memory_index = __internal_find_memory_type_index(&physical_device_memory_properties, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    if (VK_MAX_MEMORY_TYPES == m_transfer_dst_and_index_buffer_memory_index)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > m_transfer_dst_and_index_buffer_memory_index);

    // https://www.khronos.org/registry/vulkan/specs/1.0/html/chap13.html#VkMemoryRequirements
    // For images created with a color format, the memoryTypeBits member is identical for all VkImage objects created with the
    // same combination of values for the tiling member, the VK_IMAGE_CREATE_SPARSE_BINDING_BIT bit of the flags member, and
    // the VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT of the usage member in the VkImageCreateInfo structure passed to
    // vkCreateImage.

    uint32_t color_format_regular_optimal_memory_index = VK_MAX_MEMORY_TYPES;
    {
        enum VkFormat color_format = VK_FORMAT_R8G8B8A8_UNORM;

        struct VkFormatProperties format_properties;
        m_api_vk->get_physical_device_format_properties(color_format, &format_properties);
        assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
        assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT);
        assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
        assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT);

        struct VkImageCreateInfo image_ci_regular_tiling_optimal;
        image_ci_regular_tiling_optimal.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_ci_regular_tiling_optimal.pNext = NULL;
        image_ci_regular_tiling_optimal.flags = 0U;
        image_ci_regular_tiling_optimal.imageType = VK_IMAGE_TYPE_2D;
        image_ci_regular_tiling_optimal.format = color_format;
        image_ci_regular_tiling_optimal.extent.width = 8U;
        image_ci_regular_tiling_optimal.extent.height = 8U;
        image_ci_regular_tiling_optimal.extent.depth = 1U;
        image_ci_regular_tiling_optimal.mipLevels = 1U;
        image_ci_regular_tiling_optimal.arrayLayers = 1U;
        image_ci_regular_tiling_optimal.samples = VK_SAMPLE_COUNT_1_BIT;
        image_ci_regular_tiling_optimal.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_ci_regular_tiling_optimal.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_ci_regular_tiling_optimal.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_ci_regular_tiling_optimal.queueFamilyIndexCount = 0U;
        image_ci_regular_tiling_optimal.pQueueFamilyIndices = NULL;
        image_ci_regular_tiling_optimal.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImage dummy_img;
        VkResult vk_res = m_api_vk->create_image(&image_ci_regular_tiling_optimal, &dummy_img);
        assert(VK_SUCCESS == vk_res);

        struct VkMemoryRequirements mem_req;
        m_api_vk->get_image_memory_requirements(dummy_img, &mem_req);

        color_format_regular_optimal_memory_index = __internal_find_memory_type_index(&physical_device_memory_properties, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    if (VK_MAX_MEMORY_TYPES == color_format_regular_optimal_memory_index)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > color_format_regular_optimal_memory_index);

    uint32_t color_format_transient_tiling_optimal_memory_index = VK_MAX_MEMORY_TYPES;
    {
        enum VkFormat color_format = VK_FORMAT_R8G8B8A8_UNORM;

        struct VkFormatProperties format_properties;
        m_api_vk->get_physical_device_format_properties(color_format, &format_properties);
        assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT); //INPUT_ATTACHMENT
        assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT);

        struct VkImageCreateInfo image_ci_transient_tiling_optimal;
        image_ci_transient_tiling_optimal.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_ci_transient_tiling_optimal.pNext = NULL;
        image_ci_transient_tiling_optimal.flags = 0U;
        image_ci_transient_tiling_optimal.imageType = VK_IMAGE_TYPE_2D;
        image_ci_transient_tiling_optimal.format = color_format;
        image_ci_transient_tiling_optimal.extent.width = 8U;
        image_ci_transient_tiling_optimal.extent.height = 8U;
        image_ci_transient_tiling_optimal.extent.depth = 1U;
        image_ci_transient_tiling_optimal.mipLevels = 1U;
        image_ci_transient_tiling_optimal.arrayLayers = 1U;
        image_ci_transient_tiling_optimal.samples = VK_SAMPLE_COUNT_1_BIT;
        image_ci_transient_tiling_optimal.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_ci_transient_tiling_optimal.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        image_ci_transient_tiling_optimal.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_ci_transient_tiling_optimal.queueFamilyIndexCount = 0U;
        image_ci_transient_tiling_optimal.pQueueFamilyIndices = NULL;
        image_ci_transient_tiling_optimal.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImage dummy_img;
        VkResult vk_res = m_api_vk->create_image(&image_ci_transient_tiling_optimal, &dummy_img);
        assert(VK_SUCCESS == vk_res);

        struct VkMemoryRequirements mem_req;
        m_api_vk->get_image_memory_requirements(dummy_img, &mem_req);
        color_format_transient_tiling_optimal_memory_index = __internal_find_memory_type_index(&physical_device_memory_properties, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);
    }
    if (VK_MAX_MEMORY_TYPES == color_format_transient_tiling_optimal_memory_index)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > color_format_transient_tiling_optimal_memory_index);

    m_color_attachment_and_input_attachment_and_transient_attachment_memory_index = color_format_transient_tiling_optimal_memory_index;
    m_color_attachment_and_sampled_image_memory_index = color_format_regular_optimal_memory_index;
    m_transfer_dst_and_sampled_image_memory_index = color_format_regular_optimal_memory_index;
    m_transfer_dst_and_sampled_image_page_size = __internal_calc_preferred_block_size(&physical_device_memory_properties, m_transfer_dst_and_sampled_image_memory_index);
    this->gfx_malloc::transfer_dst_and_sampled_image_init(m_transfer_dst_and_sampled_image_page_size);
    //m_transfer_dst_and_sampled_image_slob.init(transfer_dst_and_sampled_image_page_size, transfer_dst_and_sampled_image_memory_index, m_api_vk);

    // https://www.khronos.org/registry/vulkan/specs/1.0/html/chap13.html#VkMemoryRequirements
    // For images created with a depth/stencil format, the memoryTypeBits member is identical for all VkImage objects created with the same
    // combination of values for the format member, the tiling member, the VK_IMAGE_CREATE_SPARSE_BINDING_BIT bit of the flags member, and
    // the VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT of the usage member in the VkImageCreateInfo structure passed to vkCreateImage.

    // https://www.khronos.org/registry/vulkan/specs/1.0/html/chap34.html#features-required-format-support
    // VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT feature must be supported for at least one of
    // VK_FORMAT_X8_D24_UNORM_PACK32 and VK_FORMAT_D32_SFLOAT, and must be supported for at least one of
    // VK_FORMAT_D24_UNORM_S8_UINT and VK_FORMAT_D32_SFLOAT_S8_UINT.

    m_format_depth = VK_FORMAT_UNDEFINED;
    m_format_depth_stencil = VK_FORMAT_UNDEFINED;
    m_depth_stencil_attachment_and_transient_attachment_memory_index = VK_MAX_MEMORY_TYPES;
    {
        struct VkFormatProperties format_properties;

        m_format_depth = VK_FORMAT_D32_SFLOAT;
        m_api_vk->get_physical_device_format_properties(m_format_depth, &format_properties);
        if (0 == (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
        {
            m_format_depth = VK_FORMAT_X8_D24_UNORM_PACK32;
            m_api_vk->get_physical_device_format_properties(m_format_depth, &format_properties);
            assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        }

        m_format_depth_stencil = VK_FORMAT_D32_SFLOAT_S8_UINT;
        m_api_vk->get_physical_device_format_properties(m_format_depth_stencil, &format_properties);
        if (0 == (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
        {
            m_format_depth_stencil = VK_FORMAT_D24_UNORM_S8_UINT;
            m_api_vk->get_physical_device_format_properties(m_format_depth_stencil, &format_properties);
            assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        }

        struct VkImageCreateInfo image_ci_depth_stencil_transient_tiling_optimal;
        image_ci_depth_stencil_transient_tiling_optimal.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_ci_depth_stencil_transient_tiling_optimal.pNext = NULL;
        image_ci_depth_stencil_transient_tiling_optimal.flags = 0U;
        image_ci_depth_stencil_transient_tiling_optimal.imageType = VK_IMAGE_TYPE_2D;
        image_ci_depth_stencil_transient_tiling_optimal.format = m_format_depth_stencil;
        image_ci_depth_stencil_transient_tiling_optimal.extent.width = 8U;
        image_ci_depth_stencil_transient_tiling_optimal.extent.height = 8U;
        image_ci_depth_stencil_transient_tiling_optimal.extent.depth = 1U;
        image_ci_depth_stencil_transient_tiling_optimal.mipLevels = 1U;
        image_ci_depth_stencil_transient_tiling_optimal.arrayLayers = 1U;
        image_ci_depth_stencil_transient_tiling_optimal.samples = VK_SAMPLE_COUNT_1_BIT;
        image_ci_depth_stencil_transient_tiling_optimal.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_ci_depth_stencil_transient_tiling_optimal.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        image_ci_depth_stencil_transient_tiling_optimal.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_ci_depth_stencil_transient_tiling_optimal.queueFamilyIndexCount = 0U;
        image_ci_depth_stencil_transient_tiling_optimal.pQueueFamilyIndices = NULL;
        image_ci_depth_stencil_transient_tiling_optimal.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImage dummy_img;
        VkResult vk_res = m_api_vk->create_image(&image_ci_depth_stencil_transient_tiling_optimal, &dummy_img);
        assert(VK_SUCCESS == vk_res);

        struct VkMemoryRequirements mem_req;
        m_api_vk->get_image_memory_requirements(dummy_img, &mem_req);
        m_depth_stencil_attachment_and_transient_attachment_memory_index = __internal_find_memory_type_index(&physical_device_memory_properties, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);
    }
    if (VK_MAX_MEMORY_TYPES == m_depth_stencil_attachment_and_transient_attachment_memory_index)
    {
        return false;
    }
    assert(VK_MAX_MEMORY_TYPES > m_depth_stencil_attachment_and_transient_attachment_memory_index);

    return true;
}

// Life of a triangle - NVIDIA's logical pipeline https://developer.nvidia.com/content/life-triangle-nvidias-logical-pipeline

void *gfx_malloc_vk::alloc_uniform_buffer(size_t size)
{
    // NVIDIA Driver 128 MB
    // \[Gruen 2015\] [Holger Gruen. "Constant Buffers without Constant Pain." NVIDIA GameWorks Blog 2015.](https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0)
    // VmaAllocator_T::CalcPreferredBlockSize
    // assert(size >= (1024ULL * 1024ULL * 128ULL));

    return NULL;
}

void *gfx_malloc_vk::transfer_dst_and_sampled_image_slob_new_pages(void *_self)
{
    class gfx_malloc_vk *self = static_cast<class gfx_malloc_vk *>(_self);

    VkResult res;
    VkDeviceMemory device_memory;
    {
        VkMemoryAllocateInfo memory_allocate_info;
        memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocate_info.pNext = NULL;
        memory_allocate_info.allocationSize = self->m_transfer_dst_and_sampled_image_page_size;
        memory_allocate_info.memoryTypeIndex = self->m_transfer_dst_and_sampled_image_memory_index;
        res = self->m_api_vk->allocate_memory(&memory_allocate_info, &device_memory);
    }
    if (VK_SUCCESS == res)
    {
        return device_memory;
    }
    else
    {
        return NULL;
    }
}

void gfx_malloc_vk::transfer_dst_and_sampled_image_slob_free_pages(void *page_memory_device, void *_self)
{
    class gfx_malloc_vk *self = static_cast<class gfx_malloc_vk *>(_self);
    self->m_api_vk->free_memory(static_cast<VkDeviceMemory>(page_memory_device));
    return;
}

VkDeviceMemory gfx_malloc_vk::internal_transfer_dst_and_sampled_image_alloc(VkMemoryRequirements const *memory_requirements, void **out_page_handle, uint64_t *out_offset, uint64_t *out_size)
{
    assert(((1U << m_transfer_dst_and_sampled_image_memory_index) & memory_requirements->memoryTypeBits) != 0);

    void *sp;
    void *page_memory = this->gfx_malloc::transfer_dst_and_sampled_image_alloc(
        memory_requirements->size, memory_requirements->alignment,
        transfer_dst_and_sampled_image_slob_new_pages, this, &sp, out_offset);

    if (NULL != page_memory)
    {
        (*out_page_handle) = sp;
        (*out_size) = memory_requirements->size;
        return static_cast<VkDeviceMemory>(page_memory);
    }
    else
    {
        (*out_page_handle) = NULL;
        (*out_size) = 0U;
        return NULL;
    }

#if 0
    assert(((1U << m_transfer_dst_and_sampled_image_slob.m_memory_index) & memory_requirements->memoryTypeBits) != 0);
    class slob_page *sp = m_transfer_dst_and_sampled_image_slob.alloc(memory_requirements->size, memory_requirements->alignment, out_offset);
    class slob_page_vk *sp_vk = static_cast<class slob_page_vk *>(sp);
    if (NULL != sp_vk)
    {
        (*out_page_handle) = sp_vk;
        (*out_size) = memory_requirements->size;
        return sp_vk->m_page;
    }
    else
    {
        (*out_page_handle) = NULL;
        (*out_size) = 0U;
        return VK_NULL_HANDLE;
    }
#endif
}

void gfx_malloc_vk::internal_transfer_dst_and_sampled_image_free(void *page_handle, uint64_t offset, uint64_t size, VkDeviceMemory device_memory)
{
    return this->gfx_malloc::transfer_dst_and_sampled_image_free(page_handle, offset, size, device_memory, transfer_dst_and_sampled_image_slob_free_pages, this);

    // class slob_page_vk *sp_vk = static_cast<class slob_page_vk *>(page_handle);
    // assert(device_memory == sp_vk->m_page);
    // m_transfer_dst_and_sampled_image_slob.free(sp_vk, offset, size);
}