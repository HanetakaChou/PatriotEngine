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
#include "pt_gfx_device_vk.h"
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_log.h>
#include <assert.h>

gfx_device_vk::gfx_device_vk()
{
    return;
}

bool gfx_device_vk::init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window)
{
    this->m_allocator_callbacks.pUserData = NULL;
    this->m_allocator_callbacks.pfnAllocation = [](void *, size_t size, size_t alignment, VkSystemAllocationScope) -> void * { return mcrt_aligned_malloc(size, alignment); };
    this->m_allocator_callbacks.pfnReallocation = [](void *, void *pOriginal, size_t size, size_t alignment, VkSystemAllocationScope) -> void * { return mcrt_aligned_realloc(pOriginal, size, alignment); };
    this->m_allocator_callbacks.pfnFree = [](void *, void *pMemory) -> void
    { return mcrt_free(pMemory); };
    this->m_allocator_callbacks.pfnInternalAllocation = [](void *, size_t, VkInternalAllocationType, VkSystemAllocationScope) -> void {};
    this->m_allocator_callbacks.pfnInternalFree = [](void *, size_t, VkInternalAllocationType, VkSystemAllocationScope) -> void {};

    PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr = vkGetInstanceProcAddr;

    this->m_instance = VK_NULL_HANDLE;
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

#if defined(VK_API_VERSION_1_2)
        char const *enabled_layer_names[1] = {"VK_LAYER_KHRONOS_validation"};
        instance_create_info.enabledLayerCount = 1;
        instance_create_info.ppEnabledLayerNames = enabled_layer_names;
#elif defined(VK_API_VERSION_1_1)
#if VK_HEADER_VERSION >= 106
        char const *enabled_layer_names[1] = {"VK_LAYER_KHRONOS_validation"};
        instance_create_info.enabledLayerCount = 1;
        instance_create_info.ppEnabledLayerNames = enabled_layer_names;
#else
        char const *enabled_layer_names[1] = {"VK_LAYER_LUNARG_standard_validation"};
        instance_create_info.enabledLayerCount = 1;
        instance_create_info.ppEnabledLayerNames = enabled_layer_names;
#endif
#else
        char const *enabled_layer_names[5] = {"VK_LAYER_GOOGLE_threading", "VK_LAYER_LUNARG_parameter_validation", "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_core_validation", "VK_LAYER_GOOGLE_unique_objects"};
        instance_create_info.enabledLayerCount = 5;
        instance_create_info.ppEnabledLayerNames = enabled_layer_names;
#endif
        char const *enabled_extension_names[3] = {VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, platform_surface_extension_name()};
        instance_create_info.enabledExtensionCount = 3U;
        instance_create_info.ppEnabledExtensionNames = enabled_extension_names;
#else
        instance_create_info.enabledLayerCount = 0;
        instance_create_info.ppEnabledLayerNames = NULL;
        char const *enabled_extension_names[2] = {VK_KHR_SURFACE_EXTENSION_NAME, platform_surface_extension_name()};
        instance_create_info.enabledExtensionCount = 2U;
        instance_create_info.ppEnabledExtensionNames = enabled_extension_names;
#endif

        PT_MAYBE_UNUSED VkResult vk_res = vk_create_instance(&instance_create_info, &m_allocator_callbacks, &this->m_instance);
        assert(VK_SUCCESS == vk_res);
    }
    assert(VK_NULL_HANDLE != this->m_instance);

    vk_get_instance_proc_addr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(vk_get_instance_proc_addr(this->m_instance, "vkGetInstanceProcAddr"));
    assert(NULL != vk_get_instance_proc_addr);

    this->m_vk_get_physical_device_memory_properties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceMemoryProperties"));
    assert(NULL != this->m_vk_get_physical_device_memory_properties);

    this->m_vk_get_physical_device_format_properties = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceFormatProperties"));
    assert(NULL != this->m_vk_get_physical_device_format_properties);

    this->m_vk_get_physical_device_surface_support = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceSurfaceSupportKHR"));
    assert(NULL != this->m_vk_get_physical_device_surface_support);

    this->m_vk_get_physical_device_surface_capablilities = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
    assert(NULL != this->m_vk_get_physical_device_surface_capablilities);

    this->m_vk_get_physical_device_surface_formats = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
    assert(NULL != this->m_vk_get_physical_device_surface_formats);

    this->m_vk_get_physical_device_surface_present_modes = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
    assert(NULL != this->m_vk_get_physical_device_surface_present_modes);

