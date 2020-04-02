/*
 * Copyright (c) 2015-2016 The Khronos Group Inc.
 * Copyright (c) 2015-2016 Valve Corporation
 * Copyright (c) 2015-2016 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Chia-I Wu <olv@lunarg.com>
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: Ian Elliott <ian@LunarG.com>
 * Author: Ian Elliott <ianelliott@google.com>
 * Author: Jon Ashburn <jon@lunarg.com>
 * Author: Gwan-gyeong Mun <elongbug@gmail.com>
 * Author: Tony Barbour <tony@LunarG.com>
 * Author: Bill Hollings <bill.hollings@brenwill.com>
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>

#include <xcb/xcb.h>

#define VK_USE_PLATFORM_XCB_KHR 1
#include <vulkan/vulkan.h>

#include "linmath.h"

#define APP_SHORT_NAME "cube"
#define APP_LONG_NAME "The Vulkan Cube Demo Program"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#if defined(NDEBUG) && defined(__GNUC__)
#define U_ASSERT_ONLY __attribute__((unused))
#else
#define U_ASSERT_ONLY
#endif

/*
 * structure to track all objects related to a texture.
 */
struct texture_object
{
  VkSampler sampler;

  VkImage image;
  VkImageLayout imageLayout;

  VkMemoryAllocateInfo mem_alloc;
  VkDeviceMemory mem;
  VkImageView view;

  uint32_t tex_width;
  uint32_t tex_height;
};

struct staging_texture_object
{
  uint32_t stagingOffset;
  uint32_t bufferRowLength;
  uint32_t bufferImageHeight;
};

static int validation_error = 0;

struct vktexcube_vs_uniform
{
  float m[4][4];
};

struct vktexcube_vs_pushconstant
{
  float vp[4][4];
};

typedef struct
{
  VkImage image;
  VkImageView view;
  VkFramebuffer framebuffer;
} SwapchainImageResources;

#include <vector>

// Allow a maximum of two outstanding presentation operations.
// We can use fences to throttle if we get too far ahead of the image presents
uint32_t const FRAME_LAG = 2;

#include "VK/StagingBuffer.h"

struct demo
{
  uint32_t enabled_extension_count;
  uint32_t enabled_layer_count;
  char const *extension_names[64];
  char const *enabled_layers[64];

  PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
  PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
  PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
  PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
  PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
  PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
  PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
  PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
  PFN_vkQueuePresentKHR fpQueuePresentKHR;

  xcb_connection_t *connection;
  xcb_screen_t *screen;
  xcb_window_t xcb_window;
  xcb_atom_t atom_wm_delete_window;

  VkInstance inst;
  VkPhysicalDevice gpu;
  VkDevice device;
  VkQueue graphics_queue;
  VkQueue present_queue;
  uint32_t graphics_queue_family_index;
  uint32_t present_queue_family_index;
  VkPhysicalDeviceProperties gpu_props;
  VkQueueFamilyProperties *queue_props;
  uint32_t queue_family_count;
  VkPhysicalDeviceMemoryProperties memory_properties;
  bool separate_present_queue;

  uint32_t width;
  uint32_t height;
  VkFormat format;
  VkColorSpaceKHR color_space;
  VkSurfaceKHR surface;

  VkPresentModeKHR presentMode;
  bool prepared;

  //current_buffer related

  uint32_t current_buffer;

  std::vector<SwapchainImageResources> swapchain_image_resources;
  std::vector<SwapchainImageResources> old_swapchain_image_resources;

  VkSwapchainKHR swapchain;
  VkSwapchainKHR oldswapchain;

  struct
  {
    VkFormat format;

    VkImage image;
    VkMemoryAllocateInfo mem_alloc;
    VkDeviceMemory mem;
    VkImageView view;
  } depth;

  struct
  {
    VkImage image;
    VkDeviceMemory mem;
    VkImageView view;
  } old_depth;

  struct StagingBuffer mStagingBuffer;

  VkBuffer stagingbuffer_buffer;
  VkDeviceMemory stagingbuffer_mem;

  VkImage dds_image;
  VkDeviceMemory dds_mem;
  VkSampler dds_sampler;
  VkImageView dds_view;

  // frame_index related

  int frame_index;

  VkFence fences[FRAME_LAG];
  VkFence present_queue_fences[FRAME_LAG];

  VkCommandPool cmd_pool[FRAME_LAG];
  VkCommandBuffer cmd[FRAME_LAG];
  VkCommandPool present_cmd_pool[FRAME_LAG];
  VkCommandBuffer graphics_to_present_cmd[FRAME_LAG];

  VkSemaphore image_acquired_semaphores[FRAME_LAG];
  VkSemaphore draw_complete_semaphores[FRAME_LAG];
  VkSemaphore image_ownership_semaphores[FRAME_LAG];

  VkBuffer uniform_buffer[FRAME_LAG];
  VkDeviceMemory uniform_memory[FRAME_LAG];
  VkDescriptorSet descriptor_set[FRAME_LAG];

  VkDescriptorPool desc_pool;

  //assert related

  struct
  {
    VkBuffer buffer;
    VkMemoryAllocateInfo mem_alloc;
    VkDeviceMemory mem;
  } staging_buffer;

  VkDeviceMemory meshdata_memory;
  VkBuffer vertex_buffer[1];
  VkBuffer vertex_buffer_addition[1];

  struct staging_texture_object staging_texture[1];
  struct texture_object texture_assets[1];

  VkPipelineLayout pipeline_layout;
  VkDescriptorSetLayout desc_layout;
  VkRenderPass render_pass;
  VkPipelineCache pipelineCache;
  VkShaderModule vert_shader_module;
  VkShaderModule frag_shader_module;
  VkPipeline pipeline;

  mat4x4 projection_matrix;
  mat4x4 view_matrix;
  mat4x4 model_matrix;

  bool pause;
  float spin_angle;
  float spin_increment;

  bool quit;
  bool validate;
  bool use_break;
  bool suppress_popups;

  uint32_t curFrame;
  uint32_t frameCount;

  PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
  PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;
  PFN_vkDebugReportMessageEXT DebugReportMessage;
  VkDebugReportCallbackEXT msg_callback;
};

// Forward declaration:
static void demo_init(struct demo *demo, int argc, char **argv);

static void demo_init_connection(struct demo *demo);

static void demo_create_xcb_window(struct demo *demo);

static void demo_cleanup_xcb_window(struct demo *demo);

static void demo_init_vk(struct demo *demo);

static void demo_create_device(struct demo *demo);

static void demo_init_vk_surface(struct demo *demo);

static void demo_prepare_assets(struct demo *demo);

static void demo_prepare_textures(struct demo *demo, VkCommandBuffer tmp_cmd);

static void demo_prepare_texture_staging_buffer(struct demo *demo, struct staging_texture_object *tex_obj);

static void demo_prepare_texture_image(struct demo *demo,
                                       struct texture_object *tex_obj,
                                       VkImageTiling tiling,
                                       VkImageUsageFlags usage,
                                       VkFlags required_props);

static void demo_destroy_texture_staging_buffer(struct demo *demo, struct staging_texture_object *tex_objs);

static void demo_set_image_layout(struct demo *demo,
                                  VkCommandBuffer tmp_cmd,
                                  VkImage image,
                                  VkImageAspectFlags aspectMask,
                                  VkImageLayout old_image_layout,
                                  VkImageLayout new_image_layout,
                                  VkAccessFlags srcAccessMask,
                                  VkPipelineStageFlags src_stages,
                                  VkPipelineStageFlags dest_stages);

static void demo_flush_init_cmd(struct demo *demo, VkCommandBuffer tmp_cmd);

static void demo_prepare_cube_data_buffers(struct demo *demo);

static bool memory_type_from_properties(struct demo *demo, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);

static void demo_prepare_descriptor_layout(struct demo *demo);

static void demo_prepare_render_pass(struct demo *demo);

static void demo_prepare_pipeline(struct demo *demo);

static void demo_prepare_descriptor_pool(struct demo *demo);

static void demo_prepare_descriptor_set(struct demo *demo);

static void demo_prepare(struct demo *demo);

static void demo_prepare_stagingbuffer(struct demo *demo);

static void demo_cleanup_stagingbuffer(struct demo *demo);

static void demo_loadTexture_DDS(struct demo *demo);

static void demo_cleanupTexture_DDS(struct demo *demo);

static void demo_prepare_ringbuffer(struct demo *demo);

static void demo_load_pipeline_cache(struct demo *demo);

static void demo_store_pipeline_cache(struct demo *demo);

static void demo_prepare_swapchain(struct demo *demo);

static void demo_prepare_depth(struct demo *demo);

static void demo_prepare_framebuffers(struct demo *demo);

static void demo_cleanup(struct demo *demo);

static void demo_cleanup_swapchain(struct demo *demo);

static void demo_run_xcb(struct demo *demo);

static void demo_handle_xcb_event(struct demo *demo, const xcb_generic_event_t *event);

static void demo_draw(struct demo *demo);

static void demo_resize(struct demo *demo);

static void demo_update_data_buffer(struct demo *demo);

static void demo_update_data_pushconstant(struct demo *demo, struct vktexcube_vs_pushconstant *data_pushconstant);

static void demo_draw_build_cmd(struct demo *demo, VkCommandBuffer cmd_buf);

static void demo_build_image_ownership_cmd(struct demo *demo, int frame_index);

VKAPI_ATTR VkBool32 VKAPI_CALL BreakCallback(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
                                             uint64_t srcObject, size_t location, int32_t msgCode,
                                             const char *pLayerPrefix, const char *pMsg, void *pUserData);

VKAPI_ATTR VkBool32 VKAPI_CALL dbgFunc(VkFlags msgFlags,
                                       VkDebugReportObjectTypeEXT objType,
                                       uint64_t srcObject, size_t location,
                                       int32_t msgCode,
                                       const char *pLayerPrefix,
                                       const char *pMsg, void *pUserData);

void ERR_EXIT(char const *err_msg, char const *err_class);

void *rendermain(void *arg);

#include <new>

int main(int argc, char **argv)
{
  char buf[sizeof(demo)];
  struct demo *demo = new (buf) struct demo;

  demo->swapchain = VK_NULL_HANDLE;
  demo->oldswapchain = VK_NULL_HANDLE;
  demo->depth.view = VK_NULL_HANDLE;
  demo->depth.image = VK_NULL_HANDLE;
  demo->depth.mem = VK_NULL_HANDLE;
  demo->old_depth.view = VK_NULL_HANDLE;
  demo->old_depth.image = VK_NULL_HANDLE;
  demo->old_depth.mem = VK_NULL_HANDLE;

  demo_init(demo, argc, argv);

  demo_init_connection(demo);

  demo_create_xcb_window(demo);

  xcb_flush(demo->connection);

  //seperate message pump
  //use FRAME_LAG to throttle
  //use VK_ERROR_OUT_OF_DATE_KHR to sync with WSI window_size
  pthread_t thread;
  pthread_create(&thread, NULL, rendermain, demo);

  demo_run_xcb(demo);

  void *value_ptr;
  pthread_join(thread, &value_ptr);
  assert(value_ptr == NULL);

  demo_cleanup_xcb_window(demo);

  return validation_error;
}

void *rendermain(void *arg)
{
  struct demo *demo = static_cast<struct demo *>(arg);

  demo_init_vk(demo);

  demo_init_vk_surface(demo);

  demo_prepare_stagingbuffer(demo);

  demo_loadTexture_DDS(demo);

  demo_prepare_assets(demo);

  demo_prepare_cube_data_buffers(demo);

  demo_prepare_descriptor_layout(demo);
  demo_prepare_render_pass(demo);
  demo_prepare_pipeline(demo);

  demo_prepare_descriptor_pool(demo);
  demo_prepare_descriptor_set(demo);

  demo_prepare(demo);

  while (!demo->quit)
  {
    demo_draw(demo);
    demo->curFrame++;
    if (demo->frameCount != UINT32_MAX && demo->curFrame == demo->frameCount)
    {
      demo->quit = true;
    }
  }

  demo_cleanupTexture_DDS(demo);

  demo_cleanup_stagingbuffer(demo);

  demo_cleanup(demo);

  return NULL;
}

static void demo_run_xcb(struct demo *demo)
{
  while (!demo->quit)
  {
    xcb_generic_event_t *event = xcb_wait_for_event(demo->connection);
    if (event != NULL)
    {
      demo_handle_xcb_event(demo, event);
      free(event);
    }
    else
    {
      demo->quit = true;
    }
  }
}

static void demo_handle_xcb_event(struct demo *demo, const xcb_generic_event_t *event)
{
  uint8_t event_code = event->response_type & 0x7f;
  switch (event_code)
  {
  case XCB_KEY_RELEASE:
  {
    const xcb_key_release_event_t *key = (const xcb_key_release_event_t *)event;
    switch (key->detail)
    {
    case 0x9: // Escape
      demo->quit = true;
      break;
    case 0x71: // left arrow key
      demo->spin_angle -= demo->spin_increment;
      break;
    case 0x72: // right arrow key
      demo->spin_angle += demo->spin_increment;
      break;
    case 0x41: // space bar
      demo->pause = !demo->pause;
      break;
    }
  }
  break;
  case XCB_CLIENT_MESSAGE:
  {
    if ((*(xcb_client_message_event_t *)event).data.data32[0] == demo->atom_wm_delete_window)
    {
      demo->quit = true;
    }
  }
  break;
  default:
    break;
  }
}

