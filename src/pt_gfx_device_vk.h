//
// Copyright (C) YuqiaoZhang(HanetakaChou)
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

#ifndef _PT_GFX_DEVICE_VK_H_
#define _PT_GFX_DEVICE_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include <pt_mcrt_intrin.h>
#include <vulkan/vulkan.h>

#if defined(PT_POSIX)
#if defined(PT_POSIX_LINUX)
#define PT_VK_LAYER_KHRONOS_VALIDATION 1
#elif defined(PT_POSIX_MACH)
#if defined(PT_POSIX_MATH_IOS)
#define PT_VK_LAYER_KHRONOS_VALIDATION 0
#elif defined(PT_POSIX_MATH_OSX)
#define PT_VK_LAYER_KHRONOS_VALIDATION 1
#else
#error Unknown Mach Platform
#endif
#else
#error Unknown Platform
#endif
#elif defined(PT_WIN32)
#define PT_VK_LAYER_KHRONOS_VALIDATION 1
#else
#error Unknown Platform
#endif

#if defined(PT_POSIX)
#if defined(PT_POSIX_LINUX)
#define PT_VK_KHR_PORTABILITY_SUBSET 0
#elif defined(PT_POSIX_MACH)
#define PT_VK_KHR_PORTABILITY_SUBSET 1
#else
#error Unknown Platform
#endif
#elif defined(PT_WIN32)
#define PT_VK_KHR_PORTABILITY_SUBSET 0
#else
#error Unknown Platform
#endif

class gfx_device_vk
{
    VkAllocationCallbacks m_vk_mcrt_allocation_callbacks;
    VkAllocationCallbacks *m_vk_allocation_callbacks;

    uint32_t m_api_version;
    VkInstance m_instance;
    PFN_vkGetInstanceProcAddr m_vk_get_instance_proc_addr;

#if (!defined(NDEBUG)) && defined(PT_VK_LAYER_KHRONOS_VALIDATION) && (PT_VK_LAYER_KHRONOS_VALIDATION)
    VkDebugReportCallbackEXT m_debug_report_callback;
#endif

    VkPhysicalDevice m_physical_device;
    uint32_t m_physical_device_pipeline_vendor_id;
    uint32_t m_physical_device_pipeline_device_id;
    mcrt_uuid m_physical_device_pipeline_cache_uuid;
    VkDeviceSize m_physical_device_limits_buffer_image_granularity;
    VkDeviceSize m_physical_device_limits_min_uniform_buffer_offset_alignment; //Dynimac Uniform Buffer
    VkDeviceSize m_physical_device_limits_optimal_buffer_copy_offset_alignment;
    VkDeviceSize m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment;
    VkDeviceSize m_physical_device_limits_non_coherent_atom_size; //we don't need non-coherent (for readback?)
    bool m_physical_device_feature_texture_compression_ASTC_LDR;
    bool m_physical_device_feature_texture_compression_BC;

    VkDevice m_device;
    PFN_vkGetDeviceProcAddr m_vk_get_device_proc_addr;
    bool m_has_dedicated_transfer_queue;
    uint32_t m_queue_graphics_family_index;
    uint32_t m_queue_transfer_family_index;
    uint32_t m_queue_graphics_queue_index;
    uint32_t m_queue_transfer_queue_index;
    VkQueue m_queue_graphics;
    VkQueue m_queue_transfer;

    PFN_vkGetPhysicalDeviceMemoryProperties m_vk_get_physical_device_memory_properties;
    PFN_vkGetPhysicalDeviceFormatProperties m_vk_get_physical_device_format_properties;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR m_vk_get_physical_device_surface_support;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR m_vk_get_physical_device_surface_capablilities;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR m_vk_get_physical_device_surface_formats;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR m_vk_get_physical_device_surface_present_modes;
    PFN_vkVoidFunction m_vk_platform_create_surface;
    PFN_vkDestroySurfaceKHR m_vk_destroy_surface;

