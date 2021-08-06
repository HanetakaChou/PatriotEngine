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

#ifndef _PT_GFX_CONNECTION_VK_H_
#define _PT_GFX_CONNECTION_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_thread.h>
#include <pt_mcrt_task.h>
#include "pt_gfx_connection_common.h"
#include "pt_gfx_device_vk.h"
#include "pt_gfx_malloc.h"
#include "pt_gfx_malloc_vk.h"
#include "pt_gfx_streaming_object.h"
#include <vulkan/vulkan.h>

class gfx_connection_vk : public gfx_connection_common
{
    class gfx_device_vk m_device;
    class gfx_malloc_vk m_malloc;

    //deque
    struct async_resource_job_result
    {
        VkFence m_fence;
        //m_vertexbuffers
        //m_indexbuffers
        //m_textures
        uint64_t m_ringbuffer_begin;
        uint64_t m_ringbuffer_end;
    };

    // The unique uniform buffer.
    // \[Gruen 2015\] [Holger Gruen. "Constant Buffers without Constant Pain." NVIDIA GameWorks Blog 2015.](https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0)
    // \[Microsoft\] [Microsoft. "Ring buffer scenario." Microsoft Docs.](https://docs.microsoft.com/en-us/windows/win32/direct3d12/fence-based-resource-management#ring-buffer-scenario)
    VkBuffer m_uniform_buffer;
    uint64_t m_uniform_buffer_ring_buffer_begin;
    uint64_t m_uniform_buffer_ring_buffer_end;

    class gfx_texture_common *create_texture() override;
    void wsi_on_resized(wsi_window_ref wsi_window, float width, float height) override;
    void wsi_on_redraw_needed_acquire(wsi_window_ref wsi_window, float width, float height) override;
    void wsi_on_redraw_needed_release() override;

    // Perhaps we should prepare different intermediate textures for differenct frames

    // Frame Throttling
    static uint32_t const FRAME_THROTTLING_COUNT = 3U;
    uint32_t m_frame_throtting_index;
    VkCommandPool m_graphics_commmand_pool[FRAME_THROTTLING_COUNT];

    // Streaming
    static uint32_t const STREAMING_THROTTLING_COUNT = 3U;
    static uint32_t const STREAMING_THREAD_COUNT = 32U;
    uint32_t m_streaming_throttling_index;

    // [RingBuffer](https://docs.microsoft.com/en-us/windows/win32/direct3d12/fence-based-resource-management) related
    uint64_t m_transfer_src_buffer_begin_and_end; // Staging Buffer
    uint32_t m_transfer_src_buffer_streaming_task_max_end[STREAMING_THROTTLING_COUNT];

    // The size of the transfer src buffer is limited and thus the number of the streaming_object should be limited
    //
    // At most time, "m_streaming_throttling_count" may equal "m_streaming_object_count"
    // However, "mcrt_task_wait_for_all" may occur between "current_streaming_throttling_index" and "mcrt_task_increment_ref_count" in "read_input_stream"
    static uint32_t const STREAMING_THROTTLING_THRESHOLD = 88U; // < (STREAMING_OBJECT_COUNT - STREAMING_THREAD_COUNT)
    uint32_t m_streaming_throttling_count[STREAMING_THROTTLING_COUNT];

    // TODO
    // padding // cacheline // false sharing
    // struct
    // {
    //
    // padding // false sharing
    // } [STREAMING_THREAD_COUNT]
    VkCommandPool m_streaming_command_pool[STREAMING_THROTTLING_COUNT][STREAMING_THREAD_COUNT];
    VkCommandBuffer m_streaming_command_buffer[STREAMING_THROTTLING_COUNT][STREAMING_THREAD_COUNT];
    VkCommandPool m_streaming_acquire_ownership_command_pool[STREAMING_THROTTLING_COUNT][STREAMING_THREAD_COUNT];
    VkCommandBuffer m_streaming_acquire_ownership_command_buffer[STREAMING_THROTTLING_COUNT][STREAMING_THREAD_COUNT];
    VkSemaphore m_streaming_semaphore[STREAMING_THROTTLING_COUNT];
    VkFence m_streaming_fence[STREAMING_THROTTLING_COUNT];
    mcrt_task_ref m_streaming_task_root[STREAMING_THROTTLING_COUNT];

    // we don't need the operations below since the fence ensures that there is no consumer
    //
    // Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 2 Public Free List / freeListPush + repatriatePublicFreeList
    // "This is ABA safe without concern for versioning because the concurrent data structure is a single consumer (the owning thread) and multiple producers."
    //
    static uint32_t const STREAMING_OBJECT_COUNT = 128U;
    uint32_t m_streaming_object_count[STREAMING_THROTTLING_COUNT];
    class gfx_streaming_object *m_streaming_object_list[STREAMING_THROTTLING_COUNT][STREAMING_OBJECT_COUNT];

    inline VkCommandBuffer streaming_task_get_command_buffer(uint32_t streaming_throttling_index);
    inline VkCommandBuffer streaming_task_get_acquire_ownership_command_buffer(uint32_t streaming_throttling_index);
    inline void reduce_streaming_task();

    bool init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window);
    inline gfx_connection_vk();
    void destroy() override;
    inline ~gfx_connection_vk();

    friend class gfx_connection_common *gfx_connection_vk_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window);