static void demo_draw(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;

  // Create fences that we can use to throttle if we get too far ahead of the image presents
  while (((err = vkGetFenceStatus(demo->device, demo->fences[demo->frame_index])) != VK_SUCCESS))
  {
    assert(err == VK_NOT_READY);
    sched_yield();
  }

  // Fence manage [Re-used Resources](https://docs.microsoft.com/en-us/windows/win32/direct3d12/memory-management-strategies)
  // [RingBuffer](https://docs.microsoft.com/en-us/windows/win32/direct3d12/fence-based-resource-management) may use diffirent (number of) fences
  demo_update_data_buffer(demo);

  //update descriptor
  {
    VkDescriptorBufferInfo buffer_info;
    buffer_info.offset = 0;
    buffer_info.range = sizeof(struct vktexcube_vs_uniform);
    buffer_info.buffer = demo->uniform_buffer[demo->frame_index];

    VkDescriptorImageInfo tex_descs[1];
    memset(&tex_descs, 0, sizeof(tex_descs));
    tex_descs[0].sampler = demo->dds_sampler; //texture_assets[0].sampler;
    tex_descs[0].imageView = demo->dds_view;  //texture_assets[0].view;
    tex_descs[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet writes[2];
    memset(&writes, 0, sizeof(writes));
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &buffer_info;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding = 1;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo = tex_descs;

    writes[0].dstSet = demo->descriptor_set[demo->frame_index];
    writes[1].dstSet = demo->descriptor_set[demo->frame_index];

    vkUpdateDescriptorSets(demo->device, 2, writes, 0, NULL);
  }

  // It is believed that any operations(draw, copy, dispatch etc) in Vulkan consists of multiple stages.
  err = vkResetCommandPool(demo->device, demo->cmd_pool[demo->frame_index], 0U);
  assert(!err);

  // since only the COLOR_ATTACHMENT_OUTPUT stage of the below vkQueueSubmit waits the semphore, we can still overlap above
  // we only need to wait when populate the framebuffer field in VkRenderPassBeginInfo
  do
  {
    // Get the index of the next available swapchain image:
    err = demo->fpAcquireNextImageKHR(
        demo->device,
        demo->swapchain,
        UINT64_MAX,
        demo->image_acquired_semaphores[demo->frame_index], VK_NULL_HANDLE,
        &demo->current_buffer);

    if (err == VK_ERROR_OUT_OF_DATE_KHR)
    {
      // demo->swapchain is out of date (e.g. the window was resized) and
      // must be recreated:
      demo_resize(demo);
    }
    else if (err == VK_SUBOPTIMAL_KHR)
    {
      // demo->swapchain is not as optimal as it could be, but the platform's
      // presentation engine will still present the image correctly.
      break;
    }
    else if (err == VK_ERROR_SURFACE_LOST_KHR)
    {
      demo->quit = true;
      break;
    }
    else
    {
      assert(!err);
    }
  } while ((!demo->quit) && (err != VK_SUCCESS));

  if (demo->quit)
  {
    return;
  }

  demo_draw_build_cmd(demo, demo->cmd[demo->frame_index]);

  vkResetFences(demo->device, 1, &demo->fences[demo->frame_index]);

  VkSubmitInfo submit_info;
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.pNext = NULL;
  // Wait for the image acquired semaphore to be signaled to ensure
  // that the image won't be rendered to until the presentation
  // engine has fully released ownership to the application, and it is
  // okay to render to the image.
  VkPipelineStageFlags _wait_stages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore _wait_semphores[1] = {demo->image_acquired_semaphores[demo->frame_index]};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitDstStageMask = _wait_stages;
  submit_info.pWaitSemaphores = _wait_semphores;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &demo->cmd[demo->frame_index];
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &demo->draw_complete_semaphores[demo->frame_index];
  err = vkQueueSubmit(demo->graphics_queue, 1, &submit_info, demo->fences[demo->frame_index]);
  assert(!err);

  if (demo->separate_present_queue)
  {
    while (((err = vkGetFenceStatus(demo->device, demo->present_queue_fences[demo->frame_index])) != VK_SUCCESS))
    {
      assert(err == VK_NOT_READY);
      sched_yield();
    }

    err = vkResetCommandPool(demo->device, demo->present_cmd_pool[demo->frame_index], 0U);
    assert(!err);

    demo_build_image_ownership_cmd(demo, demo->frame_index);

    vkResetFences(demo->device, 1, &demo->present_queue_fences[demo->frame_index]);

    // If we are using separate queues, change image ownership to the
    // present queue before presenting, waiting for the draw complete
    // semaphore and signalling the ownership released semaphore when finished
    VkPipelineStageFlags _wait_stages[1] = {VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitDstStageMask = _wait_stages;
    submit_info.pWaitSemaphores = &demo->draw_complete_semaphores[demo->frame_index];
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &demo->graphics_to_present_cmd[demo->frame_index];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &demo->image_ownership_semaphores[demo->frame_index];
    err = vkQueueSubmit(demo->present_queue, 1, &submit_info, demo->present_queue_fences[demo->frame_index]);
    assert(!err);
  }

  // If we are using separate queues we have to wait for image ownership,
  // otherwise wait for draw complete
  VkPresentInfoKHR present = {
      VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      NULL,
      1,
      (demo->separate_present_queue) ? &demo->image_ownership_semaphores[demo->frame_index] : &demo->draw_complete_semaphores[demo->frame_index],
      1,
      &demo->swapchain,
      &demo->current_buffer,
      NULL};

  err = demo->fpQueuePresentKHR(demo->present_queue, &present);
  demo->frame_index += 1;
  demo->frame_index %= FRAME_LAG;

  if (err == VK_ERROR_OUT_OF_DATE_KHR)
  {
    // demo->swapchain is out of date (e.g. the window was resized) and
    // must be recreated:
    demo_resize(demo);
  }
  else if (err == VK_SUBOPTIMAL_KHR)
  {
    // demo->swapchain is not as optimal as it could be, but the platform's
    // presentation engine will still present the image correctly.
  }
  else
  {
    assert(!err);
  }

  // Store away the cache that we've populated.  This could conceivably happen
  // earlier, depends on when the pipeline cache stops being populated
  // internally.
  // demo_store_pipeline_cache(demo);
}

static void demo_resize(struct demo *demo)
{
  // Don't react to resize until after first initialization.
  if (!demo->prepared)
  {
    return;
  }

  // In order to properly resize the window, we must re-create the swapchain
  // AND redo the command buffers, etc.
  demo->prepared = false;

  assert(VK_NULL_HANDLE == demo->oldswapchain);
  demo->oldswapchain = demo->swapchain;
  demo->swapchain = VK_NULL_HANDLE;
  assert(0 == demo->old_swapchain_image_resources.size());
  demo->old_swapchain_image_resources.swap(demo->swapchain_image_resources);

  assert(VK_NULL_HANDLE == demo->old_depth.image);
  assert(VK_NULL_HANDLE == demo->old_depth.mem);
  assert(VK_NULL_HANDLE == demo->old_depth.view);
  demo->old_depth.image = demo->depth.image;
  demo->old_depth.mem = demo->depth.mem;
  demo->old_depth.view = demo->depth.view;
  demo->depth.view = VK_NULL_HANDLE;
  demo->depth.image = VK_NULL_HANDLE;
  demo->depth.mem = VK_NULL_HANDLE;

  // re-perform the demo_prepare() function, which will re-create the
  // swapchain:
  demo_prepare_swapchain(demo);

  // perform part of the demo_cleanup() function:
  vkDeviceWaitIdle(demo->device);

  for (uint32_t i = 0; i < demo->old_swapchain_image_resources.size(); i++)
  {
    vkDestroyFramebuffer(demo->device, demo->old_swapchain_image_resources[i].framebuffer, NULL);
    vkDestroyImageView(demo->device, demo->old_swapchain_image_resources[i].view, NULL);
  }
  demo->old_swapchain_image_resources.clear();

  vkDestroyImageView(demo->device, demo->old_depth.view, NULL);
  vkDestroyImage(demo->device, demo->old_depth.image, NULL);
  vkFreeMemory(demo->device, demo->old_depth.mem, NULL);
  demo->old_depth.view = VK_NULL_HANDLE;
  demo->old_depth.image = VK_NULL_HANDLE;
  demo->old_depth.mem = VK_NULL_HANDLE;

  // If we just re-created an existing swapchain, we should destroy the old
  // swapchain at this point.
  // Note: destroying the swapchain also cleans up all its associated
  // presentable images once the platform is done with them.
  if (demo->oldswapchain != VK_NULL_HANDLE)
  {
    demo->fpDestroySwapchainKHR(demo->device, demo->oldswapchain, NULL);
  }
  demo->oldswapchain = VK_NULL_HANDLE;

  demo->prepared = true;
}

static void demo_update_data_buffer(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;

  void *pdata_map;
  err = vkMapMemory(demo->device, demo->uniform_memory[demo->frame_index], 0, VK_WHOLE_SIZE, 0, &pdata_map);
  assert(!err);

  struct vktexcube_vs_uniform *pData = static_cast<struct vktexcube_vs_uniform *>(pdata_map);

  mat4x4 Model;

  if (!demo->pause)
  {
    // Rotate around the Y axis
    mat4x4_dup(Model, demo->model_matrix);
    mat4x4_rotate(demo->model_matrix, Model, 0.0f, 1.0f, 0.0f, (float)degreesToRadians(demo->spin_angle));
  }

  memcpy(&pData->m[0][0], (const void *)&demo->model_matrix[0][0], sizeof(Model));

  vkUnmapMemory(demo->device, demo->uniform_memory[demo->frame_index]);
}

static void demo_update_data_pushconstant(struct demo *demo, struct vktexcube_vs_pushconstant *data_pushconstant)
{
  mat4x4_mul(data_pushconstant->vp, demo->projection_matrix, demo->view_matrix);
}

static void demo_draw_build_cmd(struct demo *demo, VkCommandBuffer cmd_buf)
{
  VkResult U_ASSERT_ONLY err;

  struct vktexcube_vs_pushconstant data_pushconstant;
  demo_update_data_pushconstant(demo, &data_pushconstant);

  const VkCommandBufferBeginInfo cmd_buf_info = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      NULL,
      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      NULL,
  };
  const VkClearValue clear_values[2] = {
      [0] = {.color.float32 = {0.2f, 0.2f, 0.2f, 0.2f}},
      [1] = {.depthStencil = {1.0f, 0}},
  };
  const VkRenderPassBeginInfo rp_begin = {
      VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      NULL,
      demo->render_pass,
      demo->swapchain_image_resources[demo->current_buffer].framebuffer,
      {{0, 0}, {demo->width, demo->height}},
      2,
      clear_values,
  };

  err = vkBeginCommandBuffer(cmd_buf, &cmd_buf_info);
  assert(!err);

  vkCmdBeginRenderPass(cmd_buf, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, demo->pipeline);
  VkViewport viewport;
  memset(&viewport, 0, sizeof(viewport));
  viewport.height = (float)demo->height;
  viewport.width = (float)demo->width;
  viewport.minDepth = (float)0.0f;
  viewport.maxDepth = (float)1.0f;
  vkCmdSetViewport(cmd_buf, 0, 1, &viewport);
  VkRect2D scissor;
  memset(&scissor, 0, sizeof(scissor));
  scissor.extent.width = demo->width;
  scissor.extent.height = demo->height;
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  vkCmdSetScissor(cmd_buf, 0, 1, &scissor);
  vkCmdPushConstants(cmd_buf, demo->pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(vktexcube_vs_pushconstant), &data_pushconstant);
  vkCmdBindDescriptorSets(
      cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, demo->pipeline_layout, 0, 1,
      &demo->descriptor_set[demo->frame_index], 0,
      NULL);
  VkBuffer vb_[2] = {demo->vertex_buffer[0], demo->vertex_buffer_addition[0]};
  VkDeviceSize vk_offset[2] = {0, 0};
  vkCmdBindVertexBuffers(cmd_buf, 0, 2, vb_, vk_offset);
  vkCmdDraw(cmd_buf, 12 * 3, 1, 0, 0);
  // Note that ending the renderpass changes the image's layout from
  // COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR
  vkCmdEndRenderPass(cmd_buf);

  if (demo->separate_present_queue)
  {
    // We have to transfer ownership from the graphics queue family to the
    // present queue family to be able to present.  Note that we don't have
    // to transfer from present queue family back to graphics queue family at
    // the start of the next frame because we don't care about the image's
    // contents at that point.
    VkImageMemoryBarrier image_ownership_barrier = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        NULL,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        0,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        demo->graphics_queue_family_index,
        demo->present_queue_family_index,
        demo->swapchain_image_resources[demo->current_buffer].image,
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    vkCmdPipelineBarrier(
        cmd_buf,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0,
        0, NULL, 0, NULL, 1, &image_ownership_barrier);
  }
  err = vkEndCommandBuffer(cmd_buf);
  assert(!err);
}

static void demo_build_image_ownership_cmd(struct demo *demo, int frame_index)
{
  VkResult U_ASSERT_ONLY err;

  const VkCommandBufferBeginInfo cmd_buf_info = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      NULL,
      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      NULL,
  };
  err = vkBeginCommandBuffer(demo->graphics_to_present_cmd[frame_index], &cmd_buf_info);
  assert(!err);

  VkImageMemoryBarrier image_ownership_barrier = {
      VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      NULL,
      0,
      0,
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      demo->graphics_queue_family_index,
      demo->present_queue_family_index,
      demo->swapchain_image_resources[frame_index].image,
      {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

  vkCmdPipelineBarrier(
      demo->graphics_to_present_cmd[frame_index],
      VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
      0,
      0, NULL, 0, NULL, 1, &image_ownership_barrier);

  err = vkEndCommandBuffer(demo->graphics_to_present_cmd[frame_index]);
  assert(!err);
}

static void demo_init(struct demo *demo, int argc, char **argv)
{
  vec3 eye = {0.0f, 3.0f, 5.0f};
  vec3 origin = {0, 0, 0};
  vec3 up = {0.0f, 1.0f, 0.0};

  memset(demo, 0, sizeof(*demo));
  demo->presentMode = VK_PRESENT_MODE_FIFO_KHR;
  demo->frameCount = UINT32_MAX;

  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--use_staging") == 0)
    {
      fprintf(stderr, "--use_staging is deprecated and no longer does anything \n");
      continue;
    }
    if ((strcmp(argv[i], "--present_mode") == 0) && (i < argc - 1))
    {
      demo->presentMode = static_cast<VkPresentModeKHR>(atoi(argv[i + 1]));
      i++;
      continue;
    }
    if (strcmp(argv[i], "--break") == 0)
    {
      demo->use_break = true;
      continue;
    }
    if (strcmp(argv[i], "--validate") == 0)
    {
      demo->validate = true;
      continue;
    }
    if (strcmp(argv[i], "--xlib") == 0)
    {
      fprintf(stderr, "--xlib is deprecated and no longer does anything \n");
      continue;
    }
    if (strcmp(argv[i], "--c") == 0 && demo->frameCount == UINT32_MAX && i < argc - 1 && sscanf(argv[i + 1], "%" SCNu32, &demo->frameCount) == 1)
    {
      i++;
      continue;
    }
    if (strcmp(argv[i], "--suppress_popups") == 0)
    {
      demo->suppress_popups = true;
      continue;
    }

    fprintf(stderr,
            "Usage:\n  %s [--use_staging] [--validate] "
            "       [--break] [--c <framecount>] [--suppress_popups]\n"
            "       [--present_mode {0,1,2,3}]\n"
            "\n"
            "Options for --present_mode:\n"
            "  %d: VK_PRESENT_MODE_IMMEDIATE_KHR\n"
            "  %d: VK_PRESENT_MODE_MAILBOX_KHR\n"
            "  %d: VK_PRESENT_MODE_FIFO_KHR (default)\n"
            "  %d: VK_PRESENT_MODE_FIFO_RELAXED_KHR\n",
            APP_SHORT_NAME, VK_PRESENT_MODE_IMMEDIATE_KHR,
            VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR,
            VK_PRESENT_MODE_FIFO_RELAXED_KHR);
    fflush(stderr);
    exit(1);
  }

  demo->width = 500;
  demo->height = 500;

  demo->spin_angle = 4.0f;
  demo->spin_increment = 0.2f;
  demo->pause = false;

  mat4x4_perspective(demo->projection_matrix, (float)degreesToRadians(45.0f), 1.0f, 0.1f, 100.0f);
  mat4x4_look_at(demo->view_matrix, eye, origin, up);
  mat4x4_identity(demo->model_matrix);

  demo->projection_matrix[1][1] *= -1; // Flip projection matrix from GL to Vulkan orientation.
}

static void demo_init_connection(struct demo *demo)
{
  const xcb_setup_t *setup;
  xcb_screen_iterator_t iter;
  int scr;

  demo->connection = xcb_connect(NULL, &scr);
  if (xcb_connection_has_error(demo->connection) > 0)
  {
    printf("Cannot find a compatible Vulkan installable client driver "
           "(ICD).\nExiting ...\n");
    fflush(stdout);
    exit(1);
  }

  setup = xcb_get_setup(demo->connection);
  iter = xcb_setup_roots_iterator(setup);
  while (scr-- > 0)
    xcb_screen_next(&iter);

  demo->screen = iter.data;
}

static void demo_create_xcb_window(struct demo *demo)
{
  uint32_t value_mask, value_list[32];

  demo->xcb_window = xcb_generate_id(demo->connection);

  value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  value_list[0] = demo->screen->black_pixel;
  value_list[1] = XCB_EVENT_MASK_KEY_RELEASE;

  xcb_create_window(demo->connection, demo->screen->root_depth, demo->xcb_window,
                    demo->screen->root, 0, 0, demo->width, demo->height, 0,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, demo->screen->root_visual,
                    value_mask, value_list);

  /* Magic code that will send notification when window is destroyed */
  xcb_intern_atom_cookie_t cookie =
      xcb_intern_atom(demo->connection, 1, 12, "WM_PROTOCOLS");
  xcb_intern_atom_reply_t *reply =
      xcb_intern_atom_reply(demo->connection, cookie, 0);
  xcb_atom_t atom1 = reply->atom;
  free(reply);

  xcb_intern_atom_cookie_t cookie2 =
      xcb_intern_atom(demo->connection, 1, 16, "WM_DELETE_WINDOW");
  xcb_intern_atom_reply_t *reply2 =
      xcb_intern_atom_reply(demo->connection, cookie2, 0);
  demo->atom_wm_delete_window = reply2->atom;
  free(reply2);

  xcb_intern_atom_cookie_t cookie3 =
      xcb_intern_atom(demo->connection, 1, 12, "_NET_WM_NAME");
  xcb_intern_atom_reply_t *reply3 =
      xcb_intern_atom_reply(demo->connection, cookie3, 0);
  xcb_atom_t atom3 = reply3->atom;
  free(reply3);

  xcb_intern_atom_cookie_t cookie4 =
      xcb_intern_atom(demo->connection, 1, 11, "UTF8_STRING");
  xcb_intern_atom_reply_t *reply4 =
      xcb_intern_atom_reply(demo->connection, cookie4, 0);
  xcb_atom_t atom4 = reply4->atom;
  free(reply4);

  xcb_change_property(demo->connection, XCB_PROP_MODE_REPLACE, demo->xcb_window, atom3, atom4, 8U, strlen(APP_LONG_NAME), APP_LONG_NAME);

  xcb_map_window(demo->connection, demo->xcb_window);
}

static void demo_cleanup_xcb_window(struct demo *demo)
{
  xcb_destroy_window(demo->connection, demo->xcb_window);
  xcb_disconnect(demo->connection);
}

/*
 * Return 1 (true) if all layer names specified in check_names
 * can be found in given layer properties.
 */
static VkBool32 demo_check_layers(uint32_t check_count, char const **check_names,
                                  uint32_t layer_count,
                                  VkLayerProperties *layers)
{
  for (uint32_t i = 0; i < check_count; i++)
  {
    VkBool32 found = 0;
    for (uint32_t j = 0; j < layer_count; j++)
    {
      if (!strcmp(check_names[i], layers[j].layerName))
      {
        found = 1;
        break;
      }
    }
    if (!found)
    {
      fprintf(stderr, "Cannot find layer: %s\n", check_names[i]);
      return 0;
    }
  }
  return 1;
}

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                           \
  {                                                                        \
    demo->fp##entrypoint =                                                 \
        (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint); \
    if (demo->fp##entrypoint == NULL)                                      \
    {                                                                      \
      ERR_EXIT("vkGetInstanceProcAddr failed to find vk" #entrypoint,      \
               "vkGetInstanceProcAddr Failure");                           \
    }                                                                      \
  }

static PFN_vkGetDeviceProcAddr g_gdpa = NULL;

#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                 \
  {                                                                           \
    if (!g_gdpa)                                                              \
      g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(                \
          demo->inst, "vkGetDeviceProcAddr");                                 \
    demo->fp##entrypoint = (PFN_vk##entrypoint)g_gdpa(dev, "vk" #entrypoint); \
    if (demo->fp##entrypoint == NULL)                                         \
    {                                                                         \
      ERR_EXIT("vkGetDeviceProcAddr failed to find vk" #entrypoint,           \
               "vkGetDeviceProcAddr Failure");                                \
    }                                                                         \
  }

static void demo_init_vk(struct demo *demo)
{
  VkResult err;
  uint32_t instance_extension_count = 0;
  uint32_t instance_layer_count = 0;
  uint32_t validation_layer_count = 0;
  char const **instance_validation_layers = NULL;
  demo->enabled_extension_count = 0;
  demo->enabled_layer_count = 0;

  char const *instance_validation_layers_alt1[] = {
      "VK_LAYER_LUNARG_standard_validation"};

  char const *instance_validation_layers_alt2[] = {
      "VK_LAYER_GOOGLE_threading", "VK_LAYER_LUNARG_parameter_validation",
      "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_core_validation",
      "VK_LAYER_GOOGLE_unique_objects"};

  /* Look for validation layers */
  VkBool32 validation_found = 0;
  if (demo->validate)
  {
    err = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
    assert(!err);

    instance_validation_layers = instance_validation_layers_alt1;
    if (instance_layer_count > 0)
    {
      VkLayerProperties *instance_layers = static_cast<VkLayerProperties *>(
          malloc(sizeof(VkLayerProperties) * instance_layer_count));

      err = vkEnumerateInstanceLayerProperties(&instance_layer_count, instance_layers);
      assert(!err);

      validation_found = demo_check_layers(ARRAY_SIZE(instance_validation_layers_alt1), instance_validation_layers, instance_layer_count, instance_layers);
      if (validation_found)
      {
        demo->enabled_layer_count = ARRAY_SIZE(instance_validation_layers_alt1);
        demo->enabled_layers[0] = "VK_LAYER_LUNARG_standard_validation";
        validation_layer_count = 1;
      }
      else
      {
        // use alternative set of validation layers
        instance_validation_layers = instance_validation_layers_alt2;
        demo->enabled_layer_count = ARRAY_SIZE(instance_validation_layers_alt2);
        validation_found = demo_check_layers(ARRAY_SIZE(instance_validation_layers_alt2), instance_validation_layers, instance_layer_count, instance_layers);
        validation_layer_count = ARRAY_SIZE(instance_validation_layers_alt2);
        for (uint32_t i = 0; i < validation_layer_count; i++)
        {
          demo->enabled_layers[i] = instance_validation_layers[i];
        }
      }
      free(instance_layers);
    }

    if (!validation_found)
    {
      ERR_EXIT("vkEnumerateInstanceLayerProperties failed to find "
               "required validation layer.\n\n"
               "Please look at the Getting Started guide for additional "
               "information.\n",
               "vkCreateInstance Failure");
    }
  }

  /* Look for instance extensions */
  VkBool32 surfaceExtFound = 0;
  VkBool32 platformSurfaceExtFound = 0;
  memset(demo->extension_names, 0, sizeof(demo->extension_names));

  err = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count,
                                               NULL);
  assert(!err);

  if (instance_extension_count > 0)
  {
    VkExtensionProperties *instance_extensions = static_cast<VkExtensionProperties *>(malloc(sizeof(VkExtensionProperties) * instance_extension_count));
    err = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, instance_extensions);
    assert(!err);
    for (uint32_t i = 0; i < instance_extension_count; i++)
    {
      if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName))
      {
        surfaceExtFound = 1;
        demo->extension_names[demo->enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
      }
      if (!strcmp(VK_KHR_XCB_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName))
      {
        platformSurfaceExtFound = 1;
        demo->extension_names[demo->enabled_extension_count++] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
      }
      if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, instance_extensions[i].extensionName))
      {
        if (demo->validate)
        {
          demo->extension_names[demo->enabled_extension_count++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
        }
      }
      assert(demo->enabled_extension_count < 64);
    }

    free(instance_extensions);
  }

  if (!surfaceExtFound)
  {
    ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find "
             "the " VK_KHR_SURFACE_EXTENSION_NAME
             " extension.\n\nDo you have a compatible "
             "Vulkan installable client driver (ICD) installed?\nPlease "
             "look at the Getting Started guide for additional "
             "information.\n",
             "vkCreateInstance Failure");
  }
  if (!platformSurfaceExtFound)
  {
    ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find "
             "the " VK_KHR_XCB_SURFACE_EXTENSION_NAME
             " extension.\n\nDo you have a compatible "
             "Vulkan installable client driver (ICD) installed?\nPlease "
             "look at the Getting Started guide for additional "
             "information.\n",
             "vkCreateInstance Failure");
  }
  const VkApplicationInfo app = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = NULL,
      .pApplicationName = APP_SHORT_NAME,
      .applicationVersion = 0,
      .pEngineName = APP_SHORT_NAME,
      .engineVersion = 0,
      .apiVersion = VK_API_VERSION_1_0,
  };
  VkInstanceCreateInfo inst_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = NULL,
      .pApplicationInfo = &app,
      .enabledLayerCount = demo->enabled_layer_count,
      .ppEnabledLayerNames = (const char *const *)instance_validation_layers,
      .enabledExtensionCount = demo->enabled_extension_count,
      .ppEnabledExtensionNames = (const char *const *)demo->extension_names,
  };

  /*
   * This is info for a temp callback to use during CreateInstance.
   * After the instance is created, we use the instance-based
   * function to register the final callback.
   */
  VkDebugReportCallbackCreateInfoEXT dbgCreateInfoTemp;
  if (demo->validate)
  {
    dbgCreateInfoTemp.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    dbgCreateInfoTemp.pNext = NULL;
    dbgCreateInfoTemp.pfnCallback = demo->use_break ? BreakCallback : dbgFunc;
    dbgCreateInfoTemp.pUserData = demo;
    dbgCreateInfoTemp.flags =
        VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    inst_info.pNext = &dbgCreateInfoTemp;
  }

  uint32_t gpu_count;

  err = vkCreateInstance(&inst_info, NULL, &demo->inst);
  if (err == VK_ERROR_INCOMPATIBLE_DRIVER)
  {
    ERR_EXIT("Cannot find a compatible Vulkan installable client driver "
             "(ICD).\n\nPlease look at the Getting Started guide for "
             "additional information.\n",
             "vkCreateInstance Failure");
  }
  else if (err == VK_ERROR_EXTENSION_NOT_PRESENT)
  {
    ERR_EXIT("Cannot find a specified extension library"
             ".\nMake sure your layers path is set appropriately.\n",
             "vkCreateInstance Failure");
  }
  else if (err)
  {
    ERR_EXIT("vkCreateInstance failed.\n\nDo you have a compatible Vulkan "
             "installable client driver (ICD) installed?\nPlease look at "
             "the Getting Started guide for additional information.\n",
             "vkCreateInstance Failure");
  }

  /* Make initial call to query gpu_count, then second call for gpu info*/
  err = vkEnumeratePhysicalDevices(demo->inst, &gpu_count, NULL);
  assert(!err && gpu_count > 0);

  if (gpu_count > 0)
  {
    VkPhysicalDevice *physical_devices = static_cast<VkPhysicalDevice *>(
        malloc(sizeof(VkPhysicalDevice) * gpu_count));
    err = vkEnumeratePhysicalDevices(demo->inst, &gpu_count, physical_devices);
    assert(!err);
    /* For cube demo we just grab the first physical device */
    demo->gpu = physical_devices[0];
    free(physical_devices);
  }
  else
  {
    ERR_EXIT("vkEnumeratePhysicalDevices reported zero accessible devices.\n\n"
             "Do you have a compatible Vulkan installable client driver (ICD) "
             "installed?\nPlease look at the Getting Started guide for "
             "additional information.\n",
             "vkEnumeratePhysicalDevices Failure");
  }

  /* Look for device extensions */
  uint32_t device_extension_count = 0;
  VkBool32 swapchainExtFound = 0;
  demo->enabled_extension_count = 0;
  memset(demo->extension_names, 0, sizeof(demo->extension_names));

  err = vkEnumerateDeviceExtensionProperties(demo->gpu, NULL,
                                             &device_extension_count, NULL);
  assert(!err);

  if (device_extension_count > 0)
  {
    VkExtensionProperties *device_extensions =
        static_cast<VkExtensionProperties *>(
            malloc(sizeof(VkExtensionProperties) * device_extension_count));
    err = vkEnumerateDeviceExtensionProperties(
        demo->gpu, NULL, &device_extension_count, device_extensions);
    assert(!err);

    for (uint32_t i = 0; i < device_extension_count; i++)
    {
      if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                  device_extensions[i].extensionName))
      {
        swapchainExtFound = 1;
        demo->extension_names[demo->enabled_extension_count++] =
            VK_KHR_SWAPCHAIN_EXTENSION_NAME;
      }
      assert(demo->enabled_extension_count < 64);
    }

    free(device_extensions);
  }

  if (!swapchainExtFound)
  {
    ERR_EXIT("vkEnumerateDeviceExtensionProperties failed to find "
             "the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
             " extension.\n\nDo you have a compatible "
             "Vulkan installable client driver (ICD) installed?\nPlease "
             "look at the Getting Started guide for additional "
             "information.\n",
             "vkCreateInstance Failure");
  }

  if (demo->validate)
  {
    demo->CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(demo->inst, "vkCreateDebugReportCallbackEXT");
    demo->DestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(demo->inst, "vkDestroyDebugReportCallbackEXT");
    if (!demo->CreateDebugReportCallback)
    {
      ERR_EXIT("GetProcAddr: Unable to find vkCreateDebugReportCallbackEXT\n", "vkGetProcAddr Failure");
    }
    if (!demo->DestroyDebugReportCallback)
    {
      ERR_EXIT("GetProcAddr: Unable to find vkDestroyDebugReportCallbackEXT\n", "vkGetProcAddr Failure");
    }
    demo->DebugReportMessage = (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(demo->inst, "vkDebugReportMessageEXT");
    if (!demo->DebugReportMessage)
    {
      ERR_EXIT("GetProcAddr: Unable to find vkDebugReportMessageEXT\n", "vkGetProcAddr Failure");
    }

    VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
    dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    dbgCreateInfo.pNext = NULL;
    dbgCreateInfo.pfnCallback = demo->use_break ? BreakCallback : dbgFunc;
    dbgCreateInfo.pUserData = demo;
    dbgCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    err = demo->CreateDebugReportCallback(demo->inst, &dbgCreateInfo, NULL, &demo->msg_callback);
    switch (err)
    {
    case VK_SUCCESS:
      break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      ERR_EXIT("CreateDebugReportCallback: out of host memory\n", "CreateDebugReportCallback Failure");
      break;
    default:
      ERR_EXIT("CreateDebugReportCallback: unknown failure\n", "CreateDebugReportCallback Failure");
      break;
    }
  }

  vkGetPhysicalDeviceProperties(demo->gpu, &demo->gpu_props);

  /* Call with NULL data to get count */
  vkGetPhysicalDeviceQueueFamilyProperties(demo->gpu, &demo->queue_family_count, NULL);
  assert(demo->queue_family_count >= 1);

  demo->queue_props = (VkQueueFamilyProperties *)malloc(demo->queue_family_count * sizeof(VkQueueFamilyProperties));
  vkGetPhysicalDeviceQueueFamilyProperties(demo->gpu, &demo->queue_family_count, demo->queue_props);

  // Query fine-grained feature support for this device.
  //  If app has specific feature requirements it should check supported
  //  features based on this query
  VkPhysicalDeviceFeatures physDevFeatures;
  vkGetPhysicalDeviceFeatures(demo->gpu, &physDevFeatures);

  GET_INSTANCE_PROC_ADDR(demo->inst, GetPhysicalDeviceSurfaceSupportKHR);
  GET_INSTANCE_PROC_ADDR(demo->inst, GetPhysicalDeviceSurfaceCapabilitiesKHR);
  GET_INSTANCE_PROC_ADDR(demo->inst, GetPhysicalDeviceSurfaceFormatsKHR);
  GET_INSTANCE_PROC_ADDR(demo->inst, GetPhysicalDeviceSurfacePresentModesKHR);
  GET_INSTANCE_PROC_ADDR(demo->inst, GetSwapchainImagesKHR);

  // Iterate over each queue to learn whether it supports presenting:
  VkBool32 *supportsPresent = (VkBool32 *)malloc(demo->queue_family_count * sizeof(VkBool32));
  for (uint32_t i = 0; i < demo->queue_family_count; i++)
  {

    supportsPresent[i] = reinterpret_cast<PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR>(vkGetInstanceProcAddr(demo->inst, "vkGetPhysicalDeviceXcbPresentationSupportKHR"))(demo->gpu, i, demo->connection, demo->screen->root_visual);
  }

  // Search for a graphics and a present queue in the array of queue
  // families, try to find one that supports both
  uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
  uint32_t presentQueueFamilyIndex = UINT32_MAX;
  for (uint32_t i = 0; i < demo->queue_family_count; i++)
  {
    if ((demo->queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
    {
      if (graphicsQueueFamilyIndex == UINT32_MAX)
      {
        graphicsQueueFamilyIndex = i;
      }

      if (supportsPresent[i] == VK_TRUE)
      {
        graphicsQueueFamilyIndex = i;
        presentQueueFamilyIndex = i;
        break;
      }
    }
  }

  if (presentQueueFamilyIndex == UINT32_MAX)
  {
    // If didn't find a queue that supports both graphics and present, then
    // find a separate present queue.
    for (uint32_t i = 0; i < demo->queue_family_count; ++i)
    {
      if (supportsPresent[i] == VK_TRUE)
      {
        presentQueueFamilyIndex = i;
        break;
      }
    }
  }

  // Generate error if could not find both a graphics and a present queue
  if (graphicsQueueFamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX)
  {
    ERR_EXIT("Could not find both graphics and present queues\n", "Swapchain Initialization Failure");
  }

  demo->graphics_queue_family_index = graphicsQueueFamilyIndex;
  demo->present_queue_family_index = presentQueueFamilyIndex;
  demo->separate_present_queue = (demo->graphics_queue_family_index != demo->present_queue_family_index);
  free(supportsPresent);

  demo_create_device(demo);

  GET_DEVICE_PROC_ADDR(demo->device, CreateSwapchainKHR);
  GET_DEVICE_PROC_ADDR(demo->device, DestroySwapchainKHR);
  GET_DEVICE_PROC_ADDR(demo->device, GetSwapchainImagesKHR);
  GET_DEVICE_PROC_ADDR(demo->device, AcquireNextImageKHR);
  GET_DEVICE_PROC_ADDR(demo->device, QueuePresentKHR);

  vkGetDeviceQueue(demo->device, demo->graphics_queue_family_index, 0, &demo->graphics_queue);

  if (!demo->separate_present_queue)
  {
    demo->present_queue = demo->graphics_queue;
  }
  else
  {
    vkGetDeviceQueue(demo->device, demo->present_queue_family_index, 0, &demo->present_queue);
  }
  demo->quit = false;
  demo->curFrame = 0;

  // Create semaphores to synchronize acquiring presentable buffers before
  // rendering and waiting for drawing to be complete before presenting
  VkSemaphoreCreateInfo semaphoreCreateInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, NULL, 0};

  // Create fences that we can use to throttle if we get too far
  // ahead of the image presents
  VkFenceCreateInfo fence_ci = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, NULL, VK_FENCE_CREATE_SIGNALED_BIT};

  for (uint32_t i = 0; i < FRAME_LAG; i++)
  {
    err = vkCreateFence(demo->device, &fence_ci, NULL, &demo->fences[i]);
    assert(!err);

    err = vkCreateSemaphore(demo->device, &semaphoreCreateInfo, NULL, &demo->image_acquired_semaphores[i]);
    assert(!err);

    err = vkCreateSemaphore(demo->device, &semaphoreCreateInfo, NULL, &demo->draw_complete_semaphores[i]);
    assert(!err);

    if (demo->separate_present_queue)
    {
      err = vkCreateFence(demo->device, &fence_ci, NULL, &demo->present_queue_fences[i]);
      assert(!err);

      err = vkCreateSemaphore(demo->device, &semaphoreCreateInfo, NULL, &demo->image_ownership_semaphores[i]);
      assert(!err);
    }
  }
  demo->frame_index = 0;

  // Get Memory information and properties
  vkGetPhysicalDeviceMemoryProperties(demo->gpu, &demo->memory_properties);
}

