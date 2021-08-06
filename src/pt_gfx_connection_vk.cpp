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
#include "pt_gfx_connection_vk.h"
#include "pt_gfx_texture_vk.h"
#include <new>
#include <pt_mcrt_log.h>

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
    this->m_streaming_throttling_index = 0U;
    this->m_transfer_src_buffer_begin_and_end = 0U;
    for (uint32_t streaming_throttling_index = 0U; streaming_throttling_index < STREAMING_THROTTLING_COUNT; ++streaming_throttling_index)
    {
        this->m_streaming_throttling_count[streaming_throttling_index] = 0U;

        this->m_transfer_src_buffer_streaming_task_max_end[streaming_throttling_index] = 0U;

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
                    VkResult res_create_command_pool = m_device.create_command_Pool(&command_pool_create_info, &this->m_streaming_command_pool[streaming_throttling_index][streaming_thread_index]);
                    assert(VK_SUCCESS == res_create_command_pool);
                }
                //acquire ownership
                {
                    VkCommandPoolCreateInfo command_pool_create_info = {
                        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                        NULL,
                        0U,
                        m_device.queue_graphics_family_index()};
                    VkResult res_create_command_pool = m_device.create_command_Pool(&command_pool_create_info, &this->m_streaming_acquire_ownership_command_pool[streaming_throttling_index][streaming_thread_index]);
                    assert(VK_SUCCESS == res_create_command_pool);
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
                VkResult res_create_command_pool = m_device.create_command_Pool(&command_pool_create_info, &this->m_streaming_command_pool[streaming_throttling_index][streaming_thread_index]);
                assert(VK_SUCCESS == res_create_command_pool);
            }

            this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index] = VK_NULL_HANDLE;
        }

        if (m_device.has_dedicated_transfer_queue())
        {
            VkSemaphoreCreateInfo semaphore_create_info;
            semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphore_create_info.pNext = NULL;
            semaphore_create_info.flags = 0U;
            VkResult res_create_semaphore = this->m_device.create_semaphore(&semaphore_create_info, &this->m_streaming_semaphore[streaming_throttling_index]);
            assert(VK_SUCCESS == res_create_semaphore);
        }

        {
            VkFenceCreateInfo fence_create_info;
            fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_create_info.pNext = NULL;
            fence_create_info.flags = ((STREAMING_THROTTLING_COUNT - 1U) != streaming_throttling_index) ? 0U : VK_FENCE_CREATE_SIGNALED_BIT;
            VkResult res_create_fence = this->m_device.create_fence(&fence_create_info, &this->m_streaming_fence[streaming_throttling_index]);
            assert(VK_SUCCESS == res_create_fence);
        }

        this->m_streaming_task_root[streaming_throttling_index] = mcrt_task_allocate_root(NULL);
        mcrt_task_set_ref_count(this->m_streaming_task_root[streaming_throttling_index], 1U);

        this->m_streaming_object_count[streaming_throttling_index] = 0U;
    }

    return true;
}

bool gfx_connection_vk::streaming_throttling(uint32_t streaming_throttling_index)
{
    // TBB
    // tally_completion_of_predecessor
    // SchedulerTraits::has_slow_atomic

    if (STREAMING_THROTTLING_THRESHOLD <= mcrt_atomic_load(&this->m_streaming_throttling_count[streaming_throttling_index]))
    {
        return false;
    }
    else
    {
        uint32_t streaming_throttling_count = mcrt_atomic_inc_u32(&this->m_streaming_throttling_count[streaming_throttling_index]) - 1U;
        if (PT_LIKELY(streaming_throttling_count < STREAMING_THROTTLING_THRESHOLD))
        {
            return true;
        }
        else
        {
            mcrt_atomic_dec_u32(&this->m_streaming_throttling_count[streaming_throttling_index]);
            assert(0);
            return false;
        }
    }
}

bool gfx_connection_vk::streaming_object_list_push(uint32_t streaming_throttling_index, class gfx_streaming_object *streaming_object)
{
    uint32_t streaming_object_index = mcrt_atomic_inc_u32(&this->m_streaming_object_count[streaming_throttling_index]) - 1U;
    if (PT_LIKELY(streaming_object_index < STREAMING_OBJECT_COUNT))
    {
        this->m_streaming_object_list[streaming_throttling_index][streaming_object_index] = streaming_object;
        return true;
    }
    else
    {
        mcrt_atomic_dec_u32(&this->m_streaming_object_count[streaming_throttling_index]);
        assert(0);
        return false;
    }
}

