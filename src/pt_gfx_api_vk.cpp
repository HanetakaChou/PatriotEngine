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
#include "pt_gfx_api_vk.h"
#include <pt_mcrt_malloc.h>
#include <assert.h>
#include <algorithm>
#include <stdio.h>

gfx_api_vk::gfx_api_vk()
{
}

gfx_api_vk::~gfx_api_vk()
{
#ifndef NDEBUG
    PFN_vkDestroyDebugReportCallbackEXT vk_destroy_debug_report_callback_ext = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT"));
    assert(NULL != vk_destroy_debug_report_callback_ext);
    vk_destroy_debug_report_callback_ext(m_instance, m_debug_report_callback, &m_allocator_callbacks);
#endif
}

bool gfx_api_vk::init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual)
{
    m_wsi_connection = wsi_connection;
    m_wsi_visual = wsi_visual;

    m_allocator_callbacks.pUserData = NULL;
    m_allocator_callbacks.pfnAllocation = [](void *, size_t size, size_t alignment, VkSystemAllocationScope) -> void * { return mcrt_aligned_malloc(size, alignment); };
    m_allocator_callbacks.pfnReallocation = [](void *, void *pOriginal, size_t size, size_t alignment, VkSystemAllocationScope) -> void * { return mcrt_aligned_realloc(pOriginal, size, alignment); };
    m_allocator_callbacks.pfnFree = [](void *, void *pMemory) -> void { return mcrt_free(pMemory); };
    m_allocator_callbacks.pfnInternalAllocation = [](void *, size_t, VkInternalAllocationType, VkSystemAllocationScope) -> void {};
    m_allocator_callbacks.pfnInternalFree = [](void *, size_t, VkInternalAllocationType, VkSystemAllocationScope) -> void {};

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
        constexpr uint32_t const MY_VK_HEADER_VERSION_COMPLETE = (VK_HEADER_VERSION_COMPLETE);
        constexpr uint32_t const MY_VK_HEADER_VERSION_1_1_106 = (VK_MAKE_VERSION(1, 1, 106));
#if (MY_VK_HEADER_VERSION_1_1_106 <= MY_VK_HEADER_VERSION_COMPLETE)
        char const *enabled_layer_names[1] = {"VK_LAYER_KHRONOS_validation"};
        instance_create_info.enabledLayerCount = 1;
        instance_create_info.ppEnabledLayerNames = enabled_layer_names;
#else
        //char const *enabled_layer_names[] = {"VK_LAYER_GOOGLE_threading", "VK_LAYER_LUNARG_parameter_validation", "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_core_validation", "VK_LAYER_GOOGLE_unique_objects"};
        //instance_create_info.enabledLayerCount = 5;
        //instance_create_info.ppEnabledLayerNames = enabled_layer_names;
        char const *enabled_layer_names[1] = {"VK_LAYER_LUNARG_standard_validation"};
        instance_create_info.enabledLayerCount = 1;
        instance_create_info.ppEnabledLayerNames = enabled_layer_names;
#endif
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

        VkResult vk_res = vk_create_instance(&instance_create_info, &m_allocator_callbacks, &m_instance);
        if (VK_SUCCESS != vk_res)
        {
            return false;
        }
    }

    vk_get_instance_proc_addr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(vk_get_instance_proc_addr(m_instance, "vkGetInstanceProcAddr"));
    assert(NULL != vk_get_instance_proc_addr);

    this->m_vk_get_physical_device_memory_properties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceMemoryProperties"));
    assert(NULL != this->m_vk_get_physical_device_memory_properties);

    this->m_vk_get_physical_device_format_properties = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceFormatProperties"));
    assert(NULL != this->m_vk_get_physical_device_format_properties);

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
            return static_cast<gfx_api_vk *>(pUserData)->debug_report_callback(flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
        };
        debug_report_callback_create_info.pUserData = this;

        VkResult vk_res = vk_create_debug_report_callback_ext(m_instance, &debug_report_callback_create_info, &m_allocator_callbacks, &m_debug_report_callback);
        assert(VK_SUCCESS == vk_res);
    }