public:
    inline void get_physical_device_format_properties(VkFormat format, VkFormatProperties *out_format_properties) { return m_device.get_physical_device_format_properties(format, out_format_properties); }

    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_offset_alignment() { return m_device.physical_device_limits_optimal_buffer_copy_offset_alignment(); }
    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_row_pitch_alignment() { return m_device.physical_device_limits_optimal_buffer_copy_row_pitch_alignment(); }

    inline VkResult create_image(VkImageCreateInfo const *create_info, VkImage *image) { return m_device.create_image(create_info, image); }
    inline void get_image_memory_requirements(VkImage image, VkMemoryRequirements *memory_requirements) { return m_device.get_image_memory_requirements(image, memory_requirements); }
    inline VkResult bind_image_memory(VkImage image, VkDeviceMemory memory, VkDeviceSize memory_offset) { return m_device.bind_image_memory(image, memory, memory_offset); }
    inline void destroy_image(VkImage image) { return this->m_device.destroy_image(image); }

    inline void *transfer_src_buffer_pointer() { return m_malloc.transfer_src_buffer_pointer(); }
    inline VkDeviceSize transfer_src_buffer_size() { return m_malloc.transfer_src_buffer_size(); }
    inline VkBuffer transfer_src_buffer() { return m_malloc.transfer_src_buffer(); }
    inline uint64_t *transfer_src_buffer_begin_and_end() { return &m_transfer_src_buffer_begin_and_end; }
    inline uint32_t *transfer_src_buffer_streaming_task_max_end(uint32_t streaming_throttling_index) { return &m_transfer_src_buffer_streaming_task_max_end[streaming_throttling_index]; }

    static inline uint32_t transfer_src_buffer_unpack_begin(uint64_t transfer_src_buffer_begin_and_end) { return (transfer_src_buffer_begin_and_end >> 32U); }
    static inline uint32_t transfer_src_buffer_unpack_end(uint64_t transfer_src_buffer_begin_and_end) { return (transfer_src_buffer_begin_and_end & 0XFFFFFFFFU); }
    static inline uint64_t transfer_src_buffer_pack_begin_and_end(uint32_t transfer_src_buffer_begin, uint32_t transfer_src_buffer_end) { return ((uint64_t(transfer_src_buffer_begin) << 32U) | (uint64_t(transfer_src_buffer_end))); }

    //uniform buffer
    //assert(0 == (pMemoryRequirements->alignment % m_physical_device_limits_min_uniform_buffer_offset_alignment)

    inline VkDeviceMemory transfer_dst_and_sampled_image_alloc(VkMemoryRequirements const *memory_requirements, void **out_page_handle, uint64_t *out_offset, uint64_t *out_size) { return m_malloc.transfer_dst_and_sampled_image_alloc(memory_requirements, out_page_handle, out_offset, out_size); }
    inline void transfer_dst_and_sampled_image_free(void *page_handle, uint64_t offset, uint64_t size, VkDeviceMemory device_memory) { return m_malloc.transfer_dst_and_sampled_image_free(page_handle, offset, size, device_memory); }

    //Streaming
    inline uint32_t current_streaming_throttling_index() { return mcrt_atomic_load(&this->m_streaming_throttling_index); }
    bool streaming_throttling(uint32_t streaming_throttling_index);
    inline mcrt_task_ref streaming_task_root(uint32_t streaming_throttling_index) { return m_streaming_task_root[streaming_throttling_index]; }
    bool streaming_object_list_push(uint32_t streaming_throttling_index, class gfx_streaming_object *streaming_object);
    void copy_buffer_to_image(uint32_t streaming_throttling_index, VkBuffer src_buffer, VkImage dst_image, VkImageSubresourceRange const *subresource_range, uint32_t region_count, const VkBufferImageCopy *regions);
};

class gfx_connection_common *gfx_connection_vk_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window);

// Streaming in CryEngine - StatObj

// The logic/application layer requests load
// CEntity::LoadGeometry
// ...
//
// CRenderProxy::RegisterForRendering // add to the m_deferredRenderProxyStreamingPriorityUpdates

// The renderloop uses camera to deduce the LOD which impacts the streaming
// C3DEngine::RenderWorld
// C3DEngine::UpdateRenderingCamera
//   traverse the m_deferredRenderProxyStreamingPriorityUpdates
//     CObjManager::UpdateRenderNodeStreamingPriority
//     CObjManager::PrecacheStatObj
//     ...
//     CStatObj::UpdateStreamableComponents
//     ...
//     CObjManager::RegisterForStreaming // add to the m_arrStreamableObjects
//   clear the m_deferredRenderProxyStreamingPriorityUpdates

//
// C3DEngine::SyncProcessStreamingUpdate
// CObjManager::ProcessObjectsStreaming // remove from m_arrStreamableObjects // add to m_arrStreamableToLoad

// Called at the end of the frame
// C3DEngine::SyncProcessStreamingUpdate
// CObjManager::ProcessObjectsStreaming_Finish
//   traverse the m_arrStreamableToLoad
//     CStatObj::StartStreaming
//     CStreamEngine::StartRead // forward to the streaming module
//   clear the m_arrStreamableToLoad

// The streaming module

// callback on complete
// CStatObj::StreamAsyncOnComplete
// m_eStreamingStatus = ecss_Ready

#endif