#undef GET_DEVICE_PROC_ADDR
#undef GET_INSTANCE_PROC_ADDR

static void demo_create_device(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;
  float queue_priorities[1] = {0.0};
  VkDeviceQueueCreateInfo queues[2];
  queues[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queues[0].pNext = NULL;
  queues[0].queueFamilyIndex = demo->graphics_queue_family_index;
  queues[0].queueCount = 1;
  queues[0].pQueuePriorities = queue_priorities;
  queues[0].flags = 0;

  VkPhysicalDeviceFeatures enableFeatures = {0};
  enableFeatures.textureCompressionASTC_LDR = VK_TRUE;
  enableFeatures.textureCompressionBC = VK_TRUE;

  VkDeviceCreateInfo device = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = NULL,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = queues,
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = NULL,
      .enabledExtensionCount = demo->enabled_extension_count,
      .ppEnabledExtensionNames = (const char *const *)demo->extension_names,
      .pEnabledFeatures =
          &enableFeatures // If specific features are required, pass them in here
  };
  if (demo->separate_present_queue)
  {
    queues[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queues[1].pNext = NULL;
    queues[1].queueFamilyIndex = demo->present_queue_family_index;
    queues[1].queueCount = 1;
    queues[1].pQueuePriorities = queue_priorities;
    queues[1].flags = 0;
    device.queueCreateInfoCount = 2;
  }
  err = vkCreateDevice(demo->gpu, &device, NULL, &demo->device);
  assert(!err);
}

static void demo_init_vk_surface(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;

  //create_surface
  VkXcbSurfaceCreateInfoKHR createInfo;
  createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
  createInfo.pNext = NULL;
  createInfo.flags = 0;
  createInfo.connection = demo->connection;
  createInfo.window = demo->xcb_window;
  err = reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(vkGetInstanceProcAddr(demo->inst, "vkCreateXcbSurfaceKHR"))(demo->inst, &createInfo, NULL, &demo->surface);
  assert(!err);

  // this should be guaranteed by the platform-specific can_present call
  VkBool32 supported;
  err = demo->fpGetPhysicalDeviceSurfaceSupportKHR(demo->gpu, demo->present_queue_family_index, demo->surface, &supported);
  assert(VK_SUCCESS == err);
  assert(supported);

  // Get the list of VkFormat's that are supported:
  uint32_t formatCount;
  err = demo->fpGetPhysicalDeviceSurfaceFormatsKHR(demo->gpu, demo->surface, &formatCount, NULL);
  assert(!err);
  VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
  err = demo->fpGetPhysicalDeviceSurfaceFormatsKHR(demo->gpu, demo->surface, &formatCount, surfFormats);
  assert(!err);
  // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
  // the surface has no preferred format.  Otherwise, at least one
  // supported format will be returned.
  if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
  {
    demo->format = VK_FORMAT_B8G8R8A8_UNORM;
  }
  else
  {
    assert(formatCount >= 1);
    demo->format = surfFormats[0].format;
  }
  demo->color_space = surfFormats[0].colorSpace;
  free(surfFormats);
}

static void demo_prepare_assets(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;

  const VkCommandPoolCreateInfo cmd_pool_info = {
      VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      NULL,
      0,
      demo->graphics_queue_family_index,
  };

  VkCommandPool tmp_cmd_pool;
  err = vkCreateCommandPool(demo->device, &cmd_pool_info, NULL, &tmp_cmd_pool);
  assert(!err);

  VkCommandBuffer tmp_cmd;
  const VkCommandBufferAllocateInfo cmd = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      NULL,
      tmp_cmd_pool,
      VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      1,
  };
  err = vkAllocateCommandBuffers(demo->device, &cmd, &tmp_cmd);
  assert(!err);

  const VkCommandBufferBeginInfo cmd_buf_info = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      NULL,
      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      NULL,
  };
  err = vkBeginCommandBuffer(tmp_cmd, &cmd_buf_info);
  assert(!err);

  demo_prepare_textures(demo, tmp_cmd);

  /*
   * Prepare functions above may generate pipeline commands
   * that need to be flushed before beginning the render loop.
   */
  demo_flush_init_cmd(demo, tmp_cmd);

  vkFreeCommandBuffers(demo->device, tmp_cmd_pool, 1, &tmp_cmd);

  vkDestroyCommandPool(demo->device, tmp_cmd_pool, NULL);

  demo_destroy_texture_staging_buffer(demo, &demo->staging_texture[0]);
}

