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
#include <pt_math.h>
#include "pt_gfx_connection_common.h"
#include "pt_gfx_device_vk.h"
#include "pt_gfx_malloc_vk.h"
#include "pt_gfx_streaming_object.h"
#include <vulkan/vulkan.h>

class gfx_connection_vk final : public gfx_connection_common
{
    class gfx_device_vk m_device;
    class gfx_malloc_vk m_malloc;

    // The unique uniform buffer.
    // \[Gruen 2015\] [Holger Gruen. "Constant Buffers without Constant Pain." NVIDIA GameWorks Blog 2015.](https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0)
    // \[Microsoft\] [Microsoft. "Ring buffer scenario." Microsoft Docs.](https://docs.microsoft.com/en-us/windows/win32/direct3d12/fence-based-resource-management#ring-buffer-scenario)
    //VkBuffer m_uniform_buffer;
    //uint64_t m_uniform_buffer_ring_buffer_begin;
    //uint64_t m_uniform_buffer_ring_buffer_end;

    class gfx_texture_common *create_texture() override;
    void wsi_on_resized(wsi_window_ref wsi_window, float width, float height) override;
    void wsi_on_redraw_needed_acquire(wsi_window_ref wsi_window, float width, float height) override;
    void wsi_on_redraw_needed_release() override;

    // Streaming
    static uint32_t const STREAMING_THROTTLING_COUNT = 3U;
    uint32_t m_streaming_throttling_index;
    uint32_t m_spin_lock_streaming_throttling_index;

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    uint32_t m_streaming_task_executing_count[STREAMING_THROTTLING_COUNT];
    bool m_streaming_task_reducing[STREAMING_THROTTLING_COUNT];
#endif

    // Staging Buffer
    // [RingBuffer](https://docs.microsoft.com/en-us/windows/win32/direct3d12/fence-based-resource-management) related
    uint64_t m_transfer_src_buffer_begin[STREAMING_THROTTLING_COUNT];
    uint64_t m_transfer_src_buffer_end[STREAMING_THROTTLING_COUNT];
    uint64_t m_transfer_src_buffer_size[STREAMING_THROTTLING_COUNT];

    static uint32_t const STREAMING_THREAD_COUNT = 32U;

    // TODO
    // tweak the layout of this struct according to the cacheline to reduce false sharing
    //
    // padding // cacheline // false sharing
    // struct
    // {
    //
    // padding // false sharing
    // } [STREAMING_THREAD_COUNT]
    VkCommandPool m_streaming_transfer_command_pool[STREAMING_THROTTLING_COUNT][STREAMING_THREAD_COUNT];
    VkCommandBuffer m_streaming_transfer_command_buffer[STREAMING_THROTTLING_COUNT][STREAMING_THREAD_COUNT];
    VkCommandPool m_streaming_graphics_command_pool[STREAMING_THROTTLING_COUNT][STREAMING_THREAD_COUNT];
    VkCommandBuffer m_streaming_graphics_command_buffer[STREAMING_THROTTLING_COUNT][STREAMING_THREAD_COUNT];

    VkSemaphore m_streaming_semaphore[STREAMING_THROTTLING_COUNT];
    VkFence m_streaming_fence[STREAMING_THROTTLING_COUNT];

    mcrt_task_arena_ref m_task_arena;
    mcrt_task_ref m_streaming_task_root[STREAMING_THROTTLING_COUNT];
    mcrt_task_ref m_streaming_task_respawn_root;

    static uint32_t const STREAMING_TASK_RESPAWN_LINEAR_LIST_COUNT = 64U;
    uint32_t m_streaming_task_respawn_linear_list_count[STREAMING_THROTTLING_COUNT];
    mcrt_task_ref m_streaming_task_respawn_linear_list[STREAMING_THROTTLING_COUNT][STREAMING_TASK_RESPAWN_LINEAR_LIST_COUNT];
    struct streaming_task_respawn_task_respawn_link_list
    {
        struct streaming_task_respawn_task_respawn_link_list *m_next;
        mcrt_task_ref m_task;
    };
    struct streaming_task_respawn_task_respawn_link_list *m_streaming_task_respawn_link_list_head[STREAMING_THROTTLING_COUNT];

    static uint32_t const STREAMING_OBJECT_LINEAR_LIST_COUNT = 32U;
    uint32_t m_streaming_object_linear_list_count[STREAMING_THROTTLING_COUNT];
    class gfx_streaming_object *m_streaming_object_linear_list[STREAMING_THROTTLING_COUNT][STREAMING_OBJECT_LINEAR_LIST_COUNT];
    struct streaming_object_link_list
    {
        struct streaming_object_link_list *m_next;
        class gfx_streaming_object *m_streaming_object;
    };
    struct streaming_object_link_list *m_streaming_object_link_list_head[STREAMING_THROTTLING_COUNT];

    inline VkCommandBuffer streaming_task_get_transfer_command_buffer(uint32_t streaming_throttling_index, uint32_t streaming_thread_inde);
    inline VkCommandBuffer streaming_task_get_graphics_command_buffer(uint32_t streaming_throttling_index, uint32_t streaming_thread_inde);
    inline void reduce_streaming_task();

    // Frame Throttling
    static uint32_t const FRAME_THROTTLING_COUNT = 3U;
    uint32_t m_frame_throtting_index;
    VkCommandPool m_graphics_commmand_pool[FRAME_THROTTLING_COUNT];

    // RenderPass
    // Ideally, we can use only one renderpass by using the preserve attachments
    VkRenderPass m_render_pass;

