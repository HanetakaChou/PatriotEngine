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
#include <pt_mcrt_log.h>
#include <pt_mcrt_assert.h>
#include "pt_gfx_connection_vk.h"
#include "pt_gfx_texture_vk.h"
#include <new>

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

inline gfx_connection_vk::gfx_connection_vk()
{
}

inline bool gfx_connection_vk::init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window)
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
            PT_MAYBE_UNUSED VkResult res_create_command_pool = m_device.create_command_Pool(&command_pool_create_info, &this->m_graphics_commmand_pool[frame_throttling_index]);
            assert(VK_SUCCESS == res_create_command_pool);
        }
    }

    // Streaming
    if (!this->init_streaming())
    {
        return false;
    }

    // SwapChain Renderpass Framebuffer
    if (!this->init_swapchain_and_renderpass_and_framebuffer(wsi_connection, wsi_visual, wsi_window))
    {
        return false;
    }

    //
    if (!this->init_descriptor_and_pipeline_layout())
    {
        return false;
    }

    return true;
}

inline bool gfx_connection_vk::init_streaming()
{
    this->m_streaming_throttling_index = 0U;
    this->m_spin_lock_streaming_throttling_index = 0U;
    for (uint32_t streaming_throttling_index = 0U; streaming_throttling_index < STREAMING_THROTTLING_COUNT; ++streaming_throttling_index)
    {
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
        this->m_streaming_task_executing_count[streaming_throttling_index] = 0;
        this->m_streaming_task_reducing[streaming_throttling_index] = false;
#endif
        this->m_transfer_src_buffer_begin[streaming_throttling_index] = ((this->m_malloc.transfer_src_buffer_size() * streaming_throttling_index) / 3U);
        this->m_transfer_src_buffer_end[streaming_throttling_index] = this->m_transfer_src_buffer_begin[streaming_throttling_index];
        this->m_transfer_src_buffer_size[streaming_throttling_index] = ((this->m_malloc.transfer_src_buffer_size() * (streaming_throttling_index + 1U)) / 3U) - ((this->m_malloc.transfer_src_buffer_size() * streaming_throttling_index) / 3U);

        for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
        {
            if (m_device.has_dedicated_transfer_queue())
            {
                {
                    VkCommandPoolCreateInfo command_pool_create_info = {
                        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                        NULL,
                        0U,
                        m_device.queue_transfer_family_index()};
                    PT_MAYBE_UNUSED VkResult res_create_command_pool = m_device.create_command_Pool(&command_pool_create_info, &this->m_streaming_transfer_command_pool[streaming_throttling_index][streaming_thread_index]);
                    assert(VK_SUCCESS == res_create_command_pool);
                }

                if (this->m_device.queue_transfer_family_index() != this->m_device.queue_graphics_family_index())
                {
                    VkCommandPoolCreateInfo command_pool_create_info = {
                        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                        NULL,
                        0U,
                        m_device.queue_graphics_family_index()};
                    PT_MAYBE_UNUSED VkResult res_create_command_pool = m_device.create_command_Pool(&command_pool_create_info, &this->m_streaming_graphics_command_pool[streaming_throttling_index][streaming_thread_index]);
                    assert(VK_SUCCESS == res_create_command_pool);

                    VkSemaphoreCreateInfo semaphore_create_info;
                    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                    semaphore_create_info.pNext = NULL;
                    semaphore_create_info.flags = 0U;
                    PT_MAYBE_UNUSED VkResult res_create_semaphore = this->m_device.create_semaphore(&semaphore_create_info, &this->m_streaming_semaphore[streaming_throttling_index]);
                    assert(VK_SUCCESS == res_create_semaphore);
                }
            }
            else
            {
                // use the same graphics queue but different command pools
                VkCommandPoolCreateInfo command_pool_create_info = {
                    VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                    NULL,
                    0U,
                    m_device.queue_graphics_family_index()};
                PT_MAYBE_UNUSED VkResult res_create_command_pool = m_device.create_command_Pool(&command_pool_create_info, &this->m_streaming_graphics_command_pool[streaming_throttling_index][streaming_thread_index]);
                assert(VK_SUCCESS == res_create_command_pool);
            }

            this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index] = VK_NULL_HANDLE;
        }

        {
            VkFenceCreateInfo fence_create_info;
            fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_create_info.pNext = NULL;
            fence_create_info.flags = ((STREAMING_THROTTLING_COUNT - 1U) != streaming_throttling_index) ? 0U : VK_FENCE_CREATE_SIGNALED_BIT;
            PT_MAYBE_UNUSED VkResult res_create_fence = this->m_device.create_fence(&fence_create_info, &this->m_streaming_fence[streaming_throttling_index]);
            assert(VK_SUCCESS == res_create_fence);
        }

        this->m_streaming_task_root[streaming_throttling_index] = mcrt_task_allocate_root(NULL);
        mcrt_task_set_ref_count(this->m_streaming_task_root[streaming_throttling_index], 1U);

        // never used for wait
        this->m_streaming_task_respawn_root = mcrt_task_allocate_root(NULL);
        mcrt_task_set_ref_count(this->m_streaming_task_respawn_root, 1U);

        // init arena
        mcrt_task_wait_for_all(this->m_streaming_task_respawn_root);
        mcrt_task_set_ref_count(this->m_streaming_task_respawn_root, 1U);

        this->m_task_arena = mcrt_task_arena_attach();
        assert(mcrt_task_arena_is_active(this->m_task_arena));
        assert(NULL != mcrt_task_arena_internal_arena(this->m_task_arena));

        this->m_streaming_task_respawn_linear_list_count[streaming_throttling_index] = 0U;
        this->m_streaming_task_respawn_link_list_head[streaming_throttling_index] = NULL;

        this->m_streaming_object_linear_list_count[streaming_throttling_index] = 0U;
        this->m_streaming_object_link_list_head[streaming_throttling_index] = NULL;
    }

    return true;
}

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
void gfx_connection_vk::streaming_task_debug_executing_begin(uint32_t streaming_throttling_index)
{
    MCRT_ASSERT(!mcrt_atomic_load(&this->m_streaming_task_reducing[streaming_throttling_index]));
    mcrt_atomic_inc_u32(&this->m_streaming_task_executing_count[streaming_throttling_index]);
}

void gfx_connection_vk::streaming_task_debug_executing_end(uint32_t streaming_throttling_index)
{
    MCRT_ASSERT(!mcrt_atomic_load(&this->m_streaming_task_reducing[streaming_throttling_index]));
    mcrt_atomic_dec_u32(&this->m_streaming_task_executing_count[streaming_throttling_index]);
}
#endif

