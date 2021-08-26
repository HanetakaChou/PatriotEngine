//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef _PT_GFX_CONNECTION_VK_H_
#define _PT_GFX_CONNECTION_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_task.h>
#include <pt_mcrt_rwlock.h>
#include <pt_mcrt_scalable_allocator.h>
#include <pt_math.h>
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_thread.h>
#include "pt_gfx_connection_base.h"
#include "pt_gfx_device_vk.h"
#include "pt_gfx_malloc_vk.h"
#include "pt_gfx_streaming_object_base.h"
#include "pt_gfx_frame_object_base.h"
#include "pt_gfx_node_vk.h"
#include "pt_gfx_mesh_vk.h"
#include "pt_gfx_material_vk.h"
#include "pt_gfx_texture_vk.h"
#include <vulkan/vulkan.h>
#include <vector>

class gfx_connection_vk final : public gfx_connection_base
{
    class gfx_device_vk m_device;
    class gfx_malloc_vk m_malloc;

    // MCRT

    // Avoid false sharing
#if defined(PT_X64) || defined(PT_X86) || defined(PT_ARM64) || defined(PT_ARM)
    enum {ESTIMATED_CACHE_LINE_SIZE = 64U};
#else
#error Unknown Architecture
#endif

    // Frame
    enum
    {
        OPAQUE_SUBPASS_INDEX = 0U,
        SUBPASS_COUNT = 1U
    };
    VkCommandBuffer *m_frame_graphcis_execute_command_buffers[SUBPASS_COUNT];
    struct frame_thread_block
    {
        VkCommandPool m_frame_graphics_secondary_commmand_pool;
        VkCommandBuffer m_frame_graphics_secondary_command_buffer[SUBPASS_COUNT];
        uint8_t m_padding[ESTIMATED_CACHE_LINE_SIZE - (sizeof(m_frame_graphics_secondary_commmand_pool) + sizeof(m_frame_graphics_secondary_command_buffer))];
    };
    static_assert(ESTIMATED_CACHE_LINE_SIZE >= (sizeof(frame_thread_block::m_frame_graphics_secondary_commmand_pool) + sizeof(frame_thread_block::m_frame_graphics_secondary_command_buffer)), "");
    static_assert(std::is_pod<struct frame_thread_block>::value, "");
    static_assert(sizeof(struct frame_thread_block) == ESTIMATED_CACHE_LINE_SIZE, "");
    struct frame_thread_block *m_frame_thread_block[FRAME_THROTTLING_COUNT];

    VkCommandPool m_frame_graphics_primary_commmand_pool[FRAME_THROTTLING_COUNT];
    VkCommandBuffer m_frame_graphics_primary_command_buffer[FRAME_THROTTLING_COUNT];

    VkSemaphore m_frame_semaphore_acquire_next_image[FRAME_THROTTLING_COUNT];
    VkSemaphore m_frame_semaphore_queue_submit[FRAME_THROTTLING_COUNT];
    VkFence m_frame_fence[FRAME_THROTTLING_COUNT];

    uint32_t m_frame_swapchain_image_index[FRAME_THROTTLING_COUNT];
    bool m_frame_swapchain_image_acquired[FRAME_THROTTLING_COUNT];

    // Scene
    template <typename T>
    using mcrt_vector = std::vector<T, mcrt::scalable_allocator<T>>;
    mcrt_vector<class gfx_node_vk *> m_scene_node_list;
    mcrt_vector<size_t> m_scene_node_list_free_index_list;

    mcrt_vector<class gfx_frame_object_base *> m_frame_object_unused_list[FRAME_THROTTLING_COUNT];

    inline VkCommandBuffer frame_task_get_secondary_command_buffer(uint32_t frame_throttling_index, uint32_t frame_thread_index, uint32_t subpass_index);
    inline void acquire_frame();
    inline void release_frame();

    struct opaque_subpass_task_data
    {
        class gfx_connection_vk *m_gfx_connection;
    };
    static mcrt_task_ref opaque_subpass_task_execute(mcrt_task_ref self);

    // The unique uniform buffer.
    // \[Gruen 2015\] [Holger Gruen. "Constant Buffers without Constant Pain." NVIDIA GameWorks Blog 2015.](https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0)
    // \[Microsoft\] [Microsoft. "Ring buffer scenario." Microsoft Docs.](https://docs.microsoft.com/en-us/windows/win32/direct3d12/fence-based-resource-management#ring-buffer-scenario)
    // BoneMatrix
    // VkBuffer m_uniform_buffer;
    // uint64_t m_uniform_buffer_ring_buffer_begin;
    // uint64_t m_uniform_buffer_ring_buffer_end;

    // RenderPass
    // Ideally, we can use only one renderpass by using the preserve attachments
    VkRenderPass m_render_pass;

    // https://www.khronos.org/registry/vulkan/specs/1.0/html/chap33.html#limits-minmax
    // maxPushConstantsSize 128
    // minUniformBufferOffsetAlignment 256
    static VkDeviceSize const m_limit_min_max_push_constants_size = 128U;
    static uint32_t const m_limit_max_min_uniform_buffer_offset_alignment = 256U;

