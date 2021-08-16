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
#include <pt_mcrt_task.h>
#include <pt_mcrt_scalable_allocator.h>
#include "pt_gfx_texture_base.h"
#include "pt_gfx_connection_vk.h"
#include <vulkan/vulkan.h>
#include <string>


class gfx_texture_vk final : public gfx_texture_base
{
    VkImage m_image;

    uint64_t m_gfx_malloc_offset;
    uint64_t m_gfx_malloc_size;
    void *m_gfx_malloc_page_handle;
    VkDeviceMemory m_gfx_malloc_device_memory;

    using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;

    bool read_input_stream(
        class gfx_connection_base *gfx_connection,
        char const *initial_filename,
        gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
        void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream)) override;

    struct read_input_stream_task_data
    {
        mcrt_string m_initial_filename;
        gfx_input_stream_ref(PT_PTR *m_input_stream_init_callback)(char const *initial_filename);
        intptr_t(PT_PTR *m_input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count);
        int64_t(PT_PTR *m_input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence);
        void(PT_PTR *m_input_stream_destroy_callback)(gfx_input_stream_ref input_stream);
        class gfx_connection_vk *m_gfx_connection;
        class gfx_texture_vk *m_gfx_texture;
    };

    static mcrt_task_ref read_input_stream_task_execute(mcrt_task_ref self);

    static inline mcrt_task_ref read_input_stream_task_execute_internal(uint32_t *output_streaming_throttling_index, bool *output_recycle, mcrt_task_ref self);

    void destroy(class gfx_connection_base *gfx_connection) override;

    void streaming_destroy_callback(class gfx_connection_base *gfx_connection) override;

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

    static inline uint32_t calc_subresource(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mipLevels, uint32_t arrayLayers);

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

    struct vk_rgba_format_info_t
    {
        uint32_t pixel_bytes;
    };
    static struct vk_rgba_format_info_t const vk_rgba_format_info_table[(VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 - VK_FORMAT_R4G4_UNORM_PACK8) + 1];

    struct vk_depth_stencil_format_info_t
    {
        uint32_t depth_bytes;
        uint32_t stencil_bytes;
    };
    static struct vk_depth_stencil_format_info_t const vk_depth_stencil_format_info_table[(VK_FORMAT_D32_SFLOAT_S8_UINT - VK_FORMAT_D16_UNORM) + 1];
    static_assert(VK_FORMAT_D16_UNORM == (VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 + 1), "VK_FORMAT_D16_UNORM == (VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 + 1)");

    struct vk_compressed_format_info_t
    {
        uint32_t compressed_block_width;
        uint32_t compressed_block_height;
        uint32_t compressed_block_depth;
        uint32_t compressed_block_size_in_bytes;
    };
    static struct vk_compressed_format_info_t const vk_compressed_format_info_table[(VK_FORMAT_ASTC_12x12_SRGB_BLOCK - VK_FORMAT_BC1_RGB_UNORM_BLOCK) + 1];
    static_assert(VK_FORMAT_BC1_RGB_UNORM_BLOCK == (VK_FORMAT_D32_SFLOAT_S8_UINT + 1), "VK_FORMAT_BC1_RGB_UNORM_BLOCK == (VK_FORMAT_D32_SFLOAT_S8_UINT + 1)");

    struct vulkan_format_info_t
    {
        uint32_t _union_tag;

        union
        {
            struct
            {
                uint32_t pixelBytes;
            } rgba;
            struct
            {
                uint32_t depthBytes;
                uint32_t stencilBytes;
            } depthstencil;
            struct
            {
                uint32_t compressedBlockWidth;
                uint32_t compressedBlockHeight;
                uint32_t compressedBlockDepth;
                uint32_t compressedBlockSizeInBytes;
            } compressed;
        };
    };
    static struct vulkan_format_info_t const vulkan_format_info_table[(VK_FORMAT_ASTC_12x12_SRGB_BLOCK - VK_FORMAT_UNDEFINED) + 1];
    static_assert(((VK_FORMAT_ASTC_12x12_SRGB_BLOCK - VK_FORMAT_UNDEFINED) + 1) == (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])), "(VK_FORMAT_ASTC_12x12_SRGB_BLOCK - VK_FORMAT_UNDEFINED + 1) == (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0]))");

    static inline uint32_t get_format_aspect_count(VkFormat vk_format);

    static inline uint32_t get_format_aspect_mask(VkFormat vk_format, uint32_t aspectIndex);

    static inline bool is_format_compressed(VkFormat vk_format);

    static inline bool is_format_rgba(VkFormat vk_format);

    static inline bool is_format_depth_stencil(VkFormat vk_format);

    static inline uint32_t get_compressed_format_block_width(VkFormat vk_format);

    static inline uint32_t get_compressed_format_block_height(VkFormat vk_format);

    static inline uint32_t get_compressed_format_block_depth(VkFormat vk_format);

    static inline uint32_t get_compressed_format_block_size_in_bytes(VkFormat vk_format);

    static inline uint32_t get_rgba_format_pixel_bytes(VkFormat vk_format);

    static inline uint32_t get_depth_stencil_format_pixel_bytes(VkFormat vk_format, uint32_t aspectIndex);

public:
    inline gfx_texture_vk() : gfx_texture_base(), m_image(VK_NULL_HANDLE), m_gfx_malloc_offset(uint64_t(-1)), m_gfx_malloc_size(uint64_t(-1)), m_gfx_malloc_page_handle(NULL), m_gfx_malloc_device_memory(VK_NULL_HANDLE) {}
};

#endif
