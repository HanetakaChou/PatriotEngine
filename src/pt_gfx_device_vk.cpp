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

#include <stddef.h>
#include "pt_gfx_device_vk.h"
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_memset.h>
#include <pt_mcrt_log.h>
#include <assert.h>
#include <string.h>

gfx_device_vk::gfx_device_vk()
    : m_instance(VK_NULL_HANDLE),
#ifndef NDEBUG
      m_debug_report_callback(VK_NULL_HANDLE),
#endif
      m_physical_device(VK_NULL_HANDLE)
{
    return;
}

static void *VKAPI_PTR __internal_allocation_callback(void *, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
static void *VKAPI_PTR __internal_reallocation_callback(void *pUserData, void *pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
static void VKAPI_PTR __internal_free_callback(void *, void *pMemory);

#ifndef NDEBUG
static VkBool32 VKAPI_PTR __internal_debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage, void *pUserData)
{
    return static_cast<gfx_device_vk *>(pUserData)->debug_report_callback(flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
}
#endif

bool gfx_device_vk::init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual)
{
    this->m_vk_mcrt_allocation_callbacks.pUserData = NULL;
    this->m_vk_mcrt_allocation_callbacks.pfnAllocation = __internal_allocation_callback;
    this->m_vk_mcrt_allocation_callbacks.pfnReallocation = __internal_reallocation_callback;
    this->m_vk_mcrt_allocation_callbacks.pfnFree = __internal_free_callback;
    this->m_vk_mcrt_allocation_callbacks.pfnInternalAllocation = NULL;
    this->m_vk_mcrt_allocation_callbacks.pfnInternalFree = NULL;

    // perhaps due to bugs
    // the instance may free the same memory multiple times
    this->m_vk_allocation_callbacks = NULL;
    // this->m_vk_allocation_callbacks = &m_vk_mcrt_allocation_callbacks;

    PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr = vkGetInstanceProcAddr;

    this->m_instance = VK_NULL_HANDLE;
    {
        PFN_vkCreateInstance vk_create_instance = reinterpret_cast<PFN_vkCreateInstance>(vk_get_instance_proc_addr(NULL, "vkCreateInstance"));
        assert(NULL != vk_create_instance);

        VkApplicationInfo application_info;
        application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        application_info.pNext = NULL;
        application_info.pApplicationName = "PatriotEngine";
        application_info.applicationVersion = 0U;
        application_info.pEngineName = "PatriotEngine";
        application_info.engineVersion = 0U;
        application_info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instance_create_info;
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pNext = NULL;
        instance_create_info.flags = 0U;
        instance_create_info.pApplicationInfo = &application_info;
#ifndef NDEBUG

#if defined(VK_API_VERSION_1_2)
        char const *enabled_layer_names[1] = {"VK_LAYER_KHRONOS_validation"};
        instance_create_info.enabledLayerCount = 1U;
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

        PT_MAYBE_UNUSED VkResult vk_res = vk_create_instance(&instance_create_info, this->m_vk_allocation_callbacks, &this->m_instance);
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

    this->m_vk_platform_create_surface = vk_get_instance_proc_addr(this->m_instance, platform_create_surface_function_name());
    assert(NULL != this->m_vk_platform_create_surface);

    this->m_vk_destroy_surface = reinterpret_cast<PFN_vkDestroySurfaceKHR>(vk_get_instance_proc_addr(this->m_instance, "vkDestroySurfaceKHR"));
    assert(NULL != this->m_vk_destroy_surface);

#ifndef NDEBUG
    assert(VK_NULL_HANDLE == this->m_debug_report_callback);
    {
        PFN_vkCreateDebugReportCallbackEXT vk_create_debug_report_callback_ext = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vk_get_instance_proc_addr(m_instance, "vkCreateDebugReportCallbackEXT"));
        assert(NULL != vk_create_debug_report_callback_ext);

        VkDebugReportCallbackCreateInfoEXT debug_report_callback_create_info;
        debug_report_callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        debug_report_callback_create_info.pNext = NULL;
        debug_report_callback_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debug_report_callback_create_info.pfnCallback = __internal_debug_report_callback;
        debug_report_callback_create_info.pUserData = this;

        VkResult vk_res = vk_create_debug_report_callback_ext(this->m_instance, &debug_report_callback_create_info, this->m_vk_allocation_callbacks, &this->m_debug_report_callback);
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
                PT_MAYBE_UNUSED VkResult res_enumerate_physical_devices_before = vk_enumerate_physical_devices(instance, &physical_device_count_before, NULL);
                assert(VK_SUCCESS == res_enumerate_physical_devices_before);

                (*m_physical_device_count) = physical_device_count_before;
                (*m_physical_devices) = static_cast<VkPhysicalDevice *>(mcrt_aligned_malloc(sizeof(VkPhysicalDevice) * (*m_physical_device_count), alignof(VkPhysicalDevice)));
                PT_MAYBE_UNUSED VkResult res_enumerate_physical_devices = vk_enumerate_physical_devices(instance, m_physical_device_count, (*m_physical_devices));
                assert(physical_device_count_before == (*m_physical_device_count));
                assert(VK_SUCCESS == res_enumerate_physical_devices);
            }
            inline ~internal_physical_devices_guard()
            {
                mcrt_aligned_free((*m_physical_devices));
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

        this->m_physical_device_pipeline_vendor_id = physical_device_properties.vendorID;
        this->m_physical_device_pipeline_device_id = physical_device_properties.deviceID;
        static_assert(sizeof(mcrt_uuid) == sizeof(VkPhysicalDeviceProperties::pipelineCacheUUID), "");
        this->m_physical_device_pipeline_cache_uuid = mcrt_uuid_load(physical_device_properties.pipelineCacheUUID);
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
                (*m_queue_family_properties) = static_cast<VkQueueFamilyProperties *>(mcrt_aligned_malloc(sizeof(VkQueueFamilyProperties) * (*m_queue_family_property_count), alignof(VkQueueFamilyProperties)));
                vk_get_physical_device_queue_family_properties(physical_device, m_queue_family_property_count, (*m_queue_family_properties));
                assert(queue_family_property_count_before == (*m_queue_family_property_count));
            }
            inline ~internal_queue_family_properties_guard()
            {
                mcrt_aligned_free((*m_queue_family_properties));
            }
        } instance_internal_queue_family_properties_guard(&queue_family_properties, &queue_family_property_count, m_instance, vk_get_instance_proc_addr, m_physical_device);

        //TODO
        //support seperated present queue

        for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_property_count; ++queue_family_index)
        {
            VkQueueFamilyProperties queue_family_property = queue_family_properties[queue_family_index];

            if ((queue_family_property.queueFlags & VK_QUEUE_GRAPHICS_BIT) && platform_physical_device_presentation_support(vk_get_instance_proc_addr, m_physical_device, queue_family_index, wsi_connection, wsi_visual))
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

    // perhaps due to bugs
    // the instance may free the same memory multiple times
    this->m_vk_allocation_callbacks = &m_vk_mcrt_allocation_callbacks;

    this->m_device = VK_NULL_HANDLE;
    {

        PFN_vkCreateDevice vk_create_device = reinterpret_cast<PFN_vkCreateDevice>(vk_get_instance_proc_addr(m_instance, "vkCreateDevice"));
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

        PT_MAYBE_UNUSED VkResult vk_res = vk_create_device(this->m_physical_device, &device_create_info, this->m_vk_allocation_callbacks, &this->m_device);
        assert(VK_SUCCESS == vk_res);
    }
    assert(VK_NULL_HANDLE != this->m_device);

    PFN_vkGetDeviceProcAddr vk_get_device_proc_addr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vk_get_instance_proc_addr(m_instance, "vkGetDeviceProcAddr"));
    assert(NULL != vk_get_device_proc_addr);

    vk_get_device_proc_addr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vk_get_device_proc_addr(this->m_device, "vkGetDeviceProcAddr"));
    assert(NULL != vk_get_device_proc_addr);

    this->m_vk_create_buffer = reinterpret_cast<PFN_vkCreateBuffer>(vk_get_device_proc_addr(this->m_device, "vkCreateBuffer"));
    assert(NULL != this->m_vk_create_buffer);

    this->m_vk_create_image = reinterpret_cast<PFN_vkCreateImage>(vk_get_device_proc_addr(this->m_device, "vkCreateImage"));
    assert(NULL != this->m_vk_create_image);

    this->m_vk_destroy_buffer = reinterpret_cast<PFN_vkDestroyBuffer>(vk_get_device_proc_addr(this->m_device, "vkDestroyBuffer"));
    assert(NULL != this->m_vk_destroy_buffer);

    this->m_vk_destroy_image = reinterpret_cast<PFN_vkDestroyImage>(vk_get_device_proc_addr(this->m_device, "vkDestroyImage"));
    assert(NULL != this->m_vk_destroy_image);

    this->m_vk_get_buffer_memory_requirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(vk_get_device_proc_addr(this->m_device, "vkGetBufferMemoryRequirements"));
    assert(NULL != this->m_vk_get_buffer_memory_requirements);

    this->m_vk_get_image_memory_requirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(vk_get_device_proc_addr(this->m_device, "vkGetImageMemoryRequirements"));
    assert(NULL != this->m_vk_get_image_memory_requirements);

    this->m_vk_allocate_memory = reinterpret_cast<PFN_vkAllocateMemory>(vk_get_device_proc_addr(this->m_device, "vkAllocateMemory"));
    assert(NULL != this->m_vk_allocate_memory);

    this->m_vk_free_memory = reinterpret_cast<PFN_vkFreeMemory>(vk_get_device_proc_addr(this->m_device, "vkFreeMemory"));
    assert(NULL != this->m_vk_free_memory);

    this->m_vk_bind_buffer_memory = reinterpret_cast<PFN_vkBindBufferMemory>(vk_get_device_proc_addr(this->m_device, "vkBindBufferMemory"));
    assert(NULL != this->m_vk_bind_buffer_memory);

    this->m_vk_bind_image_memory = reinterpret_cast<PFN_vkBindImageMemory>(vk_get_device_proc_addr(this->m_device, "vkBindImageMemory"));
    assert(NULL != this->m_vk_bind_image_memory);

    this->m_vk_map_memory = reinterpret_cast<PFN_vkMapMemory>(vk_get_device_proc_addr(this->m_device, "vkMapMemory"));
    assert(NULL != this->m_vk_map_memory);

    this->m_vk_unmap_memory = reinterpret_cast<PFN_vkUnmapMemory>(vk_get_device_proc_addr(this->m_device, "vkUnmapMemory"));
    assert(NULL != this->m_vk_unmap_memory);

    this->m_vk_create_command_pool = reinterpret_cast<PFN_vkCreateCommandPool>(vk_get_device_proc_addr(this->m_device, "vkCreateCommandPool"));
    assert(NULL != this->m_vk_create_command_pool);

    this->m_vk_reset_command_pool = reinterpret_cast<PFN_vkResetCommandPool>(vk_get_device_proc_addr(this->m_device, "vkResetCommandPool"));
    assert(NULL != this->m_vk_reset_command_pool);

    this->m_vk_destroy_command_pool = reinterpret_cast<PFN_vkDestroyCommandPool>(vk_get_device_proc_addr(this->m_device, "vkDestroyCommandPool"));
    assert(NULL != this->m_vk_destroy_command_pool);

    this->m_vk_allocate_command_buffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(vk_get_device_proc_addr(this->m_device, "vkAllocateCommandBuffers"));
    assert(NULL != this->m_vk_allocate_command_buffers);

    this->m_vk_begin_command_buffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(vk_get_device_proc_addr(this->m_device, "vkBeginCommandBuffer"));
    assert(NULL != this->m_vk_begin_command_buffer);

    this->m_vk_end_command_buffer = reinterpret_cast<PFN_vkEndCommandBuffer>(vk_get_device_proc_addr(this->m_device, "vkEndCommandBuffer"));
    assert(NULL != this->m_vk_end_command_buffer);

    this->m_vk_cmd_copy_buffer = reinterpret_cast<PFN_vkCmdCopyBuffer>(vk_get_device_proc_addr(this->m_device, "vkCmdCopyBuffer"));
    assert(NULL != this->m_vk_cmd_copy_buffer);

    this->m_vk_cmd_pipeline_barrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(vk_get_device_proc_addr(this->m_device, "vkCmdPipelineBarrier"));
    assert(NULL != this->m_vk_cmd_pipeline_barrier);

    this->m_vk_cmd_copy_buffer_to_image = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(vk_get_device_proc_addr(this->m_device, "vkCmdCopyBufferToImage"));
    assert(NULL != this->m_vk_cmd_copy_buffer_to_image);

    this->m_vk_queue_submit = reinterpret_cast<PFN_vkQueueSubmit>(vk_get_device_proc_addr(this->m_device, "vkQueueSubmit"));
    assert(NULL != this->m_vk_queue_submit);

    this->m_vk_create_fence = reinterpret_cast<PFN_vkCreateFence>(vk_get_device_proc_addr(this->m_device, "vkCreateFence"));
    assert(NULL != this->m_vk_create_fence);

    this->m_vk_wait_for_fences = reinterpret_cast<PFN_vkWaitForFences>(vk_get_device_proc_addr(this->m_device, "vkWaitForFences"));
    assert(NULL != this->m_vk_wait_for_fences);

    this->m_vk_reset_fences = reinterpret_cast<PFN_vkResetFences>(vk_get_device_proc_addr(this->m_device, "vkResetFences"));
    assert(NULL != this->m_vk_reset_fences);

    this->m_vk_destory_fence = reinterpret_cast<PFN_vkDestroyFence>(vk_get_device_proc_addr(this->m_device, "vkDestroyFence"));
    assert(NULL != this->m_vk_destory_fence);

    this->m_vk_create_semaphore = reinterpret_cast<PFN_vkCreateSemaphore>(vk_get_device_proc_addr(this->m_device, "vkCreateSemaphore"));
    assert(NULL != this->m_vk_create_semaphore);

    this->m_vk_destroy_semaphore = reinterpret_cast<PFN_vkDestroySemaphore>(vk_get_device_proc_addr(this->m_device, "vkDestroySemaphore"));
    assert(NULL != this->m_vk_destroy_semaphore);

    this->m_vk_create_swapchain = reinterpret_cast<PFN_vkCreateSwapchainKHR>(vk_get_device_proc_addr(this->m_device, "vkCreateSwapchainKHR"));
    assert(NULL != this->m_vk_create_swapchain);

    this->m_vk_destroy_swapchain = reinterpret_cast<PFN_vkDestroySwapchainKHR>(vk_get_device_proc_addr(this->m_device, "vkDestroySwapchainKHR"));
    assert(NULL != this->m_vk_destroy_swapchain);

    this->m_vk_get_swapchain_images = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vk_get_device_proc_addr(this->m_device, "vkGetSwapchainImagesKHR"));
    assert(NULL != this->m_vk_get_swapchain_images);

    this->m_vk_create_render_pass = reinterpret_cast<PFN_vkCreateRenderPass>(vk_get_device_proc_addr(this->m_device, "vkCreateRenderPass"));
    assert(NULL != this->m_vk_create_render_pass);

    this->m_vk_destroy_render_pass = reinterpret_cast<PFN_vkDestroyRenderPass>(vk_get_device_proc_addr(this->m_device, "vkDestroyRenderPass"));
    assert(NULL != this->m_vk_destroy_render_pass);

    this->m_vk_create_image_view = reinterpret_cast<PFN_vkCreateImageView>(vk_get_device_proc_addr(this->m_device, "vkCreateImageView"));
    assert(NULL != this->m_vk_create_image_view);

    this->m_vk_destory_image_view = reinterpret_cast<PFN_vkDestroyImageView>(vk_get_device_proc_addr(this->m_device, "vkDestroyImageView"));
    assert(NULL != this->m_vk_destory_image_view);

    this->m_vk_create_framebuffer = reinterpret_cast<PFN_vkCreateFramebuffer>(vk_get_device_proc_addr(this->m_device, "vkCreateFramebuffer"));
    assert(NULL != this->m_vk_create_framebuffer);

    this->m_vk_destory_framebuffer = reinterpret_cast<PFN_vkDestroyFramebuffer>(vk_get_device_proc_addr(this->m_device, "vkDestroyFramebuffer"));
    assert(NULL != this->m_vk_destory_framebuffer);

    this->m_vk_create_sampler = reinterpret_cast<PFN_vkCreateSampler>(vk_get_device_proc_addr(this->m_device, "vkCreateSampler"));
    assert(NULL != this->m_vk_create_sampler);

    this->m_vk_destroy_sampler = reinterpret_cast<PFN_vkDestroySampler>(vk_get_device_proc_addr(this->m_device, "vkDestroySampler"));
    assert(NULL != this->m_vk_destroy_sampler);

    this->m_vk_create_descriptor_set_layout = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(vk_get_device_proc_addr(this->m_device, "vkCreateDescriptorSetLayout"));
    assert(NULL != this->m_vk_create_descriptor_set_layout);

    this->m_vk_destroy_descriptor_set_layout = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(vk_get_device_proc_addr(this->m_device, "vkDestroyDescriptorSetLayout"));
    assert(NULL != this->m_vk_destroy_descriptor_set_layout);

    this->m_vk_create_pipeline_layout = reinterpret_cast<PFN_vkCreatePipelineLayout>(vk_get_device_proc_addr(this->m_device, "vkCreatePipelineLayout"));
    assert(NULL != this->m_vk_create_pipeline_layout);

    this->m_vk_destroy_pipeline_layout = reinterpret_cast<PFN_vkDestroyPipelineLayout>(vk_get_device_proc_addr(this->m_device, "vkDestroyPipelineLayout"));
    assert(NULL != this->m_vk_destroy_pipeline_layout);

    this->m_vk_create_descriptor_pool = reinterpret_cast<PFN_vkCreateDescriptorPool>(vk_get_device_proc_addr(this->m_device, "vkCreateDescriptorPool"));
    assert(NULL != this->m_vk_create_descriptor_pool);

    this->m_vk_destroy_descriptor_pool = reinterpret_cast<PFN_vkDestroyDescriptorPool>(vk_get_device_proc_addr(this->m_device, "vkDestroyDescriptorPool"));
    assert(NULL != this->m_vk_destroy_descriptor_pool);

    this->m_vk_allocate_descriptor_sets = reinterpret_cast<PFN_vkAllocateDescriptorSets>(vk_get_device_proc_addr(this->m_device, "vkAllocateDescriptorSets"));
    assert(NULL != this->m_vk_allocate_descriptor_sets);

    this->m_vk_update_descriptor_sets = reinterpret_cast<PFN_vkUpdateDescriptorSets>(vk_get_device_proc_addr(this->m_device, "vkUpdateDescriptorSets"));
    assert(NULL != this->m_vk_update_descriptor_sets);

    this->m_vk_create_shader_module = reinterpret_cast<PFN_vkCreateShaderModule>(vk_get_device_proc_addr(this->m_device, "vkCreateShaderModule"));
    assert(NULL != this->m_vk_create_shader_module);

    this->m_vk_destroy_shader_module = reinterpret_cast<PFN_vkDestroyShaderModule>(vk_get_device_proc_addr(this->m_device, "vkDestroyShaderModule"));
    assert(NULL != this->m_vk_destroy_shader_module);

    this->m_vk_create_graphics_pipelines = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(vk_get_device_proc_addr(this->m_device, "vkCreateGraphicsPipelines"));
    assert(NULL != this->m_vk_create_graphics_pipelines);

    this->m_vk_destroy_pipeline = reinterpret_cast<PFN_vkDestroyPipeline>(vk_get_device_proc_addr(this->m_device, "vkDestroyPipeline"));
    assert(NULL != this->m_vk_destroy_pipeline);

    this->m_vk_create_pipeline_cache = reinterpret_cast<PFN_vkCreatePipelineCache>(vk_get_device_proc_addr(this->m_device, "vkCreatePipelineCache"));
    assert(NULL != this->m_vk_create_pipeline_cache);

    this->m_vk_get_pipeline_cache_data = reinterpret_cast<PFN_vkGetPipelineCacheData>(vk_get_device_proc_addr(this->m_device, "vkGetPipelineCacheData"));
    assert(NULL != this->m_vk_get_pipeline_cache_data);

    this->m_vk_destroy_pipeline_cache = reinterpret_cast<PFN_vkDestroyPipelineCache>(vk_get_device_proc_addr(this->m_device, "vkDestroyPipelineCache"));
    assert(NULL != this->m_vk_destroy_pipeline_cache);

    this->m_vk_acquire_next_image = reinterpret_cast<PFN_vkAcquireNextImageKHR>(vk_get_device_proc_addr(this->m_device, "vkAcquireNextImageKHR"));
    assert(NULL != this->m_vk_acquire_next_image);

    this->m_vk_queue_present = reinterpret_cast<PFN_vkQueuePresentKHR>(vk_get_device_proc_addr(this->m_device, "vkQueuePresentKHR"));
    assert(NULL != this->m_vk_queue_present);

    this->m_vk_cmd_begin_render_pass = reinterpret_cast<PFN_vkCmdBeginRenderPass>(vk_get_device_proc_addr(this->m_device, "vkCmdBeginRenderPass"));
    assert(NULL != this->m_vk_cmd_begin_render_pass);

    this->m_vk_cmd_execute_commands = reinterpret_cast<PFN_vkCmdExecuteCommands>(vk_get_device_proc_addr(this->m_device, "vkCmdExecuteCommands"));
    assert(NULL != this->m_vk_cmd_execute_commands);

    this->m_vk_cmd_bind_pipeline = reinterpret_cast<PFN_vkCmdBindPipeline>(vk_get_device_proc_addr(this->m_device, "vkCmdBindPipeline"));
    assert(NULL != this->m_vk_cmd_bind_pipeline);

    this->m_vk_cmd_set_viewport = reinterpret_cast<PFN_vkCmdSetViewport>(vk_get_device_proc_addr(this->m_device, "vkCmdSetViewport"));
    assert(NULL != this->m_vk_cmd_set_viewport);

    this->m_vk_cmd_set_scissor = reinterpret_cast<PFN_vkCmdSetScissor>(vk_get_device_proc_addr(this->m_device, "vkCmdSetScissor"));
    assert(NULL != this->m_vk_cmd_set_scissor);

    this->m_vk_cmd_bind_vertex_buffers = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(vk_get_device_proc_addr(this->m_device, "vkCmdBindVertexBuffers"));
    assert(NULL != this->m_vk_cmd_bind_vertex_buffers);

    this->m_vk_cmd_bind_index_buffer = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(vk_get_device_proc_addr(this->m_device, "vkCmdBindIndexBuffer"));
    assert(NULL != this->m_vk_cmd_bind_index_buffer);

    this->m_vk_cmd_push_constants = reinterpret_cast<PFN_vkCmdPushConstants>(vk_get_device_proc_addr(this->m_device, "vkCmdPushConstants"));
    assert(NULL != this->m_vk_cmd_push_constants);

    this->m_vk_cmd_bind_descriptor_sets = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(vk_get_device_proc_addr(this->m_device, "vkCmdBindDescriptorSets"));
    assert(NULL != this->m_vk_cmd_bind_descriptor_sets);

    this->m_vk_cmd_draw = reinterpret_cast<PFN_vkCmdDraw>(vk_get_device_proc_addr(this->m_device, "vkCmdDraw"));
    assert(NULL != this->m_vk_cmd_draw);

    this->m_vk_cmd_end_render_pass = reinterpret_cast<PFN_vkCmdEndRenderPass>(vk_get_device_proc_addr(this->m_device, "vkCmdEndRenderPass"));
    assert(NULL != this->m_vk_cmd_end_render_pass);

    this->m_queue_graphics = VK_NULL_HANDLE;
    this->m_queue_transfer = VK_NULL_HANDLE;
    {
        PFN_vkGetDeviceQueue vk_get_device_queue = reinterpret_cast<PFN_vkGetDeviceQueue>(vk_get_device_proc_addr(this->m_device, "vkGetDeviceQueue"));
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
    PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(vkGetInstanceProcAddr(this->m_instance, "vkGetInstanceProcAddr"));

    vk_get_instance_proc_addr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(vk_get_instance_proc_addr(this->m_instance, "vkGetInstanceProcAddr"));
    assert(NULL != vk_get_instance_proc_addr);

    PFN_vkGetDeviceProcAddr vk_get_device_proc_addr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vk_get_instance_proc_addr(m_instance, "vkGetDeviceProcAddr"));
    assert(NULL != vk_get_device_proc_addr);

    vk_get_device_proc_addr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vk_get_device_proc_addr(this->m_device, "vkGetDeviceProcAddr"));
    assert(NULL != vk_get_device_proc_addr);

    PFN_vkDestroyDevice vk_destroy_device = reinterpret_cast<PFN_vkDestroyDevice>(vk_get_device_proc_addr(this->m_device, "vkDestroyDevice"));
    assert(NULL != vk_destroy_device);

    vk_destroy_device(this->m_device, this->m_vk_allocation_callbacks);

    // perhaps due to bugs
    // the instance may free the same memory multiple times
    this->m_vk_allocation_callbacks = NULL;