    // push constant
    uint32_t m_push_constant_mat_vp_offset;
    uint32_t m_push_constant_mat_vp_size;
    uint32_t m_push_constant_mat_m_offset;
    uint32_t m_push_constant_mat_m_size;

    //
    VkSampler m_immutable_sampler;

    // Descriptor
    VkDescriptorSetLayout m_descriptor_set_layout_each_object_shared;
    VkDescriptorSetLayout m_descriptor_set_layout_each_object_private;
    VkPipelineLayout m_pipeline_layout;

    enum
    {
        MAX_FRAME_OBJECT_INUSE_COUNT = 4096U
    };
    VkDescriptorPool m_descriptor_pool_each_object_private;
    mcrt_vector<VkDescriptorSet> m_descriptor_set_object_private_free_list;

    //
    VkShaderModule m_shader_module_mesh_vertex;
    VkShaderModule m_shader_module_mesh_fragment;
    VkPipeline m_pipeline_mesh;
    VkPipelineCache m_pipeline_cache_mesh;

    // reversed-Z
    // https://developer.nvidia.com/content/depth-precision-visualized
    static VkCompareOp const m_z_nearer = VK_COMPARE_OP_LESS_OR_EQUAL;
    static constexpr float const m_z_farthest = 1.0f;

    // Framebuffer
    // The memory allocator is not required since the number of the framebuffer images is verily limited
    // Perhaps we should prepare different intermediate textures for differenct frames ???
    VkImage m_depth_image;
    VkDeviceMemory m_depth_device_memory;
    VkImageView m_depth_image_view;
    VkFramebuffer *m_framebuffers;

    // WSI
    uint32_t m_wsi_width;
    uint32_t m_wsi_height;

    // Perspective Matraix
    float m_aspect_ratio; //width-divide-height

    // SwapChain
    VkSurfaceKHR m_surface;
    uint32_t m_framebuffer_width;
    uint32_t m_framebuffer_height;
    VkFormat m_swapchain_image_format;
    uint32_t m_swapchain_image_count;
    VkImage *m_swapchain_images;
    VkImageView *m_swapchain_image_views;
    VkSwapchainKHR m_swapchain;

    // Streaming
    VkCommandBuffer *m_streaming_transfer_submit_info_command_buffers;
    VkCommandBuffer *m_streaming_graphics_submit_info_command_buffers;
    struct streaming_thread_block
    {
        VkCommandPool m_streaming_transfer_command_pool;
        VkCommandPool m_streaming_graphics_command_pool;
        VkCommandBuffer m_streaming_transfer_command_buffer;
        VkCommandBuffer m_streaming_graphics_command_buffer;
        uint8_t m_padding[ESTIMATED_CACHE_LINE_SIZE - (sizeof(m_streaming_transfer_command_pool) + sizeof(m_streaming_transfer_command_buffer) + sizeof(m_streaming_transfer_command_pool) + sizeof(m_streaming_graphics_command_buffer))];
    };
    static_assert(ESTIMATED_CACHE_LINE_SIZE >= (sizeof(streaming_thread_block::m_streaming_transfer_command_pool) + sizeof(streaming_thread_block::m_streaming_transfer_command_buffer) + sizeof(streaming_thread_block::m_streaming_transfer_command_pool) + sizeof(streaming_thread_block::m_streaming_graphics_command_buffer)), "");
    static_assert(std::is_pod<struct streaming_thread_block>::value, "");
    static_assert(sizeof(struct streaming_thread_block) == ESTIMATED_CACHE_LINE_SIZE, "");
    struct streaming_thread_block *m_streaming_thread_block[STREAMING_THROTTLING_COUNT];

    VkSemaphore m_streaming_semaphore[STREAMING_THROTTLING_COUNT];
    VkFence m_streaming_fence[STREAMING_THROTTLING_COUNT];

    inline VkCommandBuffer streaming_task_get_transfer_command_buffer(uint32_t streaming_throttling_index, uint32_t streaming_thread_index);
    inline VkCommandBuffer streaming_task_get_graphics_command_buffer(uint32_t streaming_throttling_index, uint32_t streaming_thread_index);
    inline void reduce_streaming_task();

    inline gfx_connection_vk();
    inline ~gfx_connection_vk();
    inline bool init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual);
    void destroy() override;

    inline bool init_frame();
    inline bool init_pipeline_layout();
    inline bool init_shader();
    inline bool update_surface(wsi_connection_ref wsi_connection, wsi_window_ref wsi_window);
    inline bool update_framebuffer();
    inline bool load_pipeline_cache(char const *pipeline_cache_name, VkPipelineCache *pipeline_cache);
    inline void destroy_frame();
    inline void destory_surface();
    inline void destory_framebuffer();
    inline void destory_pipeline_layout();
    inline void destory_shader();
    inline void store_pipeline_cache(char const *pipeline_cache_name, VkPipelineCache *pipeline_cache);

    inline bool init_streaming();
    inline void destroy_streaming();


    friend class gfx_connection_base *gfx_connection_vk_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual);
    class gfx_node_base *create_node() override;
    class gfx_mesh_base *create_mesh() override;
    class gfx_material_base *create_material() override;
    class gfx_texture_base *create_texture() override;
    bool on_wsi_window_created(wsi_connection_ref wsi_connection, wsi_window_ref wsi_window, float width, float height) override;
    void on_wsi_window_destroyed() override;
    void on_wsi_resized(float width, float height) override;
    void on_wsi_redraw_needed_acquire() override;
    void on_wsi_redraw_needed_release() override;

    inline void copy_vertex_index_buffer(uint32_t streaming_throttling_index, uint32_t streaming_thread_index, VkAccessFlags dst_access_mask, VkBuffer src_buffer, VkBuffer dst_buffer, uint32_t region_count, VkBufferCopy *const regions);

