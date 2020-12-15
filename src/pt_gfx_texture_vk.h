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

    struct specific_header_vk_t
    {
        bool isCubeCompatible;
        VkImageType imageType;
        VkFormat format;
        VkExtent3D extent;
        uint32_t mipLevels;
        uint32_t arrayLayers;
    };

    static inline enum VkImageType common_to_vulkan_type_translate(uint32_t common_type);

    static inline enum VkFormat common_to_vulkan_format_translate(uint32_t common_format);

    static inline struct specific_header_vk_t common_to_specific_header_translate(struct common_header_t const *common_header);

    bool read_input_stream(
        char const *initial_filename,
        gfx_input_stream(PT_PTR *input_stream_init_callback)(char const *initial_filename),
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream input_stream, int64_t offset, int whence),
        void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream input_stream)) override;

    void destroy() override;

public:
    inline gfx_texture_vk(gfx_connection_vk *gfx_connection) : m_image(VK_NULL_HANDLE), m_gfx_connection(gfx_connection) {}
};

#endif