static void demo_prepare_textures(struct demo *demo, VkCommandBuffer tmp_cmd)
{
  const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
  VkFormatProperties props;

  vkGetPhysicalDeviceFormatProperties(demo->gpu, tex_format, &props);

  VkResult U_ASSERT_ONLY err;

  // TextureVk::setSubImage libANGLE/renderer/vulkan/TextureVk.cpp
  // ImageHelper::stageSubresourceUpdateImpl libANGLE/renderer/vulkan/TextureVk.cpp

  // BuildInternalFormatInfoMap libANGLE/formatutils.cpp
  // From ES 3.0.1 spec, table 3.12
  //                   | Internal format     |sized| R | G | B | A |S | Format         | Type                             | Component type        | SRGB | Texture supported                                | Filterable     | Texture attachment                               | Renderbuffer                                   | Blend
  //AddRGBAFormat(&map, GL_RGBA8,             true,  8,  8,  8,  8, 0, GL_RGBA,         GL_UNSIGNED_BYTE,                  GL_UNSIGNED_NORMALIZED, false, RequireESOrExt<3, 0, &Extensions::textureStorage>, AlwaysSupported, RequireESOrExt<3, 0, &Extensions::textureStorage>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>);
  // From EXT_texture_compression_bptc
  //                         | Internal format                         | W | H |D | BS |CC| SRGB | Texture supported                              | Filterable     | Texture attachment | Renderbuffer  | Blend
  //AddCompressedFormat(&map, GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,         4,  4, 1, 128, 4, false, RequireExt<&Extensions::textureCompressionBPTC>, AlwaysSupported, NeverSupported,      NeverSupported, NeverSupported);

  // gFormatInfoTable libANGLE/renderer/Format_table_autogen.cpp
  // { FormatID::R8G8B8A8_UNORM, GL_RGBA8, GL_RGBA8, GenerateMip<R8G8B8A8>, NoCopyFunctions, ReadColor<R8G8B8A8, GLfloat>, WriteColor<R8G8B8A8, GLfloat>, GL_UNSIGNED_NORMALIZED, 8, 8, 8, 8, 0, 0, 0, 4, 0, false, false, false, gl::VertexAttribType::UnsignedByte },

  // ImageHelper::stageSubresourceUpdateImpl libANGLE/renderer/vulkan/TextureVk.cpp

  if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)
  {
    /* Must use staging buffer to copy linear texture to optimized */

    demo_prepare_texture_staging_buffer(demo, &demo->staging_texture[0]);

    demo_prepare_texture_image(demo, &demo->texture_assets[0], VK_IMAGE_TILING_OPTIMAL, (VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    demo_set_image_layout(
        demo,
        tmp_cmd,
        demo->texture_assets[0].image,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        0,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT);

    VkBufferImageCopy copy_region = {
        demo->staging_texture[0].stagingOffset,
        demo->staging_texture[0].bufferRowLength,
        demo->staging_texture[0].bufferImageHeight,
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
        {0, 0, 0},
        {demo->texture_assets[0].tex_width, demo->texture_assets[0].tex_height, 1}};

    vkCmdCopyBufferToImage(tmp_cmd, demo->staging_buffer.buffer, demo->texture_assets[0].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

    demo_set_image_layout(
        demo, tmp_cmd, demo->texture_assets[0].image, VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, demo->texture_assets[0].imageLayout,
        VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
  }
  else
  {
    /* Can't support VK_FORMAT_R8G8B8A8_UNORM !? */
    assert(!"No support for R8G8B8A8_UNORM as texture image format");
  }

  const VkSamplerCreateInfo sampler = {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .pNext = NULL,
      .magFilter = VK_FILTER_NEAREST,
      .minFilter = VK_FILTER_NEAREST,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
      .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .mipLodBias = 0.0f,
      .anisotropyEnable = VK_FALSE,
      .maxAnisotropy = 1,
      .compareOp = VK_COMPARE_OP_NEVER,
      .minLod = 0.0f,
      .maxLod = 0.0f,
      .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
      .unnormalizedCoordinates = VK_FALSE,
  };

  VkImageViewCreateInfo view = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext = NULL,
      .image = VK_NULL_HANDLE,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = tex_format,
      .components =
          {
              VK_COMPONENT_SWIZZLE_R,
              VK_COMPONENT_SWIZZLE_G,
              VK_COMPONENT_SWIZZLE_B,
              VK_COMPONENT_SWIZZLE_A,
          },
      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
      .flags = 0,
  };

  /* create sampler */
  err = vkCreateSampler(demo->device, &sampler, NULL, &demo->texture_assets[0].sampler);
  assert(!err);

  /* create image view */
  view.image = demo->texture_assets[0].image;
  err = vkCreateImageView(demo->device, &view, NULL, &demo->texture_assets[0].view);
  assert(!err);
}

bool loadTexture_PPM(uint8_t *rgba_data, uint32_t const *outputRowPitch, uint32_t *width, uint32_t *height)
{
#include "generated/lunarg.ppm.h"

  char *cPtr = (char *)lunarg_ppm;
  if ((unsigned char *)cPtr >= (lunarg_ppm + lunarg_ppm_len) || strncmp(cPtr, "P6\n", 3))
  {
    return false;
  }

  cPtr = strchr(cPtr, '\n');
  if (NULL == cPtr)
  {
    return false;
  }
  ++cPtr;

  if (sscanf(cPtr, "%u %u", width, height) != 2)
  {
    return false;
  }

  if (rgba_data == NULL)
  {
    return true;
  }

  cPtr = strchr(cPtr, '\n');
  if (NULL == cPtr)
  {
    return false;
  }
  ++cPtr;

  if ((unsigned char *)cPtr >= (lunarg_ppm + lunarg_ppm_len) || strncmp(cPtr, "255\n", 4))
  {
    return false;
  }

  cPtr = strchr(cPtr, '\n');
  if (NULL == cPtr)
  {
    return false;
  }
  ++cPtr;

  for (int y = 0; y < *height; y++)
  {
    uint8_t *rowPtr = rgba_data;
    for (int x = 0; x < *width; x++)
    {
      memcpy(rowPtr, cPtr, 3);
      rowPtr[3] = 255; /* Alpha of 1 */
      rowPtr += 4;
      cPtr += 3;
    }
    rgba_data += (*outputRowPitch);
  }

  return true;
}

#include "generated/l_hires-NVIDIA.dds.h"

#include "TextureLoader_DDS.h"
#include "VK/TextureLoader_VK.h"
static void demo_loadTexture_DDS(struct demo *demo)
{
  struct TextureLoader_NeutralHeader header;
  size_t header_offset = 0;
  DDSTextureLoader_LoadHeaderFromMemory(_________Assets_Lenna_l_hires_NVIDIA_dds, _________Assets_Lenna_l_hires_NVIDIA_dds_len, &header, &header_offset);

  struct TextureLoader_SpecificHeader vkheader = TextureLoader_ToSpecificHeader(&header);

  VkFormatProperties props;
  vkGetPhysicalDeviceFormatProperties(demo->gpu, vkheader.format, &props);
  assert(props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);


  VkFormatProperties props1;
  vkGetPhysicalDeviceFormatProperties(demo->gpu, VK_FORMAT_ASTC_4x4_UNORM_BLOCK, &props1);
  assert(props1.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

  uint32_t NumSubresource = TextureLoader_GetFormatAspectCount(vkheader.format) * vkheader.arrayLayers * vkheader.mipLevels;

  struct TextureLoader_MemcpyDest dest[15];
  struct VkBufferImageCopy regions[15];
  size_t TotalSize = TextureLoader_GetCopyableFootprints(&vkheader,
                                                         demo->gpu_props.limits.optimalBufferCopyOffsetAlignment, demo->gpu_props.limits.optimalBufferCopyRowPitchAlignment,
                                                         NumSubresource, dest, regions);

  uint8_t *ptr;
  VkDeviceSize offset;
  demo->mStagingBuffer.allocate(TotalSize, demo->gpu_props.limits.optimalBufferCopyOffsetAlignment, &ptr, &offset);

  for (int i = 0; i < NumSubresource; ++i)
  {
    dest[i].stagingOffset += offset;
  }

  for (int i = 0; i < NumSubresource; ++i)
  {
    regions[i].bufferOffset += offset;
  }

  DDSTextureLoader_FillDataFromMemory(_________Assets_Lenna_l_hires_NVIDIA_dds, _________Assets_Lenna_l_hires_NVIDIA_dds_len, ptr, NumSubresource, dest, &header, &header_offset);

  VkResult U_ASSERT_ONLY err;
  bool U_ASSERT_ONLY pass;

  //prepare cmd buffer

  const VkCommandPoolCreateInfo cmd_pool_info = {
      VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      NULL,
      0,
      demo->graphics_queue_family_index,
  };

  VkCommandPool tmp_cmd_pool;
  err = vkCreateCommandPool(demo->device, &cmd_pool_info, NULL, &tmp_cmd_pool);
  assert(!err);

  VkCommandBuffer tmp_cmd;
  const VkCommandBufferAllocateInfo cmd = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      NULL,
      tmp_cmd_pool,
      VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      1,
  };
  err = vkAllocateCommandBuffers(demo->device, &cmd, &tmp_cmd);
  assert(!err);

  const VkCommandBufferBeginInfo cmd_buf_info = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      NULL,
      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      NULL,
  };
  err = vkBeginCommandBuffer(tmp_cmd, &cmd_buf_info);
  assert(!err);

  //prepare_image

  struct VkImageCreateInfo const image_create_info = {
      VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      NULL,
      (!vkheader.isCubeCompatible) ? 0U : VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
      vkheader.imageType,
      vkheader.format,
      {vkheader.extent.width, vkheader.extent.height, vkheader.extent.depth},
      vkheader.mipLevels,
      vkheader.arrayLayers,
      VK_SAMPLE_COUNT_1_BIT,
      VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      0U,
      NULL,
      VK_IMAGE_LAYOUT_UNDEFINED};

  err = vkCreateImage(demo->device, &image_create_info, NULL, &demo->dds_image);
  assert(!err);

  VkMemoryRequirements mem_reqs;
  vkGetImageMemoryRequirements(demo->device, demo->dds_image, &mem_reqs);

  uint32_t typeIndex;
  pass = memory_type_from_properties(demo, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &typeIndex);
  assert(pass);

  VkMemoryAllocateInfo dds_mem_alloc = {
      VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      NULL,
      mem_reqs.size,
      typeIndex};

  err = vkAllocateMemory(demo->device, &dds_mem_alloc, NULL, &demo->dds_mem);
  assert(!err);

  err = vkBindImageMemory(demo->device, demo->dds_image, demo->dds_mem, 0);
  assert(!err);

  //memory layout

  VkImageMemoryBarrier image_memory_barrier_pre[1] = {
      {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
       NULL,
       0,
       VK_ACCESS_TRANSFER_WRITE_BIT,
       VK_IMAGE_LAYOUT_UNDEFINED,
       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
       VK_QUEUE_FAMILY_IGNORED,
       VK_QUEUE_FAMILY_IGNORED,
       demo->dds_image,
       {VK_IMAGE_ASPECT_COLOR_BIT, 0, vkheader.mipLevels, 0, 1}}};

  vkCmdPipelineBarrier(tmp_cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_pre);

  vkCmdCopyBufferToImage(tmp_cmd, demo->mStagingBuffer.buffer(), demo->dds_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, NumSubresource, regions);

  VkImageMemoryBarrier image_memory_barrier_post[1] = {
      {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
       NULL,
       0,
       VK_ACCESS_SHADER_READ_BIT,
       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
       VK_QUEUE_FAMILY_IGNORED,
       VK_QUEUE_FAMILY_IGNORED,
       demo->dds_image,
       {VK_IMAGE_ASPECT_COLOR_BIT, 0, vkheader.mipLevels, 0, 1}}};
  vkCmdPipelineBarrier(tmp_cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, image_memory_barrier_post);

  //flush cmd
  err = vkEndCommandBuffer(tmp_cmd);
  assert(!err);

  VkFence fence;
  VkFenceCreateInfo fence_ci = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, NULL, 0};
  err = vkCreateFence(demo->device, &fence_ci, NULL, &fence);
  assert(!err);

  const VkCommandBuffer cmd_bufs[] = {tmp_cmd};
  VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO,
                              NULL,
                              0,
                              NULL,
                              NULL,
                              1,
                              cmd_bufs,
                              0,
                              NULL};

  err = vkQueueSubmit(demo->graphics_queue, 1, &submit_info, fence);
  assert(!err);

  err = vkWaitForFences(demo->device, 1, &fence, VK_TRUE, UINT64_MAX);
  assert(!err);

  vkDestroyFence(demo->device, fence, NULL);

  vkFreeCommandBuffers(demo->device, tmp_cmd_pool, 1, &tmp_cmd);

  vkDestroyCommandPool(demo->device, tmp_cmd_pool, NULL);

  // create sampler
  const VkSamplerCreateInfo sampler = {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .pNext = NULL,
      .magFilter = VK_FILTER_NEAREST,
      .minFilter = VK_FILTER_NEAREST,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
      .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .mipLodBias = 0.0f,
      .anisotropyEnable = VK_FALSE,
      .maxAnisotropy = 1,
      .compareOp = VK_COMPARE_OP_NEVER,
      .minLod = 0.0f,
      .maxLod = 0.0f,
      .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
      .unnormalizedCoordinates = VK_FALSE,
  };
  err = vkCreateSampler(demo->device, &sampler, NULL, &demo->dds_sampler);
  assert(!err);

  // create image view

  VkImageViewCreateInfo view = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext = NULL,
      .image = VK_NULL_HANDLE,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = vkheader.format,
      .components =
          {
              VK_COMPONENT_SWIZZLE_R,
              VK_COMPONENT_SWIZZLE_G,
              VK_COMPONENT_SWIZZLE_B,
              VK_COMPONENT_SWIZZLE_A,
          },
      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, vkheader.mipLevels, 0, 1},
      .flags = 0,
  };

  view.image = demo->dds_image;
  err = vkCreateImageView(demo->device, &view, NULL, &demo->dds_view);
  assert(!err);
}

