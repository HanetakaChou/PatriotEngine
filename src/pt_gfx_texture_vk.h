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

#ifndef _PT_GFX_TEXTURE_VK_H_
#define _PT_GFX_TEXTURE_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_texture_common.h"
#include "pt_gfx_connection_vk.h"
#include <vulkan/vulkan.h>

class gfx_texture_vk : public gfx_texture_common
{
    VkImage m_image;

    gfx_connection_vk *m_gfx_connection;

    void destroy() override;

    bool read_input_stream(
        char const *initial_filename,
        gfx_input_stream(PT_PTR *input_stream_init_callback)(char const *initial_filename),
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream input_stream, int64_t offset, int whence),
        void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream input_stream)) override;

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

    static inline enum VkImageType common_to_vulkan_type_translate(uint32_t common_type);

    static inline enum VkFormat common_to_vulkan_format_translate(uint32_t common_format);

    static inline uint32_t calc_subresource(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mipLevels, uint32_t arrayLayers);

    static inline size_t get_copyable_footprints(
        struct specific_header_vk_t const *specific_header_vk,
        VkDeviceSize physical_device_limits_optimal_buffer_copy_offset_alignment, VkDeviceSize physical_device_limits_optimal_buffer_copy_row_pitch_alignment,
        size_t num_subresources, struct load_memcpy_dest_t *out_memcpy_dest, struct VkBufferImageCopy *out_cmdcopy_dest);

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

    static struct vulkan_format_info_t const vulkan_format_info_table[VK_FORMAT_ASTC_12x12_SRGB_BLOCK - VK_FORMAT_UNDEFINED + 1];

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
    inline gfx_texture_vk(gfx_connection_vk *gfx_connection) : m_image(VK_NULL_HANDLE), m_gfx_connection(gfx_connection) {}
};

#endif