#ifndef NDEBUG
    PFN_vkDestroyDebugReportCallbackEXT vk_destroy_debug_report_callback_ext = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vk_get_instance_proc_addr(m_instance, "vkDestroyDebugReportCallbackEXT"));
    assert(NULL != vk_destroy_debug_report_callback_ext);
    vk_destroy_debug_report_callback_ext(m_instance, m_debug_report_callback, this->m_vk_allocation_callbacks);
    m_debug_report_callback = VK_NULL_HANDLE;
#endif

    PFN_vkDestroyInstance vk_destroy_instance = reinterpret_cast<PFN_vkDestroyInstance>(vk_get_instance_proc_addr(this->m_instance, "vkDestroyInstance"));
    assert(NULL != vk_destroy_instance);

    vk_destroy_instance(this->m_instance,this->m_vk_allocation_callbacks);
}

gfx_device_vk::~gfx_device_vk()
{
    assert(VK_NULL_HANDLE == m_debug_report_callback);
}

#ifndef NDEBUG
inline VkBool32 gfx_device_vk::debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage)
{
    mcrt_log_print("[%s] : %s \n", pLayerPrefix, pMessage);
    return VK_FALSE;
}
#endif

#include <sys/cdefs.h> //__BIONIC__
//#include <features.h> //__GLIBC__