static void demo_cleanupTexture_DDS(struct demo *demo)
{
  vkDestroyImageView(demo->device, demo->dds_view, NULL);
  vkDestroyImage(demo->device, demo->dds_image, NULL);
  vkFreeMemory(demo->device, demo->dds_mem, NULL);
  vkDestroySampler(demo->device, demo->dds_sampler, NULL);
}

template <typename T>
T roundUp(const T value, const T alignment)
{
  auto temp = value + alignment - static_cast<T>(1);
  return temp - temp % alignment;
}

static void demo_prepare_texture_staging_buffer(struct demo *demo, struct staging_texture_object *tex_obj)
{
  // Context::texSubImage2D libANGLE/Context.cpp
  // Texture::setSubImage libANGLE/Texture.cpp
  // TextureVk::setSubImage libANGLE/renderer/vulkan/TextureVk.cpp
  // TextureVk::setSubImageImpl libANGLE/renderer/vulkan/TextureVk.cpp
  // ImageHelper::stageSubresourceUpdate libANGLE/renderer/vulkan/vk_helpers.cpp
  // ImageHelper::stageSubresourceUpdateImpl libANGLE/renderer/vulkan/vk_helpers.cpp
  // ImageHelper::CalculateBufferInfo libANGLE/renderer/vulkan/vk_helpers.cpp
  // ImageHelper::stageSubresourceUpdateImpl libANGLE/renderer/vulkan/vk_helpers.cpp

  // BuildInternalFormatInfoMap libANGLE/formatutils.cpp
  // From ES 3.0.1 spec, table 3.12
  //                    | Internal format     |sized| R | G | B | A |S | Format         | Type                             | Component type        | SRGB | Texture supported                                | Filterable     | Texture attachment                               | Renderbuffer                                   | Blend
  // AddRGBAFormat(&map, GL_RGBA8,             true,  8,  8,  8,  8, 0, GL_RGBA,         GL_UNSIGNED_BYTE,                  GL_UNSIGNED_NORMALIZED, false, RequireESOrExt<3, 0, &Extensions::textureStorage>, AlwaysSupported, RequireESOrExt<3, 0, &Extensions::textureStorage>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>);
  // From EXT_texture_compression_bptc
  //                          | Internal format                         | W | H |D | BS |CC| SRGB | Texture supported                              | Filterable     | Texture attachment | Renderbuffer  | Blend
  // AddCompressedFormat(&map, GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,         4,  4, 1, 128, 4, false, RequireExt<&Extensions::textureCompressionBPTC>, AlwaysSupported, NeverSupported,      NeverSupported, NeverSupported);

  // gFormatInfoTable libANGLE/renderer/Format_table_autogen.cpp
  // { FormatID::R8G8B8A8_UNORM, GL_RGBA8, GL_RGBA8, GenerateMip<R8G8B8A8>, NoCopyFunctions, ReadColor<R8G8B8A8, GLfloat>, WriteColor<R8G8B8A8, GLfloat>, GL_UNSIGNED_NORMALIZED, 8, 8, 8, 8, 0, 0, 0, 4, 0, false, false, false, gl::VertexAttribType::UnsignedByte },

  const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
  uint32_t tex_width;
  uint32_t tex_height;
  VkResult U_ASSERT_ONLY err;
  bool U_ASSERT_ONLY pass;

  if (!loadTexture_PPM(NULL, NULL, &tex_width, &tex_height))
  {
    ERR_EXIT("Failed to load textures", "Load Texture Failure");
  }

  //related to your asset
  uint32_t inputRowPitch = tex_width * (4 * 1);
  uint32_t inputDepthPitch = inputRowPitch * tex_width;
  uint32_t inputSkipBytes = 0;

  //related to vulkan
  //tex_obj->tex_width = tex_width;
  //tex_obj->tex_height = tex_height;

  uint32_t storageFormat_pixelBytes = 4;
  uint32_t glExtents_width = tex_width;
  uint32_t glExtents_height = tex_height;
  uint32_t glExtents_depth = 1;

  uint32_t outputRowPitch = storageFormat_pixelBytes * glExtents_width;
  uint32_t outputDepthPitch = outputRowPitch * glExtents_height;

  tex_obj->bufferRowLength = glExtents_width;
  tex_obj->bufferImageHeight = glExtents_height;

  uint32_t allocationSize = outputDepthPitch * glExtents_depth;

  //Format::getImageCopyBufferAlignment libANGLE/renderer/vulkan/vk_format_utils.cpp
  uint32_t alignment = 4;

  uint32_t sizeToAllocate = roundUp(allocationSize, alignment);

  const VkBufferCreateInfo buffer_create_info = {
      VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      NULL,
      0,
      sizeToAllocate,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      0U,
      NULL};

  err = vkCreateBuffer(demo->device, &buffer_create_info, NULL, &demo->staging_buffer.buffer);
  assert(!err);

  VkMemoryRequirements mem_reqs;
  vkGetBufferMemoryRequirements(demo->device, demo->staging_buffer.buffer, &mem_reqs);

  demo->staging_buffer.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  demo->staging_buffer.mem_alloc.pNext = NULL;
  demo->staging_buffer.mem_alloc.allocationSize = mem_reqs.size;
  demo->staging_buffer.mem_alloc.memoryTypeIndex = 0;

  pass = memory_type_from_properties(demo, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &demo->staging_buffer.mem_alloc.memoryTypeIndex);
  assert(pass);

  /* allocate memory */
  err = vkAllocateMemory(demo->device, &demo->staging_buffer.mem_alloc, NULL, &(demo->staging_buffer.mem));
  assert(!err);

  /* bind memory */
  err = vkBindBufferMemory(demo->device, demo->staging_buffer.buffer, demo->staging_buffer.mem, 0);
  assert(!err);

  void *data;
  err = vkMapMemory(demo->device, demo->staging_buffer.mem, 0, demo->staging_buffer.mem_alloc.allocationSize, 0, &data);
  assert(!err);

  //GetLoadFunctionsMap libANGLE/renderer/load_functions_table_autogen.cpp
  assert(inputRowPitch == outputRowPitch);
  assert(inputDepthPitch == outputDepthPitch);

  uint8_t *rgba_data = static_cast<uint8_t *>(data);
  if (!loadTexture_PPM(rgba_data, &outputRowPitch, &tex_width, &tex_height))
  {
    fprintf(stderr, "Error loading texture \n");
  }

  vkUnmapMemory(demo->device, demo->staging_buffer.mem);

  tex_obj->stagingOffset = 0;
}