public:
    //uniform buffer
    //assert(0 == (pMemoryRequirements->alignment % m_physical_device_limits_min_uniform_buffer_offset_alignment)

    // Streaming
    inline void *transfer_src_buffer_pointer() { return m_malloc.transfer_src_buffer_pointer(); }
    inline VkBuffer transfer_src_buffer() { return m_malloc.transfer_src_buffer(); }

    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_offset_alignment() { return m_device.physical_device_limits_optimal_buffer_copy_offset_alignment(); }
    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_row_pitch_alignment() { return m_device.physical_device_limits_optimal_buffer_copy_row_pitch_alignment(); }

    void copy_vertex_buffer(uint32_t streaming_throttling_index, uint32_t streaming_thread_index, VkBuffer src_buffer, VkBuffer dst_buffer, uint32_t region_count, VkBufferCopy *const regions);
    void copy_index_buffer(uint32_t streaming_throttling_index, uint32_t streaming_thread_index, VkBuffer src_buffer, VkBuffer dst_buffer, uint32_t region_count, VkBufferCopy *const regions);
    void copy_buffer_to_image(uint32_t streaming_throttling_index, uint32_t streaming_thread_index, VkBuffer src_buffer, VkImage dst_image, VkImageSubresourceRange const *subresource_range, uint32_t region_count, const VkBufferImageCopy *regions);

    inline VkResult create_buffer(VkBufferCreateInfo const *create_info, VkBuffer *buffer) { return this->m_device.create_buffer(create_info, buffer); }
    inline void get_buffer_memory_requirements(VkBuffer buffer, VkMemoryRequirements *memory_requirements) { return this->m_device.get_buffer_memory_requirements(buffer, memory_requirements); }
    VkDeviceMemory transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(VkMemoryRequirements const *memory_requirements, void **out_page_handle, uint64_t *out_offset, uint64_t *out_size) { return this->m_malloc.transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(&this->m_device, memory_requirements, out_page_handle, out_offset, out_size); }
    inline VkResult bind_buffer_memory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memory_offset) { return this->m_device.bind_buffer_memory(buffer, memory, memory_offset); }
    void transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(void *page_handle, uint64_t offset, uint64_t size, VkDeviceMemory device_memory) { this->m_malloc.transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(&this->m_device, page_handle, offset, size, device_memory); }
    inline void destroy_buffer(VkBuffer buffer) { return this->m_device.destroy_buffer(buffer); }

    inline void get_physical_device_format_properties(VkFormat format, VkFormatProperties *format_properties) { return m_device.get_physical_device_format_properties(format, format_properties); }
    inline VkResult create_image(VkImageCreateInfo const *create_info, VkImage *image) { return m_device.create_image(create_info, image); }
    inline void get_image_memory_requirements(VkImage image, VkMemoryRequirements *memory_requirements) { return m_device.get_image_memory_requirements(image, memory_requirements); }
    inline VkDeviceMemory transfer_dst_and_sampled_image_alloc(VkMemoryRequirements const *memory_requirements, void **out_page_handle, uint64_t *out_offset, uint64_t *out_size) { return this->m_malloc.transfer_dst_and_sampled_image_alloc(&this->m_device, memory_requirements, out_page_handle, out_offset, out_size); }
    inline VkResult bind_image_memory(VkImage image, VkDeviceMemory memory, VkDeviceSize memory_offset) { return m_device.bind_image_memory(image, memory, memory_offset); }
    inline void transfer_dst_and_sampled_image_free(void *page_handle, uint64_t offset, uint64_t size, VkDeviceMemory device_memory) { return this->m_malloc.transfer_dst_and_sampled_image_free(&this->m_device, page_handle, offset, size, device_memory); }
    inline void destroy_image(VkImage image) { return this->m_device.destroy_image(image); }

    inline VkResult create_image_view(VkImageViewCreateInfo const *create_info, VkImageView *view) { return this->m_device.create_image_view(create_info, view); }
    inline void destroy_image_view(VkImageView image_view) { return this->m_device.destroy_image_view(image_view); }

    bool allocate_descriptor_set(VkDescriptorSet *descriptor_set);
    void init_descriptor_set(VkDescriptorSet descriptor_set, uint32_t texture_count, class gfx_texture_base const *const *gfx_textures);
    void free_descriptor_set(VkDescriptorSet descriptor_set);
};

class gfx_connection_base *gfx_connection_vk_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual);

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
