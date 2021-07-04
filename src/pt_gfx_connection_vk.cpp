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
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_atomic.h>
#include "pt_gfx_connection_vk.h"
#include "pt_gfx_texture_vk.h"
#include <new>

inline gfx_connection_vk::gfx_connection_vk()
{
}

bool gfx_connection_vk::init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window)
{
    if (!m_device.init(wsi_connection, wsi_visual, wsi_window))
    {
        return false;
    }

    if (!m_malloc.init(&m_device))
    {
        return false;
    }

    //Frame Throttling
    {
        this->m_frame_throtting_index = 0U;

        for (uint32_t frame_throttling_index = 0U; frame_throttling_index < FRAME_THROTTLING_COUNT; ++frame_throttling_index)
        {
            VkCommandPoolCreateInfo command_pool_create_info = {
                VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                NULL,
                0U,
                m_device.queue_graphics_family_index()};
            VkResult res_create_command_pool = m_device.create_command_Pool(&command_pool_create_info, &this->m_graphics_commmand_pool[frame_throttling_index]);
            assert(VK_SUCCESS == res_create_command_pool);
        }
    }

    // Streaming
    this->m_streaming_affinity_mask = 0U;

    if (m_device.has_dedicated_transfer_queue())
    {
        for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
        {
            VkCommandPoolCreateInfo command_pool_create_info = {
                VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                NULL,
                0U,
                m_device.queue_transfer_family_index()};
            VkResult res_create_command_pool = m_device.create_command_Pool(&command_pool_create_info, &this->m_streaming_command_pool[streaming_thread_index]);
            assert(VK_SUCCESS == res_create_command_pool);
        }
    }
    else
    {
        // use the same graphics queue but different command pools
        for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
        {
            VkCommandPoolCreateInfo command_pool_create_info = {
                VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                NULL,
                0U,
                m_device.queue_graphics_family_index()};
            VkResult res_create_command_pool = m_device.create_command_Pool(&command_pool_create_info, &this->m_streaming_command_pool[streaming_thread_index]);
            assert(VK_SUCCESS == res_create_command_pool);
        }
    }

    for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
    {
        this->m_streaming_command_buffer[streaming_thread_index] = NULL;
    }

    return true;
}

void gfx_connection_vk::destroy()
{
    this->~gfx_connection_vk();
    mcrt_free(this);
}

inline gfx_connection_vk::~gfx_connection_vk()
{
}

class gfx_connection_common *gfx_connection_vk_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window)
{
    class gfx_connection_vk *connection = new (mcrt_aligned_malloc(sizeof(gfx_connection_vk), alignof(gfx_connection_vk))) gfx_connection_vk();
    if (connection->init(wsi_connection, wsi_visual, wsi_window))
    {
        return connection;
    }
    else
    {
        connection->destroy();
        return NULL;
    }
}

inline uint32_t gfx_connection_vk::streaming_thread_index_get()
{
    uint32_t streaming_thread_index = uint32_t(-1);

    mcrt_native_thread_id native_thread_id = mcrt_native_thread_id_get();

    //
    {
        uint32_t streaming_affinity_mask_temp = mcrt_atomic_load(&this->m_streaming_affinity_mask);
        for (uint32_t i = 0U; i < STREAMING_THREAD_COUNT; ++i)
        {
            if ((0U != (streaming_affinity_mask_temp & (1U << i))) && (native_thread_id == this->m_streaming_native_thread_id[i]))
            {
                streaming_thread_index = i;
                break;
            }
        }
    }

    //
    {
        if (uint32_t(-1) == streaming_thread_index)
        {
            streaming_thread_index = this->streaming_thread_index_allocate();
            if (uint32_t(-1) != streaming_thread_index)
            {
                this->m_streaming_native_thread_id[streaming_thread_index] = native_thread_id;
            }
        }
    }

    return streaming_thread_index;
}

inline uint32_t gfx_connection_vk::streaming_thread_index_allocate()
{
    static_assert(STREAMING_THREAD_COUNT < 32U, "");

    uint32_t streaming_affinity_mask_old = uint32_t(-1);
    uint32_t streaming_affinity_mask_find = uint32_t(-1);
    uint32_t streaming_thread_index_find = uint32_t(-1);
    bool find_avaliable = false;
    do
    {
        streaming_affinity_mask_old = mcrt_atomic_load(&this->m_streaming_affinity_mask);

        find_avaliable = false;
        for (uint32_t i = 0U; i < STREAMING_THREAD_COUNT; ++i)
        {
            streaming_affinity_mask_find = (1U << i);
            if (0U == (streaming_affinity_mask_old & streaming_affinity_mask_find))
            {
                streaming_thread_index_find = i;
                find_avaliable = true;
                break;
            }
        }
    } while (find_avaliable && (streaming_affinity_mask_old != mcrt_atomic_cas_u32(&this->m_streaming_affinity_mask, streaming_affinity_mask_old | streaming_affinity_mask_find, streaming_affinity_mask_old)));

    if (find_avaliable)
    {
        return streaming_thread_index_find;
    }
    else
    {
        return uint32_t(-1);
    }
}

