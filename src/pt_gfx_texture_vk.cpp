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

#include <stddef.h>
#include <assert.h>
#include <pt_mcrt_intrin.h>
#include <pt_mcrt_malloc.h>
#include "pt_gfx_texture_vk.h"
#include "pt_gfx_texture_base_load.h"
#include <new>

struct specific_header_vk_t
{
    bool isCubeCompatible;
    VkImageType imageType;
    VkFormat format;
    VkExtent3D extent;
    uint32_t mipLevels;
    uint32_t arrayLayers;
};
static inline struct specific_header_vk_t common_to_specific_header_translate(struct common_header_t const *common_header);
static inline enum VkImageType common_to_vulkan_type_translate(enum gfx_texture_common_type_t common_type);
static inline enum VkFormat common_to_vulkan_format_translate(enum gfx_texture_common_format_t common_format);

static inline size_t get_copyable_footprints(
    struct specific_header_vk_t const *specific_header_vk,
    VkDeviceSize physical_device_limits_optimal_buffer_copy_offset_alignment, VkDeviceSize physical_device_limits_optimal_buffer_copy_row_pitch_alignment,
    size_t base_offset,
    size_t num_subresources, struct load_memcpy_dest_t *out_memcpy_dest, struct VkBufferImageCopy *out_cmdcopy_dest);

static inline uint32_t calc_aspect_subresource_num_vk(uint32_t mip_levels, uint32_t array_layers);
static inline uint32_t calc_subresource_index_vk(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mipLevels, uint32_t arrayLayers);

static inline uint32_t get_format_aspect_count(VkFormat vk_format);
static inline uint32_t get_format_aspect_mask(VkFormat vk_format, uint32_t aspect_index);
static inline bool is_format_rgba(VkFormat vk_format);
static inline bool is_format_depth_stencil(VkFormat vk_format);
static inline bool is_format_compressed(VkFormat vk_format);
static inline uint32_t get_rgba_format_pixel_bytes(VkFormat vk_format);
static inline uint32_t get_depth_stencil_format_pixel_bytes(VkFormat vk_format, uint32_t aspect_index);
static inline uint32_t get_compressed_format_block_width(VkFormat vk_format);
static inline uint32_t get_compressed_format_block_height(VkFormat vk_format);
static inline uint32_t get_compressed_format_block_depth(VkFormat vk_format);
static inline uint32_t get_compressed_format_block_size_in_bytes(VkFormat vk_format);

struct texture_streaming_stage_second_thread_stack_data_t
{
    struct common_header_t m_common_header;
    size_t m_common_data_offset;
    struct specific_header_vk_t m_specific_header_vk;
    struct load_memcpy_dest_t *m_memcpy_dest;
    VkBufferImageCopy *m_cmdcopy_dest;
};
//static_assert(sizeof(struct texture_streaming_stage_second_thread_stack_data_t) <= sizeof(struct streaming_stage_second_thread_stack_data_user_defined_t), "");

bool gfx_texture_vk::texture_streaming_stage_first_pre_populate_task_data_callback(class gfx_connection_base *gfx_connection_base, struct common_header_t const *neutral_header)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);

    struct specific_header_vk_t specific_header_vk = common_to_specific_header_translate(neutral_header);

    // vkGetPhysicalDeviceFormatProperties
    // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior
    {
        struct VkFormatProperties physical_device_format_properties;
        gfx_connection->get_physical_device_format_properties(specific_header_vk.format, &physical_device_format_properties);
        if (PT_UNLIKELY(0 == (physical_device_format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)))
        {
            return false;
        }
    }

    assert(VK_NULL_HANDLE == this->m_image);
    {
        VkImageCreateInfo create_info;
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        create_info.pNext = NULL;
        create_info.flags = ((!specific_header_vk.isCubeCompatible) ? 0U : VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
        create_info.imageType = specific_header_vk.imageType;
        create_info.format = specific_header_vk.format;
        create_info.extent = specific_header_vk.extent;
        create_info.mipLevels = specific_header_vk.mipLevels;
        create_info.arrayLayers = specific_header_vk.arrayLayers;
        create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0U;
        create_info.pQueueFamilyIndices = NULL;
        create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        PT_MAYBE_UNUSED VkResult res = gfx_connection->create_image(&create_info, &this->m_image);
        assert(VK_SUCCESS == res);
    }

    assert(VK_NULL_HANDLE == this->m_gfx_malloc_device_memory);
    {
        VkMemoryRequirements memory_requirements;
        gfx_connection->get_image_memory_requirements(m_image, &memory_requirements);

        // vkAllocateMemory
        // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior

        this->m_gfx_malloc_device_memory = gfx_connection->transfer_dst_and_sampled_image_alloc(&memory_requirements, &this->m_gfx_malloc_page_handle, &this->m_gfx_malloc_offset, &this->m_gfx_malloc_size);
        if (PT_UNLIKELY(VK_NULL_HANDLE == this->m_gfx_malloc_device_memory))
        {
            gfx_connection->destroy_image(this->m_image);
            this->m_image = VK_NULL_HANDLE;
            return false;
        }
    }
    assert(VK_NULL_HANDLE != this->m_gfx_malloc_device_memory);

    // bind memory
    {
        PT_MAYBE_UNUSED VkResult res_bind_image_memory = gfx_connection->bind_image_memory(this->m_image, this->m_gfx_malloc_device_memory, this->m_gfx_malloc_offset);
        assert(VK_SUCCESS == res_bind_image_memory);
    }

    // image view
    {
        VkImageAspectFlags aspect_mask = 0U;
        {
            uint32_t aspect_count = get_format_aspect_count(specific_header_vk.format);
            for (uint32_t aspect_index = 0U; aspect_index < aspect_count; ++aspect_index)
            {
                aspect_mask |= get_format_aspect_mask(specific_header_vk.format, aspect_index);
            }
        }

        VkImageViewCreateInfo image_view_create_info;
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.pNext = NULL;
        image_view_create_info.flags = 0U;
        image_view_create_info.image = this->m_image;
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = specific_header_vk.format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = aspect_mask;
        image_view_create_info.subresourceRange.baseMipLevel = 0U;
        image_view_create_info.subresourceRange.levelCount = specific_header_vk.mipLevels;
        image_view_create_info.subresourceRange.baseArrayLayer = 0U;
        image_view_create_info.subresourceRange.layerCount = specific_header_vk.arrayLayers;

        PT_MAYBE_UNUSED VkResult res_create_image_view = gfx_connection->create_image_view(&image_view_create_info, &this->m_image_view);
        assert(VK_SUCCESS == res_create_image_view);
    }

    return true;
}

bool gfx_texture_vk::texture_streaming_stage_second_pre_calculate_total_size_callback(struct common_header_t const *neutral_header, void **memcpy_dest, void **cmdcopy_dest)
{
    struct specific_header_vk_t specific_header_vk = common_to_specific_header_translate(neutral_header);

    uint32_t num_subresource = get_format_aspect_count(specific_header_vk.format) * calc_aspect_subresource_num_vk(specific_header_vk.arrayLayers, specific_header_vk.mipLevels);

    (*memcpy_dest) = static_cast<struct load_memcpy_dest_t *>(mcrt_aligned_malloc(sizeof(struct load_memcpy_dest_t) * num_subresource, alignof(struct load_memcpy_dest_t)));
    (*cmdcopy_dest) = static_cast<struct VkBufferImageCopy *>(mcrt_aligned_malloc(sizeof(struct VkBufferImageCopy) * num_subresource, alignof(struct VkBufferImageCopy)));
    
    return true;
}

size_t gfx_texture_vk::texture_streaming_stage_second_calculate_total_size_callback(class gfx_connection_base *gfx_connection_base, struct common_header_t const *neutral_header, void *memcpy_dest_void, void *cmdcopy_dest_void, uint64_t base_offset)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    struct load_memcpy_dest_t *memcpy_dest = static_cast<struct load_memcpy_dest_t *>(memcpy_dest_void);
    struct VkBufferImageCopy *cmdcopy_dest = static_cast<struct VkBufferImageCopy *>(cmdcopy_dest_void);
    
    struct specific_header_vk_t specific_header_vk = common_to_specific_header_translate(neutral_header);

    uint32_t num_subresource = get_format_aspect_count(specific_header_vk.format) * calc_aspect_subresource_num_vk(specific_header_vk.arrayLayers, specific_header_vk.mipLevels);

    size_t total_size = get_copyable_footprints(&specific_header_vk, gfx_connection->physical_device_limits_optimal_buffer_copy_offset_alignment(), gfx_connection->physical_device_limits_optimal_buffer_copy_row_pitch_alignment(), base_offset, num_subresource, memcpy_dest, cmdcopy_dest);
    return total_size;
}

void gfx_texture_vk::texture_streaming_stage_second_post_calculate_total_size_fail_callback(void *memcpy_dest, void *cmdcopy_dest)
{
    mcrt_aligned_free(memcpy_dest);
    mcrt_aligned_free(cmdcopy_dest);
}

