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
#include <pt_gfx_common.h>
#include "pt_gfx_malloc_common.h"
#include "pt_gfx_malloc_vk.h"
#include "pt_gfx_connection_vk.h"
#include <vulkan/vulkan.h>
#include <assert.h>

static inline uint32_t internal_find_memory_type_index(struct VkPhysicalDeviceMemoryProperties const *physical_device_memory_properties, uint32_t memory_requirements_memory_type_bits, VkMemoryPropertyFlags required_property_flags)
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

static inline uint32_t internal_find_memory_type_index(struct VkPhysicalDeviceMemoryProperties const *physical_device_memory_properties, uint32_t memory_requirements_memory_type_bits, VkMemoryPropertyFlags required_property_flags, VkMemoryPropertyFlags preferred_property_flags)
{
    VkMemoryPropertyFlags optimal_property_flags = (required_property_flags | preferred_property_flags);
    uint32_t memory_type_index = internal_find_memory_type_index(physical_device_memory_properties, memory_requirements_memory_type_bits, optimal_property_flags);
    if (VK_MAX_MEMORY_TYPES != memory_type_index)
    {
        assert(VK_MAX_MEMORY_TYPES > memory_type_index);
        return memory_type_index;
    }
    else
    {
        return internal_find_memory_type_index(physical_device_memory_properties, memory_requirements_memory_type_bits, required_property_flags);
    }
}