static void demo_prepare_texture_image(struct demo *demo,
                                       struct texture_object *tex_obj,
                                       VkImageTiling tiling,
                                       VkImageUsageFlags usage,
                                       VkFlags required_props)
{
  const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
  uint32_t tex_width;
  uint32_t tex_height;

  if (!loadTexture_PPM(NULL, NULL, &tex_width, &tex_height))
  {
    ERR_EXIT("Failed to load textures", "Load Texture Failure");
  }

  VkResult U_ASSERT_ONLY err;
  bool U_ASSERT_ONLY pass;
  tex_obj->tex_width = tex_width;
  tex_obj->tex_height = tex_height;

  const VkImageCreateInfo image_create_info = {
      VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      NULL,
      0,
      VK_IMAGE_TYPE_2D,
      tex_format,
      {tex_width, tex_height, 1},
      1,
      1,
      VK_SAMPLE_COUNT_1_BIT,
      tiling,
      usage,
      VK_SHARING_MODE_EXCLUSIVE,
      0U,
      NULL,
      VK_IMAGE_LAYOUT_UNDEFINED,
  };

  VkMemoryRequirements mem_reqs;

  err = vkCreateImage(demo->device, &image_create_info, NULL, &tex_obj->image);
  assert(!err);

  vkGetImageMemoryRequirements(demo->device, tex_obj->image, &mem_reqs);

  tex_obj->mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  tex_obj->mem_alloc.pNext = NULL;
  tex_obj->mem_alloc.allocationSize = mem_reqs.size;
  tex_obj->mem_alloc.memoryTypeIndex = 0;

  pass = memory_type_from_properties(demo, mem_reqs.memoryTypeBits, required_props, &tex_obj->mem_alloc.memoryTypeIndex);
  assert(pass);

  /* allocate memory */
  err = vkAllocateMemory(demo->device, &tex_obj->mem_alloc, NULL, &(tex_obj->mem));
  assert(!err);

  /* bind memory */
  err = vkBindImageMemory(demo->device, tex_obj->image, tex_obj->mem, 0);
  assert(!err);

  tex_obj->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

static void demo_destroy_texture_staging_buffer(struct demo *demo, struct staging_texture_object *tex_objs)
{
  /* clean up staging resources */
  vkFreeMemory(demo->device, demo->staging_buffer.mem, NULL);
  vkDestroyBuffer(demo->device, demo->staging_buffer.buffer, NULL);
}

static void demo_set_image_layout(struct demo *demo,
                                  VkCommandBuffer tmp_cmd,
                                  VkImage image,
                                  VkImageAspectFlags aspectMask,
                                  VkImageLayout old_image_layout,
                                  VkImageLayout new_image_layout,
                                  VkAccessFlags srcAccessMask,
                                  VkPipelineStageFlags src_stages,
                                  VkPipelineStageFlags dest_stages)
{
  assert(tmp_cmd);

  VkImageMemoryBarrier image_memory_barrier = {
      VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      NULL,
      srcAccessMask,
      0,
      old_image_layout,
      new_image_layout,
      VK_QUEUE_FAMILY_IGNORED,
      VK_QUEUE_FAMILY_IGNORED,
      image,
      {aspectMask, 0, 1, 0, 1}};

  switch (new_image_layout)
  {
  case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
    /* Make sure anything that was copying from this image has completed */
    image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    break;

  case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
    image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    break;

  case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    image_memory_barrier.dstAccessMask =
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    break;

  case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    image_memory_barrier.dstAccessMask =
        VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    break;

  case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    break;

  case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
    image_memory_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    break;

  default:
    image_memory_barrier.dstAccessMask = 0;
    break;
  }

  VkImageMemoryBarrier *pmemory_barrier = &image_memory_barrier;

  vkCmdPipelineBarrier(tmp_cmd, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, pmemory_barrier);
};

static void demo_flush_init_cmd(struct demo *demo, VkCommandBuffer tmp_cmd)
{
  VkResult U_ASSERT_ONLY err;

  assert(tmp_cmd);

  err = vkEndCommandBuffer(tmp_cmd);
  assert(!err);

  VkFence fence;
  VkFenceCreateInfo fence_ci = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, NULL, 0};
  err = vkCreateFence(demo->device, &fence_ci, NULL, &fence);
  assert(!err);

  const VkCommandBuffer cmd_bufs[] = {tmp_cmd};
  VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO,
                              NULL,
                              0,
                              NULL,
                              NULL,
                              1,
                              cmd_bufs,
                              0,
                              NULL};

  err = vkQueueSubmit(demo->graphics_queue, 1, &submit_info, fence);
  assert(!err);

  err = vkWaitForFences(demo->device, 1, &fence, VK_TRUE, UINT64_MAX);
  assert(!err);

  vkDestroyFence(demo->device, fence, NULL);
}

static void demo_prepare_cube_data_buffers(struct demo *demo)
{

  //--------------------------------------------------------------------------------------
  // Mesh and VertexFormat Data
  //--------------------------------------------------------------------------------------
  // clang-format off
static const float g_vertex_buffer_data[] = {
    -1.0f,-1.0f,-1.0f,  // -X side
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,

    -1.0f,-1.0f,-1.0f,  // -Z side
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,

    -1.0f,-1.0f,-1.0f,  // -Y side
     1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,

    -1.0f, 1.0f,-1.0f,  // +Y side
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,

     1.0f, 1.0f,-1.0f,  // +X side
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,

    -1.0f, 1.0f, 1.0f,  // +Z side
    -1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
};

static const float g_uv_buffer_data[] = {
    0.0f, 1.0f,  // -X side
    1.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,

    1.0f, 1.0f,  // -Z side
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,

    1.0f, 0.0f,  // -Y side
    1.0f, 1.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    1.0f, 0.0f,  // +Y side
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,

    1.0f, 0.0f,  // +X side
    0.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,

    0.0f, 0.0f,  // +Z side
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
};
  // clang-format on

  VkResult U_ASSERT_ONLY err;
  bool U_ASSERT_ONLY pass;

  VkBufferCreateInfo buf_info_vertex = {
      VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      NULL,
      0,
      sizeof(g_vertex_buffer_data),
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      0,
      NULL};
  err = vkCreateBuffer(demo->device, &buf_info_vertex, NULL, &demo->vertex_buffer[0]);
  assert(!err);

  VkBufferCreateInfo buf_info_vertex_addition = {
      VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      NULL,
      0,
      sizeof(g_uv_buffer_data),
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      0,
      NULL};
  err = vkCreateBuffer(demo->device, &buf_info_vertex_addition, NULL, &demo->vertex_buffer_addition[0]);
  assert(!err);

  VkMemoryRequirements mem_reqs_vertex;
  vkGetBufferMemoryRequirements(demo->device, demo->vertex_buffer[0], &mem_reqs_vertex);

  VkMemoryRequirements mem_reqs_vertex_addition;
  vkGetBufferMemoryRequirements(demo->device, demo->vertex_buffer_addition[0], &mem_reqs_vertex_addition);

  VkMemoryAllocateInfo mem_alloc_vertex = {
      VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      NULL,
      roundUp(mem_reqs_vertex.size, mem_reqs_vertex_addition.alignment) + mem_reqs_vertex_addition.size,
      0};
  pass = memory_type_from_properties(demo, (mem_reqs_vertex.memoryTypeBits & mem_reqs_vertex_addition.memoryTypeBits), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &mem_alloc_vertex.memoryTypeIndex);
  assert(pass);

  err = vkAllocateMemory(demo->device, &mem_alloc_vertex, NULL, &demo->meshdata_memory);
  assert(!err);

  void *pdata_map;
  err = vkMapMemory(demo->device, demo->meshdata_memory, 0, VK_WHOLE_SIZE, 0, &pdata_map);
  assert(!err);

  memcpy(reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(pdata_map) + 0), g_vertex_buffer_data, sizeof(g_vertex_buffer_data));

  memcpy(reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(pdata_map) + roundUp(mem_reqs_vertex.size, mem_reqs_vertex_addition.alignment)), g_uv_buffer_data, sizeof(g_uv_buffer_data));

  vkUnmapMemory(demo->device, demo->meshdata_memory);

  err = vkBindBufferMemory(demo->device, demo->vertex_buffer[0], demo->meshdata_memory, 0);
  assert(!err);

  err = vkBindBufferMemory(demo->device, demo->vertex_buffer_addition[0], demo->meshdata_memory, roundUp(mem_reqs_vertex.size, mem_reqs_vertex_addition.alignment));
  assert(!err);
}

static bool memory_type_from_properties(struct demo *demo, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex)
{
  // Search memtypes to find first index with those properties
  for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++)
  {
    if ((typeBits & 1) == 1)
    {
      // Type is available, does it match user properties?
      if ((demo->memory_properties.memoryTypes[i].propertyFlags &
           requirements_mask) == requirements_mask)
      {
        *typeIndex = i;
        return true;
      }
    }
    typeBits >>= 1;
  }
  // No memory types matched, return failure
  return false;
}

static VkShaderModule demo_prepare_shader_module(struct demo *demo,
                                                 const uint32_t *code,
                                                 size_t size)
{
  VkShaderModule module;
  VkShaderModuleCreateInfo moduleCreateInfo;
  VkResult U_ASSERT_ONLY err;

  moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  moduleCreateInfo.pNext = NULL;
  moduleCreateInfo.flags = 0;
  moduleCreateInfo.codeSize = size;
  moduleCreateInfo.pCode = code;

  err = vkCreateShaderModule(demo->device, &moduleCreateInfo, NULL, &module);
  assert(!err);

  return module;
}

static void demo_prepare_vs(struct demo *demo)
{
  const uint32_t vs_code[] = {
#include "generated/cube.vert.inc"
  };
  demo->vert_shader_module = demo_prepare_shader_module(demo, vs_code, sizeof(vs_code));
}

static void demo_prepare_fs(struct demo *demo)
{
  const uint32_t fs_code[] = {
#include "generated/cube.frag.inc"
  };
  demo->frag_shader_module = demo_prepare_shader_module(demo, fs_code, sizeof(fs_code));
}

static void demo_prepare_descriptor_layout(struct demo *demo)
{
  const VkDescriptorSetLayoutBinding layout_bindings[2] = {
      [0] =
          {
              .binding = 0,
              .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
              .descriptorCount = 1,
              .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
              .pImmutableSamplers = NULL,
          },
      [1] =
          {
              .binding = 1,
              .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
              .descriptorCount = 1,
              .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
              .pImmutableSamplers = NULL,
          },
  };
  const VkDescriptorSetLayoutCreateInfo descriptor_layout = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pNext = NULL,
      .bindingCount = 2,
      .pBindings = layout_bindings,
  };
  VkResult U_ASSERT_ONLY err;

  err = vkCreateDescriptorSetLayout(demo->device, &descriptor_layout, NULL, &demo->desc_layout);
  assert(!err);

  const VkPushConstantRange pushConstantRanges[1] = {VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(vktexcube_vs_pushconstant)};
  const VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      NULL,
      0,
      1,
      &demo->desc_layout,
      1,
      pushConstantRanges};

  err = vkCreatePipelineLayout(demo->device, &pPipelineLayoutCreateInfo, NULL, &demo->pipeline_layout);
  assert(!err);
}

static void demo_prepare_render_pass(struct demo *demo)
{
  demo->depth.format = VK_FORMAT_D16_UNORM;

  // The initial layout for the color and depth attachments will be
  // LAYOUT_UNDEFINED because at the start of the renderpass, we don't care
  // about their contents. At the start of the subpass, the color attachment's
  // layout will be transitioned to LAYOUT_COLOR_ATTACHMENT_OPTIMAL and the
  // depth stencil attachment's layout will be transitioned to
  // LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL.  At the end of the renderpass, the
  // color attachment's layout will be transitioned to LAYOUT_PRESENT_SRC_KHR to
  // be ready to present.  This is all done as part of the renderpass, no
  // barriers are necessary.
  const VkAttachmentDescription attachments[2] = {
      [0] =
          {
              .format = demo->format,
              .flags = 0,
              .samples = VK_SAMPLE_COUNT_1_BIT,
              .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
              .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
              .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
              .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
              .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
              .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
          },
      [1] =
          {
              .format = demo->depth.format,
              .flags = 0,
              .samples = VK_SAMPLE_COUNT_1_BIT,
              .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
              .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
              .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
              .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
              .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
              .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
          },
  };
  const VkAttachmentReference color_reference = {
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
  };
  const VkAttachmentReference depth_reference = {
      .attachment = 1,
      .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
  };
  const VkSubpassDescription subpass = {
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .flags = 0,
      .inputAttachmentCount = 0,
      .pInputAttachments = NULL,
      .colorAttachmentCount = 1,
      .pColorAttachments = &color_reference,
      .pResolveAttachments = NULL,
      .pDepthStencilAttachment = &depth_reference,
      .preserveAttachmentCount = 0,
      .pPreserveAttachments = NULL,
  };
  const VkRenderPassCreateInfo rp_info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .attachmentCount = 2,
      .pAttachments = attachments,
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount = 0,
      .pDependencies = NULL,
  };
  VkResult U_ASSERT_ONLY err;

  err = vkCreateRenderPass(demo->device, &rp_info, NULL, &demo->render_pass);
  assert(!err);
}