bool gfx_texture_vk::texture_streaming_stage_second_post_calculate_total_size_success_callback(class gfx_connection_base *gfx_connection_base, uint32_t streaming_throttling_index, struct common_header_t const *neutral_header, size_t const *neutral_data_offset, void *memcpy_dest_void, void *cmdcopy_dest_void, gfx_input_stream_ref gfx_input_stream, intptr_t(PT_PTR *gfx_input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *gfx_input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence))
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    struct load_memcpy_dest_t *memcpy_dest = static_cast<struct load_memcpy_dest_t *>(memcpy_dest_void);
    struct VkBufferImageCopy *cmdcopy_dest = static_cast<struct VkBufferImageCopy *>(cmdcopy_dest_void);

    struct specific_header_vk_t specific_header_vk = common_to_specific_header_translate(neutral_header);

    uint32_t num_subresource = get_format_aspect_count(specific_header_vk.format) * calc_aspect_subresource_num_vk(specific_header_vk.arrayLayers, specific_header_vk.mipLevels);

    // load_data_from_input_stream
    if (!load_data_from_input_stream(neutral_header, neutral_data_offset, gfx_connection->transfer_src_buffer_pointer(), num_subresource, memcpy_dest, calc_subresource_index_vk, gfx_input_stream, gfx_input_stream_read_callback, gfx_input_stream_seek_callback))
    {
        mcrt_aligned_free(memcpy_dest);
        mcrt_aligned_free(cmdcopy_dest);
        return false;
    }
    mcrt_aligned_free(memcpy_dest);

    // copy_buffer_to_image
    {
        uint32_t streaming_thread_index = mcrt_this_task_arena_current_thread_index();
        VkBuffer transfer_src_buffer = gfx_connection->transfer_src_buffer();

        uint32_t aspect_count = get_format_aspect_count(specific_header_vk.format);
        for (uint32_t aspect_index = 0U; aspect_index < aspect_count; ++aspect_index)
        {
            VkImageSubresourceRange aspect_subresource_range = {get_format_aspect_mask(specific_header_vk.format, aspect_index), 0, specific_header_vk.mipLevels, 0, specific_header_vk.arrayLayers};
            uint32_t aspect_subresource_num = calc_aspect_subresource_num_vk(specific_header_vk.arrayLayers, specific_header_vk.mipLevels);
            uint32_t aspect_subresource_index = calc_subresource_index_vk(0U, 0U, aspect_index, specific_header_vk.mipLevels, specific_header_vk.arrayLayers);
            gfx_connection->copy_buffer_to_image(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, this->m_image, &aspect_subresource_range, aspect_subresource_num, cmdcopy_dest + aspect_subresource_index);
        }
    }
    mcrt_aligned_free(cmdcopy_dest);

    return true;
}

bool gfx_texture_vk::streaming_stage_first_pre_populate_task_data_callback(class gfx_connection_base *gfx_connection_base, gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int), void *thread_stack_user_defined_void)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);

    struct common_header_t common_header;
    size_t common_data_offset;
    if (!load_header_from_input_stream(&common_header, &common_data_offset, input_stream, input_stream_read_callback, input_stream_seek_callback))
    {
        return false;
    }

    specific_header_vk_t specific_header_vk = common_to_specific_header_translate(&common_header);

    // vkGetPhysicalDeviceFormatProperties
    // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior
    {
        struct VkFormatProperties physical_device_format_properties;
        gfx_connection->get_physical_device_format_properties(specific_header_vk.format, &physical_device_format_properties);
        if (PT_UNLIKELY(0 == (physical_device_format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)))
        {
            return false;
        }
    }

    assert(VK_NULL_HANDLE == this->m_image);
    {
        VkImageCreateInfo create_info;
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        create_info.pNext = NULL;
        create_info.flags = ((!specific_header_vk.isCubeCompatible) ? 0U : VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
        create_info.imageType = specific_header_vk.imageType;
        create_info.format = specific_header_vk.format;
        create_info.extent = specific_header_vk.extent;
        create_info.mipLevels = specific_header_vk.mipLevels;
        create_info.arrayLayers = specific_header_vk.arrayLayers;
        create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0U;
        create_info.pQueueFamilyIndices = NULL;
        create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        PT_MAYBE_UNUSED VkResult res = gfx_connection->create_image(&create_info, &this->m_image);
        assert(VK_SUCCESS == res);
    }

    assert(VK_NULL_HANDLE == this->m_gfx_malloc_device_memory);
    {
        VkMemoryRequirements memory_requirements;
        gfx_connection->get_image_memory_requirements(m_image, &memory_requirements);

        // vkAllocateMemory
        // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior

        this->m_gfx_malloc_device_memory = gfx_connection->transfer_dst_and_sampled_image_alloc(&memory_requirements, &this->m_gfx_malloc_page_handle, &this->m_gfx_malloc_offset, &this->m_gfx_malloc_size);
        if (PT_UNLIKELY(VK_NULL_HANDLE == this->m_gfx_malloc_device_memory))
        {
            gfx_connection->destroy_image(this->m_image);
            this->m_image = VK_NULL_HANDLE;
            return false;
        }
    }
    assert(VK_NULL_HANDLE != this->m_gfx_malloc_device_memory);

    // bind memory
    {
        PT_MAYBE_UNUSED VkResult res_bind_image_memory = gfx_connection->bind_image_memory(this->m_image, this->m_gfx_malloc_device_memory, this->m_gfx_malloc_offset);
        assert(VK_SUCCESS == res_bind_image_memory);
    }

    // image view
    {
        VkImageAspectFlags aspect_mask = 0U;
        {
            uint32_t aspect_count = get_format_aspect_count(specific_header_vk.format);
            for (uint32_t aspect_index = 0U; aspect_index < aspect_count; ++aspect_index)
            {
                aspect_mask |= get_format_aspect_mask(specific_header_vk.format, aspect_index);
            }
        }

        VkImageViewCreateInfo image_view_create_info;
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.pNext = NULL;
        image_view_create_info.flags = 0U;
        image_view_create_info.image = this->m_image;
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = specific_header_vk.format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = aspect_mask;
        image_view_create_info.subresourceRange.baseMipLevel = 0U;
        image_view_create_info.subresourceRange.levelCount = specific_header_vk.mipLevels;
        image_view_create_info.subresourceRange.baseArrayLayer = 0U;
        image_view_create_info.subresourceRange.layerCount = specific_header_vk.arrayLayers;

        PT_MAYBE_UNUSED VkResult res_create_image_view = gfx_connection->create_image_view(&image_view_create_info, &this->m_image_view);
        assert(VK_SUCCESS == res_create_image_view);
    }

    return true;
}

void gfx_texture_vk::streaming_stage_first_populate_task_data_callback(void *thread_stack_user_defined_void, struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined_void)
{
}

bool gfx_texture_vk::streaming_stage_second_pre_calculate_total_size_callback(struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined_void, gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int), struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined_void)
{
    static_assert(sizeof(struct texture_streaming_stage_second_thread_stack_data_t) <= sizeof(struct streaming_stage_second_thread_stack_data_user_defined_t), "");
    struct texture_streaming_stage_second_thread_stack_data_t *thread_stack_data_user_defined = reinterpret_cast<struct texture_streaming_stage_second_thread_stack_data_t *>(thread_stack_data_user_defined_void);

    thread_stack_data_user_defined->m_specific_header_vk = common_to_specific_header_translate(&thread_stack_data_user_defined->m_common_header);

    uint32_t num_subresource = get_format_aspect_count(thread_stack_data_user_defined->m_specific_header_vk.format) * calc_aspect_subresource_num_vk(thread_stack_data_user_defined->m_specific_header_vk.arrayLayers, thread_stack_data_user_defined->m_specific_header_vk.mipLevels);

    thread_stack_data_user_defined->m_memcpy_dest = static_cast<struct load_memcpy_dest_t *>(mcrt_aligned_malloc(sizeof(struct load_memcpy_dest_t) * num_subresource, alignof(struct load_memcpy_dest_t)));
    thread_stack_data_user_defined->m_cmdcopy_dest = static_cast<struct VkBufferImageCopy *>(mcrt_aligned_malloc(sizeof(struct VkBufferImageCopy) * num_subresource, alignof(struct VkBufferImageCopy)));
    return true;
}

size_t gfx_texture_vk::streaming_stage_second_calculate_total_size_callback(uint64_t base_offset, struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined_void, class gfx_connection_base *gfx_connection_base, struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined_void)
{
    static_assert(sizeof(struct texture_streaming_stage_second_thread_stack_data_t) <= sizeof(struct streaming_stage_second_thread_stack_data_user_defined_t), "");
    struct texture_streaming_stage_second_thread_stack_data_t *thread_stack_data_user_defined = reinterpret_cast<struct texture_streaming_stage_second_thread_stack_data_t *>(thread_stack_data_user_defined_void);
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);

    uint32_t num_subresource = get_format_aspect_count(thread_stack_data_user_defined->m_specific_header_vk.format) * calc_aspect_subresource_num_vk(thread_stack_data_user_defined->m_specific_header_vk.arrayLayers, thread_stack_data_user_defined->m_specific_header_vk.mipLevels);

    size_t total_size = get_copyable_footprints(&thread_stack_data_user_defined->m_specific_header_vk, gfx_connection->physical_device_limits_optimal_buffer_copy_offset_alignment(), gfx_connection->physical_device_limits_optimal_buffer_copy_row_pitch_alignment(), base_offset, num_subresource, thread_stack_data_user_defined->m_memcpy_dest, thread_stack_data_user_defined->m_cmdcopy_dest);
    return total_size;
}