void gfx_connection_vk::streaming_object_list_push(uint32_t streaming_throttling_index, class gfx_streaming_object *streaming_object)
{
    uint32_t streaming_object_index = mcrt_atomic_inc_u32(&this->m_streaming_object_linear_list_count[streaming_throttling_index]) - 1U;
    if (streaming_object_index < STREAMING_OBJECT_LINEAR_LIST_COUNT)
    {
        this->m_streaming_object_linear_list[streaming_throttling_index][streaming_object_index] = streaming_object;
    }
    else
    {
        mcrt_atomic_dec_u32(&this->m_streaming_object_linear_list_count[streaming_throttling_index]);

        // performance issue

        // ABA safe since the fence ensures that there is no consumer
        struct streaming_object_link_list *streaming_object_link_list_node = new (mcrt_aligned_malloc(sizeof(struct streaming_object_link_list), alignof(struct streaming_object_link_list))) streaming_object_link_list{};
        streaming_object_link_list_node->m_streaming_object = streaming_object;

        struct streaming_object_link_list *streaming_object_link_list_head;
        do
        {
            streaming_object_link_list_head = mcrt_atomic_load(&this->m_streaming_object_link_list_head[streaming_throttling_index]);
            streaming_object_link_list_node->m_next = streaming_object_link_list_head;
        } while (streaming_object_link_list_head != mcrt_atomic_cas_ptr(&this->m_streaming_object_link_list_head[streaming_throttling_index], streaming_object_link_list_node, streaming_object_link_list_head));
    }
}

void gfx_connection_vk::streaming_task_respawn_list_push(uint32_t streaming_throttling_index, mcrt_task_ref streaming_task)
{
    uint32_t streaming_task_respawn_index = mcrt_atomic_inc_u32(&this->m_streaming_task_respawn_linear_list_count[streaming_throttling_index]) - 1U;
    if (streaming_task_respawn_index < STREAMING_TASK_RESPAWN_LINEAR_LIST_COUNT)
    {
        this->m_streaming_task_respawn_linear_list[streaming_throttling_index][streaming_task_respawn_index] = streaming_task;
    }
    else
    {
        mcrt_atomic_dec_u32(&this->m_streaming_task_respawn_linear_list_count[streaming_throttling_index]);

        // performance issue

        // Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 2 Public Free List / freeListPush + repatriatePublicFreeList
        // "This is ABA safe without concern for versioning because the concurrent data structure is a single consumer (the owning thread) and multiple producers."

        // ABA safe since the fence ensures that there is no consumer
        struct streaming_task_respawn_task_respawn_link_list *streaming_task_respawn_link_list_node = new (mcrt_aligned_malloc(sizeof(struct streaming_task_respawn_task_respawn_link_list), alignof(struct streaming_task_respawn_task_respawn_link_list))) streaming_task_respawn_task_respawn_link_list{};
        streaming_task_respawn_link_list_node->m_task = streaming_task;

        struct streaming_task_respawn_task_respawn_link_list *streaming_task_respawn_link_list_head;
        do
        {
            streaming_task_respawn_link_list_head = mcrt_atomic_load(&this->m_streaming_task_respawn_link_list_head[streaming_throttling_index]);
            streaming_task_respawn_link_list_node->m_next = streaming_task_respawn_link_list_head;
        } while (streaming_task_respawn_link_list_head != mcrt_atomic_cas_ptr(&this->m_streaming_task_respawn_link_list_head[streaming_throttling_index], streaming_task_respawn_link_list_node, streaming_task_respawn_link_list_head));
    }
}

inline VkCommandBuffer gfx_connection_vk::streaming_task_get_transfer_command_buffer(uint32_t streaming_throttling_index, uint32_t streaming_thread_index)
{
    assert(this->m_device.has_dedicated_transfer_queue());
    assert(uint32_t(-1) != streaming_thread_index);
    assert(streaming_thread_index < STREAMING_THREAD_COUNT);
    assert(mcrt_this_task_arena_max_concurrency() < STREAMING_THREAD_COUNT);

    if (PT_UNLIKELY(VK_NULL_HANDLE == this->m_streaming_transfer_command_buffer[streaming_throttling_index][streaming_thread_index]))
    {
        VkCommandBufferAllocateInfo command_buffer_allocate_info;
        command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.pNext = NULL;
        command_buffer_allocate_info.commandPool = this->m_streaming_transfer_command_pool[streaming_throttling_index][streaming_thread_index];
        command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_allocate_info.commandBufferCount = 1U;

        PT_MAYBE_UNUSED VkResult res_allocate_command_buffers = m_device.allocate_command_buffers(&command_buffer_allocate_info, &this->m_streaming_transfer_command_buffer[streaming_throttling_index][streaming_thread_index]);
        assert(VK_SUCCESS == res_allocate_command_buffers);

        VkCommandBufferBeginInfo command_buffer_begin_info = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            NULL,
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            NULL,
        };
        PT_MAYBE_UNUSED VkResult res_begin_command_buffer = m_device.begin_command_buffer(this->m_streaming_transfer_command_buffer[streaming_throttling_index][streaming_thread_index], &command_buffer_begin_info);
        assert(VK_SUCCESS == res_begin_command_buffer);
    }

    return this->m_streaming_transfer_command_buffer[streaming_throttling_index][streaming_thread_index];
}

inline VkCommandBuffer gfx_connection_vk::streaming_task_get_graphics_command_buffer(uint32_t streaming_throttling_index, uint32_t streaming_thread_index)
{
    assert((this->m_device.has_dedicated_transfer_queue() && (this->m_device.queue_transfer_family_index() != this->m_device.queue_graphics_family_index())) || (!this->m_device.has_dedicated_transfer_queue()));

    assert(uint32_t(-1) != streaming_thread_index);
    assert(streaming_thread_index < STREAMING_THREAD_COUNT);
    assert(mcrt_this_task_arena_max_concurrency() < STREAMING_THREAD_COUNT);

    if (PT_UNLIKELY(VK_NULL_HANDLE == this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index]))
    {
        VkCommandBufferAllocateInfo command_buffer_allocate_info;
        command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.pNext = NULL;
        command_buffer_allocate_info.commandPool = this->m_streaming_graphics_command_pool[streaming_throttling_index][streaming_thread_index];
        command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_allocate_info.commandBufferCount = 1U;

        PT_MAYBE_UNUSED VkResult res_allocate_command_buffers = m_device.allocate_command_buffers(&command_buffer_allocate_info, &this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index]);
        assert(VK_SUCCESS == res_allocate_command_buffers);

        VkCommandBufferBeginInfo command_buffer_begin_info = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            NULL,
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            NULL,
        };
        PT_MAYBE_UNUSED VkResult res_begin_command_buffer = m_device.begin_command_buffer(this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index], &command_buffer_begin_info);
        assert(VK_SUCCESS == res_begin_command_buffer);
    }

    return this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index];
}