static void demo_prepare_pipeline(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;

  demo_prepare_vs(demo);
  demo_prepare_fs(demo);

  // Two stages: vs and fs
  VkPipelineShaderStageCreateInfo shaderStages[2];
  memset(&shaderStages, 0, 2 * sizeof(VkPipelineShaderStageCreateInfo));

  shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStages[0].module = demo->vert_shader_module;
  shaderStages[0].pName = "main";

  shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStages[1].module = demo->frag_shader_module;
  shaderStages[1].pName = "main";

  VkVertexInputBindingDescription vertex_input_binding_[2] = {
      {0, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX},
      {1, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_VERTEX}};

  VkVertexInputAttributeDescription vertex_input_attrs_[2] = {
      {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0}, // Position
      {1, 1, VK_FORMAT_R32G32_SFLOAT, 0}     // UV
  };

  VkPipelineVertexInputStateCreateInfo vertex_input_state_{
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      NULL,
      0,
      2,
      vertex_input_binding_,
      2,
      vertex_input_attrs_};

  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_ = {
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      NULL,
      0,
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      false};

  VkPipelineViewportStateCreateInfo vp = {
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      NULL,
      0,
      1,
      NULL,
      1,
      NULL};

  VkPipelineRasterizationStateCreateInfo rs = {
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      NULL,
      0,
      VK_FALSE,
      VK_FALSE,
      VK_POLYGON_MODE_FILL,
      VK_CULL_MODE_BACK_BIT,
      VK_FRONT_FACE_COUNTER_CLOCKWISE,
      VK_FALSE,
      0.0f,
      0.0f,
      0.0f,
      1.0f};

  VkPipelineMultisampleStateCreateInfo ms = {
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      NULL,
      0,
      VK_SAMPLE_COUNT_1_BIT,
      VK_FALSE,
      0.0f,
      NULL,
      VK_FALSE,
      VK_FALSE};

  VkPipelineDepthStencilStateCreateInfo ds = {
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
      NULL,
      0,
      VK_TRUE,
      VK_TRUE,
      VK_COMPARE_OP_LESS_OR_EQUAL,
      VK_FALSE,
      VK_FALSE,
      {VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS, 255, 255, 255},
      {VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS, 255, 255, 255},
      0.0f,
      1.0f};

  VkPipelineColorBlendAttachmentState att_state[1] = {
      {VK_FALSE, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT}};
  VkPipelineColorBlendStateCreateInfo cb = {
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      NULL,
      0,
      VK_FALSE,
      VK_LOGIC_OP_CLEAR,
      1,
      att_state,
      {0.0f, 0.0f, 0.0f, 0.0f}};

  VkDynamicState dynamicStateEnables[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, NULL, 0, 2, dynamicStateEnables};

  VkGraphicsPipelineCreateInfo pipeline = {
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      NULL,
      0,
      ARRAY_SIZE(shaderStages),
      shaderStages,
      &vertex_input_state_,
      &input_assembly_state_,
      NULL,
      &vp,
      &rs,
      &ms,
      &ds,
      &cb,
      &dynamicState,
      demo->pipeline_layout,
      demo->render_pass,
      0,
      VK_NULL_HANDLE,
      0};

  // Check disk for existing cache data
  demo_load_pipeline_cache(demo);

  err = vkCreateGraphicsPipelines(demo->device, demo->pipelineCache, 1, &pipeline, NULL, &demo->pipeline);
  assert(!err);

  vkDestroyShaderModule(demo->device, demo->frag_shader_module, NULL);
  vkDestroyShaderModule(demo->device, demo->vert_shader_module, NULL);

  // Store away the cache that we've populated.  This could conceivably happen
  // earlier, depends on when the pipeline cache stops being populated
  // internally.
  demo_store_pipeline_cache(demo);
}

static void demo_prepare_descriptor_pool(struct demo *demo)
{
  const VkDescriptorPoolSize type_counts[2] = {
      [0] =
          {
              .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
              .descriptorCount = FRAME_LAG,
          },
      [1] =
          {
              .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
              .descriptorCount = FRAME_LAG * 1,
          },
  };
  const VkDescriptorPoolCreateInfo descriptor_pool = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .pNext = NULL,
      .maxSets = FRAME_LAG,
      .poolSizeCount = 2,
      .pPoolSizes = type_counts,
  };
  VkResult U_ASSERT_ONLY err;

  err = vkCreateDescriptorPool(demo->device, &descriptor_pool, NULL, &demo->desc_pool);
  assert(!err);
}

static void demo_prepare_descriptor_set(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;

  VkDescriptorSetAllocateInfo alloc_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .pNext = NULL,
      .descriptorPool = demo->desc_pool,
      .descriptorSetCount = 1,
      .pSetLayouts = &demo->desc_layout};

  for (unsigned int i = 0; i < FRAME_LAG; i++)
  {
    err = vkAllocateDescriptorSets(demo->device, &alloc_info, &demo->descriptor_set[i]);
    assert(!err);
  }
}

static void demo_prepare(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;

  for (uint32_t i = 0; i < FRAME_LAG; i++)
  {
    const VkCommandPoolCreateInfo cmd_pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .queueFamilyIndex = demo->graphics_queue_family_index,
        .flags = 0,
    };
    err = vkCreateCommandPool(demo->device, &cmd_pool_info, NULL, &demo->cmd_pool[i]);
    assert(!err);

    const VkCommandBufferAllocateInfo cmd_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = demo->cmd_pool[i],
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    err = vkAllocateCommandBuffers(demo->device, &cmd_info, &demo->cmd[i]);
    assert(!err);

    if (demo->separate_present_queue)
    {
      const VkCommandPoolCreateInfo present_cmd_pool_info = {
          .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
          .pNext = NULL,
          .queueFamilyIndex = demo->present_queue_family_index,
          .flags = 0,
      };
      err = vkCreateCommandPool(demo->device, &present_cmd_pool_info, NULL, &demo->present_cmd_pool[i]);
      assert(!err);

      const VkCommandBufferAllocateInfo present_cmd_info = {
          VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
          NULL,
          demo->present_cmd_pool[i],
          VK_COMMAND_BUFFER_LEVEL_PRIMARY,
          1,
      };

      err = vkAllocateCommandBuffers(demo->device, &present_cmd_info, &demo->graphics_to_present_cmd[i]);
      assert(!err);
    }
  }

  demo_prepare_swapchain(demo);

  demo_prepare_ringbuffer(demo);

  demo->prepared = true;
}

static void demo_prepare_stagingbuffer(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;
  bool U_ASSERT_ONLY pass;

  size_t buffersize = 1024 * 1024 * 64;

  VkBufferCreateInfo buf_info = {
      VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      NULL,
      0,
      buffersize,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      0,
      NULL};

  err = vkCreateBuffer(demo->device, &buf_info, NULL, &demo->stagingbuffer_buffer);
  assert(!err);

  VkMemoryRequirements mem_reqs;
  vkGetBufferMemoryRequirements(demo->device, demo->stagingbuffer_buffer, &mem_reqs);

  uint32_t typeIndex;
  pass = memory_type_from_properties(demo, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &typeIndex);
  assert(pass);

  VkMemoryAllocateInfo mem_alloc = {
      VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      NULL,
      mem_reqs.size,
      typeIndex};

  err = vkAllocateMemory(demo->device, &mem_alloc, NULL, &demo->stagingbuffer_mem);
  assert(!err);

  err = vkBindBufferMemory(demo->device, demo->stagingbuffer_buffer, demo->stagingbuffer_mem, 0);
  assert(!err);

  void *pdata_map;
  err = vkMapMemory(demo->device, demo->stagingbuffer_mem, 0, VK_WHOLE_SIZE, 0, &pdata_map);
  assert(!err);

  demo->mStagingBuffer.init(demo->stagingbuffer_buffer, demo->stagingbuffer_mem, buffersize, static_cast<uint8_t *>(pdata_map));
}

static void demo_cleanup_stagingbuffer(struct demo *demo)
{
  vkDestroyBuffer(demo->device, demo->stagingbuffer_buffer, NULL);
  vkFreeMemory(demo->device, demo->stagingbuffer_mem, NULL);
}

static void demo_prepare_ringbuffer(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;
  bool U_ASSERT_ONLY pass;

  VkBufferCreateInfo buf_info;
  VkMemoryRequirements mem_reqs;
  VkMemoryAllocateInfo mem_alloc;

  memset(&buf_info, 0, sizeof(buf_info));
  buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  buf_info.size = sizeof(struct vktexcube_vs_uniform);

  for (unsigned int i = 0; i < FRAME_LAG; i++)
  {
    err = vkCreateBuffer(demo->device, &buf_info, NULL, &demo->uniform_buffer[i]);
    assert(!err);

    vkGetBufferMemoryRequirements(demo->device, demo->uniform_buffer[i], &mem_reqs);

    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = NULL;
    mem_alloc.allocationSize = mem_reqs.size;
    mem_alloc.memoryTypeIndex = 0;

    pass = memory_type_from_properties(demo, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &mem_alloc.memoryTypeIndex);
    assert(pass);

    err = vkAllocateMemory(demo->device, &mem_alloc, NULL, &demo->uniform_memory[i]);
    assert(!err);

    err = vkBindBufferMemory(demo->device, demo->uniform_buffer[i], demo->uniform_memory[i], 0);
    assert(!err);
  }
}

static void demo_prepare_swapchain(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;

  // Check the surface capabilities and formats
  VkSurfaceCapabilitiesKHR surfCapabilities;
  err = demo->fpGetPhysicalDeviceSurfaceCapabilitiesKHR(demo->gpu, demo->surface, &surfCapabilities);
  if (VK_SUCCESS == err)
  {
    assert(VK_SUCCESS == err);
  }
  else if (VK_ERROR_SURFACE_LOST_KHR == err)
  {
    demo->quit = true;
    return;
  }
  else
  {
    assert(VK_SUCCESS == err);
  }

  uint32_t presentModeCount;
  err = demo->fpGetPhysicalDeviceSurfacePresentModesKHR(demo->gpu, demo->surface, &presentModeCount, NULL);
  assert(!err);
  VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
  assert(presentModes);
  err = demo->fpGetPhysicalDeviceSurfacePresentModesKHR(demo->gpu, demo->surface, &presentModeCount, presentModes);
  assert(!err);

  VkExtent2D swapchainExtent;
  // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
  if (surfCapabilities.currentExtent.width == 0xFFFFFFFF)
  {
    // If the surface size is undefined, the size is set to the size
    // of the images requested, which must fit within the minimum and
    // maximum values.
    swapchainExtent.width = demo->width;
    swapchainExtent.height = demo->height;

    if (swapchainExtent.width < surfCapabilities.minImageExtent.width)
    {
      swapchainExtent.width = surfCapabilities.minImageExtent.width;
    }
    else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width)
    {
      swapchainExtent.width = surfCapabilities.maxImageExtent.width;
    }

    if (swapchainExtent.height < surfCapabilities.minImageExtent.height)
    {
      swapchainExtent.height = surfCapabilities.minImageExtent.height;
    }
    else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height)
    {
      swapchainExtent.height = surfCapabilities.maxImageExtent.height;
    }
  }
  else
  {
    // If the surface size is defined, the swap chain size must match
    swapchainExtent = surfCapabilities.currentExtent;
    demo->width = surfCapabilities.currentExtent.width;
    demo->height = surfCapabilities.currentExtent.height;
  }

  // The FIFO present mode is guaranteed by the spec to be supported
  // and to have no tearing.  It's a great default present mode to use.
  VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

  //  There are times when you may wish to use another present mode.  The
  //  following code shows how to select them, and the comments provide some
  //  reasons you may wish to use them.
  //
  // It should be noted that Vulkan 1.0 doesn't provide a method for
  // synchronizing rendering with the presentation engine's display.  There
  // is a method provided for throttling rendering with the display, but
  // there are some presentation engines for which this method will not work.
  // If an application doesn't throttle its rendering, and if it renders much
  // faster than the refresh rate of the display, this can waste power on
  // mobile devices.  That is because power is being spent rendering images
  // that may never be seen.

  // VK_PRESENT_MODE_IMMEDIATE_KHR is for applications that don't care about
  // tearing, or have some way of synchronizing their rendering with the
  // display.
  // VK_PRESENT_MODE_MAILBOX_KHR may be useful for applications that
  // generally render a new presentable image every refresh cycle, but are
  // occasionally early.  In this case, the application wants the new image
  // to be displayed instead of the previously-queued-for-presentation image
  // that has not yet been displayed.
  // VK_PRESENT_MODE_FIFO_RELAXED_KHR is for applications that generally
  // render a new presentable image every refresh cycle, but are occasionally
  // late.  In this case (perhaps because of stuttering/latency concerns),
  // the application wants the late image to be immediately displayed, even
  // though that may mean some tearing.

  if (demo->presentMode != swapchainPresentMode)
  {

    for (size_t i = 0; i < presentModeCount; ++i)
    {
      if (presentModes[i] == demo->presentMode)
      {
        swapchainPresentMode = demo->presentMode;
        break;
      }
    }
  }
  if (swapchainPresentMode != demo->presentMode)
  {
    ERR_EXIT("Present mode specified is not supported\n", "Present mode unsupported");
  }
  free(presentModes);

  // Determine the number of VkImages to use in the swap chain.
  // Application desires to acquire 3 images at a time for triple
  // buffering
  uint32_t desiredNumOfSwapchainImages = 3;
  if (desiredNumOfSwapchainImages < surfCapabilities.minImageCount)
  {
    desiredNumOfSwapchainImages = surfCapabilities.minImageCount;
  }
  // If maxImageCount is 0, we can ask for as many images as we want;
  // otherwise we're limited to maxImageCount
  if ((surfCapabilities.maxImageCount > 0) && (desiredNumOfSwapchainImages > surfCapabilities.maxImageCount))
  {
    // Application must settle for fewer images than desired:
    desiredNumOfSwapchainImages = surfCapabilities.maxImageCount;
  }

  VkSurfaceTransformFlagBitsKHR preTransform;
  if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
  {
    preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  }
  else
  {
    preTransform = surfCapabilities.currentTransform;
  }

  // Find a supported composite alpha mode - one of these is guaranteed to be
  // set
  VkCompositeAlphaFlagBitsKHR compositeAlpha =
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
  };
  for (uint32_t i = 0; i < ARRAY_SIZE(compositeAlphaFlags); i++)
  {
    if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i])
    {
      compositeAlpha = compositeAlphaFlags[i];
      break;
    }
  }

  VkSwapchainCreateInfoKHR swapchain_ci = {
      VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      NULL,
      0U,
      demo->surface,
      desiredNumOfSwapchainImages,
      demo->format,
      demo->color_space,
      {
          swapchainExtent.width,
          swapchainExtent.height,
      },
      1,
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      0,
      NULL,
      preTransform,
      compositeAlpha,
      swapchainPresentMode,
      true,
      demo->oldswapchain};

  assert(VK_NULL_HANDLE == demo->swapchain);
  err = demo->fpCreateSwapchainKHR(demo->device, &swapchain_ci, NULL, &demo->swapchain);
  assert(!err);

  uint32_t swapchainImageCount;
  err = demo->fpGetSwapchainImagesKHR(demo->device, demo->swapchain, &swapchainImageCount, NULL);
  assert(!err);

  VkImage *swapchainImages = (VkImage *)malloc(swapchainImageCount * sizeof(VkImage));
  assert(swapchainImages);
  err = demo->fpGetSwapchainImagesKHR(demo->device, demo->swapchain, &swapchainImageCount, swapchainImages);
  assert(!err);

  assert(0 == demo->swapchain_image_resources.size());
  demo->swapchain_image_resources.resize(swapchainImageCount);

  for (uint32_t i = 0; i < swapchainImageCount; i++)
  {
    VkImageViewCreateInfo color_image_view = {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        NULL,
        0,
        VK_NULL_HANDLE,
        VK_IMAGE_VIEW_TYPE_2D,
        demo->format,
        {
            VK_COMPONENT_SWIZZLE_R,
            VK_COMPONENT_SWIZZLE_G,
            VK_COMPONENT_SWIZZLE_B,
            VK_COMPONENT_SWIZZLE_A,
        },
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    demo->swapchain_image_resources[i].image = swapchainImages[i];

    color_image_view.image = demo->swapchain_image_resources[i].image;

    err = vkCreateImageView(demo->device, &color_image_view, NULL, &demo->swapchain_image_resources[i].view);
    assert(!err);
  }
  free(swapchainImages);

  demo_prepare_depth(demo);

  demo_prepare_framebuffers(demo);

  demo->current_buffer = 0;
}