    PFN_vkCreateBuffer m_vk_create_buffer;
    PFN_vkCreateImage m_vk_create_image;
    PFN_vkDestroyBuffer m_vk_destroy_buffer;
    PFN_vkDestroyImage m_vk_destroy_image;
    PFN_vkGetBufferMemoryRequirements m_vk_get_buffer_memory_requirements;
    PFN_vkGetImageMemoryRequirements m_vk_get_image_memory_requirements;
    PFN_vkAllocateMemory m_vk_allocate_memory;
    PFN_vkFreeMemory m_vk_free_memory;
    PFN_vkBindBufferMemory m_vk_bind_buffer_memory;
    PFN_vkBindImageMemory m_vk_bind_image_memory;
    PFN_vkMapMemory m_vk_map_memory;
    PFN_vkUnmapMemory m_vk_unmap_memory;
    PFN_vkCreateCommandPool m_vk_create_command_pool;
    PFN_vkResetCommandPool m_vk_reset_command_pool;
    PFN_vkDestroyCommandPool m_vk_destroy_command_pool;
    PFN_vkAllocateCommandBuffers m_vk_allocate_command_buffers;
    PFN_vkFreeCommandBuffers m_vk_free_command_buffers;
    PFN_vkBeginCommandBuffer m_vk_begin_command_buffer;
    PFN_vkEndCommandBuffer m_vk_end_command_buffer;
    PFN_vkCmdCopyBuffer m_vk_cmd_copy_buffer;
    PFN_vkCmdPipelineBarrier m_vk_cmd_pipeline_barrier;
    PFN_vkCmdCopyBufferToImage m_vk_cmd_copy_buffer_to_image;
    PFN_vkQueueSubmit m_vk_queue_submit;
    PFN_vkCreateFence m_vk_create_fence;
    PFN_vkWaitForFences m_vk_wait_for_fences;
    PFN_vkResetFences m_vk_reset_fences;
    PFN_vkDestroyFence m_vk_destory_fence;
    PFN_vkCreateSemaphore m_vk_create_semaphore;
    PFN_vkDestroySemaphore m_vk_destroy_semaphore;
    PFN_vkCreateSwapchainKHR m_vk_create_swapchain;
    PFN_vkDestroySwapchainKHR m_vk_destroy_swapchain;
    PFN_vkGetSwapchainImagesKHR m_vk_get_swapchain_images;
    PFN_vkCreateRenderPass m_vk_create_render_pass;
    PFN_vkDestroyRenderPass m_vk_destroy_render_pass;
    PFN_vkCreateImageView m_vk_create_image_view;
    PFN_vkDestroyImageView m_vk_destory_image_view;
    PFN_vkCreateFramebuffer m_vk_create_framebuffer;
    PFN_vkDestroyFramebuffer m_vk_destory_framebuffer;
    PFN_vkCreateSampler m_vk_create_sampler;
    PFN_vkDestroySampler m_vk_destroy_sampler;
    PFN_vkCreateDescriptorSetLayout m_vk_create_descriptor_set_layout;
    PFN_vkDestroyDescriptorSetLayout m_vk_destroy_descriptor_set_layout;
    PFN_vkCreatePipelineLayout m_vk_create_pipeline_layout;
    PFN_vkDestroyPipelineLayout m_vk_destroy_pipeline_layout;
    PFN_vkCreateDescriptorPool m_vk_create_descriptor_pool;
    PFN_vkDestroyDescriptorPool m_vk_destroy_descriptor_pool;
    PFN_vkAllocateDescriptorSets m_vk_allocate_descriptor_sets;
    PFN_vkUpdateDescriptorSets m_vk_update_descriptor_sets;
    PFN_vkCreateShaderModule m_vk_create_shader_module;
    PFN_vkDestroyShaderModule m_vk_destroy_shader_module;
    PFN_vkCreateGraphicsPipelines m_vk_create_graphics_pipelines;
    PFN_vkDestroyPipeline m_vk_destroy_pipeline;
    PFN_vkCreatePipelineCache m_vk_create_pipeline_cache;
    PFN_vkGetPipelineCacheData m_vk_get_pipeline_cache_data;
    PFN_vkDestroyPipelineCache m_vk_destroy_pipeline_cache;
    PFN_vkAcquireNextImageKHR m_vk_acquire_next_image;
    PFN_vkQueuePresentKHR m_vk_queue_present;
    PFN_vkCmdBeginRenderPass m_vk_cmd_begin_render_pass;
    PFN_vkCmdExecuteCommands m_vk_cmd_execute_commands;
    PFN_vkCmdBindPipeline m_vk_cmd_bind_pipeline;
    PFN_vkCmdSetViewport m_vk_cmd_set_viewport;
    PFN_vkCmdSetScissor m_vk_cmd_set_scissor;
    PFN_vkCmdBindVertexBuffers m_vk_cmd_bind_vertex_buffers;
    PFN_vkCmdBindIndexBuffer m_vk_cmd_bind_index_buffer;
    PFN_vkCmdPushConstants m_vk_cmd_push_constants;
    PFN_vkCmdBindDescriptorSets m_vk_cmd_bind_descriptor_sets;
    PFN_vkCmdDraw m_vk_cmd_draw;
    PFN_vkCmdDrawIndexed m_vk_cmd_draw_indexed;
    PFN_vkCmdEndRenderPass m_vk_cmd_end_render_pass;