bool gfx_texture_vk::streaming_stage_second_post_calculate_total_size_callback(bool staging_buffer_allocate_success, uint32_t streaming_throttling_index, struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined_void, class gfx_connection_base *gfx_connection_base, gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int), struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined_void)
{
    static_assert(sizeof(struct texture_streaming_stage_second_thread_stack_data_t) <= sizeof(struct streaming_stage_second_thread_stack_data_user_defined_t), "");
    struct texture_streaming_stage_second_thread_stack_data_t *thread_stack_data_user_defined = reinterpret_cast<struct texture_streaming_stage_second_thread_stack_data_t *>(thread_stack_data_user_defined_void);
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);

    if (staging_buffer_allocate_success)
    {
        uint32_t num_subresource = get_format_aspect_count(thread_stack_data_user_defined->m_specific_header_vk.format) * calc_aspect_subresource_num_vk(thread_stack_data_user_defined->m_specific_header_vk.arrayLayers, thread_stack_data_user_defined->m_specific_header_vk.mipLevels);

        // load_data_from_input_stream
        if (!load_data_from_input_stream(&thread_stack_data_user_defined->m_common_header, &thread_stack_data_user_defined->m_common_data_offset, gfx_connection->transfer_src_buffer_pointer(), num_subresource, thread_stack_data_user_defined->m_memcpy_dest, calc_subresource_index_vk, input_stream, input_stream_read_callback, input_stream_seek_callback))
        {
            mcrt_aligned_free(thread_stack_data_user_defined->m_memcpy_dest);
            mcrt_aligned_free(thread_stack_data_user_defined->m_cmdcopy_dest);
            return false;
        }
        mcrt_aligned_free(thread_stack_data_user_defined->m_memcpy_dest);

        // copy_buffer_to_image
        {
            uint32_t streaming_thread_index = mcrt_this_task_arena_current_thread_index();
            VkBuffer transfer_src_buffer = gfx_connection->transfer_src_buffer();

            uint32_t aspect_count = get_format_aspect_count(thread_stack_data_user_defined->m_specific_header_vk.format);
            for (uint32_t aspect_index = 0U; aspect_index < aspect_count; ++aspect_index)
            {
                VkImageSubresourceRange aspect_subresource_range = {get_format_aspect_mask(thread_stack_data_user_defined->m_specific_header_vk.format, aspect_index), 0, thread_stack_data_user_defined->m_specific_header_vk.mipLevels, 0, thread_stack_data_user_defined->m_specific_header_vk.arrayLayers};
                uint32_t aspect_subresource_num = calc_aspect_subresource_num_vk(thread_stack_data_user_defined->m_specific_header_vk.arrayLayers, thread_stack_data_user_defined->m_specific_header_vk.mipLevels);
                uint32_t aspect_subresource_index = calc_subresource_index_vk(0U, 0U, aspect_index, thread_stack_data_user_defined->m_specific_header_vk.mipLevels, thread_stack_data_user_defined->m_specific_header_vk.arrayLayers);
                gfx_connection->copy_buffer_to_image(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, this->m_image, &aspect_subresource_range, aspect_subresource_num, thread_stack_data_user_defined->m_cmdcopy_dest + aspect_subresource_index);
            }
        }
        mcrt_aligned_free(thread_stack_data_user_defined->m_cmdcopy_dest);
    }
    else
    {
        mcrt_aligned_free(thread_stack_data_user_defined->m_memcpy_dest);
        mcrt_aligned_free(thread_stack_data_user_defined->m_cmdcopy_dest);
    }

    return true;
}

void gfx_texture_vk::streaming_destroy_callback(class gfx_connection_base *gfx_connection_base)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    this->unified_destory(gfx_connection);
}

bool gfx_texture_vk::streaming_done_callback(class gfx_connection_base *gfx_connection_base)
{
    return true;
}

void gfx_texture_vk::frame_destroy_callback(class gfx_connection_base *gfx_connection_base)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    this->unified_destory(gfx_connection);
}

inline void gfx_texture_vk::unified_destory(class gfx_connection_vk *gfx_connection)
{
    if (VK_NULL_HANDLE != this->m_image_view)
    {
        gfx_connection->destroy_image_view(this->m_image_view);
    }

    if (VK_NULL_HANDLE != this->m_image)
    {
        gfx_connection->destroy_image(this->m_image);
    }

    if (VK_NULL_HANDLE != this->m_gfx_malloc_device_memory)
    {
        gfx_connection->transfer_dst_and_sampled_image_free(this->m_gfx_malloc_page_handle, this->m_gfx_malloc_offset, this->m_gfx_malloc_size, this->m_gfx_malloc_device_memory);
    }

    this->~gfx_texture_vk();
    mcrt_aligned_free(this);
}


// Format
static inline struct specific_header_vk_t common_to_specific_header_translate(struct common_header_t const *common_header)
{
    struct specific_header_vk_t specific_header_vk;

    specific_header_vk.isCubeCompatible = common_header->isCubeMap;
    specific_header_vk.imageType = common_to_vulkan_type_translate(common_header->type);
    specific_header_vk.format = common_to_vulkan_format_translate(common_header->format);
    specific_header_vk.extent.width = common_header->width;
    specific_header_vk.extent.height = common_header->height;
    specific_header_vk.extent.depth = common_header->depth;
    specific_header_vk.mipLevels = common_header->mipLevels;
    specific_header_vk.arrayLayers = common_header->arrayLayers;

    return specific_header_vk;
}

static inline enum VkImageType common_to_vulkan_type_translate(enum gfx_texture_common_type_t common_type)
{
    static enum VkImageType const common_to_vulkan_type_map[] = {
        VK_IMAGE_TYPE_1D,
        VK_IMAGE_TYPE_2D,
        VK_IMAGE_TYPE_3D};
    static_assert(PT_GFX_TEXTURE_COMMON_TYPE_RANGE_SIZE == (sizeof(common_to_vulkan_type_map) / sizeof(common_to_vulkan_type_map[0])), "");

    assert(common_type < (sizeof(common_to_vulkan_type_map) / sizeof(common_to_vulkan_type_map[0])));
    return common_to_vulkan_type_map[common_type];
}

static inline enum VkFormat common_to_vulkan_format_translate(enum gfx_texture_common_format_t common_format)
{
    static enum VkFormat const common_to_vulkan_format_map[] = {
        VK_FORMAT_R4G4_UNORM_PACK8,
        VK_FORMAT_R4G4B4A4_UNORM_PACK16,
        VK_FORMAT_B4G4R4A4_UNORM_PACK16,
        VK_FORMAT_R5G6B5_UNORM_PACK16,
        VK_FORMAT_B5G6R5_UNORM_PACK16,
        VK_FORMAT_R5G5B5A1_UNORM_PACK16,
        VK_FORMAT_B5G5R5A1_UNORM_PACK16,
        VK_FORMAT_A1R5G5B5_UNORM_PACK16,
        VK_FORMAT_R8_UNORM,
        VK_FORMAT_R8_SNORM,
        VK_FORMAT_R8_USCALED,
        VK_FORMAT_R8_SSCALED,
        VK_FORMAT_R8_UINT,
        VK_FORMAT_R8_SINT,
        VK_FORMAT_R8_SRGB,
        VK_FORMAT_R8G8_UNORM,
        VK_FORMAT_R8G8_SNORM,
        VK_FORMAT_R8G8_USCALED,
        VK_FORMAT_R8G8_SSCALED,
        VK_FORMAT_R8G8_UINT,
        VK_FORMAT_R8G8_SINT,
        VK_FORMAT_R8G8_SRGB,
        VK_FORMAT_R8G8B8_UNORM,
        VK_FORMAT_R8G8B8_SNORM,
        VK_FORMAT_R8G8B8_USCALED,
        VK_FORMAT_R8G8B8_SSCALED,
        VK_FORMAT_R8G8B8_UINT,
        VK_FORMAT_R8G8B8_SINT,
        VK_FORMAT_R8G8B8_SRGB,
        VK_FORMAT_B8G8R8_UNORM,
        VK_FORMAT_B8G8R8_SNORM,
        VK_FORMAT_B8G8R8_USCALED,
        VK_FORMAT_B8G8R8_SSCALED,
        VK_FORMAT_B8G8R8_UINT,
        VK_FORMAT_B8G8R8_SINT,
        VK_FORMAT_B8G8R8_SRGB,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_R8G8B8A8_SNORM,
        VK_FORMAT_R8G8B8A8_USCALED,
        VK_FORMAT_R8G8B8A8_SSCALED,
        VK_FORMAT_R8G8B8A8_UINT,
        VK_FORMAT_R8G8B8A8_SINT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_B8G8R8A8_UNORM,
        VK_FORMAT_B8G8R8A8_SNORM,
        VK_FORMAT_B8G8R8A8_USCALED,
        VK_FORMAT_B8G8R8A8_SSCALED,
        VK_FORMAT_B8G8R8A8_UINT,
        VK_FORMAT_B8G8R8A8_SINT,
        VK_FORMAT_B8G8R8A8_SRGB,
        VK_FORMAT_A8B8G8R8_UNORM_PACK32,
        VK_FORMAT_A8B8G8R8_SNORM_PACK32,
        VK_FORMAT_A8B8G8R8_USCALED_PACK32,
        VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
        VK_FORMAT_A8B8G8R8_UINT_PACK32,
        VK_FORMAT_A8B8G8R8_SINT_PACK32,
        VK_FORMAT_A8B8G8R8_SRGB_PACK32,
        VK_FORMAT_A2R10G10B10_UNORM_PACK32,
        VK_FORMAT_A2R10G10B10_SNORM_PACK32,
        VK_FORMAT_A2R10G10B10_USCALED_PACK32,
        VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
        VK_FORMAT_A2R10G10B10_UINT_PACK32,
        VK_FORMAT_A2R10G10B10_SINT_PACK32,
        VK_FORMAT_A2B10G10R10_UNORM_PACK32,
        VK_FORMAT_A2B10G10R10_SNORM_PACK32,
        VK_FORMAT_A2B10G10R10_USCALED_PACK32,
        VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
        VK_FORMAT_A2B10G10R10_UINT_PACK32,
        VK_FORMAT_A2B10G10R10_SINT_PACK32,
        VK_FORMAT_R16_UNORM,
        VK_FORMAT_R16_SNORM,
        VK_FORMAT_R16_USCALED,
        VK_FORMAT_R16_SSCALED,
        VK_FORMAT_R16_UINT,
        VK_FORMAT_R16_SINT,
        VK_FORMAT_R16_SFLOAT,
        VK_FORMAT_R16G16_UNORM,
        VK_FORMAT_R16G16_SNORM,
        VK_FORMAT_R16G16_USCALED,
        VK_FORMAT_R16G16_SSCALED,
        VK_FORMAT_R16G16_UINT,
        VK_FORMAT_R16G16_SINT,
        VK_FORMAT_R16G16_SFLOAT,
        VK_FORMAT_R16G16B16_UNORM,
        VK_FORMAT_R16G16B16_SNORM,
        VK_FORMAT_R16G16B16_USCALED,
        VK_FORMAT_R16G16B16_SSCALED,
        VK_FORMAT_R16G16B16_UINT,
        VK_FORMAT_R16G16B16_SINT,
        VK_FORMAT_R16G16B16_SFLOAT,
        VK_FORMAT_R16G16B16A16_UNORM,
        VK_FORMAT_R16G16B16A16_SNORM,
        VK_FORMAT_R16G16B16A16_USCALED,
        VK_FORMAT_R16G16B16A16_SSCALED,
        VK_FORMAT_R16G16B16A16_UINT,
        VK_FORMAT_R16G16B16A16_SINT,
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_FORMAT_R32_UINT,
        VK_FORMAT_R32_SINT,
        VK_FORMAT_R32_SFLOAT,
        VK_FORMAT_R32G32_UINT,
        VK_FORMAT_R32G32_SINT,
        VK_FORMAT_R32G32_SFLOAT,
        VK_FORMAT_R32G32B32_UINT,
        VK_FORMAT_R32G32B32_SINT,
        VK_FORMAT_R32G32B32_SFLOAT,
        VK_FORMAT_R32G32B32A32_UINT,
        VK_FORMAT_R32G32B32A32_SINT,
        VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_FORMAT_R64_UINT,
        VK_FORMAT_R64_SINT,
        VK_FORMAT_R64_SFLOAT,
        VK_FORMAT_R64G64_UINT,
        VK_FORMAT_R64G64_SINT,
        VK_FORMAT_R64G64_SFLOAT,
        VK_FORMAT_R64G64B64_UINT,
        VK_FORMAT_R64G64B64_SINT,
        VK_FORMAT_R64G64B64_SFLOAT,
        VK_FORMAT_R64G64B64A64_UINT,
        VK_FORMAT_R64G64B64A64_SINT,
        VK_FORMAT_R64G64B64A64_SFLOAT,
        VK_FORMAT_B10G11R11_UFLOAT_PACK32,
        VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
        VK_FORMAT_D16_UNORM,
        VK_FORMAT_X8_D24_UNORM_PACK32,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_BC1_RGB_UNORM_BLOCK,
        VK_FORMAT_BC1_RGB_SRGB_BLOCK,
        VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
        VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
        VK_FORMAT_BC2_UNORM_BLOCK,
        VK_FORMAT_BC2_SRGB_BLOCK,
        VK_FORMAT_BC3_UNORM_BLOCK,
        VK_FORMAT_BC3_SRGB_BLOCK,
        VK_FORMAT_BC4_UNORM_BLOCK,
        VK_FORMAT_BC4_SNORM_BLOCK,
        VK_FORMAT_BC5_UNORM_BLOCK,
        VK_FORMAT_BC5_SNORM_BLOCK,
        VK_FORMAT_BC6H_UFLOAT_BLOCK,
        VK_FORMAT_BC6H_SFLOAT_BLOCK,
        VK_FORMAT_BC7_UNORM_BLOCK,
        VK_FORMAT_BC7_SRGB_BLOCK,
        VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
        VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
        VK_FORMAT_EAC_R11_UNORM_BLOCK,
        VK_FORMAT_EAC_R11_SNORM_BLOCK,
        VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
        VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
        VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
        VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
        VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
        VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
        VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
        VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
        VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
        VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
        VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
        VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
        VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
        VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
        VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
        VK_FORMAT_ASTC_12x12_SRGB_BLOCK};
    static_assert(PT_GFX_TEXTURE_COMMON_FORMAT_RANGE_SIZE == (sizeof(common_to_vulkan_format_map) / sizeof(common_to_vulkan_format_map[0])), "");

    assert(common_format < (sizeof(common_to_vulkan_format_map) / sizeof(common_to_vulkan_format_map[0])));
    return common_to_vulkan_format_map[common_format];
}