#ifndef NDEBUG
    this->m_debug_report_callback = VK_NULL_HANDLE;
    {
        PFN_vkCreateDebugReportCallbackEXT vk_create_debug_report_callback_ext = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT"));
        assert(NULL != vk_create_debug_report_callback_ext);

        VkDebugReportCallbackCreateInfoEXT debug_report_callback_create_info;
        debug_report_callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        debug_report_callback_create_info.pNext = NULL;
        debug_report_callback_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debug_report_callback_create_info.pfnCallback = [](VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage, void *pUserData) -> VkBool32
        {
            return static_cast<gfx_device_vk *>(pUserData)->debug_report_callback(flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
        };
        debug_report_callback_create_info.pUserData = this;

        VkResult vk_res = vk_create_debug_report_callback_ext(this->m_instance, &debug_report_callback_create_info, &this->m_allocator_callbacks, &this->m_debug_report_callback);
        assert(VK_SUCCESS == vk_res);
    }
#endif

    this->m_physical_device = VK_NULL_HANDLE;
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
                PT_MAYBE_UNUSED VkResult vk_res_before = vk_enumerate_physical_devices(instance, &physical_device_count_before, NULL);
                assert(VK_SUCCESS == vk_res_before);

                (*m_physical_device_count) = physical_device_count_before;
                (*m_physical_devices) = static_cast<VkPhysicalDevice *>(mcrt_malloc(sizeof(VkPhysicalDevice) * (*m_physical_device_count)));
                PT_MAYBE_UNUSED VkResult vk_res = vk_enumerate_physical_devices(instance, m_physical_device_count, (*m_physical_devices));
                assert(physical_device_count_before == (*m_physical_device_count));
                assert(VK_SUCCESS == vk_res);
            }
            inline ~internal_physical_devices_guard()
            {
                mcrt_free((*m_physical_devices));
            }
        } instance_internal_physical_devices_guard(&physical_devices, &physical_device_count, m_instance, vk_get_instance_proc_addr);

        PFN_vkGetPhysicalDeviceProperties vk_get_physical_device_properties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(vk_get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceProperties"));
        assert(NULL != vk_get_physical_device_properties);

        // The lower index may imply the user preference // e.g. VK_LAYER_MESA_device_select
        uint32_t physical_device_index_first_discrete_gpu = uint32_t(-1);
        uint32_t physical_device_index_first_integrated_gpu = uint32_t(-1);

        for (uint32_t physical_device_index = 0; (uint32_t(-1) == physical_device_index_first_discrete_gpu) && (physical_device_index < physical_device_count); ++physical_device_index)
        {
            struct VkPhysicalDeviceProperties physical_device_properties;
            vk_get_physical_device_properties(physical_devices[physical_device_index], &physical_device_properties);

            if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == physical_device_properties.deviceType)
            {
                physical_device_index_first_discrete_gpu = physical_device_index;
            }
            else if (VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU == physical_device_properties.deviceType)
            {
                physical_device_index_first_integrated_gpu = physical_device_index;
            }
        }

        if (uint32_t(-1) != physical_device_index_first_discrete_gpu)
        {
            this->m_physical_device = physical_devices[physical_device_index_first_discrete_gpu];
        }
        else if (uint32_t(-1) != physical_device_index_first_integrated_gpu)
        {
            this->m_physical_device = physical_devices[physical_device_index_first_integrated_gpu];
        }
    }
    assert(VK_NULL_HANDLE != this->m_physical_device);

    this->m_physical_device_limits_buffer_image_granularity = VkDeviceSize(-1);
    this->m_physical_device_limits_min_uniform_buffer_offset_alignment = VkDeviceSize(-1);
    this->m_physical_device_limits_optimal_buffer_copy_offset_alignment = VkDeviceSize(-1);
    this->m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment = VkDeviceSize(-1);
    this->m_physical_device_limits_non_coherent_atom_size = VkDeviceSize(-1);
    {
        PFN_vkGetPhysicalDeviceProperties vk_get_physical_device_properties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(vk_get_instance_proc_addr(this->m_instance, "vkGetPhysicalDeviceProperties"));
        assert(NULL != vk_get_physical_device_properties);

        struct VkPhysicalDeviceProperties physical_device_properties;
        vk_get_physical_device_properties(this->m_physical_device, &physical_device_properties);

        this->m_physical_device_limits_buffer_image_granularity = physical_device_properties.limits.bufferImageGranularity;
        this->m_physical_device_limits_min_uniform_buffer_offset_alignment = physical_device_properties.limits.minUniformBufferOffsetAlignment;
        this->m_physical_device_limits_optimal_buffer_copy_offset_alignment = physical_device_properties.limits.optimalBufferCopyOffsetAlignment;
        this->m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment = physical_device_properties.limits.optimalBufferCopyRowPitchAlignment;
        this->m_physical_device_limits_non_coherent_atom_size = physical_device_properties.limits.nonCoherentAtomSize;
    }

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
    // m_queue_GCT
    // m_queue_CT
    // m_queue_transfer
    this->m_has_dedicated_transfer_queue = false;
    this->m_queue_graphics_family_index = VK_QUEUE_FAMILY_IGNORED;
    this->m_queue_transfer_family_index = VK_QUEUE_FAMILY_IGNORED;
    this->m_queue_graphics_queue_index = uint32_t(-1);
    this->m_queue_transfer_queue_index = uint32_t(-1);
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
        } instance_internal_queue_family_properties_guard(&queue_family_properties, &queue_family_property_count, m_instance, vk_get_instance_proc_addr, m_physical_device);

        //TODO
        //support seperated present queue

        for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_property_count; ++queue_family_index)
        {
            VkQueueFamilyProperties queue_family_property = queue_family_properties[queue_family_index];

            if ((queue_family_property.queueFlags & VK_QUEUE_GRAPHICS_BIT) && platform_physical_device_presentation_support(m_physical_device, queue_family_index, wsi_connection, wsi_visual, wsi_window))
            {
                this->m_queue_graphics_family_index = queue_family_index;
                this->m_queue_graphics_queue_index = 0U;
                break;
            }
        }

        if (VK_QUEUE_FAMILY_IGNORED != this->m_queue_graphics_family_index)
        {
            // transfer queues

            for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_property_count; ++queue_family_index)
            {
                if ((this->m_queue_graphics_family_index != queue_family_index) && (VK_QUEUE_TRANSFER_BIT == (queue_family_properties[queue_family_index].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))))
                {
                    this->m_queue_transfer_family_index = queue_family_index;
                    this->m_queue_transfer_queue_index = 0U;
                    this->m_has_dedicated_transfer_queue = true;
                    break;
                }
            }

            // fall back to other graphics / compute queues

            // vkspec // either GRAPHICS or COMPUTE implies TRANSFER // make TRANSFER optional

            if (VK_QUEUE_FAMILY_IGNORED == this->m_queue_transfer_family_index)
            {
                for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_property_count; ++queue_family_index)
                {
                    if ((this->m_queue_graphics_family_index != queue_family_index) && (0 != (queue_family_properties[queue_family_index].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))))
                    {
                        this->m_queue_transfer_family_index = queue_family_index;
                        this->m_queue_transfer_queue_index = 0U;
                        this->m_has_dedicated_transfer_queue = true;
                        break;
                    }
                }
            }

            // try the same queue family

            if (VK_QUEUE_FAMILY_IGNORED == this->m_queue_transfer_family_index)
            {
                if (2U <= queue_family_properties[this->m_queue_graphics_family_index].queueCount)
                {
                    this->m_queue_transfer_family_index = this->m_queue_graphics_family_index;
                    this->m_queue_transfer_queue_index = 1U;
                    this->m_has_dedicated_transfer_queue = true;
                }
                else
                {
                    this->m_queue_transfer_family_index = VK_QUEUE_FAMILY_IGNORED;
                    this->m_queue_transfer_queue_index = uint32_t(-1);
                    this->m_has_dedicated_transfer_queue = false;
                }
            }
        }
    }
    assert((VK_QUEUE_FAMILY_IGNORED != this->m_queue_graphics_family_index) && ((!this->m_has_dedicated_transfer_queue) || (VK_QUEUE_FAMILY_IGNORED != this->m_queue_transfer_family_index)));

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
        float queue_graphics_priorities[1] = {1.0f};
        float queue_transfer_priorities[1] = {1.0f};
        struct VkDeviceQueueCreateInfo device_queue_create_infos[2];
        if (m_has_dedicated_transfer_queue)
        {
            if (m_queue_graphics_family_index != m_queue_transfer_family_index)
            {
                device_queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                device_queue_create_infos[0].pNext = NULL;
                device_queue_create_infos[0].flags = 0U;
                device_queue_create_infos[0].queueFamilyIndex = m_queue_graphics_family_index;
                assert(0U == m_queue_graphics_queue_index);
                device_queue_create_infos[0].queueCount = 1U;
                device_queue_create_infos[0].pQueuePriorities = queue_graphics_priorities;

                device_queue_create_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                device_queue_create_infos[1].pNext = NULL;
                device_queue_create_infos[1].flags = 0U;
                device_queue_create_infos[1].queueFamilyIndex = m_queue_transfer_family_index;
                assert(0U == m_queue_transfer_queue_index);
                device_queue_create_infos[1].queueCount = 1U;
                device_queue_create_infos[1].pQueuePriorities = queue_transfer_priorities;

                device_create_info.pQueueCreateInfos = device_queue_create_infos;
                device_create_info.queueCreateInfoCount = 2U;
            }
            else
            {
                device_queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                device_queue_create_infos[0].pNext = NULL;
                device_queue_create_infos[0].flags = 0U;
                device_queue_create_infos[0].queueFamilyIndex = m_queue_graphics_family_index;
                assert(0U == m_queue_graphics_queue_index);
                assert(1U == m_queue_transfer_queue_index);
                device_queue_create_infos[0].queueCount = 2U;
                device_queue_create_infos[0].pQueuePriorities = queue_graphics_priorities;
                device_create_info.pQueueCreateInfos = device_queue_create_infos;
                device_create_info.queueCreateInfoCount = 1U;
            }
        }
        else
        {
            device_queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            device_queue_create_infos[0].pNext = NULL;
            device_queue_create_infos[0].flags = 0U;
            device_queue_create_infos[0].queueFamilyIndex = m_queue_graphics_family_index;
            assert(0U == m_queue_graphics_queue_index);
            device_queue_create_infos[0].queueCount = 1U;
            device_queue_create_infos[0].pQueuePriorities = queue_graphics_priorities;
            device_create_info.pQueueCreateInfos = device_queue_create_infos;
            device_create_info.queueCreateInfoCount = 1U;
        }
        device_create_info.enabledLayerCount = 0U;
        device_create_info.ppEnabledLayerNames = NULL;
        char const *enabled_extension_names[1] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        device_create_info.enabledExtensionCount = 1U;
        device_create_info.ppEnabledExtensionNames = enabled_extension_names;
        assert(VK_FALSE == 0);
        VkPhysicalDeviceFeatures enabled_features = {0}; //all members are set to VK_FALSE
        enabled_features.textureCompressionASTC_LDR = this->m_physical_device_feature_texture_compression_ASTC_LDR ? VK_TRUE : VK_FALSE;
        enabled_features.textureCompressionBC = this->m_physical_device_feature_texture_compression_BC ? VK_TRUE : VK_FALSE;
        device_create_info.pEnabledFeatures = &enabled_features;

        PT_MAYBE_UNUSED VkResult vk_res = vk_create_device(this->m_physical_device, &device_create_info, &m_allocator_callbacks, &this->m_device);
        assert(VK_SUCCESS == vk_res);
    }
    assert(VK_NULL_HANDLE != this->m_device);

    PFN_vkGetDeviceProcAddr vk_get_device_proc_addr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vk_get_instance_proc_addr(m_instance, "vkGetDeviceProcAddr"));
    assert(NULL != vk_get_device_proc_addr);

    vk_get_device_proc_addr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vk_get_device_proc_addr(m_device, "vkGetDeviceProcAddr"));
    assert(NULL != vk_get_device_proc_addr);

    this->m_vk_create_buffer = reinterpret_cast<PFN_vkCreateBuffer>(vk_get_device_proc_addr(m_device, "vkCreateBuffer"));
    assert(NULL != this->m_vk_create_buffer);

    this->m_vk_create_image = reinterpret_cast<PFN_vkCreateImage>(vk_get_device_proc_addr(m_device, "vkCreateImage"));
    assert(NULL != this->m_vk_create_image);

    this->m_vk_destroy_buffer = reinterpret_cast<PFN_vkDestroyBuffer>(vk_get_device_proc_addr(m_device, "vkDestroyBuffer"));
    assert(NULL != this->m_vk_destroy_buffer);

    this->m_vk_destroy_image = reinterpret_cast<PFN_vkDestroyImage>(vk_get_device_proc_addr(m_device, "vkDestroyImage"));
    assert(NULL != this->m_vk_destroy_image);

    this->m_vk_get_buffer_memory_requirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(vk_get_device_proc_addr(m_device, "vkGetBufferMemoryRequirements"));
    assert(NULL != this->m_vk_get_buffer_memory_requirements);

    this->m_vk_get_image_memory_requirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(vk_get_device_proc_addr(m_device, "vkGetImageMemoryRequirements"));
    assert(NULL != this->m_vk_get_image_memory_requirements);

    this->m_vk_allocate_memory = reinterpret_cast<PFN_vkAllocateMemory>(vk_get_device_proc_addr(m_device, "vkAllocateMemory"));
    assert(NULL != this->m_vk_allocate_memory);

    this->m_vk_free_memory = reinterpret_cast<PFN_vkFreeMemory>(vk_get_device_proc_addr(m_device, "vkFreeMemory"));
    assert(NULL != this->m_vk_free_memory);

    this->m_vk_bind_buffer_memory = reinterpret_cast<PFN_vkBindBufferMemory>(vk_get_device_proc_addr(m_device, "vkBindBufferMemory"));
    assert(NULL != this->m_vk_bind_buffer_memory);

    this->m_vk_bind_image_memory = reinterpret_cast<PFN_vkBindImageMemory>(vk_get_device_proc_addr(m_device, "vkBindImageMemory"));
    assert(NULL != this->m_vk_bind_image_memory);

    this->m_vk_map_memory = reinterpret_cast<PFN_vkMapMemory>(vk_get_device_proc_addr(m_device, "vkMapMemory"));
    assert(NULL != this->m_vk_map_memory);

    this->m_vk_unmap_memory = reinterpret_cast<PFN_vkUnmapMemory>(vk_get_device_proc_addr(m_device, "vkUnmapMemory"));
    assert(NULL != this->m_vk_unmap_memory);

    this->m_vk_create_command_pool = reinterpret_cast<PFN_vkCreateCommandPool>(vk_get_device_proc_addr(m_device, "vkCreateCommandPool"));
    assert(NULL != this->m_vk_create_command_pool);

    this->m_vk_reset_command_pool = reinterpret_cast<PFN_vkResetCommandPool>(vk_get_device_proc_addr(m_device, "vkResetCommandPool"));
    assert(NULL != this->m_vk_reset_command_pool);

    this->m_vk_allocate_command_buffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(vk_get_device_proc_addr(m_device, "vkAllocateCommandBuffers"));
    assert(NULL != this->m_vk_allocate_command_buffers);

    this->m_vk_begin_command_buffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(vk_get_device_proc_addr(m_device, "vkBeginCommandBuffer"));
    assert(NULL != this->m_vk_begin_command_buffer);

    this->m_vk_end_command_buffer = reinterpret_cast<PFN_vkEndCommandBuffer>(vk_get_device_proc_addr(m_device, "vkEndCommandBuffer"));
    assert(NULL != this->m_vk_end_command_buffer);

    this->m_vk_cmd_pipeline_barrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(vk_get_device_proc_addr(m_device, "vkCmdPipelineBarrier"));
    assert(NULL != this->m_vk_cmd_pipeline_barrier);

    this->m_vk_cmd_copy_buffer_to_image = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(vk_get_device_proc_addr(m_device, "vkCmdCopyBufferToImage"));
    assert(NULL != this->m_vk_cmd_copy_buffer_to_image);

    this->m_vk_queue_submit = reinterpret_cast<PFN_vkQueueSubmit>(vk_get_device_proc_addr(m_device, "vkQueueSubmit"));
    assert(NULL != this->m_vk_queue_submit);

    this->m_vk_create_fence = reinterpret_cast<PFN_vkCreateFence>(vk_get_device_proc_addr(m_device, "vkCreateFence"));
    assert(NULL != this->m_vk_create_fence);

    this->m_vk_wait_for_fences = reinterpret_cast<PFN_vkWaitForFences>(vk_get_device_proc_addr(m_device, "vkWaitForFences"));
    assert(NULL != this->m_vk_wait_for_fences);

    this->m_vk_reset_fences = reinterpret_cast<PFN_vkResetFences>(vk_get_device_proc_addr(m_device, "vkResetFences"));
    assert(NULL != this->m_vk_reset_fences);

    this->m_vk_destory_fence = reinterpret_cast<PFN_vkDestroyFence>(vk_get_device_proc_addr(m_device, "vkDestroyFence"));
    assert(NULL != this->m_vk_destory_fence);

    this->m_vk_create_semaphore = reinterpret_cast<PFN_vkCreateSemaphore>(vk_get_device_proc_addr(m_device, "vkCreateSemaphore"));
    assert(NULL != this->m_vk_create_semaphore);

    this->m_vk_destroy_semaphore = reinterpret_cast<PFN_vkDestroySemaphore>(vk_get_device_proc_addr(m_device, "vkDestroySemaphore"));
    assert(NULL != this->m_vk_destroy_semaphore);

    this->m_vk_create_swapchain = reinterpret_cast<PFN_vkCreateSwapchainKHR>(vk_get_device_proc_addr(m_device, "vkCreateSwapchainKHR"));
    assert(NULL != this->m_vk_create_swapchain);

    this->m_queue_graphics = VK_NULL_HANDLE;
    this->m_queue_transfer = VK_NULL_HANDLE;
    {
        PFN_vkGetDeviceQueue vk_get_device_queue = reinterpret_cast<PFN_vkGetDeviceQueue>(vk_get_device_proc_addr(m_device, "vkGetDeviceQueue"));
        assert(NULL != vk_get_device_queue);

        vk_get_device_queue(this->m_device, this->m_queue_graphics_family_index, this->m_queue_graphics_queue_index, &this->m_queue_graphics);

        if (this->m_has_dedicated_transfer_queue)
        {
            assert(VK_QUEUE_FAMILY_IGNORED != this->m_queue_transfer_family_index);
            assert(uint32_t(-1) != this->m_queue_transfer_queue_index);
            vk_get_device_queue(this->m_device, this->m_queue_transfer_family_index, this->m_queue_transfer_queue_index, &this->m_queue_transfer);
        }
    }
    assert((VK_NULL_HANDLE != this->m_queue_graphics) && ((!this->m_has_dedicated_transfer_queue) || (VK_NULL_HANDLE != this->m_queue_transfer)));

    return true;
}

void gfx_device_vk::destroy()
{
#ifndef NDEBUG
    PFN_vkDestroyDebugReportCallbackEXT vk_destroy_debug_report_callback_ext = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT"));
    assert(NULL != vk_destroy_debug_report_callback_ext);
    vk_destroy_debug_report_callback_ext(m_instance, m_debug_report_callback, &m_allocator_callbacks);
    m_debug_report_callback = NULL;
#endif
}

gfx_device_vk::~gfx_device_vk()
{
    assert(NULL == m_debug_report_callback);
}

#ifndef NDEBUG
VkBool32 gfx_device_vk::debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage)
{
    mcrt_log_print("[%s] : %s \n", pLayerPrefix, pMessage);
    return VK_FALSE;
}
#endif