void gfx_connection_vk::copy_buffer_to_image(uint32_t streaming_throttling_index, uint32_t streaming_thread_index, VkBuffer src_buffer, VkImage dst_image, VkImageSubresourceRange const *subresource_range, uint32_t region_count, const VkBufferImageCopy *regions)
{
    // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#upload-data-from-the-cpu-to-an-image-sampled-in-a-fragment-shader

    if (this->m_device.has_dedicated_transfer_queue())
    {
        if (this->m_device.queue_transfer_family_index() != this->m_device.queue_graphics_family_index())
        {
            // transfer queue
            {
                VkCommandBuffer command_buffer_transfer = this->streaming_task_get_transfer_command_buffer(streaming_throttling_index, streaming_thread_index);

                // barrier undefine - transfer_dst
                {

                    VkImageMemoryBarrier image_memory_barrier_undefine_to_transfer_dst[1] = {
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

                    m_device.cmd_pipeline_barrier(command_buffer_transfer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_undefine_to_transfer_dst);
                }

                m_device.cmd_copy_buffer_to_image(command_buffer_transfer, src_buffer, dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, region_count, regions);

                // Queue Family Ownership Transfer // Release Operation
                {
                    VkImageMemoryBarrier command_buffer_release_ownership[1] = {
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
                    m_device.cmd_pipeline_barrier(command_buffer_transfer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, command_buffer_release_ownership);
                }
            }

            // graphics queue
            {
                VkCommandBuffer command_buffer_graphics = this->streaming_task_get_graphics_command_buffer(streaming_throttling_index, streaming_thread_index);

                // Queue Family Ownership Transfer // Acquire Operation
                {
                    VkImageMemoryBarrier image_memory_barrier_acquire_ownership[1] = {
                        {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                         NULL,
                         0,
                         0,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                         this->m_device.queue_transfer_family_index(),
                         this->m_device.queue_graphics_family_index(),
                         dst_image,
                         (*subresource_range)}};

                    m_device.cmd_pipeline_barrier(command_buffer_graphics, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_acquire_ownership);
                }
            }
        }
        else
        {
            VkCommandBuffer command_buffer_transfer = this->streaming_task_get_transfer_command_buffer(streaming_throttling_index, streaming_thread_index);

            // barrier undefine - transfer_dst
            {

                VkImageMemoryBarrier image_memory_barrier_undefine_to_transfer_dst[1] = {
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

                m_device.cmd_pipeline_barrier(command_buffer_transfer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_undefine_to_transfer_dst);
            }

            m_device.cmd_copy_buffer_to_image(command_buffer_transfer, src_buffer, dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, region_count, regions);

            // barrier transfer_dst -  shader_read_only
            {
                VkImageMemoryBarrier image_memory_barrier_transfer_dst_to_shader_read_only[1] = {
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

                m_device.cmd_pipeline_barrier(command_buffer_transfer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_transfer_dst_to_shader_read_only);
            }
        }
    }
    else
    {
        VkCommandBuffer command_buffer_graphics = this->streaming_task_get_graphics_command_buffer(streaming_throttling_index, streaming_thread_index);

        // barrier undefine - transfer_dst
        {

            VkImageMemoryBarrier image_memory_barrier_undefine_to_transfer_dst[1] = {
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

            m_device.cmd_pipeline_barrier(command_buffer_graphics, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_undefine_to_transfer_dst);
        }

        m_device.cmd_copy_buffer_to_image(command_buffer_graphics, src_buffer, dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, region_count, regions);

        // barrier transfer_dst -  shader_read_only
        {
            VkImageMemoryBarrier image_memory_barrier_transfer_dst_to_shader_read_only[1] = {
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

            m_device.cmd_pipeline_barrier(command_buffer_graphics, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_transfer_dst_to_shader_read_only);
        }
    }
}

void gfx_connection_vk::reduce_streaming_task()
{
    // the third stage

    // STREAMING_THROTTLING_COUNT - 3
    // 0 - Fence / ResetCommandPool
    // 1 - sync by TBB / Submit - "current" streaming_throttling_index
    // 2 - spawn / BeginCommandBuffer

    // Multi-Threading
    // gfx_texture_vk::read_input_stream
    uint32_t streaming_throttling_index = mcrt_atomic_load(&this->m_streaming_throttling_index);

    // TODO race condition
    // allocate child not atomic
    this->streaming_throttling_index_lock();
    mcrt_atomic_store(&this->m_streaming_throttling_index, ((this->m_streaming_throttling_index + 1U) < STREAMING_THROTTLING_COUNT) ? (this->m_streaming_throttling_index + 1U) : 0U);
    this->streaming_throttling_index_unlock();

    // different master task doesn't share the task_arena
    // we need to share the same the task arena to make sure the "tbb::this_task_arena::current_thread_id" unique
    assert(NULL != mcrt_task_arena_internal_arena(this->m_task_arena));
    assert(mcrt_task_arena_internal_arena(this->m_task_arena) == mcrt_this_task_arena_internal_arena());

    // sync by TBB
    // int ref_count = mcrt_task_ref_count(this->m_streaming_task_root[streaming_throttling_index]);
    mcrt_task_wait_for_all(this->m_streaming_task_root[streaming_throttling_index]);
    mcrt_task_set_ref_count(this->m_streaming_task_root[streaming_throttling_index], 1U);

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    // ensure mutex
    MCRT_ASSERT(0U == mcrt_atomic_load(&this->m_streaming_task_executing_count[streaming_throttling_index]));
    mcrt_atomic_store(&this->m_streaming_task_reducing[streaming_throttling_index], true);
#endif

    // submit
    if (this->m_device.has_dedicated_transfer_queue())
    {
        if (this->m_device.queue_transfer_family_index() != this->m_device.queue_graphics_family_index())
        {
            VkSubmitInfo submit_info_transfer;
            submit_info_transfer.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit_info_transfer.pNext = NULL;
            submit_info_transfer.waitSemaphoreCount = 0U;
            submit_info_transfer.pWaitSemaphores = NULL;
            submit_info_transfer.pWaitDstStageMask = NULL;
            VkCommandBuffer command_buffers_transfer[STREAMING_THREAD_COUNT];
            submit_info_transfer.commandBufferCount = 0U;
            submit_info_transfer.pCommandBuffers = command_buffers_transfer;
            submit_info_transfer.signalSemaphoreCount = 1U;
            submit_info_transfer.pSignalSemaphores = &this->m_streaming_semaphore[streaming_throttling_index];

            VkSubmitInfo submit_info_graphics;
            submit_info_graphics.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit_info_graphics.pNext = NULL;
            submit_info_graphics.waitSemaphoreCount = 1U;
            submit_info_graphics.pWaitSemaphores = &this->m_streaming_semaphore[streaming_throttling_index];
            VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            submit_info_graphics.pWaitDstStageMask = &wait_dst_stage_mask;
            VkCommandBuffer command_buffers_graphics[STREAMING_THREAD_COUNT];
            submit_info_graphics.commandBufferCount = 0U;
            submit_info_graphics.pCommandBuffers = command_buffers_graphics;
            submit_info_graphics.signalSemaphoreCount = 0U;
            submit_info_graphics.pSignalSemaphores = NULL;

            for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
            {
                if (VK_NULL_HANDLE != this->m_streaming_transfer_command_buffer[streaming_throttling_index][streaming_thread_index])
                {
                    this->m_device.end_command_buffer(this->m_streaming_transfer_command_buffer[streaming_throttling_index][streaming_thread_index]);

                    command_buffers_transfer[submit_info_transfer.commandBufferCount] = this->m_streaming_transfer_command_buffer[streaming_throttling_index][streaming_thread_index];
                    ++submit_info_transfer.commandBufferCount;

                    this->m_streaming_transfer_command_buffer[streaming_throttling_index][streaming_thread_index] = VK_NULL_HANDLE;
                }

                if (VK_NULL_HANDLE != this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index])
                {
                    this->m_device.end_command_buffer(this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index]);

                    command_buffers_graphics[submit_info_graphics.commandBufferCount] = this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index];
                    ++submit_info_graphics.commandBufferCount;

                    this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index] = VK_NULL_HANDLE;
                }
            }
            assert(submit_info_transfer.commandBufferCount == submit_info_graphics.commandBufferCount);

            // VUID-VkSubmitInfo-pCommandBuffers-parameter
            // "commandBufferCount 0" is legal
            this->m_device.queue_submit(this->m_device.queue_transfer(), 1, &submit_info_transfer, VK_NULL_HANDLE);

            // acquire_ownership
            this->m_device.queue_submit(this->m_device.queue_graphics(), 1, &submit_info_graphics, this->m_streaming_fence[streaming_throttling_index]);
        }
        else
        {
            VkSubmitInfo submit_info_transfer;
            submit_info_transfer.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit_info_transfer.pNext = NULL;
            submit_info_transfer.waitSemaphoreCount = 0U;
            submit_info_transfer.pWaitSemaphores = NULL;
            submit_info_transfer.pWaitDstStageMask = NULL;
            VkCommandBuffer command_buffers_transfer[STREAMING_THREAD_COUNT];
            submit_info_transfer.commandBufferCount = 0U;
            submit_info_transfer.pCommandBuffers = command_buffers_transfer;
            submit_info_transfer.signalSemaphoreCount = 1U;
            submit_info_transfer.pSignalSemaphores = NULL;

            for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
            {
                if (VK_NULL_HANDLE != this->m_streaming_transfer_command_buffer[streaming_throttling_index][streaming_thread_index])
                {
                    this->m_device.end_command_buffer(this->m_streaming_transfer_command_buffer[streaming_throttling_index][streaming_thread_index]);

                    command_buffers_transfer[submit_info_transfer.commandBufferCount] = this->m_streaming_transfer_command_buffer[streaming_throttling_index][streaming_thread_index];
                    ++submit_info_transfer.commandBufferCount;

                    this->m_streaming_transfer_command_buffer[streaming_throttling_index][streaming_thread_index] = VK_NULL_HANDLE;
                }
            }

            this->m_device.queue_submit(this->m_device.queue_transfer(), 1, &submit_info_transfer, this->m_streaming_fence[streaming_throttling_index]);
        }
    }
    else
    {
        VkSubmitInfo submit_info_graphics;
        submit_info_graphics.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info_graphics.pNext = NULL;
        submit_info_graphics.waitSemaphoreCount = 0U;
        submit_info_graphics.pWaitSemaphores = NULL;
        submit_info_graphics.pWaitDstStageMask = NULL;
        VkCommandBuffer command_buffers_graphics[STREAMING_THREAD_COUNT];
        submit_info_graphics.commandBufferCount = 0U;
        submit_info_graphics.pCommandBuffers = command_buffers_graphics;
        submit_info_graphics.signalSemaphoreCount = 0U;
        submit_info_graphics.pSignalSemaphores = NULL;

        for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
        {
            if (VK_NULL_HANDLE != this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index])
            {
                this->m_device.end_command_buffer(this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index]);

                command_buffers_graphics[submit_info_graphics.commandBufferCount] = this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index];
                ++submit_info_graphics.commandBufferCount;

                this->m_streaming_graphics_command_buffer[streaming_throttling_index][streaming_thread_index] = VK_NULL_HANDLE;
            }
        }

        this->m_device.queue_submit(this->m_device.queue_graphics(), 1, &submit_info_graphics, this->m_streaming_fence[streaming_throttling_index]);
    }

    // respawn task
    {
        // list
        {
            uint32_t streaming_task_respawn_linear_list_count = this->m_streaming_task_respawn_linear_list_count[streaming_throttling_index];
            for (uint32_t streaming_task_respawn_index = 0U; streaming_task_respawn_index < streaming_task_respawn_linear_list_count; ++streaming_task_respawn_index)
            {
                //TODO different master task doesn't share the task_arena
                mcrt_task_spawn(this->m_streaming_task_respawn_linear_list[streaming_throttling_index][streaming_task_respawn_index]);
                this->m_streaming_task_respawn_linear_list[streaming_throttling_index][streaming_task_respawn_index] = NULL;
            }
            this->m_streaming_task_respawn_linear_list_count[streaming_throttling_index] = 0U;
#if defined(PT_GFX_PROFILE) && PT_GFX_PROFILE
            if (streaming_task_respawn_linear_list_count > 0U)
            {
                mcrt_log_print("index %i: streaming_task_respawn_linear_list_count %i \n", int(streaming_throttling_index), int(streaming_task_respawn_linear_list_count));
            }
#endif
        }

        // link list
        {
            uint32_t streaming_task_respawn_link_list_count = 0U;
            struct streaming_task_respawn_task_respawn_link_list *streaming_task_respawn_link_list_node = this->m_streaming_task_respawn_link_list_head[streaming_throttling_index];
            while (streaming_task_respawn_link_list_node != NULL)
            {
                mcrt_task_spawn(streaming_task_respawn_link_list_node->m_task);

                struct streaming_task_respawn_task_respawn_link_list *streaming_task_respawn_link_list_next = streaming_task_respawn_link_list_node->m_next;
                mcrt_free(streaming_task_respawn_link_list_node);

                streaming_task_respawn_link_list_node = streaming_task_respawn_link_list_next;

                ++streaming_task_respawn_link_list_count;
            }
            this->m_streaming_task_respawn_link_list_head[streaming_throttling_index] = NULL;
#if defined(PT_GFX_PROFILE) && PT_GFX_PROFILE
            if (streaming_task_respawn_link_list_count > 0U)
            {
                mcrt_log_print("index %i: streaming_task_respawn_link_list_count %i \n", int(streaming_throttling_index), int(streaming_task_respawn_link_list_count));
            }
#endif
        }
    }

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    // ensure mutex
    MCRT_ASSERT(0U == mcrt_atomic_load(&this->m_streaming_task_executing_count[streaming_throttling_index]));
    mcrt_atomic_store(&this->m_streaming_task_reducing[streaming_throttling_index], false);