bool gfx_malloc_vk::init(
    VkPhysicalDevice physical_device,
    PFN_vkGetPhysicalDeviceMemoryProperties vk_get_physical_device_memory_properties,
    VkDevice device,
    PFN_vkGetBufferMemoryRequirements vk_get_buffer_memory_requirements,
    PFN_vkGetImageMemoryRequirements vk_get_image_memory_requirements)
{
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    vk_get_physical_device_memory_properties(physical_device, &physical_device_memory_properties);

    // vulkaninfo
    // https://github.com/KhronosGroup/Vulkan-Tools/tree/master/vulkaninfo/vulkaninfo/vulkaninfo.h
    // GetImageCreateInfo
    // FillImageTypeSupport
    // https://github.com/KhronosGroup/Vulkan-Tools/tree/master/vulkaninfo/vulkaninfo.cpp
    // GpuDumpMemoryProps //"usable for"

    // https://www.khronos.org/registry/vulkan/specs/1.0/html/chap13.html#VkMemoryRequirements
    // For images created with a depth/stencil format, the memoryTypeBits member is identical for all VkImage objects created with the same
    // combination of values for the format member, the tiling member, the VK_IMAGE_CREATE_SPARSE_BINDING_BIT bit of the flags member, and
    // the VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT of the usage member in the VkImageCreateInfo structure passed to vkCreateImage.

    m_color_attachment_and_input_attachment_and_transient_attachment_memory_index = VK_MAX_MEMORY_TYPES;
    {
        enum VkFormat color_format = VK_FORMAT_R8G8B8A8_UNORM;

        struct VkFormatProperties format_properties;
        static_cast<class gfx_connection_vk *>(this)->get_physical_device_format_properties(color_format, &format_properties);
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
        image_ci_transient_tiling_optimal.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        image_ci_transient_tiling_optimal.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_ci_transient_tiling_optimal.queueFamilyIndexCount = 0U;
        image_ci_transient_tiling_optimal.pQueueFamilyIndices = NULL;
        image_ci_transient_tiling_optimal.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImage dummy_img;
        VkResult vk_res = static_cast<class gfx_connection_vk *>(this)->create_image(&image_ci_transient_tiling_optimal, &dummy_img);
        assert(VK_SUCCESS == vk_res);

        struct VkMemoryRequirements mem_req;
        vk_get_image_memory_requirements(device, dummy_img, &mem_req);
        m_color_attachment_and_input_attachment_and_transient_attachment_memory_index = internal_find_memory_type_index(&physical_device_memory_properties, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);
    }
    if (VK_MAX_MEMORY_TYPES == m_color_attachment_and_input_attachment_and_transient_attachment_memory_index)
    {
        return false;
    }
    assert(m_color_attachment_and_input_attachment_and_transient_attachment_memory_index < VK_MAX_MEMORY_TYPES);

    m_transfer_dst_and_sampled_image_memory_index = VK_MAX_MEMORY_TYPES;
    {
        enum VkFormat color_format = VK_FORMAT_R8G8B8A8_UNORM;

        struct VkFormatProperties format_properties;
        static_cast<class gfx_connection_vk *>(this)->get_physical_device_format_properties(color_format, &format_properties);
        assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
        assert(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

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
        image_ci_regular_tiling_optimal.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_ci_regular_tiling_optimal.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_ci_regular_tiling_optimal.queueFamilyIndexCount = 0U;
        image_ci_regular_tiling_optimal.pQueueFamilyIndices = NULL;
        image_ci_regular_tiling_optimal.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImage dummy_img;
        VkResult vk_res = static_cast<class gfx_connection_vk *>(this)->create_image(&image_ci_regular_tiling_optimal, &dummy_img);
        assert(VK_SUCCESS == vk_res);

        struct VkMemoryRequirements mem_req;
        vk_get_image_memory_requirements(device, dummy_img, &mem_req);

        m_transfer_dst_and_sampled_image_memory_index = internal_find_memory_type_index(&physical_device_memory_properties, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    if (VK_MAX_MEMORY_TYPES == m_transfer_dst_and_sampled_image_memory_index)
    {
        return false;
    }
    assert(m_transfer_dst_and_sampled_image_memory_index < VK_MAX_MEMORY_TYPES);

    return true;
}

/*
The implementation guarantees certain properties about the memory requirements returned by vkGetBufferMemoryRequirements2, vkGetImageMemoryRequirements2, vkGetBufferMemoryRequirements and vkGetImageMemoryRequirements:

The memoryTypeBits member always contains at least one bit set.

If buffer is a VkBuffer not created with the VK_BUFFER_CREATE_SPARSE_BINDING_BIT bit set, or if image is linear image, then the memoryTypeBits member always contains at least one bit set corresponding to a VkMemoryType with a propertyFlags that has both the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit and the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit set. In other words, mappable coherent memory can always be attached to these objects.

If buffer was created with VkExternalMemoryBufferCreateInfo::handleTypes set to 0 or image was created with VkExternalMemoryImageCreateInfo::handleTypes set to 0, the memoryTypeBits member always contains at least one bit set corresponding to a VkMemoryType with a propertyFlags that has the VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit set.

The memoryTypeBits member is identical for all VkBuffer objects created with the same value for the flags and usage members in the VkBufferCreateInfo structure and the handleTypes member of the VkExternalMemoryBufferCreateInfo structure passed to vkCreateBuffer. Further, if usage1 and usage2 of type VkBufferUsageFlags are such that the bits set in usage2 are a subset of the bits set in usage1, and they have the same flags and VkExternalMemoryBufferCreateInfo::handleTypes, then the bits set in memoryTypeBits returned for usage1 must be a subset of the bits set in memoryTypeBits returned for usage2, for all values of flags.

The alignment member is a power of two.

The alignment member is identical for all VkBuffer objects created with the same combination of values for the usage and flags members in the VkBufferCreateInfo structure passed to vkCreateBuffer.

The alignment member satisfies the buffer descriptor offset alignment requirements associated with the VkBuffer’s usage:

If usage included VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT or VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT, alignment must be an integer multiple of VkPhysicalDeviceLimits::minTexelBufferOffsetAlignment.

If usage included VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, alignment must be an integer multiple of VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment.

If usage included VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, alignment must be an integer multiple of VkPhysicalDeviceLimits::minStorageBufferOffsetAlignment.

For images created with a color format, the memoryTypeBits member is identical for all VkImage objects created with the same combination of values for the tiling member, the VK_IMAGE_CREATE_SPARSE_BINDING_BIT bit of the flags member, the VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT bit of the flags member, handleTypes member of VkExternalMemoryImageCreateInfo, and the VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT of the usage member in the VkImageCreateInfo structure passed to vkCreateImage.

For images created with a depth/stencil format, the memoryTypeBits member is identical for all VkImage objects created with the same combination of values for the format member, the tiling member, the VK_IMAGE_CREATE_SPARSE_BINDING_BIT bit of the flags member, the VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT bit of the flags member, handleTypes member of VkExternalMemoryImageCreateInfo, and the VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT of the usage member in the VkImageCreateInfo structure passed to vkCreateImage.

If the memory requirements are for a VkImage, the memoryTypeBits member must not refer to a VkMemoryType with a propertyFlags that has the VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT bit set if the image did not have VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT bit set in the usage member of the VkImageCreateInfo structure passed to vkCreateImage.

If the memory requirements are for a VkBuffer, the memoryTypeBits member must not refer to a VkMemoryType with a propertyFlags that has the VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT bit set.

Note
The implication of this requirement is that lazily allocated memory is disallowed for buffers in all cases.

The size member is identical for all VkBuffer objects created with the same combination of creation parameters specified in VkBufferCreateInfo and its pNext chain.

The size member is identical for all VkImage objects created with the same combination of creation parameters specified in VkImageCreateInfo and its pNext chain.

Note
This, however, does not imply that they interpret the contents of the bound memory identically with each other. That additional guarantee, however, can be explicitly requested using VK_IMAGE_CREATE_ALIAS_BIT.
*/

// Life of a triangle - NVIDIA's logical pipeline https://developer.nvidia.com/content/life-triangle-nvidias-logical-pipeline

void *gfx_malloc_vk::alloc_uniform_buffer(size_t size)
{
    // NVIDIA Driver 128 MB
    // \[Gruen 2015\] [Holger Gruen. "Constant Buffers without Constant Pain." NVIDIA GameWorks Blog 2015.](https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0)
    //calc_prefer_block_size
    //assert(size >= (1024ULL * 1024ULL * 128ULL));

    return NULL;
}