inline VkCommandBuffer gfx_connection_vk::streaming_task_get_command_buffer(uint32_t streaming_throttling_index)
{
    uint32_t streaming_thread_index = mcrt_task_arena_current_thread_index();
    assert(uint32_t(-1) != streaming_thread_index);
    assert(streaming_thread_index < STREAMING_THREAD_COUNT);
    assert(mcrt_task_arena_max_concurrency() < STREAMING_THREAD_COUNT);

    if (PT_UNLIKELY(VK_NULL_HANDLE == this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index]))
    {
        VkCommandBufferAllocateInfo command_buffer_allocate_info;
        command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.pNext = NULL;
        command_buffer_allocate_info.commandPool = this->m_streaming_command_pool[streaming_throttling_index][streaming_thread_index];
        command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_allocate_info.commandBufferCount = 1U;

        VkResult res_allocate_command_buffers = m_device.allocate_command_buffers(&command_buffer_allocate_info, &this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index]);
        assert(VK_SUCCESS == res_allocate_command_buffers);

        VkCommandBufferBeginInfo command_buffer_begin_info = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            NULL,
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            NULL,
        };
        VkResult res_begin_command_buffer = m_device.begin_command_buffer(this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index], &command_buffer_begin_info);
        assert(VK_SUCCESS == res_begin_command_buffer);
    }

    return this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index];
}

inline VkCommandBuffer gfx_connection_vk::streaming_task_get_acquire_ownership_command_buffer(uint32_t streaming_throttling_index)
{
    assert(this->m_device.has_dedicated_transfer_queue());

    uint32_t streaming_thread_index = mcrt_task_arena_current_thread_index();
    assert(uint32_t(-1) != streaming_thread_index);
    assert(streaming_thread_index < STREAMING_THREAD_COUNT);
    assert(mcrt_task_arena_max_concurrency() < STREAMING_THREAD_COUNT);

    if (PT_UNLIKELY(VK_NULL_HANDLE == this->m_streaming_acquire_ownership_command_buffer[streaming_throttling_index][streaming_thread_index]))
    {
        VkCommandBufferAllocateInfo command_buffer_allocate_info;
        command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.pNext = NULL;
        command_buffer_allocate_info.commandPool = this->m_streaming_acquire_ownership_command_pool[streaming_throttling_index][streaming_thread_index];
        command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_allocate_info.commandBufferCount = 1U;

        VkResult res_allocate_command_buffers = m_device.allocate_command_buffers(&command_buffer_allocate_info, &this->m_streaming_acquire_ownership_command_buffer[streaming_throttling_index][streaming_thread_index]);
        assert(VK_SUCCESS == res_allocate_command_buffers);

        VkCommandBufferBeginInfo command_buffer_begin_info = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            NULL,
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            NULL,
        };
        VkResult res_begin_command_buffer = m_device.begin_command_buffer(this->m_streaming_acquire_ownership_command_buffer[streaming_throttling_index][streaming_thread_index], &command_buffer_begin_info);
        assert(VK_SUCCESS == res_begin_command_buffer);
    }

    return this->m_streaming_acquire_ownership_command_buffer[streaming_throttling_index][streaming_thread_index];
}