    static char const *platform_surface_extension_name();
    bool platform_physical_device_presentation_support(PFN_vkGetInstanceProcAddr get_instance_proc_addr, VkPhysicalDevice physical_device, uint32_t queue_family_index, pt_gfx_wsi_connection_ref wsi_connection, pt_gfx_wsi_visual_ref wsi_visual);
    static char const *platform_create_surface_function_name();

public:
    gfx_device_vk();
    bool init(pt_gfx_wsi_connection_ref wsi_connection, pt_gfx_wsi_visual_ref wsi_visual);
    void destroy();
    ~gfx_device_vk();

#if (!defined(NDEBUG)) && defined(PT_VK_LAYER_KHRONOS_VALIDATION) && (PT_VK_LAYER_KHRONOS_VALIDATION)
    inline VkBool32 debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage);
#endif

    // VMA
    inline uint32_t api_version(){ return m_api_version; }
    inline VkInstance instance() { return m_instance; }
    inline PFN_vkGetInstanceProcAddr function_get_instance_proc_addr() { return m_vk_get_instance_proc_addr; }
    inline VkPhysicalDevice physical_device() { return m_physical_device; }
    inline VkDevice device() { return m_device; }
    inline PFN_vkGetDeviceProcAddr function_get_device_proc_addr() { return m_vk_get_device_proc_addr; }
    inline PFN_vkDestroyImage function_destroy_image() { return m_vk_destroy_image; }
    inline VkAllocationCallbacks* vk_allocation_callbacks() { return m_vk_allocation_callbacks; }

    inline uint32_t physical_device_pipeline_vendor_id() { return m_physical_device_pipeline_vendor_id; }
    inline uint32_t physical_device_pipeline_device_id() { return m_physical_device_pipeline_device_id; }
    inline mcrt_uuid physical_device_pipeline_cache_uuid() { return m_physical_device_pipeline_cache_uuid; }

    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_offset_alignment() { return m_physical_device_limits_optimal_buffer_copy_offset_alignment; }
    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_row_pitch_alignment() { return m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment; }

    inline bool has_dedicated_transfer_queue() { return m_has_dedicated_transfer_queue; }
    inline uint32_t queue_graphics_family_index() { return m_queue_graphics_family_index; }
    inline uint32_t queue_transfer_family_index() { return m_queue_transfer_family_index; }
    inline VkQueue queue_graphics() { return m_queue_graphics; }
    inline VkQueue queue_transfer() { return m_queue_transfer; }

    // Externally Synchronized Parameters
    // The queue parameter in vkQueueSubmit

    // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/chap4.html#fundamentals-threadingbehavior
    // vkGetPhysicalDeviceFormatProperties
    // vkCreateImage
    // vkGetBufferMemoryRequirements
    // vkGetImageMemoryRequirements
    // vkAllocateMemory
    // vkFreeMemory
    inline void get_physical_device_memory_properties(VkPhysicalDeviceMemoryProperties *memory_properties) { return m_vk_get_physical_device_memory_properties(m_physical_device, memory_properties); }
    inline void get_physical_device_format_properties(VkFormat format, VkFormatProperties *format_properties) { return m_vk_get_physical_device_format_properties(m_physical_device, format, format_properties); }
    inline VkResult create_buffer(VkBufferCreateInfo const *create_info, VkBuffer *buffer) { return m_vk_create_buffer(m_device, create_info, this->m_vk_allocation_callbacks, buffer); }
    inline VkResult create_global_image(VkImageCreateInfo const *create_info, VkImage *image) { return m_vk_create_image(m_device, create_info, this->m_vk_allocation_callbacks, image); }
    inline void destroy_global_buffer(VkBuffer buffer) { return m_vk_destroy_buffer(m_device, buffer, this->m_vk_allocation_callbacks); }
    inline void destroy_global_image(VkImage image) { return m_vk_destroy_image(m_device, image, this->m_vk_allocation_callbacks); }
    inline void get_buffer_memory_requirements(VkBuffer buffer, VkMemoryRequirements *memory_requirements) { return m_vk_get_buffer_memory_requirements(m_device, buffer, memory_requirements); }
    inline void get_image_memory_requirements(VkImage image, VkMemoryRequirements *memory_requirements) { return m_vk_get_image_memory_requirements(m_device, image, memory_requirements); }
    inline VkResult allocate_memory(VkMemoryAllocateInfo const *allocate_info, VkDeviceMemory *out_device_memory) { return m_vk_allocate_memory(m_device, allocate_info, this->m_vk_allocation_callbacks, out_device_memory); }
    inline void free_memory(VkDeviceMemory device_memory) { return m_vk_free_memory(m_device, device_memory, this->m_vk_allocation_callbacks); }
    inline VkResult bind_buffer_memory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memory_offset) { return m_vk_bind_buffer_memory(m_device, buffer, memory, memory_offset); }
    inline VkResult bind_image_memory(VkImage image, VkDeviceMemory memory, VkDeviceSize memory_offset) { return m_vk_bind_image_memory(m_device, image, memory, memory_offset); }
    inline VkResult map_memory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void **ppData) { return m_vk_map_memory(m_device, memory, offset, size, flags, ppData); }
    inline void unmap_memory(VkDeviceMemory memory) { return m_vk_unmap_memory(m_device, memory); }

    inline VkResult create_command_Pool(VkCommandPoolCreateInfo const *create_info, VkCommandPool *command_pool) { return m_vk_create_command_pool(m_device, create_info, this->m_vk_allocation_callbacks, command_pool); }
    inline VkResult reset_command_pool(VkCommandPool command_pool, VkCommandPoolResetFlags flags) { return this->m_vk_reset_command_pool(this->m_device, command_pool, flags); }
    inline void destroy_command_pool(VkCommandPool command_pool) { return this->m_vk_destroy_command_pool(this->m_device, command_pool, this->m_vk_allocation_callbacks); }

    inline VkResult allocate_command_buffers(VkCommandBufferAllocateInfo const *allocate_info, VkCommandBuffer *command_buffers) { return m_vk_allocate_command_buffers(m_device, allocate_info, command_buffers); }
    inline VkResult begin_command_buffer(VkCommandBuffer command_buffer, VkCommandBufferBeginInfo const *begin_info) { return m_vk_begin_command_buffer(command_buffer, begin_info); }
    inline VkResult end_command_buffer(VkCommandBuffer command_buffer) { return m_vk_end_command_buffer(command_buffer); }
    inline void free_command_buffers(VkCommandPool command_pool, uint32_t command_buffer_count, VkCommandBuffer const *command_buffers) { return m_vk_free_command_buffers(this->m_device, command_pool, command_buffer_count, command_buffers); }

    inline void cmd_copy_buffer(VkCommandBuffer command_buffer, VkBuffer src_buffer, VkBuffer dst_buffer, uint32_t region_count, VkBufferCopy *const regions) { return this->m_vk_cmd_copy_buffer(command_buffer, src_buffer, dst_buffer, region_count, regions); }
    inline void cmd_pipeline_barrier(VkCommandBuffer command_buffer, VkPipelineStageFlags src_stage_mask, VkPipelineStageFlags dst_stage_mask, VkDependencyFlags dependency_flags, uint32_t memory_barrier_count, VkMemoryBarrier const *memory_barriers, uint32_t buffer_memory_barrier_count, VkBufferMemoryBarrier *const buffer_memory_barriers, uint32_t image_memory_barrier_count, VkImageMemoryBarrier const *image_memory_barriers) { return m_vk_cmd_pipeline_barrier(command_buffer, src_stage_mask, dst_stage_mask, dependency_flags, memory_barrier_count, memory_barriers, buffer_memory_barrier_count, buffer_memory_barriers, image_memory_barrier_count, image_memory_barriers); }
    inline void cmd_copy_buffer_to_image(VkCommandBuffer command_buffer, VkBuffer src_buffer, VkImage dst_image, VkImageLayout dst_image_layout, uint32_t region_count, const VkBufferImageCopy *regions) { return m_vk_cmd_copy_buffer_to_image(command_buffer, src_buffer, dst_image, dst_image_layout, region_count, regions); }

    inline VkResult queue_submit(VkQueue queue, uint32_t submit_count, const VkSubmitInfo *submits, VkFence fence) { return this->m_vk_queue_submit(queue, submit_count, submits, fence); }

    inline VkResult create_fence(VkFenceCreateInfo const *create_info, VkFence *fence) { return this->m_vk_create_fence(this->m_device, create_info, this->m_vk_allocation_callbacks, fence); }
    inline VkResult wait_for_fences(uint32_t fence_count, const VkFence *fences, VkBool32 wait_all, uint64_t timeout) { return this->m_vk_wait_for_fences(this->m_device, fence_count, fences, wait_all, timeout); }
    inline VkResult reset_fences(uint32_t fence_count, const VkFence *fences) { return this->m_vk_reset_fences(this->m_device, fence_count, fences); }
    inline void destroy_fence(VkFence fence) { return this->m_vk_destory_fence(this->m_device, fence, this->m_vk_allocation_callbacks); }

    inline VkResult create_semaphore(VkSemaphoreCreateInfo const *create_info, VkSemaphore *semaphore) { return this->m_vk_create_semaphore(this->m_device, create_info, this->m_vk_allocation_callbacks, semaphore); }
    inline void destroy_semaphore(VkSemaphore semaphore) { return this->m_vk_destroy_semaphore(this->m_device, semaphore, this->m_vk_allocation_callbacks); }

    inline VkResult create_render_pass(VkRenderPassCreateInfo const *create_info, VkRenderPass *renderpass) { return this->m_vk_create_render_pass(this->m_device, create_info, this->m_vk_allocation_callbacks, renderpass); }
    inline void destroy_renderPass(VkRenderPass render_pass) { return this->m_vk_destroy_render_pass(this->m_device, render_pass, this->m_vk_allocation_callbacks); }

    inline VkResult create_image_view(VkImageViewCreateInfo const *create_info, VkImageView *view) { return this->m_vk_create_image_view(this->m_device, create_info, this->m_vk_allocation_callbacks, view); }
    inline void destroy_image_view(VkImageView image_view) { return this->m_vk_destory_image_view(this->m_device, image_view, this->m_vk_allocation_callbacks); }

    inline VkResult create_sampler(VkSamplerCreateInfo const *create_info, VkSampler *sampler) { return this->m_vk_create_sampler(this->m_device, create_info, this->m_vk_allocation_callbacks, sampler); }
    inline void destroy_sampler(VkSampler sampler) { return this->m_vk_destroy_sampler(this->m_device, sampler, this->m_vk_allocation_callbacks); }

    inline VkResult create_framebuffer(VkFramebufferCreateInfo const *create_info, VkFramebuffer *framebuffer) { return this->m_vk_create_framebuffer(this->m_device, create_info, this->m_vk_allocation_callbacks, framebuffer); }
    inline void destroy_framebuffer(VkFramebuffer framebuffer) { return this->m_vk_destory_framebuffer(this->m_device, framebuffer, this->m_vk_allocation_callbacks); }

    inline VkResult create_descriptor_set_layout(VkDescriptorSetLayoutCreateInfo const *create_info, VkDescriptorSetLayout *set_layout) { return this->m_vk_create_descriptor_set_layout(this->m_device, create_info, this->m_vk_allocation_callbacks, set_layout); }
    inline void destroy_descriptor_set_layout(VkDescriptorSetLayout descriptor_set_layout) { return this->m_vk_destroy_descriptor_set_layout(this->m_device, descriptor_set_layout, this->m_vk_allocation_callbacks); }

    inline VkResult create_pipeline_layout(VkPipelineLayoutCreateInfo const *create_info, VkPipelineLayout *pipeline_layout) { return this->m_vk_create_pipeline_layout(this->m_device, create_info, this->m_vk_allocation_callbacks, pipeline_layout); }
    inline void destroy_pipeline_layout(VkPipelineLayout pipeline_layout) { return this->m_vk_destroy_pipeline_layout(this->m_device, pipeline_layout, this->m_vk_allocation_callbacks); }

    inline VkResult create_descriptor_pool(VkDescriptorPoolCreateInfo const *create_info, VkDescriptorPool *descriptor_pool) { return this->m_vk_create_descriptor_pool(this->m_device, create_info, this->m_vk_allocation_callbacks, descriptor_pool); }
    inline void destroy_descriptor_pool(VkDescriptorPool descriptor_pool) { return this->m_vk_destroy_descriptor_pool(this->m_device, descriptor_pool, this->m_vk_allocation_callbacks); }

    inline VkResult allocate_descriptor_sets(VkDescriptorSetAllocateInfo const *allocate_info, VkDescriptorSet *descriptor_sets) { return this->m_vk_allocate_descriptor_sets(this->m_device, allocate_info, descriptor_sets); }

    inline void update_descriptor_sets(uint32_t descriptor_write_count, VkWriteDescriptorSet const *descriptor_writes, uint32_t descriptor_copy_count, VkCopyDescriptorSet const *descriptor_copies) { return this->m_vk_update_descriptor_sets(this->m_device, descriptor_write_count, descriptor_writes, descriptor_copy_count, descriptor_copies); }

    inline VkResult create_shader_module(VkShaderModuleCreateInfo const *create_info, VkShaderModule *shader_module) { return this->m_vk_create_shader_module(this->m_device, create_info, this->m_vk_allocation_callbacks, shader_module); }
    inline void destroy_shader_module(VkShaderModule shader_module) { return this->m_vk_destroy_shader_module(this->m_device, shader_module, this->m_vk_allocation_callbacks); }

    inline VkResult create_graphics_pipelines(VkPipelineCache pipeline_cache, uint32_t create_info_count, VkGraphicsPipelineCreateInfo const *create_infos, VkPipeline *pipelines) { return this->m_vk_create_graphics_pipelines(this->m_device, pipeline_cache, create_info_count, create_infos, this->m_vk_allocation_callbacks, pipelines); };
    inline void destroy_pipeline(VkPipeline pipeline) { return this->m_vk_destroy_pipeline(this->m_device, pipeline, this->m_vk_allocation_callbacks); }

    inline VkResult create_pipeline_cache(VkPipelineCacheCreateInfo const *create_info, VkPipelineCache *pipeline_cache) { return this->m_vk_create_pipeline_cache(this->m_device, create_info, this->m_vk_allocation_callbacks, pipeline_cache); }
    inline VkResult get_pipeline_cache_data(VkPipelineCache pipeline_cache, size_t *data_size, void *data) { return this->m_vk_get_pipeline_cache_data(this->m_device, pipeline_cache, data_size, data); }
    inline void destroy_pipeline_cache(VkPipelineCache pipeline_cache) { return this->m_vk_destroy_pipeline_cache(this->m_device, pipeline_cache, this->m_vk_allocation_callbacks); }

    inline VkResult acquire_next_image(VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t *image_index) { return this->m_vk_acquire_next_image(this->m_device, swapchain, timeout, semaphore, fence, image_index); }
    inline VkResult queue_present(VkQueue queue, VkPresentInfoKHR const *present_info) { return this->m_vk_queue_present(queue, present_info); }

    inline void cmd_begin_render_pass(VkCommandBuffer command_buffer, VkRenderPassBeginInfo const *render_pass_begin, VkSubpassContents contents) { return this->m_vk_cmd_begin_render_pass(command_buffer, render_pass_begin, contents); }
    inline void cmd_execute_commands(VkCommandBuffer command_buffer, uint32_t command_buffer_count, VkCommandBuffer const *command_buffers) { return this->m_vk_cmd_execute_commands(command_buffer, command_buffer_count, command_buffers); }
    inline void cmd_bind_pipeline(VkCommandBuffer command_buffer, VkPipelineBindPoint pipeline_bind_point, VkPipeline pipeline) { return this->m_vk_cmd_bind_pipeline(command_buffer, pipeline_bind_point, pipeline); }
    inline void cmd_set_viewport(VkCommandBuffer command_buffer, uint32_t first_viewport, uint32_t viewport_count, VkViewport const *viewports) { return this->m_vk_cmd_set_viewport(command_buffer, first_viewport, viewport_count, viewports); }
    inline void cmd_set_scissor(VkCommandBuffer command_buffer, uint32_t first_scissor, uint32_t scissor_count, VkRect2D const *scissors) { return this->m_vk_cmd_set_scissor(command_buffer, first_scissor, scissor_count, scissors); }
    inline void cmd_bind_vertex_buffers(VkCommandBuffer command_buffer, uint32_t first_binding, uint32_t binding_count, VkBuffer const *buffers, VkDeviceSize const *offsets) { return this->m_vk_cmd_bind_vertex_buffers(command_buffer, first_binding, binding_count, buffers, offsets); }
    inline void cmd_bind_index_buffer(VkCommandBuffer command_buffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type) { return this->m_vk_cmd_bind_index_buffer(command_buffer, buffer, offset, index_type); }
    inline void cmd_push_constants(VkCommandBuffer command_buffer, VkPipelineLayout layout, VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size, void const *values) { return this->m_vk_cmd_push_constants(command_buffer, layout, stage_flags, offset, size, values); }
    inline void cmd_bind_descriptor_sets(VkCommandBuffer command_buffer, VkPipelineBindPoint pipeline_bind_point, VkPipelineLayout layout, uint32_t first_set, uint32_t descriptor_set_count, const VkDescriptorSet *descriptor_sets, uint32_t dynamic_offset_count, const uint32_t *dynamic_offsets) { return this->m_vk_cmd_bind_descriptor_sets(command_buffer, pipeline_bind_point, layout, first_set, descriptor_set_count, descriptor_sets, dynamic_offset_count, dynamic_offsets); }
    inline void cmd_draw(VkCommandBuffer command_buffer, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) { return this->m_vk_cmd_draw(command_buffer, vertex_count, instance_count, first_vertex, first_instance); }
    inline void cmd_draw_indexed(VkCommandBuffer command_buffer, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) { return this->m_vk_cmd_draw_indexed(command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance); }
    inline void cmd_end_render_pass(VkCommandBuffer command_buffer) { return this->m_vk_cmd_end_render_pass(command_buffer); }

    VkResult platform_create_surface(VkSurfaceKHR *surface, pt_gfx_wsi_connection_ref wsi_connection, pt_gfx_wsi_window_ref wsi_window);
    inline VkResult get_physical_device_surface_support(uint32_t queue_family_index, VkSurfaceKHR surface, VkBool32 *supported) { return this->m_vk_get_physical_device_surface_support(this->m_physical_device, queue_family_index, surface, supported); }
    inline VkResult get_physical_device_surface_capablilities(VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR *surface_capabilities) { return this->m_vk_get_physical_device_surface_capablilities(this->m_physical_device, surface, surface_capabilities); }
    inline VkResult get_physical_device_surface_formats(VkSurfaceKHR surface, uint32_t *surface_format_count, VkSurfaceFormatKHR *surface_formats) { return this->m_vk_get_physical_device_surface_formats(this->m_physical_device, surface, surface_format_count, surface_formats); }
    inline VkResult get_physical_device_surface_present_modes(VkSurfaceKHR surface, uint32_t *present_mode_count, VkPresentModeKHR *present_modes) { return this->m_vk_get_physical_device_surface_present_modes(this->m_physical_device, surface, present_mode_count, present_modes); }

    inline void destroy_surface(VkSurfaceKHR surface) { return this->m_vk_destroy_surface(m_instance, surface, this->m_vk_allocation_callbacks); }

    inline VkResult create_swapchain(VkSwapchainCreateInfoKHR const *create_info, VkSwapchainKHR *swapchain) { return this->m_vk_create_swapchain(this->m_device, create_info, this->m_vk_allocation_callbacks, swapchain); }
    inline VkResult get_swapchain_images(VkSwapchainKHR swapchain, uint32_t *swapchain_image_count, VkImage *swapchain_images) { return this->m_vk_get_swapchain_images(this->m_device, swapchain, swapchain_image_count, swapchain_images); }
    inline void destroy_swapchain(VkSwapchainKHR swapchain) { return this->m_vk_destroy_swapchain(this->m_device, swapchain, this->m_vk_allocation_callbacks); }
};

#endif