void gfx_connection_vk::copy_buffer_to_image(VkBuffer src_buffer, VkImage dst_image, VkImageSubresourceRange const *subresource_range, uint32_t region_count, const VkBufferImageCopy *regions)
{
    uint32_t streaming_thread_index = this->streaming_thread_index_get();
    assert(streaming_thread_index < STREAMING_THREAD_COUNT);

    if (PT_UNLIKELY(NULL == this->m_streaming_command_buffer[streaming_thread_index]))
    {
        VkCommandBufferAllocateInfo command_buffer_allocate_info;
        command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.pNext = NULL;
        command_buffer_allocate_info.commandPool = this->m_streaming_command_pool[streaming_thread_index];
        command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_allocate_info.commandBufferCount = 1U;

        VkResult res_allocate_command_buffers = m_device.allocate_command_buffers(&command_buffer_allocate_info, &this->m_streaming_command_buffer[streaming_thread_index]);
        assert(VK_SUCCESS == res_allocate_command_buffers);

        VkCommandBufferBeginInfo command_buffer_begin_info = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            NULL,
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            NULL,
        };
        VkResult res_begin_command_buffer = m_device.begin_command_buffer(this->m_streaming_command_buffer[streaming_thread_index], &command_buffer_begin_info);
        assert(VK_SUCCESS == res_begin_command_buffer);
    }

    VkImageMemoryBarrier image_memory_barrier_before_copy[1] = {
        {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
         NULL,
         0,
         VK_ACCESS_TRANSFER_WRITE_BIT,
         VK_IMAGE_LAYOUT_UNDEFINED,
         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
         VK_QUEUE_FAMILY_IGNORED,
         VK_QUEUE_FAMILY_IGNORED,
         dst_image,
         (*subresource_range)}};

    m_device.cmd_pipeline_barrier(this->m_streaming_command_buffer[streaming_thread_index], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_before_copy);

    m_device.cmd_copy_buffer_to_image(this->m_streaming_command_buffer[streaming_thread_index], src_buffer, dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, region_count, regions);

    // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#upload-data-from-the-cpu-to-an-image-sampled-in-a-fragment-shader

    if (this->m_device.has_dedicated_transfer_queue() && (this->m_device.queue_transfer_family_index()!=this->m_device.queue_graphics_family_index()))
    {
        // Queue Family Ownership Transfer
        // Release Operation
        VkImageMemoryBarrier image_memory_barrier_after_copy[1] = {
            {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
             NULL,
             VK_ACCESS_TRANSFER_WRITE_BIT,
             0,
             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
             this->m_device.queue_transfer_family_index(),
             this->m_device.queue_graphics_family_index(),
             dst_image,
             (*subresource_range)}};
        m_device.cmd_pipeline_barrier(this->m_streaming_command_buffer[streaming_thread_index], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_after_copy);

        // Queue Family Ownership Transfer
        // Acquire Operation
        // TODO: add to list
    }
    else
    {
        VkImageMemoryBarrier image_memory_barrier_after_copy[1] = {
            {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
             NULL,
             0,
             VK_ACCESS_SHADER_READ_BIT,
             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
             VK_QUEUE_FAMILY_IGNORED,
             VK_QUEUE_FAMILY_IGNORED,
             dst_image,
             (*subresource_range)}};
        m_device.cmd_pipeline_barrier(this->m_streaming_command_buffer[streaming_thread_index], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_after_copy);
    }
}

void gfx_connection_vk::wsi_on_resized(wsi_window_ref wsi_window, float width, float height)
{
}

void gfx_connection_vk::wsi_on_redraw_needed_acquire(wsi_window_ref wsi_window, float width, float height)
{
}

void gfx_connection_vk::wsi_on_redraw_needed_release()
{
}

class gfx_texture_common *gfx_connection_vk::create_texture()
{
    gfx_texture_vk *texture = new (mcrt_aligned_malloc(sizeof(gfx_texture_vk), alignof(gfx_texture_vk))) gfx_texture_vk(this);
    return texture;
}