#endif

    m_physical_device = VK_NULL_HANDLE;
    {
        VkPhysicalDevice *physical_devices;
        uint32_t physical_device_count;
        class internal_physical_devices_guard
        {
            VkPhysicalDevice **const m_physical_devices;
            uint32_t *const m_physical_device_count;

        public:
            inline internal_physical_devices_guard(VkPhysicalDevice **physical_devices, uint32_t *physical_device_count, VkInstance instance, PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr)
                : m_physical_devices(physical_devices), m_physical_device_count(physical_device_count)
            {
                PFN_vkEnumeratePhysicalDevices vk_enumerate_physical_devices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(vk_get_instance_proc_addr(instance, "vkEnumeratePhysicalDevices"));
                assert(NULL != vk_enumerate_physical_devices);

                uint32_t physical_device_count_before;
                VkResult vk_res_before = vk_enumerate_physical_devices(instance, &physical_device_count_before, NULL);
                assert(VK_SUCCESS == vk_res_before);

                (*m_physical_device_count) = physical_device_count_before;
                (*m_physical_devices) = static_cast<VkPhysicalDevice *>(mcrt_malloc(sizeof(VkPhysicalDevice) * (*m_physical_device_count)));
                VkResult vk_res = vk_enumerate_physical_devices(instance, m_physical_device_count, (*m_physical_devices));
                assert(physical_device_count_before == (*m_physical_device_count));
                assert(VK_SUCCESS == vk_res);
            }
            inline ~internal_physical_devices_guard()
            {
                mcrt_free((*m_physical_devices));
            }
        } internal_instance_physical_devices_guard(&physical_devices, &physical_device_count, m_instance, vk_get_instance_proc_addr);

        // TODO
        // https://github.com/ValveSoftware/dxvk
        // src/dxvk/dxvk_adapter.cpp
        // DxvkAdapter::findQueueFamilies
        // src/d3d11/d3d11_swapchain.cpp
        // D3D11SwapChain::CreatePresenter

        // src/dxvk/dxvk_device.h
        // DxvkDevice::hasDedicatedTransferQueue

        // nvpro-samples/shared_sources/nvvk/context_vk.cpp
        // Context::initDevice
        // m_queueGCT
        // m_queueC
        // m_queue_T

        PFN_vkGetPhysicalDeviceProperties vk_get_physical_device_properties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceProperties"));
        assert(NULL != vk_get_physical_device_properties);

        VkPhysicalDevice physical_device_integrated_gpu = VK_NULL_HANDLE;
        VkDeviceSize physical_device_integrated_gpu_limits_buffer_image_granularity;
        VkDeviceSize physical_device_integrated_gpu_limits_min_uniform_buffer_offset_alignment;
        VkDeviceSize physical_device_integrated_gpu_limits_optimal_buffer_copy_offset_alignment;
        VkDeviceSize physical_device_integrated_gpu_limits_optimal_buffer_copy_row_pitch_alignment;
        VkDeviceSize physical_device_integrated_gpu_limits_non_coherent_atom_size;

        for (int physical_device_index = 0; physical_device_index < physical_device_count; ++physical_device_index)
        {
            VkPhysicalDevice physical_device = physical_devices[physical_device_index];

            struct VkPhysicalDeviceProperties physical_device_properties;
            vk_get_physical_device_properties(physical_device, &physical_device_properties);

            // The lower index implies the user preference // e.g. VK_LAYER_MESA_device_select

            if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == physical_device_properties.deviceType)
            {
                m_physical_device = physical_device;
                m_physical_device_limits_buffer_image_granularity = physical_device_properties.limits.bufferImageGranularity;
                m_physical_device_limits_min_uniform_buffer_offset_alignment = physical_device_properties.limits.minUniformBufferOffsetAlignment;
                m_physical_device_limits_optimal_buffer_copy_offset_alignment = physical_device_properties.limits.optimalBufferCopyOffsetAlignment;
                m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment = physical_device_properties.limits.optimalBufferCopyRowPitchAlignment;
                m_physical_device_limits_non_coherent_atom_size = physical_device_properties.limits.nonCoherentAtomSize;

                // The lowest index for discrete GPU
                break;
            }
            else if ((VK_NULL_HANDLE == physical_device_integrated_gpu) && (VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU == physical_device_properties.deviceType))
            {
                physical_device_integrated_gpu = physical_device;
                physical_device_integrated_gpu_limits_buffer_image_granularity = physical_device_properties.limits.bufferImageGranularity;
                physical_device_integrated_gpu_limits_min_uniform_buffer_offset_alignment = physical_device_properties.limits.minUniformBufferOffsetAlignment;
                physical_device_integrated_gpu_limits_optimal_buffer_copy_offset_alignment = physical_device_properties.limits.optimalBufferCopyOffsetAlignment;
                physical_device_integrated_gpu_limits_optimal_buffer_copy_row_pitch_alignment = physical_device_properties.limits.optimalBufferCopyRowPitchAlignment;
                physical_device_integrated_gpu_limits_non_coherent_atom_size = physical_device_properties.limits.nonCoherentAtomSize;
            }
        }

        if ((VK_NULL_HANDLE == m_physical_device) && (VK_NULL_HANDLE != physical_device_integrated_gpu))
        {
            m_physical_device = physical_device_integrated_gpu;
            m_physical_device_limits_buffer_image_granularity = physical_device_integrated_gpu_limits_buffer_image_granularity;
            m_physical_device_limits_min_uniform_buffer_offset_alignment = physical_device_integrated_gpu_limits_min_uniform_buffer_offset_alignment;
            m_physical_device_limits_optimal_buffer_copy_offset_alignment = physical_device_integrated_gpu_limits_optimal_buffer_copy_offset_alignment;
            m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment = physical_device_integrated_gpu_limits_optimal_buffer_copy_row_pitch_alignment;
            m_physical_device_limits_non_coherent_atom_size = physical_device_integrated_gpu_limits_non_coherent_atom_size;
        }
    }
    if (VK_NULL_HANDLE == m_physical_device)
    {
        return false;
    }

    m_has_dedicated_transfer_queue = false;
    m_queue_GP_family_index = VK_QUEUE_FAMILY_IGNORED;
    m_queue_T_family_index = VK_QUEUE_FAMILY_IGNORED;
    {
        VkQueueFamilyProperties *queue_family_properties;
        uint32_t queue_family_property_count;
        class internal_queue_family_properties_guard
        {
            VkQueueFamilyProperties **const m_queue_family_properties;
            uint32_t *const m_queue_family_property_count;

        public:
            inline internal_queue_family_properties_guard(VkQueueFamilyProperties **queue_family_properties, uint32_t *queue_family_property_count, VkInstance instance, PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr, VkPhysicalDevice physical_device)
                : m_queue_family_properties(queue_family_properties), m_queue_family_property_count(queue_family_property_count)
            {
                PFN_vkGetPhysicalDeviceQueueFamilyProperties vk_get_physical_device_queue_family_properties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(vk_get_instance_proc_addr(instance, "vkGetPhysicalDeviceQueueFamilyProperties"));
                assert(NULL != vk_get_physical_device_queue_family_properties);

                uint32_t queue_family_property_count_before;
                vk_get_physical_device_queue_family_properties(physical_device, &queue_family_property_count_before, NULL);

                (*m_queue_family_property_count) = queue_family_property_count_before;
                (*m_queue_family_properties) = static_cast<VkQueueFamilyProperties *>(mcrt_malloc(sizeof(VkQueueFamilyProperties) * (*m_queue_family_property_count)));
                vk_get_physical_device_queue_family_properties(physical_device, m_queue_family_property_count, (*m_queue_family_properties));
                assert(queue_family_property_count_before == (*m_queue_family_property_count));
            }
            inline ~internal_queue_family_properties_guard()
            {
                mcrt_free((*m_queue_family_properties));
            }
        } internal_instance_queue_family_properties_guard(&queue_family_properties, &queue_family_property_count, m_instance, vk_get_instance_proc_addr, m_physical_device);

        for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_property_count; ++queue_family_index)
        {
            VkQueueFamilyProperties queue_family_property = queue_family_properties[queue_family_index];

            if ((queue_family_property.queueFlags & VK_QUEUE_GRAPHICS_BIT) && platform_physical_device_presentation_support(m_physical_device, queue_family_index))
            {
                m_queue_GP_family_index = queue_family_index;
                m_queue_GP_queue_index = 0U;
                break;
            }
        }

        //TODO
        //support seperated present queue

        if (VK_QUEUE_FAMILY_IGNORED != m_queue_GP_family_index)
        {

            // vkspec // either GRAPHICS or COMPUTE implies TRANSFER // make TRANSFER optional

            for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_property_count; ++queue_family_index)
            {
                VkQueueFamilyProperties queue_family_property = queue_family_properties[queue_family_index];

                if ((m_queue_GP_family_index != queue_family_index) && (VK_QUEUE_TRANSFER_BIT == (queue_family_property.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))))
                {
                    m_queue_T_family_index = queue_family_index;
                    m_queue_T_queue_index = 0U;
                    m_has_dedicated_transfer_queue = true;
                    break;
                }
            }

            // fall back to other graphics / compute queues

            if (VK_QUEUE_FAMILY_IGNORED == m_queue_T_family_index)
            {
                for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_property_count; ++queue_family_index)
                {
                    VkQueueFamilyProperties queue_family_property = queue_family_properties[queue_family_index];

                    if ((m_queue_GP_family_index != queue_family_index) && (queue_family_property.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)))
                    {
                        m_queue_T_family_index = queue_family_index;
                        m_queue_T_queue_index = 0U;
                        m_has_dedicated_transfer_queue = true;
                        break;
                    }
                }
            }

            // try the same queue family

            if (VK_QUEUE_FAMILY_IGNORED == m_queue_T_family_index)
            {
                VkQueueFamilyProperties queue_family_property = queue_family_properties[m_queue_GP_family_index];
                if (2U <= queue_family_property.queueCount)
                {
                    m_queue_T_family_index = m_queue_GP_family_index;
                    m_queue_T_queue_index = 1U;
                    m_has_dedicated_transfer_queue = true;
                }
                else
                {
                    m_queue_T_family_index = m_queue_GP_family_index;
                    m_queue_T_queue_index = 0U;
                    m_has_dedicated_transfer_queue = false;
                }
            }
        }
    }
    if ((VK_QUEUE_FAMILY_IGNORED == m_queue_GP_family_index) || (VK_QUEUE_FAMILY_IGNORED == m_queue_T_family_index))
    {
        return false;
    }

    // EnabledFeatures.geometryShader = VK_FALSE; //be consistant with the Metal  //the GeometryShader can be replaced by DrawInstanced //and the StreamOutput can be replaced by RWTexture
    // EnabledFeatures.tessellationShader = VK_FALSE; //The vertex bandwidth doesn't benifit from the Tile-Arch
    //
    // EnabledFeatures.textureCompressionBC
    // BC6H_UFLOAT : R16 G16 B16 UFLOAT (HDR)
    // BC6H_SFLOAT : R16 G16 B16 SFLOAT (HDR)
    // DXGI_FORMAT_BC7_UNORM : B4-7 G4-7 R4-7 A0-8 UNORM
    // DXGI_FORMAT_BC7_UNORM_SRGB : B4-7 G4-7 R4-7 A0-8 UNORM
    {
        PFN_vkGetPhysicalDeviceFeatures vk_get_physical_device_features = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceFeatures"));
        assert(NULL != vk_get_physical_device_features);

        struct VkPhysicalDeviceFeatures physical_device_features;
        vk_get_physical_device_features(m_physical_device, &physical_device_features);

        this->m_physical_device_feature_texture_compression_ASTC_LDR = (physical_device_features.textureCompressionASTC_LDR != VK_FALSE) ? true : false;
        this->m_physical_device_feature_texture_compression_BC = (physical_device_features.textureCompressionBC != VK_FALSE) ? true : false;
    }

    this->m_device = VK_NULL_HANDLE;
    {

        PFN_vkCreateDevice vk_create_device = reinterpret_cast<PFN_vkCreateDevice>(vkGetInstanceProcAddr(m_instance, "vkCreateDevice"));
        assert(NULL != vk_create_device);

        struct VkDeviceCreateInfo device_create_info;
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pNext = NULL;
        device_create_info.flags = 0U;
        float queue_GP_priorities[1] = {1.0f};
        float queue_T_priorities[1] = {1.0f};
        struct VkDeviceQueueCreateInfo device_queue_create_infos[2];
        if (m_queue_GP_family_index != m_queue_T_family_index)
        {
            device_queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            device_queue_create_infos[0].pNext = NULL;
            device_queue_create_infos[0].flags = 0U;
            device_queue_create_infos[0].queueFamilyIndex = m_queue_GP_family_index;
            assert(0U == m_queue_GP_queue_index);
            device_queue_create_infos[0].queueCount = 1U;
            device_queue_create_infos[0].pQueuePriorities = queue_GP_priorities;

            device_queue_create_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            device_queue_create_infos[1].pNext = NULL;
            device_queue_create_infos[1].flags = 0U;
            device_queue_create_infos[1].queueFamilyIndex = m_queue_T_family_index;
            assert(0U == m_queue_T_queue_index);
            device_queue_create_infos[1].queueCount = 1U;
            device_queue_create_infos[1].pQueuePriorities = queue_T_priorities;

            device_create_info.pQueueCreateInfos = device_queue_create_infos;
            device_create_info.queueCreateInfoCount = 2U;
        }
        else
        {
            device_queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            device_queue_create_infos[0].pNext = NULL;
            device_queue_create_infos[0].flags = 0U;
            device_queue_create_infos[0].queueFamilyIndex = m_queue_GP_family_index;
            if (m_queue_GP_queue_index != m_queue_T_queue_index)
            {
                assert(1U >= m_queue_GP_queue_index);
                assert(1U >= m_queue_T_queue_index);
                device_queue_create_infos[0].queueCount = 2U;
            }
            else
            {
                assert(0U == m_queue_GP_queue_index);
                device_queue_create_infos[0].queueCount = 1U;
            }
            device_queue_create_infos[0].pQueuePriorities = queue_GP_priorities;

            device_create_info.pQueueCreateInfos = device_queue_create_infos;
            device_create_info.queueCreateInfoCount = 1U;
        }
        device_create_info.enabledLayerCount = 0U;
        device_create_info.ppEnabledLayerNames = NULL;
        char const *enabled_extension_names[1] = {platform_swapchain_extension_name(0)};
        device_create_info.enabledExtensionCount = platform_swapchain_extension_count();
        device_create_info.ppEnabledExtensionNames = enabled_extension_names;
        VkPhysicalDeviceFeatures enabled_features = {}; //all members are set to VK_FALSE
        assert(VK_FALSE == 0);
        enabled_features.textureCompressionASTC_LDR = this->m_physical_device_feature_texture_compression_ASTC_LDR ? VK_TRUE : VK_FALSE;
        enabled_features.textureCompressionBC = this->m_physical_device_feature_texture_compression_BC ? VK_TRUE : VK_FALSE;
        device_create_info.pEnabledFeatures = &enabled_features;

        VkResult vk_res = vk_create_device(this->m_physical_device, &device_create_info, &m_allocator_callbacks, &m_device);
        if (VK_SUCCESS != vk_res)
        {
            return false;
        }
    }

    PFN_vkGetDeviceProcAddr vk_get_device_proc_addr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vk_get_instance_proc_addr(m_instance, "vkGetDeviceProcAddr"));
    assert(NULL != vk_get_device_proc_addr);

    vk_get_device_proc_addr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vk_get_device_proc_addr(m_device, "vkGetDeviceProcAddr"));
    assert(NULL != vk_get_device_proc_addr);

    PFN_vkGetDeviceQueue vk_get_device_queue = reinterpret_cast<PFN_vkGetDeviceQueue>(vk_get_device_proc_addr(m_device, "vkGetDeviceQueue"));
    assert(NULL != vk_get_device_queue);

    this->m_vk_create_buffer = reinterpret_cast<PFN_vkCreateBuffer>(vk_get_device_proc_addr(m_device, "vkCreateBuffer"));
    assert(NULL != this->m_vk_create_buffer);

    this->m_vk_create_image = reinterpret_cast<PFN_vkCreateImage>(vk_get_device_proc_addr(m_device, "vkCreateImage"));
    assert(NULL != this->m_vk_create_image);

    this->m_vk_get_buffer_memory_requirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(vk_get_device_proc_addr(m_device, "vkGetBufferMemoryRequirements"));
    assert(NULL != this->m_vk_get_buffer_memory_requirements);

    this->m_vk_get_image_memory_requirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(vk_get_device_proc_addr(m_device, "vkGetImageMemoryRequirements"));
    assert(NULL != this->m_vk_get_image_memory_requirements);

    this->m_vk_allocate_memory = reinterpret_cast<PFN_vkAllocateMemory>(vk_get_device_proc_addr(m_device, "vkAllocateMemory"));
    assert(NULL != this->m_vk_allocate_memory);

    this->m_vk_free_memory = reinterpret_cast<PFN_vkFreeMemory>(vk_get_device_proc_addr(m_device, "vkFreeMemory"));
    assert(NULL != this->m_vk_free_memory);

    vk_get_device_queue(m_device, m_queue_GP_family_index, m_queue_GP_queue_index, &m_queue_GP);
    if (m_queue_T_family_index != m_queue_GP_family_index || m_queue_T_queue_index != m_queue_GP_queue_index)
    {
        assert(m_has_dedicated_transfer_queue);
        vk_get_device_queue(m_device, m_queue_T_family_index, m_queue_T_queue_index, &m_queue_T);
    }
    else
    {
        assert(!m_has_dedicated_transfer_queue);
        m_queue_T = m_queue_GP;
    }

    return true;
}

#ifndef NDEBUG
VkBool32 gfx_api_vk::debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage)
{
    printf("[%s] : %s", pLayerPrefix, pMessage);
    return VK_FALSE;
}
#endif