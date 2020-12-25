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
#include <assert.h>
#include <stdio.h>
#include <new>
#include <algorithm>

class gfx_connection_vk *gfx_connection_vk::init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual)
{
    class gfx_connection_vk *connection = new (mcrt_aligned_malloc(sizeof(gfx_connection_vk), alignof(gfx_connection_vk))) gfx_connection_vk(wsi_connection, wsi_visual);
    if (connection->init())
    {
        return connection;
    }
    else
    {
        connection->destroy();
        return NULL;
    }
}

bool gfx_connection_vk::init()
{
    m_allocator_callbacks.pUserData = NULL;
    m_allocator_callbacks.pfnAllocation = [](void *, size_t size, size_t alignment, VkSystemAllocationScope) -> void * { return mcrt_aligned_malloc(size, alignment); };
    m_allocator_callbacks.pfnReallocation = [](void *, void *pOriginal, size_t size, size_t alignment, VkSystemAllocationScope) -> void * { return mcrt_aligned_realloc(pOriginal, size, alignment); };
    m_allocator_callbacks.pfnFree = [](void *, void *pMemory) -> void { return mcrt_free(pMemory); };
    m_allocator_callbacks.pfnInternalAllocation = [](void *, size_t, VkInternalAllocationType, VkSystemAllocationScope) -> void {};
    m_allocator_callbacks.pfnInternalFree = [](void *, size_t, VkInternalAllocationType, VkSystemAllocationScope) -> void {};

    VkResult vk_res;

    PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr = vkGetInstanceProcAddr;

    m_instance = VK_NULL_HANDLE;
    {
        PFN_vkCreateInstance vk_create_instance = reinterpret_cast<PFN_vkCreateInstance>(vkGetInstanceProcAddr(NULL, "vkCreateInstance"));
        assert(NULL != vk_create_instance);

        VkInstanceCreateInfo instance_create_info;
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pNext = NULL;
        instance_create_info.flags = 0U;
        VkApplicationInfo application_info;
        application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        application_info.pNext = NULL;
        application_info.pApplicationName = "PatriotEngine";
        application_info.applicationVersion = 1;
        application_info.pEngineName = "PatriotEngine";
        application_info.engineVersion = 1;
        application_info.apiVersion = VK_API_VERSION_1_0;
        instance_create_info.pApplicationInfo = &application_info;
#ifndef NDEBUG
        //char const *enabled_layer_names[] = {"VK_LAYER_GOOGLE_threading", "VK_LAYER_LUNARG_parameter_validation", "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_core_validation", "VK_LAYER_GOOGLE_unique_objects"};
        //instance_create_info.enabledLayerCount = 5;
        //instance_create_info.ppEnabledLayerNames = enabled_layer_names;
        char const *enabled_layer_names[1] = {"VK_LAYER_KHRONOS_validation"}; //VK_LAYER_LUNARG_standard_validation
        instance_create_info.enabledLayerCount = 1;
        instance_create_info.ppEnabledLayerNames = enabled_layer_names;
        assert(platform_surface_extension_count() <= 2);
        char const *enabled_extension_names[3] = {VK_EXT_DEBUG_REPORT_EXTENSION_NAME, platform_surface_extension_name(0), platform_surface_extension_name(1)};
        instance_create_info.enabledExtensionCount = 1 + platform_surface_extension_count();
        instance_create_info.ppEnabledExtensionNames = enabled_extension_names;
#else
        instance_create_info.enabledLayerCount = 0;
        instance_create_info.ppEnabledLayerNames = NULL;
        assert(platform_surface_extension_count() <= 2);
        char const *enabled_extension_names[2] = {platform_surface_extension_name(0), platform_surface_extension_name(1)};
        instance_create_info.enabledExtensionCount = platform_surface_extension_count();
        instance_create_info.ppEnabledExtensionNames = enabled_extension_names;
#endif

        vk_res = vk_create_instance(&instance_create_info, &m_allocator_callbacks, &m_instance);
        if (VK_SUCCESS != vk_res)
        {
            return false;
        }
    }

    vk_get_instance_proc_addr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(vk_get_instance_proc_addr(m_instance, "vkGetInstanceProcAddr"));
    assert(NULL != vk_get_instance_proc_addr);

    m_vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(vk_get_instance_proc_addr(m_instance, "vkEnumeratePhysicalDevices"));
    assert(NULL != m_vkEnumeratePhysicalDevices);

    m_vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceProperties"));
    assert(NULL != m_vkGetPhysicalDeviceProperties);

    m_vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceQueueFamilyProperties"));
    assert(NULL != m_vkGetPhysicalDeviceQueueFamilyProperties);

    m_vkGetPhysicalDeviceFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceFeatures"));
    assert(NULL != m_vkGetPhysicalDeviceFeatures);

    m_vk_get_physical_device_format_properties = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceFormatProperties"));
    assert(NULL != m_vk_get_physical_device_format_properties);

#ifndef NDEBUG
    PFN_vkCreateDebugReportCallbackEXT vk_create_debug_report_callback_ext = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT"));
    assert(NULL != vk_create_debug_report_callback_ext);

    m_debug_report_callback = VK_NULL_HANDLE;
    {
        VkDebugReportCallbackCreateInfoEXT debug_report_callback_create_info;
        debug_report_callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        debug_report_callback_create_info.pNext = NULL;
        debug_report_callback_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debug_report_callback_create_info.pfnCallback = [](VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage, void *pUserData) -> VkBool32 {
            return static_cast<gfx_connection_vk *>(pUserData)->debug_report_callback(flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
        };
        debug_report_callback_create_info.pUserData = this;

        vk_res = vk_create_debug_report_callback_ext(m_instance, &debug_report_callback_create_info, &m_allocator_callbacks, &m_debug_report_callback);
        assert(VK_SUCCESS == vk_res);
    }
#endif

    m_physical_device = VK_NULL_HANDLE;
    {
        uint32_t physical_device_count;
        vk_res = m_vkEnumeratePhysicalDevices(m_instance, &physical_device_count, NULL);
        assert(VK_SUCCESS == vk_res);

        uint32_t physical_device_count_res = physical_device_count;
        VkPhysicalDevice *physical_devices = static_cast<VkPhysicalDevice *>(mcrt_malloc(sizeof(VkPhysicalDevice) * physical_device_count));
        vk_res = m_vkEnumeratePhysicalDevices(m_instance, &physical_device_count_res, physical_devices);
        assert(physical_device_count == physical_device_count_res);
        assert(VK_SUCCESS == vk_res);

        // to do
        // https://github.com/ValveSoftware/dxvk
        // src/dxvk/dxvk_adapter.cpp
        // DxvkAdapter::findQueueFamilies
        // src/d3d11/d3d11_swapchain.cpp
        // D3D11SwapChain::CreatePresenter

        // src/dxvk/dxvk_device.h
        // DxvkDevice::hasDedicatedTransferQueue

        //nvpro-samples/shared_sources/nvvk/context_vk.cpp
        //Context::initDevice
        //m_queueGCT
        //m_queueC
        //m_queue_T

        int score = 0;
        int const score_integrate_gpu = 1000;
        int const score_discrete_gpu = 1500; //1000 + 1000 > 1500
        int const score_no_gpu = -10000;
        int const score_no_queue_GP = -10000;
        int const score_no_queue_T = -10000;
        int const score_queue_GP_diff_queue_T = 1000; //100 + 50 < 1000
        int const score_compression_ASTC_LDR = 100;
        int const score_compression_BC = 50;
        int const score_no_compression = -10000;

        for (int physical_device_index = 0; physical_device_index < physical_device_count; ++physical_device_index)
        {
            int score_iter = 0;

            VkPhysicalDevice physical_device = physical_devices[physical_device_index];

            VkPhysicalDeviceType physical_device_type;
            VkDeviceSize physical_device_limits_buffer_image_granularity;
            VkDeviceSize physical_device_limits_min_uniform_buffer_offset_alignment;
            VkDeviceSize physical_device_limits_optimal_buffer_copy_offset_alignment;
            VkDeviceSize physical_device_limits_optimal_buffer_copy_row_pitch_alignment;
            VkDeviceSize physical_device_limits_non_coherent_atom_size;
            {
                struct VkPhysicalDeviceProperties physical_device_properties;
                m_vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
                physical_device_type = physical_device_properties.deviceType;
                physical_device_limits_buffer_image_granularity = physical_device_properties.limits.bufferImageGranularity;
                physical_device_limits_min_uniform_buffer_offset_alignment = physical_device_properties.limits.minUniformBufferOffsetAlignment;
                physical_device_limits_optimal_buffer_copy_offset_alignment = physical_device_properties.limits.optimalBufferCopyOffsetAlignment;
                physical_device_limits_optimal_buffer_copy_row_pitch_alignment = physical_device_properties.limits.optimalBufferCopyRowPitchAlignment;
                physical_device_limits_non_coherent_atom_size = physical_device_properties.limits.nonCoherentAtomSize;
            }
            score_iter += ((VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU == physical_device_type) ? score_integrate_gpu : 0);
            score_iter += ((VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == physical_device_type) ? score_discrete_gpu : 0);
            score_iter += ((!(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU == physical_device_type || VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == physical_device_type)) ? score_no_gpu : 0);

            uint32_t queue_GP_family_index = VK_QUEUE_FAMILY_IGNORED;
            uint32_t queue_T_family_index = VK_QUEUE_FAMILY_IGNORED;
            uint32_t queue_GP_count;
            uint32_t queue_T_count;
            bool queue_GP_diff_queue_T;
            {
                uint32_t queue_family_property_count;
                m_vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, NULL);

                struct VkQueueFamilyProperties *queue_family_properties_array = static_cast<VkQueueFamilyProperties *>(mcrt_malloc(sizeof(VkQueueFamilyProperties) * queue_family_property_count));
                uint32_t queue_family_property_count_res = queue_family_property_count;
                m_vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count_res, queue_family_properties_array);
                assert(queue_family_property_count_res == queue_family_property_count);

                int queue_GP_score = 0;
                int queue_T_score = 0;
                for (int queue_family_index = 0; queue_family_index < queue_family_property_count; ++queue_family_index)
                {
                    struct VkQueueFamilyProperties queue_family_properties = queue_family_properties_array[queue_family_index];

                    if ((queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && platform_physical_device_presentation_support(physical_device, queue_family_index))
                    {
                        int queue_GP_score_iter = 100;
                        queue_GP_score_iter += ((queue_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT) ? -1 : 0);

                        if (queue_GP_score_iter > queue_GP_score)
                        {
                            queue_GP_score = queue_GP_score_iter;
                            queue_GP_family_index = queue_family_index;
                            queue_GP_count = queue_family_properties.queueCount;
                        }
                    }

                    //spec: either GRAPHICS or COMPUTE implies TRANSFER //make TRANSFER optional
                    if (queue_family_properties.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))
                    {
                        int queue_T_score_iter = 100;
                        queue_T_score_iter += ((queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) ? -1 : 0);
                        queue_T_score_iter += ((queue_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT) ? -1 : 0);
                        if (queue_T_score_iter > queue_T_score)
                        {
                            queue_T_score = queue_T_score_iter;
                            queue_T_family_index = queue_family_index;
                            queue_T_count = queue_family_properties.queueCount;
                        }
                    }
                }

                mcrt_free(queue_family_properties_array);
            }
            score_iter += ((VK_QUEUE_FAMILY_IGNORED == queue_GP_family_index) ? score_no_queue_GP : 0);
            score_iter += ((VK_QUEUE_FAMILY_IGNORED == queue_T_family_index) ? score_no_queue_T : 0);
            assert((queue_GP_family_index == queue_T_family_index) || ((queue_GP_count + queue_T_count) >= 2));
            queue_GP_diff_queue_T = (queue_GP_family_index != queue_T_family_index) ? true : ((queue_GP_count >= 2) ? true : false);
            score_iter += queue_GP_diff_queue_T ? score_queue_GP_diff_queue_T : 0;

            bool physical_device_feature_texture_compression_ASTC_LDR;
            bool physical_device_feature_texture_compression_BC;
            {
                //EnabledFeatures.geometryShader = VK_FALSE; //be consistant with the Metal  //the GeometryShader can be replaced by DrawInstanced //and the StreamOutput can be replaced by RWTexture
                //EnabledFeatures.tessellationShader = VK_FALSE; //The vertex bandwidth doesn't benifit from the Tile-Arch

                //EnabledFeatures.textureCompressionBC
                //BC6H_UFLOAT : R16 G16 B16 UFLOAT (HDR)
                //BC6H_SFLOAT : R16 G16 B16 SFLOAT (HDR)
                //DXGI_FORMAT_BC7_UNORM : B4-7 G4-7 R4-7 A0-8 UNORM
                //DXGI_FORMAT_BC7_UNORM_SRGB : B4-7 G4-7 R4-7 A0-8 UNORM

                struct VkPhysicalDeviceFeatures physical_device_features;
                m_vkGetPhysicalDeviceFeatures(physical_device, &physical_device_features);

                physical_device_feature_texture_compression_ASTC_LDR = (physical_device_features.textureCompressionASTC_LDR != VK_FALSE) ? true : false;
                physical_device_feature_texture_compression_BC = (physical_device_features.textureCompressionBC != VK_FALSE) ? true : false;
            }
            score_iter += (physical_device_feature_texture_compression_ASTC_LDR ? score_compression_ASTC_LDR : 0);
            score_iter += (physical_device_feature_texture_compression_BC ? score_compression_BC : 0);
            score_iter += ((!(physical_device_feature_texture_compression_ASTC_LDR || physical_device_feature_texture_compression_BC)) ? score_no_compression : 0);

            if (score_iter > score)
            {
                score = score_iter;
                m_physical_device = physical_device;
                m_physical_device_limits_buffer_image_granularity = physical_device_limits_buffer_image_granularity;
                m_physical_device_limits_min_uniform_buffer_offset_alignment = physical_device_limits_min_uniform_buffer_offset_alignment;
                m_physical_device_limits_optimal_buffer_copy_offset_alignment = physical_device_limits_optimal_buffer_copy_offset_alignment;
                m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment = physical_device_limits_optimal_buffer_copy_row_pitch_alignment;
                m_physical_device_limits_non_coherent_atom_size = physical_device_limits_non_coherent_atom_size;
                m_queue_GP_diff_queue_T = queue_GP_diff_queue_T;
                if (queue_GP_family_index != queue_T_family_index)
                {
                    assert((queue_GP_count + queue_T_count) >= 2);
                    assert(queue_GP_diff_queue_T);
                    m_queue_GP_family_index = queue_GP_family_index;
                    m_queue_T_family_index = queue_T_family_index;
                    m_queue_GP_queue_index = 0;
                    m_queue_T_queue_index = 0;
                }
                else
                {
                    if (queue_GP_count >= 2)
                    {
                        assert(queue_GP_diff_queue_T);
                        m_queue_GP_family_index = queue_GP_family_index;
                        m_queue_T_family_index = queue_GP_family_index;
                        m_queue_GP_queue_index = 0;
                        m_queue_T_queue_index = 1;
                    }
                    else
                    {
                        assert(!queue_GP_diff_queue_T);
                        m_queue_GP_family_index = queue_GP_family_index;
                        m_queue_T_family_index = queue_GP_family_index;
                        m_queue_GP_queue_index = 0;
                        m_queue_T_queue_index = 0;
                    }
                }
                m_physical_device_feature_texture_compression_ASTC_LDR = physical_device_feature_texture_compression_ASTC_LDR;
                m_physical_device_feature_texture_compression_BC = physical_device_feature_texture_compression_BC;
            }
        }

        mcrt_free(physical_devices);
    }

    m_device = VK_NULL_HANDLE;
    {
        PFN_vkCreateDevice vk_create_device = reinterpret_cast<PFN_vkCreateDevice>(vkGetInstanceProcAddr(m_instance, "vkCreateDevice"));
        assert(NULL != vk_create_device);

        struct VkDeviceCreateInfo device_create_info;
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pNext = NULL;
        device_create_info.flags = 0U;
        float queue_GP_priorities[1] = {1.0f};
        float queue_T_priorities[1] = {1.0f};
        struct VkDeviceQueueCreateInfo device_queue_create_infos[2] = {
            {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0U, m_queue_GP_family_index, std::max(m_queue_GP_queue_index, m_queue_T_queue_index) + 1, queue_GP_priorities},
            {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0U, m_queue_T_family_index, std::max(m_queue_GP_queue_index, m_queue_T_queue_index) + 1, queue_T_priorities}};
        device_create_info.queueCreateInfoCount = (m_queue_GP_family_index != m_queue_T_family_index) ? 2 : 1;
        device_create_info.pQueueCreateInfos = device_queue_create_infos;
        device_create_info.enabledLayerCount = 0U;
        device_create_info.ppEnabledLayerNames = NULL;
        char const *enabled_extension_names[1] = {platform_swapchain_extension_name(0)};
        device_create_info.enabledExtensionCount = platform_swapchain_extension_count();
        device_create_info.ppEnabledExtensionNames = enabled_extension_names;
        VkPhysicalDeviceFeatures enabled_features = {}; //all members are set to VK_FALSE
        assert(VK_FALSE == 0);
        enabled_features.textureCompressionASTC_LDR = m_physical_device_feature_texture_compression_ASTC_LDR;
        enabled_features.textureCompressionBC = m_physical_device_feature_texture_compression_BC;
        device_create_info.pEnabledFeatures = &enabled_features;

        vk_res = vk_create_device(m_physical_device, &device_create_info, &m_allocator_callbacks, &m_device);
        if (VK_SUCCESS != vk_res)
        {
            return false;
        }
    }

    PFN_vkGetDeviceProcAddr vk_get_device_proc_addr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vk_get_instance_proc_addr(m_instance, "vkGetDeviceProcAddr"));
    assert(NULL != vk_get_device_proc_addr);

    vk_get_device_proc_addr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vk_get_device_proc_addr(m_device, "vkGetDeviceProcAddr"));
    assert(NULL != vk_get_device_proc_addr);

    m_vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(vk_get_device_proc_addr(m_device, "vkGetDeviceQueue"));
    assert(NULL != m_vkGetDeviceQueue);

    m_vk_create_buffer = reinterpret_cast<PFN_vkCreateBuffer>(vk_get_device_proc_addr(m_device, "vkCreateBuffer"));
    assert(NULL != m_vk_create_buffer);

    m_vk_create_image = reinterpret_cast<PFN_vkCreateImage>(vk_get_device_proc_addr(m_device, "vkCreateImage"));
    assert(NULL != m_vk_create_image);

    m_vkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(vk_get_device_proc_addr(m_device, "vkAllocateMemory"));
    assert(NULL != m_vkAllocateMemory);

    m_vkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(vk_get_device_proc_addr(m_device, "vkFreeMemory"));
    assert(NULL != m_vkFreeMemory);

    m_vkGetDeviceQueue(m_device, m_queue_GP_family_index, m_queue_GP_queue_index, &m_queue_GP);

    if (m_queue_T_family_index != m_queue_GP_family_index || m_queue_T_queue_index != m_queue_GP_queue_index)
    {
        assert(m_queue_GP_diff_queue_T);
        m_vkGetDeviceQueue(m_device, m_queue_T_family_index, m_queue_T_queue_index, &m_queue_T);
    }
    else
    {
        assert(!m_queue_GP_diff_queue_T);
        m_queue_T = m_queue_GP;
    }

    PFN_vkGetPhysicalDeviceMemoryProperties vk_get_physical_device_memory_properties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceMemoryProperties"));
    assert(NULL != vk_get_physical_device_memory_properties);

    PFN_vkGetBufferMemoryRequirements vk_get_buffer_memory_requirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(vk_get_device_proc_addr(m_device, "vkGetBufferMemoryRequirements"));
    assert(NULL != vk_get_buffer_memory_requirements);

    PFN_vkGetImageMemoryRequirements vk_get_image_memory_requirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(vk_get_device_proc_addr(m_device, "vkGetImageMemoryRequirements"));
    assert(NULL != vk_get_image_memory_requirements);

    if (false == this->gfx_malloc_vk::init(
                     m_physical_device,
                     vk_get_physical_device_memory_properties,
                     m_device,
                     vk_get_buffer_memory_requirements,
                     vk_get_image_memory_requirements))
    {
        return false;
    }

    return true;
}

#ifndef NDEBUG
VkBool32 gfx_connection_vk::debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage)
{
    printf("[%s] : %s", pLayerPrefix, pMessage);
    return VK_FALSE;
}
#endif

class gfx_texture_common *gfx_connection_vk::create_texture()
{
    gfx_texture_vk *texture = new (mcrt_aligned_malloc(sizeof(gfx_texture_vk), alignof(gfx_texture_vk))) gfx_texture_vk(this);
    return texture;
}

gfx_connection_vk::~gfx_connection_vk()
{
#ifndef NDEBUG
    PFN_vkDestroyDebugReportCallbackEXT vk_destroy_debug_report_callback_ext = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT"));
    assert(NULL != vk_destroy_debug_report_callback_ext);
    vk_destroy_debug_report_callback_ext(m_instance, m_debug_report_callback, &m_allocator_callbacks);
#endif
}

void gfx_connection_vk::destroy()
{
    this->~gfx_connection_vk();
    mcrt_free(this);
}