#if defined(PT_GFX_DEBUG_MALLOC) && PT_GFX_DEBUG_MALLOC && defined(PT_POSIX_LINUX) && defined(__GLIBC__)
#include <map>
#include <utility>
#include <pt_mcrt_spinlock.h>

#include <dlfcn.h>
#include <execinfo.h>

class gfx_malloc_verify_support
{
    mcrt_spinlock_t m_spin_lock;
    std::map<void *, void *> m_memory_objects;

public:
    inline gfx_malloc_verify_support()
    {
        mcrt_spin_init(&this->m_spin_lock);
    }

    inline void insert(void *ptr)
    {
        mcrt_spin_lock(&this->m_spin_lock);
        auto iter = this->m_memory_objects.find(ptr);
        if (this->m_memory_objects.end() != iter)
        {
            void *object_addr = iter->first;
            void *funtion_addr = iter->second;
            // set breakpoint here and the debugger can tell you the name of the function by the address
            assert(0);
            object_addr = NULL;
            funtion_addr = NULL;
        }

        void *addrs_ptr[5];
        int num_levels = backtrace(addrs_ptr, 5);
        if (num_levels >= 5)
        {
            m_memory_objects.emplace(std::piecewise_construct, std::forward_as_tuple(ptr), std::forward_as_tuple(addrs_ptr[4]));
        }
        else
        {
            m_memory_objects.emplace(std::piecewise_construct, std::forward_as_tuple(ptr), std::forward_as_tuple(static_cast<void *>(NULL)));
        }

        mcrt_spin_unlock(&this->m_spin_lock);
    }