#endif

    // wait the Fence
    // Submit ensures EndCommandBuffer
    // Fence ensures "Completed by GPU"
    // safe to ResetCommandPool
    streaming_throttling_index = (streaming_throttling_index > 0U) ? (streaming_throttling_index - 1U) : (STREAMING_THROTTLING_COUNT - 1U);
    this->m_device.wait_for_fences(1U, &this->m_streaming_fence[streaming_throttling_index], VK_TRUE, UINT64_MAX);
    this->m_device.reset_fences(1U, &this->m_streaming_fence[streaming_throttling_index]);

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    // ensure mutex
    MCRT_ASSERT(0U == mcrt_atomic_load(&this->m_streaming_task_executing_count[streaming_throttling_index]));
    mcrt_atomic_store(&this->m_streaming_task_reducing[streaming_throttling_index], true);
#endif

    // free memory
    {
        uint64_t transfer_src_buffer_begin = this->m_transfer_src_buffer_begin[streaming_throttling_index];
        uint64_t transfer_src_buffer_end = this->m_transfer_src_buffer_end[streaming_throttling_index];
        this->m_transfer_src_buffer_end[streaming_throttling_index] = transfer_src_buffer_begin;
#if defined(PT_GFX_PROFILE) && PT_GFX_PROFILE
        uint64_t transfer_src_buffer_size = this->m_transfer_src_buffer_size[streaming_throttling_index];
        uint32_t transfer_src_buffer_used = (transfer_src_buffer_end - transfer_src_buffer_begin);
        if (transfer_src_buffer_used > 0U)
        {
            mcrt_log_print("index %i: transfer_src_buffer unused memory %f mb\n", int(streaming_throttling_index), float(transfer_src_buffer_size - transfer_src_buffer_used) / 1024.0f / 1024.0f);
        }
#endif
    }

    // TODO limit the thread arena number
    if (this->m_device.has_dedicated_transfer_queue())
    {
        if (this->m_device.queue_transfer_family_index() != this->m_device.queue_graphics_family_index())
        {
            for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
            {
                this->m_device.reset_command_pool(this->m_streaming_transfer_command_pool[streaming_throttling_index][streaming_thread_index], 0U);
                this->m_device.reset_command_pool(this->m_streaming_graphics_command_pool[streaming_throttling_index][streaming_thread_index], 0U);
            }
        }
        else
        {
            for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
            {
                this->m_device.reset_command_pool(this->m_streaming_transfer_command_pool[streaming_throttling_index][streaming_thread_index], 0U);
            }
        }
    }
    else
    {
        for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
        {

            this->m_device.reset_command_pool(this->m_streaming_graphics_command_pool[streaming_throttling_index][streaming_thread_index], 0U);
        }
    }

    // streaming_object // the third stage
    {
        // link
        {
            uint32_t streaming_object_linear_list_count = this->m_streaming_object_linear_list_count[streaming_throttling_index];
            for (uint32_t streaming_object_index = 0U; streaming_object_index < streaming_object_linear_list_count; ++streaming_object_index)
            {
                this->m_streaming_object_linear_list[streaming_throttling_index][streaming_object_index]->streaming_done(this);
                this->m_streaming_object_linear_list[streaming_throttling_index][streaming_object_index] = NULL;
            }
            this->m_streaming_object_linear_list_count[streaming_throttling_index] = 0U;
#if defined(PT_GFX_PROFILE) && PT_GFX_PROFILE
            if (streaming_object_linear_list_count > 0U)
            {
                mcrt_log_print("index %i: streaming_object_linear_list_count %i \n", int(streaming_throttling_index), int(streaming_object_linear_list_count));
            }
#endif
        }

        // link list
        {
            uint32_t streaming_object_link_list_count = 0U;
            struct streaming_object_link_list *streaming_object_link_list_node = this->m_streaming_object_link_list_head[streaming_throttling_index];
            while (streaming_object_link_list_node != NULL)
            {
                streaming_object_link_list_node->m_streaming_object->streaming_done(this);

                struct streaming_object_link_list *streaming_object_link_list_next = streaming_object_link_list_node->m_next;
                mcrt_free(streaming_object_link_list_node);

                streaming_object_link_list_node = streaming_object_link_list_next;

                ++streaming_object_link_list_count;
            }
            this->m_streaming_object_link_list_head[streaming_throttling_index] = NULL;
#if defined(PT_GFX_PROFILE) && PT_GFX_PROFILE
            if (streaming_object_link_list_count > 0U)
            {
                mcrt_log_print("index %i: streaming_object_link_list_count %i \n", int(streaming_throttling_index), int(streaming_object_link_list_count));
            }
#endif
        }
    }

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    // ensure mutex
    MCRT_ASSERT(0U == mcrt_atomic_load(&this->m_streaming_task_executing_count[streaming_throttling_index]));
    mcrt_atomic_store(&this->m_streaming_task_reducing[streaming_throttling_index], false);
