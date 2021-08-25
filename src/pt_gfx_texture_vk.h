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

#ifndef _PT_GFX_TEXTURE_VK_H_
#define _PT_GFX_TEXTURE_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_texture_base.h"
#include "pt_gfx_connection_vk.h"
#include "pt_gfx_streaming_object_base.h"
#include "pt_gfx_frame_object_base.h"
#include <vulkan/vulkan.h>

class gfx_texture_vk final : public gfx_texture_base
{
    VkImage m_image;

    uint64_t m_gfx_malloc_offset;
    uint64_t m_gfx_malloc_size;
    void *m_gfx_malloc_page_handle;
    VkDeviceMemory m_gfx_malloc_device_memory;

    VkImageView m_image_view;

    bool read_input_stream(
        class gfx_connection_base *gfx_connection,
        char const *initial_filename,
        gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
        void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream)) override;

    struct specific_header_vk_t
    {
        bool isCubeCompatible;
        VkImageType imageType;
        VkFormat format;
        VkExtent3D extent;
        uint32_t mipLevels;
        uint32_t arrayLayers;
    };

    struct texture_streaming_stage_second_thread_stack_data_t
    {
        struct common_header_t m_common_header;
        size_t m_common_data_offset;
        struct specific_header_vk_t m_specific_header_vk;
        uint32_t m_num_subresource;
        struct load_memcpy_dest_t *m_memcpy_dest;
        VkBufferImageCopy *m_cmdcopy_dest;
    };
    static_assert(sizeof(struct texture_streaming_stage_second_thread_stack_data_t) <= sizeof(struct streaming_stage_second_thread_stack_data_user_defined_t), "");

    bool streaming_stage_first_pre_populate_task_data_callback(
        class gfx_connection_base *gfx_connection,
        gfx_input_stream_ref input_stream,
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
        void *thread_stack_user_defined) override;

    void streaming_stage_first_populate_task_data_callback(
        void *thread_stack_user_defined,
        struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined) override;

    bool streaming_stage_second_pre_calculate_total_size_callback(
        struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined,
        gfx_input_stream_ref input_stream,
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
        struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined) override;

    size_t streaming_stage_second_calculate_total_size_callback(
        uint64_t base_offset,
        struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined,
        class gfx_connection_base *gfx_connection,
        struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined) override;

    bool streaming_stage_second_post_calculate_total_size_callback(
        bool staging_buffer_allocate_success,
        uint32_t streaming_throttling_index,
        struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined,
        class gfx_connection_base *gfx_connection,
        gfx_input_stream_ref input_stream,
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
        struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined) override;

    void streaming_destroy_callback(class gfx_connection_base *gfx_connection) override;

    bool streaming_done_callback(class gfx_connection_base *gfx_connection) override;

    void frame_destroy_callback(class gfx_connection_base *gfx_connection) override;

    inline void unified_destory(class gfx_connection_vk *gfx_connection);

private:
    static inline struct specific_header_vk_t common_to_specific_header_translate(struct common_header_t const *common_header);

    static inline enum VkImageType common_to_vulkan_type_translate(enum gfx_texture_common_type_t common_type);

    static inline enum VkFormat common_to_vulkan_format_translate(enum gfx_texture_common_format_t common_format);

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
        size_t num_subresources, struct load_memcpy_dest_t *out_memcpy_dest, struct VkBufferImageCopy *out_cmdcopy_dest);

public:
    inline gfx_texture_vk() : gfx_texture_base(),
                              m_image(VK_NULL_HANDLE),
                              m_gfx_malloc_offset(uint64_t(-1)), m_gfx_malloc_size(uint64_t(-1)), m_gfx_malloc_page_handle(NULL), m_gfx_malloc_device_memory(VK_NULL_HANDLE),
                              m_image_view(VK_NULL_HANDLE)
    {
    }

    inline VkImageView get_image_view() const
    {
        assert((!this->is_streaming_error()) && this->is_streaming_done());
        return this->m_image_view;
    }
};

#endif