    inline void erase(void *ptr)
    {
        mcrt_spin_lock(&this->m_spin_lock);
        auto iter = this->m_memory_objects.find(ptr);
        assert(this->m_memory_objects.end() != iter);
        this->m_memory_objects.erase(iter);
        mcrt_spin_unlock(&this->m_spin_lock);
    }

    inline ~gfx_malloc_verify_support()
    {
        for (auto const &item : this->m_memory_objects)
        {
            void *object_addr = item.first;
            void *funtion_addr = item.second;
            // set breakpoint here and the debugger can tell you the name of the function by the address
            assert(0);
            object_addr = NULL;
            funtion_addr = NULL;
        }
    }

} instance_mcrt_malloc_verify_support;
#endif

static void *VKAPI_PTR __internal_allocation_callback(void *, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    if (0U != size)
    {
        void *pMemory = mcrt_aligned_malloc(size, alignment);
        assert(NULL != pMemory);
        mcrt_memset(pMemory, 0, size);
#if defined(PT_GFX_DEBUG_MALLOC) && PT_GFX_DEBUG_MALLOC && defined(PT_POSIX_LINUX) && defined(__GLIBC__)
        instance_mcrt_malloc_verify_support.insert(pMemory);
#endif
        return pMemory;
    }
    else
    {
        assert(0);
        return NULL;
    }
}

static void *VKAPI_PTR __internal_reallocation_callback(void *pUserData, void *pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    if (NULL == pOriginal)
    {
        if (0U != size)
        {
            return __internal_allocation_callback(pUserData, size, alignment, allocationScope);
        }
        else
        {
            assert(0);
            return NULL;
        }
    }
    else if (0U == size)
    {
        __internal_free_callback(pUserData, pOriginal);
        return NULL;
    }
    else
    {
        return mcrt_aligned_realloc(pOriginal, size, alignment);
    }
}

static void VKAPI_PTR __internal_free_callback(void *, void *pMemory)
{
    if (NULL != pMemory)
    {
#if defined(PT_GFX_DEBUG_MALLOC) && PT_GFX_DEBUG_MALLOC && defined(PT_POSIX_LINUX) && defined(__GLIBC__)
        instance_mcrt_malloc_verify_support.erase(pMemory);
#endif
        return mcrt_aligned_free(pMemory);
    }
    else
    {
        return;
    }
}