    // https://www.khronos.org/registry/vulkan/specs/1.0/html/chap33.html#limits-minmax
    // maxPushConstantsSize 128
    // minUniformBufferOffsetAlignment 256
    static VkDeviceSize const m_limit_min_max_push_constants_size = 128U;
    static uint32_t const m_limit_max_min_uniform_buffer_offset_alignment = 256U;

    // push constant
    math_alignas16_mat4x4 m_mat_vp;
    math_alignas16_mat4x4 m_mat_m;

    // Descriptor
    VkDescriptorSetLayout m_descriptor_set_layout_each_object_immutable;
    VkDescriptorSetLayout m_descriptor_set_layout_each_object_dynamic;
    VkPipelineLayout m_pipeline_layout;

    //
    VkShaderModule m_shader_module_mesh_vertex;
    VkShaderModule m_shader_module_mesh_fragment;
    VkPipeline m_pipeline_mesh;
    VkPipelineCache m_pipeline_cache_mesh;

    // reversed-Z
    // https://developer.nvidia.com/content/depth-precision-visualized
    static VkCompareOp const m_z_nearer = VK_COMPARE_OP_LESS_OR_EQUAL;


    // Framebuffer
    // The memory allocator is not required since the number of the framebuffer images is verily limited
    // Perhaps we should prepare different intermediate textures for differenct frames ???
    VkImage m_depth_image;
    VkDeviceMemory m_depth_device_memory;
    VkImageView m_depth_image_view;
    VkFramebuffer *m_framebuffers;

    // SwapChain
    VkSurfaceKHR m_surface;
    uint32_t m_framebuffer_width;
    uint32_t m_framebuffer_height;
    VkFormat m_swapchain_image_format;
    uint32_t m_swapchain_image_count;
    VkImage *m_swapchain_images;
    VkImageView *m_swapchain_image_views;
    VkSwapchainKHR m_swapchain;

    inline gfx_connection_vk();
    inline bool init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window);
    inline bool init_streaming();
    inline bool init_swapchain_and_renderpass_and_framebuffer(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window);
    inline bool update_swapchain();
    inline bool init_renderpass();
    inline bool update_framebuffer();
    inline bool init_descriptor_and_pipeline_layout();
    inline bool init_shader_and_pipeline();
    inline bool load_pipeline_cache(char const *pipeline_cache_name, VkPipelineCache *pipeline_cache);
    inline void store_pipeline_cache(char const *pipeline_cache_name, VkPipelineCache *pipeline_cache);

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
    inline VkBuffer transfer_src_buffer() { return m_malloc.transfer_src_buffer(); }
    inline uint64_t transfer_src_buffer_begin(uint32_t streaming_throttling_index) { return m_transfer_src_buffer_begin[streaming_throttling_index]; }
    inline uint64_t *transfer_src_buffer_end(uint32_t streaming_throttling_index) { return &m_transfer_src_buffer_end[streaming_throttling_index]; }
    inline uint64_t transfer_src_buffer_size(uint32_t streaming_throttling_index) { return m_transfer_src_buffer_size[streaming_throttling_index]; }

    //uniform buffer
    //assert(0 == (pMemoryRequirements->alignment % m_physical_device_limits_min_uniform_buffer_offset_alignment)

    inline VkDeviceMemory transfer_dst_and_sampled_image_alloc(VkMemoryRequirements const *memory_requirements, void **out_page_handle, uint64_t *out_offset, uint64_t *out_size) { return this->m_malloc.transfer_dst_and_sampled_image_alloc(&this->m_device, memory_requirements, out_page_handle, out_offset, out_size); }
    inline void transfer_dst_and_sampled_image_free(void *page_handle, uint64_t offset, uint64_t size, VkDeviceMemory device_memory) { return this->m_malloc.transfer_dst_and_sampled_image_free(&this->m_device, page_handle, offset, size, device_memory); }

    //Streaming
    inline void streaming_throttling_index_lock()
    {
        while (0U != mcrt_atomic_xchg_u32(&this->m_spin_lock_streaming_throttling_index, 1U))
        {
            mcrt_os_yield();
        }
        // mcrt_atomic_acquire_barrier();
    }

    inline uint32_t streaming_throttling_index()
    {
        return mcrt_atomic_load(&this->m_streaming_throttling_index);
    }

    inline void streaming_throttling_index_unlock()
    {
        mcrt_atomic_store(&this->m_spin_lock_streaming_throttling_index, 0U);
    }

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    void streaming_task_debug_executing_begin(uint32_t streaming_throttling_index);
    void streaming_task_debug_executing_end(uint32_t streaming_throttling_index);
#endif

    inline mcrt_task_ref streaming_task_root(uint32_t streaming_throttling_index)
    {
        return m_streaming_task_root[streaming_throttling_index];
    }
    inline mcrt_task_ref streaming_task_respawn_root() { return m_streaming_task_respawn_root; }
    inline mcrt_task_arena_ref task_arena() { return m_task_arena; }
    void streaming_object_list_push(uint32_t streaming_throttling_index, class gfx_streaming_object *streaming_object);
    void streaming_task_respawn_list_push(uint32_t streaming_throttling_index, mcrt_task_ref streaming_task);
    void copy_buffer_to_image(uint32_t streaming_throttling_index, uint32_t streaming_thread_index, VkBuffer src_buffer, VkImage dst_image, VkImageSubresourceRange const *subresource_range, uint32_t region_count, const VkBufferImageCopy *regions);
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