void gfx_connection_vk::copy_buffer_to_image(uint32_t streaming_throttling_index, VkBuffer src_buffer, VkImage dst_image, VkImageSubresourceRange const *subresource_range, uint32_t region_count, const VkBufferImageCopy *regions)
{
    // we can't use m_streaming_throttling_index here
    // we must cache the streaming_throttling_index to eusure the consistency
    VkCommandBuffer command_buffer = this->streaming_task_get_command_buffer(streaming_throttling_index);

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

    m_device.cmd_pipeline_barrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_before_copy);

    m_device.cmd_copy_buffer_to_image(command_buffer, src_buffer, dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, region_count, regions);

    // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#upload-data-from-the-cpu-to-an-image-sampled-in-a-fragment-shader

    if (this->m_device.has_dedicated_transfer_queue() && (this->m_device.queue_transfer_family_index() != this->m_device.queue_graphics_family_index()))
    {
        // Queue Family Ownership Transfer
        // Release Operation
        {
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
            m_device.cmd_pipeline_barrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_after_copy);
        }

        // Queue Family Ownership Transfer
        // Acquire Operation
        {
            VkCommandBuffer command_buffer_acquire_ownership = this->streaming_task_get_acquire_ownership_command_buffer(streaming_throttling_index);
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
            m_device.cmd_pipeline_barrier(command_buffer_acquire_ownership, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_acquire_ownership);
        }
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
        m_device.cmd_pipeline_barrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_after_copy);
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
    mcrt_atomic_store(&this->m_streaming_throttling_index, ((this->m_streaming_throttling_index + 1U) < STREAMING_THROTTLING_COUNT) ? (this->m_streaming_throttling_index + 1U) : 0U);

    //TODO race condition
    //allocate child not atomic

    // sync by TBB
    // int ref_count = mcrt_task_ref_count(this->m_streaming_task_root[streaming_throttling_index]);
    mcrt_task_wait_for_all(this->m_streaming_task_root[streaming_throttling_index]);
    mcrt_task_set_ref_count(this->m_streaming_task_root[streaming_throttling_index], 1U);

    // submit
    if (this->m_device.has_dedicated_transfer_queue())
    {
        VkSubmitInfo submit_info;
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = NULL;
        submit_info.waitSemaphoreCount = 0U;
        submit_info.pWaitSemaphores = NULL;
        submit_info.pWaitDstStageMask = NULL;
        VkCommandBuffer command_buffers[STREAMING_THREAD_COUNT];
        submit_info.commandBufferCount = 0U;
        submit_info.pCommandBuffers = command_buffers;
        submit_info.signalSemaphoreCount = 1U;
        submit_info.pSignalSemaphores = &this->m_streaming_semaphore[streaming_throttling_index];

        VkSubmitInfo submit_info_acquire_ownership;
        submit_info_acquire_ownership.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info_acquire_ownership.pNext = NULL;
        submit_info_acquire_ownership.waitSemaphoreCount = 1U;
        submit_info_acquire_ownership.pWaitSemaphores = &this->m_streaming_semaphore[streaming_throttling_index];
        VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        submit_info_acquire_ownership.pWaitDstStageMask = &wait_dst_stage_mask;
        VkCommandBuffer command_buffers_acquire_ownership[STREAMING_THREAD_COUNT];
        submit_info_acquire_ownership.commandBufferCount = 0U;
        submit_info_acquire_ownership.pCommandBuffers = command_buffers_acquire_ownership;
        submit_info_acquire_ownership.signalSemaphoreCount = 0U;
        submit_info_acquire_ownership.pSignalSemaphores = NULL;

        for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
        {
            if (VK_NULL_HANDLE != this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index])
            {
                this->m_device.end_command_buffer(this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index]);

                command_buffers[submit_info.commandBufferCount] = this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index];
                ++submit_info.commandBufferCount;

                this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index] = VK_NULL_HANDLE;
            }

            if (VK_NULL_HANDLE != this->m_streaming_acquire_ownership_command_buffer[streaming_throttling_index][streaming_thread_index])
            {
                this->m_device.end_command_buffer(this->m_streaming_acquire_ownership_command_buffer[streaming_throttling_index][streaming_thread_index]);

                command_buffers_acquire_ownership[submit_info_acquire_ownership.commandBufferCount] = this->m_streaming_acquire_ownership_command_buffer[streaming_throttling_index][streaming_thread_index];
                ++submit_info_acquire_ownership.commandBufferCount;

                this->m_streaming_acquire_ownership_command_buffer[streaming_throttling_index][streaming_thread_index] = VK_NULL_HANDLE;
            }
        }
        assert(submit_info.commandBufferCount == submit_info_acquire_ownership.commandBufferCount);

        // VUID-VkSubmitInfo-pCommandBuffers-parameter
        // "commandBufferCount 0" is legal
        this->m_device.queue_submit(this->m_device.queue_transfer(), 1, &submit_info, VK_NULL_HANDLE);

        this->m_device.queue_submit(this->m_device.queue_graphics(), 1, &submit_info_acquire_ownership, this->m_streaming_fence[streaming_throttling_index]);
    }
    else
    {
        VkSubmitInfo submit_info;
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = NULL;
        submit_info.waitSemaphoreCount = 0U;
        submit_info.pWaitSemaphores = NULL;
        submit_info.pWaitDstStageMask = NULL;
        VkCommandBuffer command_buffers[STREAMING_THREAD_COUNT];
        submit_info.commandBufferCount = 0U;
        submit_info.pCommandBuffers = command_buffers;
        submit_info.signalSemaphoreCount = 0U;
        submit_info.pSignalSemaphores = NULL;

        for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
        {
            if (VK_NULL_HANDLE != this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index])
            {
                this->m_device.end_command_buffer(this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index]);

                command_buffers[submit_info.commandBufferCount] = this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index];
                ++submit_info.commandBufferCount;

                this->m_streaming_command_buffer[streaming_throttling_index][streaming_thread_index] = VK_NULL_HANDLE;
            }
        }

        this->m_device.queue_submit(this->m_device.queue_graphics(), 1, &submit_info, this->m_streaming_fence[streaming_throttling_index]);
    }

    // wait the Fence
    // Submit ensures EndCommandBuffer
    // Fence ensures "Completed by GPU"
    // safe to ResetCommandPool
    streaming_throttling_index = (streaming_throttling_index > 0U) ? (streaming_throttling_index - 1U) : (STREAMING_THROTTLING_COUNT - 1U);
    this->m_device.wait_for_fences(1U, &this->m_streaming_fence[streaming_throttling_index], VK_TRUE, UINT64_MAX);
    this->m_device.reset_fences(1U, &this->m_streaming_fence[streaming_throttling_index]);

    // free memory
    {
        uint64_t transfer_src_buffer_begin_and_end = uint64_t(-1);
        uint32_t transfer_src_buffer_begin = uint32_t(-1);
        uint32_t transfer_src_buffer_end = uint32_t(-1);
        uint32_t transfer_src_buffer_size = uint32_t(-1);
        {
            VkDeviceSize transfer_src_buffer_size_uint64 = this->transfer_src_buffer_size();
            assert(uint64_t(transfer_src_buffer_size_uint64) < uint64_t(UINT32_MAX));
            transfer_src_buffer_size = uint32_t(transfer_src_buffer_size_uint64);
        }
        uint32_t transfer_src_buffer_streaming_task_max_end = this->m_transfer_src_buffer_streaming_task_max_end[streaming_throttling_index];
        do
        {
            transfer_src_buffer_begin_and_end = mcrt_atomic_load(&this->m_transfer_src_buffer_begin_and_end);
            transfer_src_buffer_begin = transfer_src_buffer_unpack_begin(transfer_src_buffer_begin_and_end);
            // assert(transfer_src_buffer_begin <= transfer_src_buffer_streaming_task_max_end); // the max_end remains if there is no task
            transfer_src_buffer_end = transfer_src_buffer_unpack_end(transfer_src_buffer_begin_and_end);

            // enough memeory
            assert((transfer_src_buffer_end - transfer_src_buffer_streaming_task_max_end) <= transfer_src_buffer_size);
        } while ((transfer_src_buffer_streaming_task_max_end > transfer_src_buffer_begin) && (transfer_src_buffer_begin_and_end != mcrt_atomic_cas_u64(&this->m_transfer_src_buffer_begin_and_end, transfer_src_buffer_pack_begin_and_end(transfer_src_buffer_streaming_task_max_end, transfer_src_buffer_end), transfer_src_buffer_begin_and_end)));
        // this->m_transfer_src_buffer_streaming_task_max_end[streaming_throttling_index] = 0;

#if defined(PT_GFX_PROFILE) && PT_GFX_PROFILE
        uint32_t transfer_src_buffer_used = (transfer_src_buffer_end - transfer_src_buffer_begin);
        if (transfer_src_buffer_used > 0U)
        {
            mcrt_log_print("transfer_src_buffer unused memory %f mb\n", float(transfer_src_buffer_size - transfer_src_buffer_used) / 1024.0f / 1024.0f);
        }
#endif
    }

    // TODO limit the thread arena number
    for (uint32_t streaming_thread_index = 0U; streaming_thread_index < STREAMING_THREAD_COUNT; ++streaming_thread_index)
    {
        this->m_device.reset_command_pool(this->m_streaming_command_pool[streaming_throttling_index][streaming_thread_index], 0U);

        if (this->m_device.has_dedicated_transfer_queue())
        {
            this->m_device.reset_command_pool(this->m_streaming_acquire_ownership_command_pool[streaming_throttling_index][streaming_thread_index], 0U);
        }
    }

    // throttling count
    {
        uint32_t streaming_throttling_count =  this->m_streaming_throttling_count[streaming_throttling_index];
        this->m_streaming_throttling_count[streaming_throttling_index] = 0U;
#if defined(PT_GFX_PROFILE) && PT_GFX_PROFILE
        if (streaming_throttling_count > 0U)
        {
            mcrt_log_print("streaming_throttling_count %i \n", int(streaming_throttling_count));
        }
#endif
    }

    // iterate the list
    {
        uint32_t streaming_object_count = this->m_streaming_object_count[streaming_throttling_index];
        for (uint32_t streaming_object_index = 0U; streaming_object_index < streaming_object_count; ++streaming_object_index)
        {
            this->m_streaming_object_list[streaming_throttling_index][streaming_object_index]->streaming_done();
            this->m_streaming_object_list[streaming_throttling_index][streaming_object_index] = NULL;
        }
        this->m_streaming_object_count[streaming_throttling_index] = 0U;
#if defined(PT_GFX_PROFILE) && PT_GFX_PROFILE
        if (streaming_object_count > 0U)
        {
            mcrt_log_print("streaming_object_count %i \n", int(streaming_object_count));
        }
#endif
    }
}

void gfx_connection_vk::destroy()
{
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
    gfx_texture_vk *texture = new (mcrt_aligned_malloc(sizeof(gfx_texture_vk), alignof(gfx_texture_vk))) gfx_texture_vk(this);
    return texture;
}