#endif
    return;
}

inline bool gfx_connection_vk::init_swapchain_and_renderpass_and_framebuffer(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window)
{
    {
        PT_MAYBE_UNUSED VkResult res_platform_create_surface = this->m_device.platform_create_surface(&this->m_surface, wsi_connection, wsi_visual, wsi_window);
        assert(VK_SUCCESS == res_platform_create_surface);
    }
#ifndef NDEBUG
    {
        VkBool32 supported;
        VkResult res_get_physical_device_surface_support = this->m_device.get_physical_device_surface_support(this->m_device.queue_graphics_family_index(), this->m_surface, &supported);
        assert(VK_SUCCESS == res_get_physical_device_surface_support);
        assert(VK_TRUE == supported);
    }
#endif
    this->m_framebuffer_width = 1280U;
    this->m_framebuffer_height = 720U;
    this->m_depth_image = VK_NULL_HANDLE;
    this->m_swapchain_image_count = 0U;
    this->m_swapchain = VK_NULL_HANDLE;

    return (this->update_swapchain() && this->init_renderpass() && this->update_framebuffer());
}

inline bool gfx_connection_vk::update_swapchain()
{
    //swapchain
    {
        VkSwapchainKHR old_swapchain = this->m_swapchain;

        VkSwapchainCreateInfoKHR swapchain_create_info;
        swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_create_info.pNext = NULL;
        swapchain_create_info.flags = 0U;
        swapchain_create_info.surface = this->m_surface;
        swapchain_create_info.minImageCount = FRAME_THROTTLING_COUNT;
        // imageFormat // imageColorSpace
        {
            VkSurfaceFormatKHR *surface_formats;
            uint32_t surface_formats_count;

            class internal_surface_formats_guard
            {
                VkSurfaceFormatKHR **const m_surface_formats;
                uint32_t *const m_surface_formats_count;

            public:
                inline internal_surface_formats_guard(VkSurfaceFormatKHR **surface_formats, uint32_t *surface_formats_count, VkSurfaceKHR surface, class gfx_device_vk *gfx_device)
                    : m_surface_formats(surface_formats), m_surface_formats_count(surface_formats_count)
                {
                    uint32_t surface_formats_count_before;
                    PT_MAYBE_UNUSED VkResult res_get_physical_device_surface_formats_before = gfx_device->get_physical_device_surface_formats(surface, &surface_formats_count_before, NULL);
                    assert(VK_SUCCESS == res_get_physical_device_surface_formats_before);

                    (*m_surface_formats_count) = surface_formats_count_before;
                    (*m_surface_formats) = static_cast<VkSurfaceFormatKHR *>(mcrt_malloc(sizeof(VkSurfaceFormatKHR) * (*m_surface_formats_count)));
                    PT_MAYBE_UNUSED VkResult res_get_physical_device_surface_formats = gfx_device->get_physical_device_surface_formats(surface, m_surface_formats_count, (*m_surface_formats));
                    assert(surface_formats_count_before == (*m_surface_formats_count));
                    assert(VK_SUCCESS == res_get_physical_device_surface_formats);
                }
                inline ~internal_surface_formats_guard()
                {
                    mcrt_free((*m_surface_formats));
                }
            } instance_internal_surface_formats_guard(&surface_formats, &surface_formats_count, this->m_surface, &this->m_device);

            assert(surface_formats_count >= 1U);
            this->m_swapchain_image_format = surface_formats[0].format;
            if (this->m_swapchain_image_format == VK_FORMAT_UNDEFINED)
            {
                this->m_swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;
            }
            swapchain_create_info.imageFormat = this->m_swapchain_image_format;
            swapchain_create_info.imageColorSpace = surface_formats[0].colorSpace;
        }
        // imageExtent //preTransform //compositeAlpha // Usage
        {
            VkSurfaceCapabilitiesKHR surface_capabilities;
            PT_MAYBE_UNUSED VkResult res_get_physical_device_surface_capablilities = this->m_device.get_physical_device_surface_capablilities(this->m_surface, &surface_capabilities);
            assert(VK_SUCCESS == res_get_physical_device_surface_capablilities);
            if (swapchain_create_info.imageExtent.width != 0XFFFFFFFFU)
            {
                this->m_framebuffer_width = surface_capabilities.currentExtent.width;
            }
            if (this->m_framebuffer_width < surface_capabilities.minImageExtent.width)
            {
                this->m_framebuffer_width = surface_capabilities.minImageExtent.width;
            }
            if (this->m_framebuffer_width > surface_capabilities.maxImageExtent.width)
            {
                this->m_framebuffer_width = surface_capabilities.maxImageExtent.width;
            }
            swapchain_create_info.imageExtent.width = this->m_framebuffer_width;
            if (swapchain_create_info.imageExtent.height == 0XFFFFFFFFU)
            {
                this->m_framebuffer_height = swapchain_create_info.imageExtent.height;
            }
            if (this->m_framebuffer_height < surface_capabilities.minImageExtent.height)
            {
                this->m_framebuffer_height = surface_capabilities.minImageExtent.height;
            }
            if (this->m_framebuffer_height > surface_capabilities.maxImageExtent.height)
            {
                this->m_framebuffer_height = surface_capabilities.maxImageExtent.height;
            }
            swapchain_create_info.imageExtent.height = this->m_framebuffer_height;

            swapchain_create_info.preTransform = surface_capabilities.currentTransform;

            VkCompositeAlphaFlagBitsKHR composite_alphas[] = {
                VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
                VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
                VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR};

            for (VkCompositeAlphaFlagBitsKHR composite_alpha : composite_alphas)
            {
                if (0 != (composite_alpha & surface_capabilities.supportedCompositeAlpha))
                {
                    swapchain_create_info.compositeAlpha = composite_alpha;
                    break;
                }
            }

            assert(0U != (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT & surface_capabilities.supportedUsageFlags));
            swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        // presentMode
        {
            VkPresentModeKHR *present_modes;
            uint32_t present_modes_count;

            class internal_present_modes_guard
            {
                VkPresentModeKHR **const m_present_modes;
                uint32_t *const m_present_modes_count;

            public:
                inline internal_present_modes_guard(VkPresentModeKHR **present_modes, uint32_t *present_modes_count, VkSurfaceKHR surface, class gfx_device_vk *gfx_device)
                    : m_present_modes(present_modes), m_present_modes_count(present_modes_count)
                {
                    uint32_t present_modes_count_before;
                    PT_MAYBE_UNUSED VkResult res_get_physical_device_surface_present_modes_before = gfx_device->get_physical_device_surface_present_modes(surface, &present_modes_count_before, NULL);
                    assert(VK_SUCCESS == res_get_physical_device_surface_present_modes_before);

                    (*m_present_modes_count) = present_modes_count_before;
                    (*m_present_modes) = static_cast<VkPresentModeKHR *>(mcrt_malloc(sizeof(VkPresentModeKHR) * (*m_present_modes_count)));
                    PT_MAYBE_UNUSED VkResult res_get_physical_device_surface_present_modes = gfx_device->get_physical_device_surface_present_modes(surface, m_present_modes_count, (*m_present_modes));
                    assert(present_modes_count_before == (*m_present_modes_count));
                    assert(VK_SUCCESS == res_get_physical_device_surface_present_modes);
                }
                inline ~internal_present_modes_guard()
                {
                    mcrt_free((*m_present_modes));
                }
            } instance_internal_present_modes_guard(&present_modes, &present_modes_count, this->m_surface, &this->m_device);

            swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
            for (uint32_t present_mode_index = 0U; present_mode_index < present_modes_count; ++present_mode_index)
            {
                if (VK_PRESENT_MODE_MAILBOX_KHR == present_modes[present_mode_index])
                {
                    swapchain_create_info.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                    break;
                }
            }
        }
        swapchain_create_info.imageArrayLayers = 1U;
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0U;
        swapchain_create_info.pQueueFamilyIndices = NULL;
        swapchain_create_info.clipped = VK_FALSE;
        swapchain_create_info.oldSwapchain = old_swapchain;

        PT_MAYBE_UNUSED VkResult res_create_swap_chain = this->m_device.create_swapchain(&swapchain_create_info, &this->m_swapchain);
        assert(VK_SUCCESS == res_create_swap_chain);
    }

    return true;
}

inline bool gfx_connection_vk::init_renderpass()
{
    VkAttachmentDescription attachments[2] = {
        {0U, this->m_swapchain_image_format, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR},
        {0U, this->m_malloc.format_depth(), VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}};

    VkAttachmentReference subpass_0_color_attachments[1] = {
        {0U, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}};

    VkAttachmentReference subpass_0_depth_stencil_attachment = {1U, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpasses[1] = {
        {0U, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, NULL, 1U, subpass_0_color_attachments, NULL, &subpass_0_depth_stencil_attachment, 0U, NULL}};

    VkRenderPassCreateInfo render_pass_create_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, NULL, 0U, 2U, attachments, 1U, subpasses, 0U, NULL};

    PT_MAYBE_UNUSED VkResult res_create_render_pass = this->m_device.create_render_pass(&render_pass_create_info, &this->m_render_pass);
    assert(VK_SUCCESS == res_create_render_pass);

    return true;
}

inline bool gfx_connection_vk::update_framebuffer()
{
    // wait fence

    // swapchain images and views
    {
        if (0U != this->m_swapchain_image_count)
        {
            assert(NULL != this->m_swapchain_image_views);
            assert(NULL != this->m_swapchain_images);
            assert(NULL != this->m_framebuffers);
            mcrt_free(this->m_swapchain_image_views);
            mcrt_free(this->m_swapchain_images);
            mcrt_free(this->m_framebuffers);
        }

        uint32_t swapchain_image_count_before;
        PT_MAYBE_UNUSED VkResult res_get_swapchain_images_before = this->m_device.get_swapchain_images(this->m_swapchain, &swapchain_image_count_before, NULL);
        assert(VK_SUCCESS == res_get_swapchain_images_before);

        this->m_swapchain_image_count = swapchain_image_count_before;
        this->m_swapchain_images = static_cast<VkImage *>(mcrt_malloc(sizeof(VkImage) * this->m_swapchain_image_count));
        PT_MAYBE_UNUSED VkResult res_get_swapchain_images = this->m_device.get_swapchain_images(this->m_swapchain, &this->m_swapchain_image_count, this->m_swapchain_images);
        assert(swapchain_image_count_before == this->m_swapchain_image_count);
        assert(VK_SUCCESS == res_get_swapchain_images);

        this->m_swapchain_image_views = static_cast<VkImageView *>(mcrt_malloc(sizeof(VkImageView) * this->m_swapchain_image_count));

        for (uint32_t swapchain_image_index = 0U; swapchain_image_index < this->m_swapchain_image_count; ++swapchain_image_index)
        {
            VkImageViewCreateInfo image_view_create_info;
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.pNext = NULL;
            image_view_create_info.flags = 0U;
            image_view_create_info.image = this->m_swapchain_images[swapchain_image_index];
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = this->m_swapchain_image_format;
            image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel = 0U;
            image_view_create_info.subresourceRange.levelCount = 1U;
            image_view_create_info.subresourceRange.baseArrayLayer = 0U;
            image_view_create_info.subresourceRange.layerCount = 1U;

            PT_MAYBE_UNUSED VkResult res_create_image_view = this->m_device.create_image_view(&image_view_create_info, &this->m_swapchain_image_views[swapchain_image_index]);
            assert(VK_SUCCESS == res_create_image_view);
        }
    }

    // depth image and view
    {
        if (NULL != this->m_depth_image)
        {
            assert(NULL != this->m_depth_image_view);
            assert(NULL != this->m_depth_device_memory);
            this->m_device.destroy_image_view(this->m_depth_image_view);
            this->m_device.destroy_image(this->m_depth_image);
            this->m_device.free_memory(this->m_depth_device_memory);
            mcrt_free(this->m_swapchain_image_views);
            mcrt_free(this->m_swapchain_images);
        }

        // depth
        {
            struct VkImageCreateInfo image_create_info;
            image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_create_info.pNext = NULL;
            image_create_info.flags = 0U;
            image_create_info.imageType = VK_IMAGE_TYPE_2D;
            image_create_info.format = this->m_malloc.format_depth();
            image_create_info.extent.width = this->m_framebuffer_width;
            image_create_info.extent.height = this->m_framebuffer_height;
            image_create_info.extent.depth = 1U;
            image_create_info.mipLevels = 1U;
            image_create_info.arrayLayers = 1U;
            image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
            image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            image_create_info.queueFamilyIndexCount = 0U;
            image_create_info.pQueueFamilyIndices = NULL;
            image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            PT_MAYBE_UNUSED VkResult res_create_image = this->m_device.create_image(&image_create_info, &this->m_depth_image);
            assert(VK_SUCCESS == res_create_image);
        }

        // depth memory
        {
            struct VkMemoryRequirements memory_requirements;
            this->m_device.get_image_memory_requirements(this->m_depth_image, &memory_requirements);
            assert(0U != (memory_requirements.memoryTypeBits & (1U << this->m_malloc.depth_stencil_attachment_and_transient_attachment_memory_index())));

            VkMemoryAllocateInfo memory_allocate_info;
            memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memory_allocate_info.pNext = NULL;
            memory_allocate_info.allocationSize = memory_requirements.size;
            memory_allocate_info.memoryTypeIndex = this->m_malloc.depth_stencil_attachment_and_transient_attachment_memory_index();
            PT_MAYBE_UNUSED VkResult res_allocate_memory = this->m_device.allocate_memory(&memory_allocate_info, &this->m_depth_device_memory);
            assert(VK_SUCCESS == res_allocate_memory);

            PT_MAYBE_UNUSED VkResult res_bind_buffer_memory = this->m_device.bind_image_memory(this->m_depth_image, this->m_depth_device_memory, 0U);
            assert(VK_SUCCESS == res_bind_buffer_memory);
        }

        // depth view
        {
            VkImageViewCreateInfo image_view_create_info;
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.pNext = NULL;
            image_view_create_info.flags = 0U;
            image_view_create_info.image = this->m_depth_image;
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = this->m_malloc.format_depth();
            image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel = 0U;
            image_view_create_info.subresourceRange.levelCount = 1U;
            image_view_create_info.subresourceRange.baseArrayLayer = 0U;
            image_view_create_info.subresourceRange.layerCount = 1U;

            PT_MAYBE_UNUSED VkResult res_create_image_view = this->m_device.create_image_view(&image_view_create_info, &this->m_depth_image_view);
            assert(VK_SUCCESS == res_create_image_view);
        }
    }

    // frame buffer
    {

        this->m_framebuffers = static_cast<VkFramebuffer *>(mcrt_malloc(sizeof(VkFramebuffer) * this->m_swapchain_image_count));

        for (uint32_t swapchain_image_index = 0U; swapchain_image_index < this->m_swapchain_image_count; ++swapchain_image_index)
        {
            VkImageView attachments[2] = {this->m_swapchain_image_views[swapchain_image_index], this->m_depth_image_view};

            VkFramebufferCreateInfo framebuffer_create_info;
            framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_create_info.pNext = NULL;
            framebuffer_create_info.flags = 0U;
            framebuffer_create_info.renderPass = this->m_render_pass;
            framebuffer_create_info.attachmentCount = 2U;
            framebuffer_create_info.pAttachments = attachments;
            framebuffer_create_info.width = this->m_framebuffer_width;
            framebuffer_create_info.height = this->m_framebuffer_height;
            framebuffer_create_info.layers = 1U;

            PT_MAYBE_UNUSED VkResult res_create_framebuffer = this->m_device.create_framebuffer(&framebuffer_create_info, &this->m_framebuffers[swapchain_image_index]);
            assert(VK_SUCCESS == res_create_framebuffer);
        }
    }

    return true;
}

inline bool gfx_connection_vk::init_descriptor_and_pipeline_layout()
{
    // \[Kubisch 2016\] [Christoph Kubisch. "Vulkan Shader Resource Binding." NVIDIA GameWorks Blog 2016.](https://developer.nvidia.com/vulkan-shader-resource-binding)

    // push constant // min 128 byte
    // vp 64 byte // each view
    // m 64 byte // each object

    // the related "descriptor_set" is owned by this "gfx_connection"
    // uniform buffer // the joint matrix
    {
        VkDescriptorSetLayoutBinding descriptor_set_layout_each_object_immutable_bindings[1];
        descriptor_set_layout_each_object_immutable_bindings[0].binding = 0;
        descriptor_set_layout_each_object_immutable_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        descriptor_set_layout_each_object_immutable_bindings[0].descriptorCount = 1U;
        descriptor_set_layout_each_object_immutable_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        descriptor_set_layout_each_object_immutable_bindings[0].pImmutableSamplers = NULL;

        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_each_object_immutable_create_info;
        descriptor_set_layout_each_object_immutable_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_set_layout_each_object_immutable_create_info.pNext = NULL;
        descriptor_set_layout_each_object_immutable_create_info.flags = 0U;
        descriptor_set_layout_each_object_immutable_create_info.bindingCount = 1U;
        descriptor_set_layout_each_object_immutable_create_info.pBindings = descriptor_set_layout_each_object_immutable_bindings;

        VkResult res_create_descriptor_set_layout = this->m_device.create_descriptor_set_layout(&descriptor_set_layout_each_object_immutable_create_info, &this->m_descriptor_set_layout_each_object_immutable);
        assert(VK_SUCCESS == res_create_descriptor_set_layout);
    }

    // the related "descriptor_set" is owned by the object
    // material
    {
        VkDescriptorSetLayoutBinding descriptor_set_layout_each_object_dynamic_bindings[1];
        descriptor_set_layout_each_object_dynamic_bindings[0].binding = 0;
        descriptor_set_layout_each_object_dynamic_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_set_layout_each_object_dynamic_bindings[0].descriptorCount = 1U;
        descriptor_set_layout_each_object_dynamic_bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        descriptor_set_layout_each_object_dynamic_bindings[0].pImmutableSamplers = NULL;

        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_each_object_dynamic_create_info;
        descriptor_set_layout_each_object_dynamic_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_set_layout_each_object_dynamic_create_info.pNext = NULL;
        descriptor_set_layout_each_object_dynamic_create_info.flags = 0U;
        descriptor_set_layout_each_object_dynamic_create_info.bindingCount = 1U;
        descriptor_set_layout_each_object_dynamic_create_info.pBindings = descriptor_set_layout_each_object_dynamic_bindings;

        VkResult res_create_descriptor_set_layout = this->m_device.create_descriptor_set_layout(&descriptor_set_layout_each_object_dynamic_create_info, &this->m_descriptor_set_layout_each_object_dynamic);
        assert(VK_SUCCESS == res_create_descriptor_set_layout);
    }

    // pipeline layout = descriptor layout + push constant
    {
        VkDescriptorSetLayout set_layouts[2] = {this->m_descriptor_set_layout_each_object_immutable, this->m_descriptor_set_layout_each_object_dynamic};

        VkPushConstantRange push_constant_ranges[1];
        push_constant_ranges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        push_constant_ranges[0].offset = 0U;
        push_constant_ranges[0].size = sizeof(this->m_mat_vp) + sizeof(this->m_mat_m);
        assert(push_constant_ranges[0].size <= m_limit_min_max_push_constants_size);

        VkPipelineLayoutCreateInfo pipeline_layout_create_info;
        pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_create_info.pNext = NULL;
        pipeline_layout_create_info.flags = 0U;
        pipeline_layout_create_info.setLayoutCount = 2U;
        pipeline_layout_create_info.pSetLayouts = set_layouts;
        pipeline_layout_create_info.pushConstantRangeCount = 1U;
        pipeline_layout_create_info.pPushConstantRanges = push_constant_ranges;

        VkResult res_create_pipeline_layout = this->m_device.create_pipeline_layout(&pipeline_layout_create_info, &this->m_pipeline_layout);
        assert(VK_SUCCESS == res_create_pipeline_layout);
    }

    ///math_simd_mat m = math_mat_perspective_fov_rh(1.57f, 1.0f, 1.0f, 250.0f);
    ///math_store_alignas16_mat4x4(&this->m_mat_vp, m);

    return true;
}

void gfx_connection_vk::destroy()
{
    mcrt_task_arena_terminate(this->m_task_arena);

    this->m_malloc.destroy();

    this->m_device.destroy();

    this->~gfx_connection_vk();
    mcrt_free(this);
}

inline gfx_connection_vk::~gfx_connection_vk()
{
}

void gfx_connection_vk::wsi_on_resized(wsi_window_ref wsi_window, float width, float height)
{
}

void gfx_connection_vk::wsi_on_redraw_needed_acquire(wsi_window_ref wsi_window, float width, float height)
{
    this->reduce_streaming_task();
}

void gfx_connection_vk::wsi_on_redraw_needed_release()
{
}

class gfx_texture_common *gfx_connection_vk::create_texture()
{
    gfx_texture_vk *texture = new (mcrt_aligned_malloc(sizeof(gfx_texture_vk), alignof(gfx_texture_vk))) gfx_texture_vk();
    return texture;
}