static void demo_prepare_depth(struct demo *demo)
{
  const VkImageCreateInfo image = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                   NULL,
                                   0,
                                   VK_IMAGE_TYPE_2D,
                                   demo->depth.format,
                                   {demo->width, demo->height, 1},
                                   1,
                                   1,
                                   VK_SAMPLE_COUNT_1_BIT,
                                   VK_IMAGE_TILING_OPTIMAL,
                                   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                   VK_SHARING_MODE_EXCLUSIVE,
                                   0U,
                                   NULL,
                                   VK_IMAGE_LAYOUT_UNDEFINED};

  VkImageViewCreateInfo view = {
      VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      NULL,
      0,
      VK_NULL_HANDLE,
      VK_IMAGE_VIEW_TYPE_2D,
      demo->depth.format,
      {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
       VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
      {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1},
  };

  VkMemoryRequirements mem_reqs;
  VkResult U_ASSERT_ONLY err;
  bool U_ASSERT_ONLY pass;

  /* create image */
  assert(VK_NULL_HANDLE == demo->depth.image);
  err = vkCreateImage(demo->device, &image, NULL, &demo->depth.image);
  assert(!err);

  vkGetImageMemoryRequirements(demo->device, demo->depth.image, &mem_reqs);
  assert(!err);

  demo->depth.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  demo->depth.mem_alloc.pNext = NULL;
  demo->depth.mem_alloc.allocationSize = mem_reqs.size;
  demo->depth.mem_alloc.memoryTypeIndex = 0;

  pass = memory_type_from_properties(demo, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &demo->depth.mem_alloc.memoryTypeIndex);
  assert(pass);

  /* allocate memory */
  assert(VK_NULL_HANDLE == demo->depth.mem);
  err = vkAllocateMemory(demo->device, &demo->depth.mem_alloc, NULL, &demo->depth.mem);
  assert(!err);

  /* bind memory */
  err = vkBindImageMemory(demo->device, demo->depth.image, demo->depth.mem, 0);
  assert(!err);

  /* create image view */
  view.image = demo->depth.image;
  assert(VK_NULL_HANDLE == demo->depth.view);
  err = vkCreateImageView(demo->device, &view, NULL, &demo->depth.view);
  assert(!err);
}

static void demo_prepare_framebuffers(struct demo *demo)
{
  VkImageView attachments[2];
  attachments[1] = demo->depth.view;

  const VkFramebufferCreateInfo fb_info = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = NULL,
      .renderPass = demo->render_pass,
      .attachmentCount = 2,
      .pAttachments = attachments,
      .width = demo->width,
      .height = demo->height,
      .layers = 1,
  };
  VkResult U_ASSERT_ONLY err;
  uint32_t i;

  for (i = 0; i < demo->swapchain_image_resources.size(); i++)
  {
    attachments[0] = demo->swapchain_image_resources[i].view;
    err = vkCreateFramebuffer(demo->device, &fb_info, NULL, &demo->swapchain_image_resources[i].framebuffer);
    assert(!err);
  }
}

static void demo_cleanup_swapchain(struct demo *demo)
{
  uint32_t i;

  for (i = 0; i < demo->swapchain_image_resources.size(); i++)
  {
    vkDestroyFramebuffer(demo->device, demo->swapchain_image_resources[i].framebuffer, NULL);
    vkDestroyImageView(demo->device, demo->swapchain_image_resources[i].view, NULL);
  }
  demo->swapchain_image_resources.clear();

  vkDestroyImageView(demo->device, demo->depth.view, NULL);
  vkDestroyImage(demo->device, demo->depth.image, NULL);
  vkFreeMemory(demo->device, demo->depth.mem, NULL);
  demo->depth.view = VK_NULL_HANDLE;
  demo->depth.image = VK_NULL_HANDLE;
  demo->depth.mem = VK_NULL_HANDLE;

  demo->fpDestroySwapchainKHR(demo->device, demo->swapchain, NULL);
}

static void demo_cleanup(struct demo *demo)
{
  demo->prepared = false;

  vkDeviceWaitIdle(demo->device);

  demo_cleanup_swapchain(demo);

  // Wait for fences from present operations
  for (uint32_t i = 0; i < FRAME_LAG; i++)
  {
    vkWaitForFences(demo->device, 1, &demo->fences[i], VK_TRUE, UINT64_MAX);
    vkDestroyFence(demo->device, demo->fences[i], NULL);
    vkDestroySemaphore(demo->device, demo->image_acquired_semaphores[i], NULL);
    vkDestroySemaphore(demo->device, demo->draw_complete_semaphores[i], NULL);
    if (demo->separate_present_queue)
    {
      vkDestroySemaphore(demo->device, demo->image_ownership_semaphores[i], NULL);
    }

    vkFreeCommandBuffers(demo->device, demo->cmd_pool[i], 1, &demo->cmd[i]);
    vkDestroyCommandPool(demo->device, demo->cmd_pool[i], NULL);

    if (demo->separate_present_queue)
    {
      vkWaitForFences(demo->device, 1, &demo->present_queue_fences[i], VK_TRUE, UINT64_MAX);
      vkDestroyFence(demo->device, demo->present_queue_fences[i], NULL);
      vkFreeCommandBuffers(demo->device, demo->present_cmd_pool[i], 1, &demo->graphics_to_present_cmd[i]);
      vkDestroyCommandPool(demo->device, demo->present_cmd_pool[i], NULL);
    }

    vkDestroyBuffer(demo->device, demo->uniform_buffer[i], NULL);
    vkFreeMemory(demo->device, demo->uniform_memory[i], NULL);
  }

  vkDestroyDescriptorPool(demo->device, demo->desc_pool, NULL);
  vkDestroyPipeline(demo->device, demo->pipeline, NULL);
  vkDestroyPipelineCache(demo->device, demo->pipelineCache, NULL);
  vkDestroyRenderPass(demo->device, demo->render_pass, NULL);
  vkDestroyPipelineLayout(demo->device, demo->pipeline_layout, NULL);
  vkDestroyDescriptorSetLayout(demo->device, demo->desc_layout, NULL);

  vkDestroyImageView(demo->device, demo->texture_assets[0].view, NULL);
  vkDestroyImage(demo->device, demo->texture_assets[0].image, NULL);
  vkFreeMemory(demo->device, demo->texture_assets[0].mem, NULL);
  vkDestroySampler(demo->device, demo->texture_assets[0].sampler, NULL);

  vkDestroyBuffer(demo->device, demo->vertex_buffer_addition[0], NULL);
  vkDestroyBuffer(demo->device, demo->vertex_buffer[0], NULL);
  vkFreeMemory(demo->device, demo->meshdata_memory, NULL);

  free(demo->queue_props);

  vkDestroyDevice(demo->device, NULL);

  if (demo->validate)
  {
    demo->DestroyDebugReportCallback(demo->inst, demo->msg_callback, NULL);
  }

  vkDestroySurfaceKHR(demo->inst, demo->surface, NULL);

  vkDestroyInstance(demo->inst, NULL);
}

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>
#include <string.h>

static void demo_load_pipeline_cache(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;

  bool badCache = true;

  size_t startCacheSize = 0;
  void *startCacheData = NULL;
  {
    int fd = openat64(AT_FDCWD, "vulkan_pipeline_cache/OpaquePass.vkcache", O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd != -1)
    {
      struct stat64 statbuf;
      if (fstat64(fd, &statbuf) == 0 && S_ISREG(statbuf.st_mode))
      {
        if (statbuf.st_size >= 32)
        {
          startCacheSize = statbuf.st_size;

          startCacheData = malloc(startCacheSize);
          assert(startCacheData != NULL);

          int ret = read(fd, startCacheData, startCacheSize);
          assert(ret == startCacheSize);

          uint32_t headerLength = *reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(startCacheData));
          uint32_t cacheHeaderVersion = *reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(startCacheData) + 4);
          uint32_t vendorID = *reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(startCacheData) + 8);
          uint32_t deviceID = *reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(startCacheData) + 12);
          uint8_t pipelineCacheUUID[VK_UUID_SIZE];
          memcpy(pipelineCacheUUID, reinterpret_cast<uint8_t *>(startCacheData) + 16, VK_UUID_SIZE);

          if (headerLength >= 32 && cacheHeaderVersion == VK_PIPELINE_CACHE_HEADER_VERSION_ONE && vendorID == demo->gpu_props.vendorID && deviceID == demo->gpu_props.deviceID && (memcmp(pipelineCacheUUID, demo->gpu_props.pipelineCacheUUID, VK_UUID_SIZE) == 0))
          {
            badCache = false;
          }
        }
      }
    }
  }

  VkPipelineCacheCreateInfo pipelineCache;
  pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  pipelineCache.pNext = NULL;
  pipelineCache.flags = 0;
  if (!badCache)
  {
    pipelineCache.initialDataSize = startCacheSize;
    pipelineCache.pInitialData = startCacheData;
  }
  else
  {
    pipelineCache.initialDataSize = 0;
    pipelineCache.pInitialData = NULL;
  }

  err = vkCreatePipelineCache(demo->device, &pipelineCache, NULL, &demo->pipelineCache);
  assert(!err);

  free(startCacheData);
}

static void demo_store_pipeline_cache(struct demo *demo)
{
  VkResult U_ASSERT_ONLY err;

  size_t endCacheSize = 0;
  void *endCacheData = NULL;

  err = vkGetPipelineCacheData(demo->device, demo->pipelineCache, &endCacheSize, NULL);
  assert(err == VK_SUCCESS);

  endCacheData = malloc(endCacheSize);
  assert(endCacheData != NULL);

  err = vkGetPipelineCacheData(demo->device, demo->pipelineCache, &endCacheSize, endCacheData);
  assert(err == VK_SUCCESS);

  {
    bool mkdir_needed;

    char const *path = "vulkan_pipeline_cache";

    struct stat64 statbuf;
    if (0 == fstatat64(AT_FDCWD, path, &statbuf, 0))
    {
      if (S_ISDIR(statbuf.st_mode))
      {
        mkdir_needed = false;
      }
      else
      {
        fprintf(stderr, "Cannot use %s for vulkan-pipeline-cache (not a directory) --- Remove it!\n", path);
        int ret = unlinkat(AT_FDCWD, path, 0);
        assert(0 == ret);
        mkdir_needed = true;
      }
    }
    else
    {
      assert(errno == ENOENT);
      mkdir_needed = true;
    }

    if (mkdir_needed)
    {
      int ret = mkdirat(AT_FDCWD, path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      assert(0 == ret);
    }
  }

  {
    int ret = unlinkat(AT_FDCWD, "vulkan_pipeline_cache/OpaquePass.vkcache", 0);
    assert(0 == ret || errno == ENOENT);

    int fd = openat64(AT_FDCWD, "vulkan_pipeline_cache/OpaquePass.vkcache", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    assert(fd != -1);

    ret = write(fd, endCacheData, endCacheSize);
    assert(endCacheSize == ret);

    ret = close(fd);
    assert(0 == ret);
  }

  free(endCacheData);
}

VKAPI_ATTR VkBool32 VKAPI_CALL BreakCallback(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
                                             uint64_t srcObject, size_t location, int32_t msgCode,
                                             const char *pLayerPrefix, const char *pMsg, void *pUserData)
{
  raise(SIGTRAP);

  return false;
}

VKAPI_ATTR VkBool32 VKAPI_CALL dbgFunc(VkFlags msgFlags,
                                       VkDebugReportObjectTypeEXT objType,
                                       uint64_t srcObject, size_t location,
                                       int32_t msgCode,
                                       const char *pLayerPrefix,
                                       const char *pMsg, void *pUserData)
{
  // clang-format off
    char *message = (char *)malloc(strlen(pMsg) + 100);

    assert(message);

    if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        sprintf(message, "INFORMATION: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
        validation_error = 1;
    } else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        sprintf(message, "WARNING: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
        validation_error = 1;
    } else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        sprintf(message, "PERFORMANCE WARNING: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
        validation_error = 1;
    } else if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        sprintf(message, "ERROR: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
        validation_error = 1;
    } else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        sprintf(message, "DEBUG: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
        validation_error = 1;
    } else {
        sprintf(message, "INFORMATION: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
        validation_error = 1;
    }

    printf("%s\n", message);
    fflush(stdout);

    free(message);

  // clang-format on

  /*
   * false indicates that layer should not bail-out of an
   * API call that had validation failures. This may mean that the
   * app dies inside the driver due to invalid parameter(s).
   * That's what would happen without validation layers, so we'll
   * keep that behavior here.
   */
  return false;
}

void ERR_EXIT(char const *err_msg, char const *err_class)
{
  printf("%s\n", err_msg);
  fflush(stdout);
  exit(1);
}