// https://source.winehq.org/git/vkd3d.git/
// libs/vkd3d/device.c
// d3d12_device_GetCopyableFootprints
// libs/vkd3d/utils.c
// vkd3d_formats

// https://github.com/ValveSoftware/dxvk/blob/master/src/dxvk/dxvk_context.cpp
// DxvkContext::uploadImage
static inline size_t get_copyable_footprints(
    struct specific_header_vk_t const *specific_header_vk,
    VkDeviceSize physical_device_limits_optimal_buffer_copy_offset_alignment, VkDeviceSize physical_device_limits_optimal_buffer_copy_row_pitch_alignment,
    size_t base_offset,
    size_t num_subresources, struct load_memcpy_dest_t *out_memcpy_dest, VkBufferImageCopy *out_cmdcopy_dest)
{
    // Context::texSubImage2D libANGLE/Context.cpp
    // Texture::setSubImage libANGLE/Texture.cpp
    // TextureVk::setSubImage libANGLE/renderer/vulkan/TextureVk.cpp
    // TextureVk::setSubImageImpl libANGLE/renderer/vulkan/TextureVk.cpp
    // ImageHelper::stageSubresourceUpdate libANGLE/renderer/vulkan/vk_helpers.cpp
    // ImageHelper::CalculateBufferInfo libANGLE/renderer/vulkan/vk_helpers.cpp
    // ImageHelper::stageSubresourceUpdateImpl libANGLE/renderer/vulkan/vk_helpers.cpp

    uint32_t aspectCount = get_format_aspect_count(specific_header_vk->format);

    size_t stagingOffset = base_offset;
    size_t TotalBytes = 0;

    for (uint32_t aspectIndex = 0; aspectIndex < aspectCount; ++aspectIndex)
    {
        for (uint32_t arrayLayer = 0; arrayLayer < specific_header_vk->arrayLayers; ++arrayLayer)
        {
            size_t w = specific_header_vk->extent.width;
            size_t h = specific_header_vk->extent.height;
            size_t d = specific_header_vk->extent.depth;
            for (uint32_t mipLevel = 0; mipLevel < specific_header_vk->mipLevels; ++mipLevel)
            {
                size_t outputRowPitch;
                size_t outputRowSize;
                size_t outputNumRows;
                size_t outputSlicePitch;
                size_t outputNumSlices;

                uint32_t bufferRowLength;
                uint32_t bufferImageHeight;

                size_t texel_block_size;

                if (is_format_compressed(specific_header_vk->format))
                {
                    assert(1 == get_compressed_format_block_depth(specific_header_vk->format));

                    outputRowSize = ((w + get_compressed_format_block_width(specific_header_vk->format) - 1) / get_compressed_format_block_width(specific_header_vk->format)) * get_compressed_format_block_size_in_bytes(specific_header_vk->format);
                    outputNumRows = (h + get_compressed_format_block_height(specific_header_vk->format) - 1) / get_compressed_format_block_height(specific_header_vk->format);
                    outputNumSlices = d;

                    outputRowPitch = mcrt_intrin_round_up(outputRowSize, physical_device_limits_optimal_buffer_copy_row_pitch_alignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    // bufferOffset must be a multiple of 4
                    // bufferRowLength must be 0, or greater than or equal to the width member of imageExtent
                    // bufferImageHeight must be 0, or greater than or equal to the height member of imageExtent
                    // If the calling command’s VkImage parameter is a compressed image, ... , bufferRowLength must be a multiple of the compressed texel block width
                    // If the calling command’s VkImage parameter is a compressed image, ... , bufferImageHeight must be a multiple of the compressed texel block height

                    // 19.4.1. Buffer and Image Addressing
                    // For block-compressed formats, all parameters are still specified in texels rather than compressed texel blocks, but the addressing math operates on whole compressed texel blocks.
                    // Pseudocode for compressed copy addressing is:
                    // ```
                    // rowLength = region->bufferRowLength;
                    // if (rowLength == 0) rowLength = region->imageExtent.width;
                    //
                    // imageHeight = region->bufferImageHeight;
                    // if (imageHeight == 0) imageHeight = region->imageExtent.height;
                    //
                    // compressedTexelBlockSizeInBytes = <compressed texel block size taken from the src/dstImage>;
                    // rowLength /= compressedTexelBlockWidth;
                    // imageHeight /= compressedTexelBlockHeight;
                    //
                    // address of (x,y,z) = region->bufferOffset + (((z * imageHeight) + y) * rowLength + x) * compressedTexelBlockSizeInBytes;
                    // where x,y,z range from (0,0,0) to region->imageExtent.{width/compressedTexelBlockWidth,height/compressedTexelBlockHeight,depth/compressedTexelBlockDepth}.
                    //
                    // ```

                    bufferRowLength = (outputRowPitch / get_compressed_format_block_size_in_bytes(specific_header_vk->format)) * get_compressed_format_block_width(specific_header_vk->format);
                    bufferImageHeight = outputNumRows * get_compressed_format_block_height(specific_header_vk->format);

                    //bufferRowLength = mcrt_intrin_round_up(specific_header_vk->extent.width, get_compressed_format_block_width(specific_header_vk->format));
                    //bufferImageHeight = mcrt_intrin_round_up(specific_header_vk->extent.height, get_compressed_format_block_height(specific_header_vk->format));

                    texel_block_size = get_compressed_format_block_size_in_bytes(specific_header_vk->format);
                }
                else if (is_format_rgba(specific_header_vk->format))
                {

                    outputRowSize = get_rgba_format_pixel_bytes(specific_header_vk->format) * w;
                    outputNumRows = h;
                    outputNumSlices = d;

                    outputRowPitch = mcrt_intrin_round_up(outputRowSize, physical_device_limits_optimal_buffer_copy_row_pitch_alignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    bufferRowLength = outputRowPitch / get_rgba_format_pixel_bytes(specific_header_vk->format);
                    bufferImageHeight = outputNumRows;

                    texel_block_size = get_rgba_format_pixel_bytes(specific_header_vk->format);
                }
                else
                {
                    assert(is_format_depth_stencil(specific_header_vk->format));
                    outputRowSize = get_depth_stencil_format_pixel_bytes(specific_header_vk->format, aspectIndex) * w;
                    outputNumRows = h;
                    outputNumSlices = d;

                    outputRowPitch = mcrt_intrin_round_up(outputRowSize, physical_device_limits_optimal_buffer_copy_row_pitch_alignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    bufferRowLength = outputRowPitch / get_depth_stencil_format_pixel_bytes(specific_header_vk->format, aspectIndex);
                    bufferImageHeight = outputNumRows;

                    texel_block_size = get_depth_stencil_format_pixel_bytes(specific_header_vk->format, aspectIndex);
                }

                size_t stagingOffset_new = mcrt_intrin_round_up(mcrt_intrin_round_up(mcrt_intrin_round_up(mcrt_intrin_round_up(stagingOffset, size_t(4U)), texel_block_size), physical_device_limits_optimal_buffer_copy_offset_alignment), physical_device_limits_optimal_buffer_copy_row_pitch_alignment);
                TotalBytes += (stagingOffset_new - stagingOffset);
                stagingOffset = stagingOffset_new;

                uint32_t DstSubresource = calc_subresource_index_vk(mipLevel, arrayLayer, aspectIndex, specific_header_vk->mipLevels, specific_header_vk->arrayLayers);
                assert(DstSubresource < num_subresources);

                out_memcpy_dest[DstSubresource].stagingOffset = stagingOffset;
                out_memcpy_dest[DstSubresource].outputRowPitch = outputRowPitch;
                out_memcpy_dest[DstSubresource].outputRowSize = outputRowSize;
                out_memcpy_dest[DstSubresource].outputNumRows = outputNumRows;
                out_memcpy_dest[DstSubresource].outputSlicePitch = outputSlicePitch;
                out_memcpy_dest[DstSubresource].outputNumSlices = outputNumSlices;

                out_cmdcopy_dest[DstSubresource].bufferOffset = stagingOffset;
                out_cmdcopy_dest[DstSubresource].bufferRowLength = bufferRowLength;
                out_cmdcopy_dest[DstSubresource].bufferImageHeight = bufferImageHeight;

                //GetFormatAspectFlags
                out_cmdcopy_dest[DstSubresource].imageSubresource.aspectMask = get_format_aspect_mask(specific_header_vk->format, aspectIndex);
                out_cmdcopy_dest[DstSubresource].imageSubresource.mipLevel = mipLevel;
                out_cmdcopy_dest[DstSubresource].imageSubresource.baseArrayLayer = arrayLayer;
                out_cmdcopy_dest[DstSubresource].imageSubresource.layerCount = 1;
                out_cmdcopy_dest[DstSubresource].imageOffset.x = 0;
                out_cmdcopy_dest[DstSubresource].imageOffset.y = 0;
                out_cmdcopy_dest[DstSubresource].imageOffset.z = 0;
                out_cmdcopy_dest[DstSubresource].imageExtent.width = w;
                out_cmdcopy_dest[DstSubresource].imageExtent.height = h;
                out_cmdcopy_dest[DstSubresource].imageExtent.depth = d;

                size_t surfaceSize = (outputSlicePitch * outputNumSlices);
                stagingOffset += surfaceSize;
                TotalBytes += surfaceSize;
                assert((base_offset + TotalBytes) == stagingOffset);

                w = w >> 1;
                h = h >> 1;
                d = d >> 1;
                if (w == 0)
                {
                    w = 1;
                }
                if (h == 0)
                {
                    h = 1;
                }
                if (d == 0)
                {
                    d = 1;
                }
            }
        }
    }

    return TotalBytes;
}

static inline uint32_t calc_aspect_subresource_num_vk(uint32_t mip_levels, uint32_t array_layers)
{
    return mip_levels * array_layers;
}

static inline uint32_t calc_subresource_index_vk(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mipLevels, uint32_t arrayLayers)
{
    return mipLevel + arrayLayer * mipLevels + aspectIndex * mipLevels * arrayLayers;
}

//--------------------------------------------------------------------------------------

// gFormatInfoTable libANGLE/renderer/Format_table_autogen.cpp
// { FormatID::R8G8B8A8_UNORM, GL_RGBA8, GL_RGBA8, GenerateMip<R8G8B8A8>, NoCopyFunctions, ReadColor<R8G8B8A8, GLfloat>, WriteColor<R8G8B8A8, GLfloat>, GL_UNSIGNED_NORMALIZED, 8, 8, 8, 8, 0, 0, 0, 4, 0 */ {false */ {false */ {false, gl::VertexAttribType::UnsignedByte },

// BuildInternalFormatInfoMap libANGLE/formatutils.cpp
// From ES 3.0.1 spec, table 3.12
//                    | Internal format     |sized| R | G | B | A |S | Format         | Type                             | Component type        | SRGB | Texture supported                                | Filterable     | Texture attachment                               | Renderbuffer                                   | Blend
// AddRGBAFormat(&map, GL_RGBA8,             true,  8,  8,  8,  8, 0, GL_RGBA,         GL_UNSIGNED_BYTE,                  GL_UNSIGNED_NORMALIZED */ {false, RequireESOrExt<3, 0, &Extensions::textureStorage>, AlwaysSupported, RequireESOrExt<3, 0, &Extensions::textureStorage>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>);
// From EXT_texture_compression_bptc
//                          | Internal format                         | W | H |D | BS |CC| SRGB | Texture supported                              | Filterable     | Texture attachment | Renderbuffer  | Blend
// AddCompressedFormat(&map, GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,         4,  4, 1, 128, 4 */ {false, RequireExt<&Extensions::textureCompressionBPTC>, AlwaysSupported, NeverSupported,      NeverSupported, NeverSupported);

// GetLoadFunctionsLoadFunctionsMap libANGLE/renderer/load_functions_table_autogen.cpp
// LoadToNative
// LoadCompressedToNative

static inline uint32_t get_format_aspect_count(VkFormat vk_format)
{
    if (vk_format <= VK_FORMAT_UNDEFINED)
    {
        static_assert(0 == VK_FORMAT_UNDEFINED, "");
        assert(0);
        return -1;
    }
    else if (vk_format <= VK_FORMAT_S8_UINT)
    {
        static_assert((VK_FORMAT_UNDEFINED + 1U) == VK_FORMAT_R4G4_UNORM_PACK8, "");
        return 1U;
    }
    else if (vk_format <= VK_FORMAT_D32_SFLOAT_S8_UINT)
    {
        static_assert((VK_FORMAT_S8_UINT + 1U) == VK_FORMAT_D16_UNORM_S8_UINT, "");
        return 2U;
    }
    else if (vk_format <= VK_FORMAT_ASTC_12x12_SRGB_BLOCK)
    {
        static_assert((VK_FORMAT_D32_SFLOAT_S8_UINT + 1U) == VK_FORMAT_BC1_RGB_UNORM_BLOCK, "");
        return 1U;
    }
    else
    {
        assert(0);
        return -1;
    }
}

static inline uint32_t get_format_aspect_mask(VkFormat vk_format, uint32_t aspect_index)
{
    if (vk_format <= VK_FORMAT_UNDEFINED)
    {
        static_assert(0 == VK_FORMAT_UNDEFINED, "");
        assert(0);
        return VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
    }
    else if (vk_format <= VK_FORMAT_E5B9G9R9_UFLOAT_PACK32)
    {
        static_assert((VK_FORMAT_UNDEFINED + 1U) == VK_FORMAT_R4G4_UNORM_PACK8, "");
        assert(aspect_index < 1U);
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }
    else if (vk_format <= VK_FORMAT_D32_SFLOAT)
    {
        static_assert((VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 + 1U) == VK_FORMAT_D16_UNORM, "");
        assert(aspect_index < 1U);
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    else if (vk_format <= VK_FORMAT_S8_UINT)
    {
        static_assert((VK_FORMAT_D32_SFLOAT + 1U) == VK_FORMAT_S8_UINT, "");
        assert(aspect_index < 1U);
        return VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    else if (vk_format <= VK_FORMAT_D32_SFLOAT_S8_UINT)
    {
        static_assert((VK_FORMAT_S8_UINT + 1U) == VK_FORMAT_D16_UNORM_S8_UINT, "");
        assert(aspect_index < 2U);
        VkImageAspectFlagBits aspect_masks[2] = {VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_STENCIL_BIT};
        return aspect_masks[aspect_index];
    }
    else if (vk_format <= VK_FORMAT_ASTC_12x12_SRGB_BLOCK)
    {
        static_assert((VK_FORMAT_D32_SFLOAT_S8_UINT + 1U) == VK_FORMAT_BC1_RGB_UNORM_BLOCK, "");
        assert(aspect_index < 1U);
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }
    else
    {
        assert(0);
        return VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
    }
}

static inline bool is_format_rgba(VkFormat vk_format)
{
    return (vk_format >= VK_FORMAT_R4G4_UNORM_PACK8 && vk_format <= VK_FORMAT_E5B9G9R9_UFLOAT_PACK32);
}

static inline bool is_format_depth_stencil(VkFormat vk_format)
{
    return (vk_format >= VK_FORMAT_D16_UNORM && vk_format <= VK_FORMAT_D32_SFLOAT_S8_UINT);
}

static inline bool is_format_compressed(VkFormat vk_format)
{
    return (vk_format >= VK_FORMAT_BC1_RGB_UNORM_BLOCK && vk_format <= VK_FORMAT_ASTC_12x12_SRGB_BLOCK);
}

static inline uint32_t get_rgba_format_pixel_bytes(VkFormat vk_format)
{
    assert(is_format_rgba(vk_format));

    static struct
    {
        uint32_t m_pixel_bytes;

    } const vulkan_rgba_format_info_table[] = {
        {1},  //VK_FORMAT_R4G4_UNORM_PACK8
        {2},  //VK_FORMAT_R4G4B4A4_UNORM_PACK16
        {2},  //VK_FORMAT_B4G4R4A4_UNORM_PACK16
        {2},  //VK_FORMAT_R5G6B5_UNORM_PACK16
        {2},  //VK_FORMAT_B5G6R5_UNORM_PACK16
        {2},  //VK_FORMAT_R5G5B5A1_UNORM_PACK16
        {2},  //VK_FORMAT_B5G5R5A1_UNORM_PACK16
        {2},  //VK_FORMAT_A1R5G5B5_UNORM_PACK16
        {1},  //VK_FORMAT_R8_UNORM
        {1},  //VK_FORMAT_R8_SNORM
        {1},  //VK_FORMAT_R8_USCALED
        {1},  //VK_FORMAT_R8_SSCALED
        {1},  //VK_FORMAT_R8_UINT
        {1},  //VK_FORMAT_R8_SINT
        {1},  //VK_FORMAT_R8_SRGB
        {2},  //VK_FORMAT_R8G8_UNORM
        {2},  //VK_FORMAT_R8G8_SNORM
        {2},  //VK_FORMAT_R8G8_USCALED
        {2},  //VK_FORMAT_R8G8_SSCALED
        {2},  //VK_FORMAT_R8G8_UINT
        {2},  //VK_FORMAT_R8G8_SINT
        {2},  //VK_FORMAT_R8G8_SRGB
        {3},  //VK_FORMAT_R8G8B8_UNORM
        {3},  //VK_FORMAT_R8G8B8_SNORM
        {3},  //VK_FORMAT_R8G8B8_USCALED
        {3},  //VK_FORMAT_R8G8B8_SSCALED
        {3},  //VK_FORMAT_R8G8B8_UINT
        {3},  //VK_FORMAT_R8G8B8_SINT
        {3},  //VK_FORMAT_R8G8B8_SRGB
        {3},  //VK_FORMAT_B8G8R8_UNORM
        {3},  //VK_FORMAT_B8G8R8_SNORM
        {3},  //VK_FORMAT_B8G8R8_USCALED
        {3},  //VK_FORMAT_B8G8R8_SSCALED
        {3},  //VK_FORMAT_B8G8R8_UINT
        {3},  //VK_FORMAT_B8G8R8_SINT
        {3},  //VK_FORMAT_B8G8R8_SRGB
        {4},  //VK_FORMAT_R8G8B8A8_UNORM
        {4},  //VK_FORMAT_R8G8B8A8_SNORM
        {4},  //VK_FORMAT_R8G8B8A8_USCALED
        {4},  //VK_FORMAT_R8G8B8A8_SSCALED
        {4},  //VK_FORMAT_R8G8B8A8_UINT
        {4},  //VK_FORMAT_R8G8B8A8_SINT
        {4},  //VK_FORMAT_R8G8B8A8_SRGB
        {4},  //VK_FORMAT_B8G8R8A8_UNORM
        {4},  //VK_FORMAT_B8G8R8A8_SNORM
        {4},  //VK_FORMAT_B8G8R8A8_USCALED
        {4},  //VK_FORMAT_B8G8R8A8_SSCALED
        {4},  //VK_FORMAT_B8G8R8A8_UINT
        {4},  //VK_FORMAT_B8G8R8A8_SINT
        {4},  //VK_FORMAT_B8G8R8A8_SRGB
        {4},  //VK_FORMAT_A8B8G8R8_UNORM_PACK32
        {4},  //VK_FORMAT_A8B8G8R8_SNORM_PACK32
        {4},  //VK_FORMAT_A8B8G8R8_USCALED_PACK32
        {4},  //VK_FORMAT_A8B8G8R8_SSCALED_PACK32
        {4},  //VK_FORMAT_A8B8G8R8_UINT_PACK32
        {4},  //VK_FORMAT_A8B8G8R8_SINT_PACK32
        {4},  //VK_FORMAT_A8B8G8R8_SRGB_PACK32
        {4},  //VK_FORMAT_A2R10G10B10_UNORM_PACK32
        {4},  //VK_FORMAT_A2R10G10B10_SNORM_PACK32
        {4},  //VK_FORMAT_A2R10G10B10_USCALED_PACK32
        {4},  //VK_FORMAT_A2R10G10B10_SSCALED_PACK32
        {4},  //VK_FORMAT_A2R10G10B10_UINT_PACK32
        {4},  //VK_FORMAT_A2R10G10B10_SINT_PACK32
        {4},  //VK_FORMAT_A2B10G10R10_UNORM_PACK32
        {4},  //VK_FORMAT_A2B10G10R10_SNORM_PACK32
        {4},  //VK_FORMAT_A2B10G10R10_USCALED_PACK32
        {4},  //VK_FORMAT_A2B10G10R10_SSCALED_PACK32
        {4},  //VK_FORMAT_A2B10G10R10_UINT_PACK32
        {4},  //VK_FORMAT_A2B10G10R10_SINT_PACK32
        {2},  //VK_FORMAT_R16_UNORM
        {2},  //VK_FORMAT_R16_SNORM
        {2},  //VK_FORMAT_R16_USCALED
        {2},  //VK_FORMAT_R16_SSCALED
        {2},  //VK_FORMAT_R16_UINT
        {2},  //VK_FORMAT_R16_SINT
        {2},  //VK_FORMAT_R16_SFLOAT
        {4},  //VK_FORMAT_R16G16_UNORM
        {4},  //VK_FORMAT_R16G16_SNORM
        {4},  //VK_FORMAT_R16G16_USCALED
        {4},  //VK_FORMAT_R16G16_SSCALED
        {4},  //VK_FORMAT_R16G16_UINT
        {4},  //VK_FORMAT_R16G16_SINT
        {4},  //VK_FORMAT_R16G16_SFLOAT
        {6},  //VK_FORMAT_R16G16B16_UNORM
        {6},  //VK_FORMAT_R16G16B16_SNORM
        {6},  //VK_FORMAT_R16G16B16_USCALED
        {6},  //VK_FORMAT_R16G16B16_SSCALED
        {6},  //VK_FORMAT_R16G16B16_UINT
        {6},  //VK_FORMAT_R16G16B16_SINT
        {6},  //VK_FORMAT_R16G16B16_SFLOAT
        {8},  //VK_FORMAT_R16G16B16A16_UNORM
        {8},  //VK_FORMAT_R16G16B16A16_SNORM
        {8},  //VK_FORMAT_R16G16B16A16_USCALED
        {8},  //VK_FORMAT_R16G16B16A16_SSCALED
        {8},  //VK_FORMAT_R16G16B16A16_UINT
        {8},  //VK_FORMAT_R16G16B16A16_SINT
        {8},  //VK_FORMAT_R16G16B16A16_SFLOAT
        {4},  //VK_FORMAT_R32_UINT
        {4},  //VK_FORMAT_R32_SINT
        {4},  //VK_FORMAT_R32_SFLOAT
        {8},  //VK_FORMAT_R32G32_UINT
        {8},  //VK_FORMAT_R32G32_SINT
        {8},  //VK_FORMAT_R32G32_SFLOAT
        {12}, //VK_FORMAT_R32G32B32_UINT
        {12}, //VK_FORMAT_R32G32B32_SINT
        {12}, //VK_FORMAT_R32G32B32_SFLOAT
        {16}, //VK_FORMAT_R32G32B32A32_UINT
        {16}, //VK_FORMAT_R32G32B32A32_SINT
        {16}, //VK_FORMAT_R32G32B32A32_SFLOAT
        {8},  //VK_FORMAT_R64_UINT
        {8},  //VK_FORMAT_R64_SINT
        {8},  //VK_FORMAT_R64_SFLOAT
        {16}, //VK_FORMAT_R64G64_UINT
        {16}, //VK_FORMAT_R64G64_SINT
        {16}, //VK_FORMAT_R64G64_SFLOAT
        {24}, //VK_FORMAT_R64G64B64_UINT
        {24}, //VK_FORMAT_R64G64B64_SINT
        {24}, //VK_FORMAT_R64G64B64_SFLOAT
        {32}, //VK_FORMAT_R64G64B64A64_UINT
        {32}, //VK_FORMAT_R64G64B64A64_SINT
        {32}, //VK_FORMAT_R64G64B64A64_SFLOAT
        {4},  //VK_FORMAT_B10G11R11_UFLOAT_PACK32
        {4}   //VK_FORMAT_E5B9G9R9_UFLOAT_PACK32
    };

    uint32_t format_info_index = (vk_format - VK_FORMAT_R4G4_UNORM_PACK8);
    assert(format_info_index < (sizeof(vulkan_rgba_format_info_table) / sizeof(vulkan_rgba_format_info_table[0])));

    return vulkan_rgba_format_info_table[format_info_index].m_pixel_bytes;
}

static inline uint32_t get_depth_stencil_format_pixel_bytes(VkFormat vk_format, uint32_t aspect_index)
{
    assert(is_format_depth_stencil(vk_format));

    static struct
    {
        uint32_t m_depth_bytes;
        uint32_t m_stencil_bytes;
    } const vulkan_depth_stencil_format_info_table[] = {
        {2, 0}, //VK_FORMAT_D16_UNORM
        {4, 0}, //VK_FORMAT_X8_D24_UNORM_PACK32
        {4, 0}, //VK_FORMAT_D32_SFLOAT
        {0, 1}, //VK_FORMAT_S8_UINT
        {2, 2}, //VK_FORMAT_D16_UNORM_S8_UINT         //data copied to or from the depth aspect of a VK_FORMAT_D16_UNORM or VK_FORMAT_D16_UNORM_S8_UINT format is tightly packed with one VK_FORMAT_D16_UNORM value per texel.
        {4, 4}, //VK_FORMAT_D24_UNORM_S8_UINT         //data copied to or from the depth aspect of a VK_FORMAT_X8_D24_UNORM_PACK32 or VK_FORMAT_D24_UNORM_S8_UINT format is packed with one 32-bit word per texel with the D24 value in the LSBs of the word, and undefined values in the eight MSBs.
        {4, 4}  //VK_FORMAT_D32_SFLOAT_S8_UINT        //data copied to or from the depth aspect of a VK_FORMAT_D32_SFLOAT or VK_FORMAT_D32_SFLOAT_S8_UINT format is tightly packed with one VK_FORMAT_D32_SFLOAT value per texel.
    };

    uint32_t format_info_index = (vk_format - VK_FORMAT_D16_UNORM);
    assert(format_info_index < (sizeof(vulkan_depth_stencil_format_info_table) / sizeof(vulkan_depth_stencil_format_info_table[0])));

    if (0U == vulkan_depth_stencil_format_info_table[format_info_index].m_stencil_bytes)
    {
        assert(aspect_index < 1);
        return vulkan_depth_stencil_format_info_table[format_info_index].m_depth_bytes;
    }
    else if (0U != vulkan_depth_stencil_format_info_table[format_info_index].m_depth_bytes)
    {
        assert(aspect_index < 2);
        uint32_t pixel_bytes[2] = {vulkan_depth_stencil_format_info_table[format_info_index].m_depth_bytes, vulkan_depth_stencil_format_info_table[format_info_index].m_stencil_bytes};
        return pixel_bytes[aspect_index];
    }
    else
    {
        assert(aspect_index < 1);
        return vulkan_depth_stencil_format_info_table[format_info_index].m_stencil_bytes;
    }
}

static inline uint32_t get_compressed_format_block_width(VkFormat vk_format)
{
    assert(is_format_compressed(vk_format));

    static struct
    {
        uint32_t compressedBlockWidth;
        uint32_t compressedBlockHeight;
        uint32_t compressedBlockDepth;
        uint32_t compressedBlockSizeInBytes;
    } const vulkan_compressed_format_info_table[] = {
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGB_UNORM_BLOCK       //GL_COMPRESSED_RGB_S3TC_DXT1_EXT           //R5G6B5_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGB_SRGB_BLOCK
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGBA_UNORM_BLOCK      //GL_COMPRESSED_RGBA_S3TC_DXT1_EXT          //R5G6B5A1_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGBA_SRGB_BLOCK
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC2_UNORM_BLOCK           //GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE        //R5G6B5A4_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC2_SRGB_BLOCK
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC3_UNORM_BLOCK           //GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE        //R5G6B5A8_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC3_SRGB_BLOCK
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC4_UNORM_BLOCK           //GL_COMPRESSED_RED_RGTC1_EXT               //R8_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC4_SNORM_BLOCK           //GL_COMPRESSED_SIGNED_RED_RGTC1_EXT        //R8_SNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC5_UNORM_BLOCK           //GL_COMPRESSED_RED_GREEN_RGTC2_EXT         //R8G8_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC5_SNORM_BLOCK           //GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT  //R8G8_SNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC6H_UFLOAT_BLOCK         //GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT     //R16G16B16_UFLOAT (HDR)
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC6H_SFLOAT_BLOCK         //GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT       //R16G16B16_SFLOAT (HDR)
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC7_UNORM_BLOCK           //GL_COMPRESSED_RGBA_BPTC_UNORM_EXT         //B7G7R7A8_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC7_SRGB_BLOCK            //GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT   //B7G7R7A8_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK   //GL_COMPRESSED_RGB8_ETC2
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK    //GL_COMPRESSED_SRGB8_ETC2
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK //GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK  //GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK //GL_COMPRESSED_RGBA8_ETC2_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK  //GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_EAC_R11_UNORM_BLOCK       //GL_COMPRESSED_R11_EAC
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_EAC_R11_SNORM_BLOCK       //GL_COMPRESSED_SIGNED_R11_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_EAC_R11G11_UNORM_BLOCK    //GL_COMPRESSED_RG11_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_EAC_R11G11_SNORM_BLOCK    //GL_COMPRESSED_SIGNED_RG11_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_4x4_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_4x4_KHR             //VK_EXT_astc_decode_mode
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_4x4_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR     //VK_EXT_texture_compression_astc_hdr
        {5, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x4_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_5x4_KHR             //
        {5, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x4_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR     //
        {5, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_5x5_KHR             //
        {5, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR     //
        {6, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_6x5_KHR             //
        {6, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR     //
        {6, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x6_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_6x6_KHR             //
        {6, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x6_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR     //
        {8, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x5_KHR             //
        {8, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR     //
        {8, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x6_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x6_KHR             //
        {8, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x6_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR     //
        {8, 8, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x8_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x8_KHR             //
        {8, 8, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x8_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR     //
        {10, 5, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x5_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x5_KHR            //
        {10, 5, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x5_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR    //
        {10, 6, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x6_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x6_KHR            //
        {10, 6, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x6_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR    //
        {10, 8, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x8_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x8_KHR            //
        {10, 8, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x8_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR    //
        {10, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_10x10_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_10x10_KHR           //
        {10, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_10x10_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR   //
        {12, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_12x10_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_12x10_KHR           //
        {12, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_12x10_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR   //
        {12, 12, 1, (128 / 8)}, //VK_FORMAT_ASTC_12x12_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_12x12_KHR           //
        {12, 12, 1, (128 / 8)}  //VK_FORMAT_ASTC_12x12_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR   //
    };
    uint32_t format_info_index = (vk_format - VK_FORMAT_BC1_RGB_UNORM_BLOCK);
    assert(format_info_index < (sizeof(vulkan_compressed_format_info_table) / sizeof(vulkan_compressed_format_info_table[0])));

    return vulkan_compressed_format_info_table[format_info_index].compressedBlockWidth;
}

static inline uint32_t get_compressed_format_block_height(VkFormat vk_format)
{
    assert(is_format_compressed(vk_format));

    static struct
    {
        uint32_t compressedBlockWidth;
        uint32_t compressedBlockHeight;
        uint32_t compressedBlockDepth;
        uint32_t compressedBlockSizeInBytes;
    } const vulkan_compressed_format_info_table[] = {
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGB_UNORM_BLOCK       //GL_COMPRESSED_RGB_S3TC_DXT1_EXT           //R5G6B5_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGB_SRGB_BLOCK
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGBA_UNORM_BLOCK      //GL_COMPRESSED_RGBA_S3TC_DXT1_EXT          //R5G6B5A1_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGBA_SRGB_BLOCK
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC2_UNORM_BLOCK           //GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE        //R5G6B5A4_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC2_SRGB_BLOCK
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC3_UNORM_BLOCK           //GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE        //R5G6B5A8_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC3_SRGB_BLOCK
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC4_UNORM_BLOCK           //GL_COMPRESSED_RED_RGTC1_EXT               //R8_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC4_SNORM_BLOCK           //GL_COMPRESSED_SIGNED_RED_RGTC1_EXT        //R8_SNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC5_UNORM_BLOCK           //GL_COMPRESSED_RED_GREEN_RGTC2_EXT         //R8G8_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC5_SNORM_BLOCK           //GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT  //R8G8_SNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC6H_UFLOAT_BLOCK         //GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT     //R16G16B16_UFLOAT (HDR)
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC6H_SFLOAT_BLOCK         //GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT       //R16G16B16_SFLOAT (HDR)
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC7_UNORM_BLOCK           //GL_COMPRESSED_RGBA_BPTC_UNORM_EXT         //B7G7R7A8_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC7_SRGB_BLOCK            //GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT   //B7G7R7A8_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK   //GL_COMPRESSED_RGB8_ETC2
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK    //GL_COMPRESSED_SRGB8_ETC2
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK //GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK  //GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK //GL_COMPRESSED_RGBA8_ETC2_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK  //GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_EAC_R11_UNORM_BLOCK       //GL_COMPRESSED_R11_EAC
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_EAC_R11_SNORM_BLOCK       //GL_COMPRESSED_SIGNED_R11_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_EAC_R11G11_UNORM_BLOCK    //GL_COMPRESSED_RG11_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_EAC_R11G11_SNORM_BLOCK    //GL_COMPRESSED_SIGNED_RG11_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_4x4_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_4x4_KHR             //VK_EXT_astc_decode_mode
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_4x4_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR     //VK_EXT_texture_compression_astc_hdr
        {5, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x4_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_5x4_KHR             //
        {5, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x4_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR     //
        {5, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_5x5_KHR             //
        {5, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR     //
        {6, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_6x5_KHR             //
        {6, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR     //
        {6, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x6_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_6x6_KHR             //
        {6, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x6_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR     //
        {8, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x5_KHR             //
        {8, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR     //
        {8, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x6_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x6_KHR             //
        {8, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x6_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR     //
        {8, 8, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x8_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x8_KHR             //
        {8, 8, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x8_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR     //
        {10, 5, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x5_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x5_KHR            //
        {10, 5, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x5_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR    //
        {10, 6, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x6_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x6_KHR            //
        {10, 6, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x6_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR    //
        {10, 8, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x8_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x8_KHR            //
        {10, 8, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x8_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR    //
        {10, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_10x10_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_10x10_KHR           //
        {10, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_10x10_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR   //
        {12, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_12x10_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_12x10_KHR           //
        {12, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_12x10_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR   //
        {12, 12, 1, (128 / 8)}, //VK_FORMAT_ASTC_12x12_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_12x12_KHR           //
        {12, 12, 1, (128 / 8)}  //VK_FORMAT_ASTC_12x12_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR   //
    };
    uint32_t format_info_index = (vk_format - VK_FORMAT_BC1_RGB_UNORM_BLOCK);
    assert(format_info_index < (sizeof(vulkan_compressed_format_info_table) / sizeof(vulkan_compressed_format_info_table[0])));

    return vulkan_compressed_format_info_table[format_info_index].compressedBlockHeight;
}

static inline uint32_t get_compressed_format_block_depth(VkFormat vk_format)
{
    assert(is_format_compressed(vk_format));

    static struct
    {
        uint32_t compressedBlockWidth;
        uint32_t compressedBlockHeight;
        uint32_t compressedBlockDepth;
        uint32_t compressedBlockSizeInBytes;
    } const vulkan_compressed_format_info_table[] = {
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGB_UNORM_BLOCK       //GL_COMPRESSED_RGB_S3TC_DXT1_EXT           //R5G6B5_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGB_SRGB_BLOCK
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGBA_UNORM_BLOCK      //GL_COMPRESSED_RGBA_S3TC_DXT1_EXT          //R5G6B5A1_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGBA_SRGB_BLOCK
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC2_UNORM_BLOCK           //GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE        //R5G6B5A4_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC2_SRGB_BLOCK
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC3_UNORM_BLOCK           //GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE        //R5G6B5A8_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC3_SRGB_BLOCK
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC4_UNORM_BLOCK           //GL_COMPRESSED_RED_RGTC1_EXT               //R8_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC4_SNORM_BLOCK           //GL_COMPRESSED_SIGNED_RED_RGTC1_EXT        //R8_SNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC5_UNORM_BLOCK           //GL_COMPRESSED_RED_GREEN_RGTC2_EXT         //R8G8_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC5_SNORM_BLOCK           //GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT  //R8G8_SNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC6H_UFLOAT_BLOCK         //GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT     //R16G16B16_UFLOAT (HDR)
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC6H_SFLOAT_BLOCK         //GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT       //R16G16B16_SFLOAT (HDR)
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC7_UNORM_BLOCK           //GL_COMPRESSED_RGBA_BPTC_UNORM_EXT         //B7G7R7A8_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC7_SRGB_BLOCK            //GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT   //B7G7R7A8_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK   //GL_COMPRESSED_RGB8_ETC2
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK    //GL_COMPRESSED_SRGB8_ETC2
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK //GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK  //GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK //GL_COMPRESSED_RGBA8_ETC2_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK  //GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_EAC_R11_UNORM_BLOCK       //GL_COMPRESSED_R11_EAC
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_EAC_R11_SNORM_BLOCK       //GL_COMPRESSED_SIGNED_R11_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_EAC_R11G11_UNORM_BLOCK    //GL_COMPRESSED_RG11_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_EAC_R11G11_SNORM_BLOCK    //GL_COMPRESSED_SIGNED_RG11_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_4x4_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_4x4_KHR             //VK_EXT_astc_decode_mode
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_4x4_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR     //VK_EXT_texture_compression_astc_hdr
        {5, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x4_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_5x4_KHR             //
        {5, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x4_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR     //
        {5, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_5x5_KHR             //
        {5, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR     //
        {6, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_6x5_KHR             //
        {6, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR     //
        {6, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x6_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_6x6_KHR             //
        {6, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x6_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR     //
        {8, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x5_KHR             //
        {8, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR     //
        {8, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x6_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x6_KHR             //
        {8, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x6_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR     //
        {8, 8, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x8_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x8_KHR             //
        {8, 8, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x8_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR     //
        {10, 5, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x5_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x5_KHR            //
        {10, 5, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x5_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR    //
        {10, 6, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x6_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x6_KHR            //
        {10, 6, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x6_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR    //
        {10, 8, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x8_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x8_KHR            //
        {10, 8, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x8_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR    //
        {10, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_10x10_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_10x10_KHR           //
        {10, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_10x10_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR   //
        {12, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_12x10_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_12x10_KHR           //
        {12, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_12x10_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR   //
        {12, 12, 1, (128 / 8)}, //VK_FORMAT_ASTC_12x12_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_12x12_KHR           //
        {12, 12, 1, (128 / 8)}  //VK_FORMAT_ASTC_12x12_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR   //
    };
    uint32_t format_info_index = (vk_format - VK_FORMAT_BC1_RGB_UNORM_BLOCK);
    assert(format_info_index < (sizeof(vulkan_compressed_format_info_table) / sizeof(vulkan_compressed_format_info_table[0])));

    return vulkan_compressed_format_info_table[format_info_index].compressedBlockDepth;
}

static inline uint32_t get_compressed_format_block_size_in_bytes(VkFormat vk_format)
{
    assert(is_format_compressed(vk_format));

    static struct
    {
        uint32_t compressedBlockWidth;
        uint32_t compressedBlockHeight;
        uint32_t compressedBlockDepth;
        uint32_t compressedBlockSizeInBytes;
    } const vulkan_compressed_format_info_table[] = {
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGB_UNORM_BLOCK       //GL_COMPRESSED_RGB_S3TC_DXT1_EXT           //R5G6B5_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGB_SRGB_BLOCK
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGBA_UNORM_BLOCK      //GL_COMPRESSED_RGBA_S3TC_DXT1_EXT          //R5G6B5A1_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC1_RGBA_SRGB_BLOCK
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC2_UNORM_BLOCK           //GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE        //R5G6B5A4_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC2_SRGB_BLOCK
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC3_UNORM_BLOCK           //GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE        //R5G6B5A8_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC3_SRGB_BLOCK
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC4_UNORM_BLOCK           //GL_COMPRESSED_RED_RGTC1_EXT               //R8_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_BC4_SNORM_BLOCK           //GL_COMPRESSED_SIGNED_RED_RGTC1_EXT        //R8_SNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC5_UNORM_BLOCK           //GL_COMPRESSED_RED_GREEN_RGTC2_EXT         //R8G8_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC5_SNORM_BLOCK           //GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT  //R8G8_SNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC6H_UFLOAT_BLOCK         //GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT     //R16G16B16_UFLOAT (HDR)
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC6H_SFLOAT_BLOCK         //GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT       //R16G16B16_SFLOAT (HDR)
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC7_UNORM_BLOCK           //GL_COMPRESSED_RGBA_BPTC_UNORM_EXT         //B7G7R7A8_UNORM
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_BC7_SRGB_BLOCK            //GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT   //B7G7R7A8_UNORM
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK   //GL_COMPRESSED_RGB8_ETC2
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK    //GL_COMPRESSED_SRGB8_ETC2
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK //GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK  //GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK //GL_COMPRESSED_RGBA8_ETC2_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK  //GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_EAC_R11_UNORM_BLOCK       //GL_COMPRESSED_R11_EAC
        {4, 4, 1, (64 / 8)},    //VK_FORMAT_EAC_R11_SNORM_BLOCK       //GL_COMPRESSED_SIGNED_R11_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_EAC_R11G11_UNORM_BLOCK    //GL_COMPRESSED_RG11_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_EAC_R11G11_SNORM_BLOCK    //GL_COMPRESSED_SIGNED_RG11_EAC
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_4x4_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_4x4_KHR             //VK_EXT_astc_decode_mode
        {4, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_4x4_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR     //VK_EXT_texture_compression_astc_hdr
        {5, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x4_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_5x4_KHR             //
        {5, 4, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x4_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR     //
        {5, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_5x5_KHR             //
        {5, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_5x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR     //
        {6, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_6x5_KHR             //
        {6, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR     //
        {6, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x6_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_6x6_KHR             //
        {6, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_6x6_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR     //
        {8, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x5_KHR             //
        {8, 5, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR     //
        {8, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x6_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x6_KHR             //
        {8, 6, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x6_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR     //
        {8, 8, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x8_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x8_KHR             //
        {8, 8, 1, (128 / 8)},   //VK_FORMAT_ASTC_8x8_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR     //
        {10, 5, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x5_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x5_KHR            //
        {10, 5, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x5_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR    //
        {10, 6, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x6_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x6_KHR            //
        {10, 6, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x6_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR    //
        {10, 8, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x8_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x8_KHR            //
        {10, 8, 1, (128 / 8)},  //VK_FORMAT_ASTC_10x8_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR    //
        {10, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_10x10_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_10x10_KHR           //
        {10, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_10x10_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR   //
        {12, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_12x10_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_12x10_KHR           //
        {12, 10, 1, (128 / 8)}, //VK_FORMAT_ASTC_12x10_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR   //
        {12, 12, 1, (128 / 8)}, //VK_FORMAT_ASTC_12x12_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_12x12_KHR           //
        {12, 12, 1, (128 / 8)}  //VK_FORMAT_ASTC_12x12_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR   //
    };
    uint32_t format_info_index = (vk_format - VK_FORMAT_BC1_RGB_UNORM_BLOCK);
    assert(format_info_index < (sizeof(vulkan_compressed_format_info_table) / sizeof(vulkan_compressed_format_info_table[0])));

    return vulkan_compressed_format_info_table[format_info_index].compressedBlockSizeInBytes;
}
