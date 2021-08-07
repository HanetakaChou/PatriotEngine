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
#include <assert.h>
#include <pt_mcrt_intrin.h>
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_malloc.h>
#include "pt_gfx_texture_vk.h"
#include <vulkan/vulkan.h>
#include <new>

bool gfx_texture_vk::read_input_stream(
    char const *initial_filename,
    gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
    intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
    int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
    void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
{

    // How to implement pipeline "serial - parallel - serial"
    // follow [McCool 2012] "Structured Parallel Programming: Patterns for Efficient Computation." / 9.4.2 Pipeline in Cilk Plus
    // the second stage is trivially implemented by the task spawned by the first stage
    // the third stage can be implemented as "reduce"

    // We don't have the third stage here

    // the first stage

    streaming_status_t streaming_status = mcrt_atomic_load(&this->m_streaming_status);
    bool streaming_error = mcrt_atomic_load(&this->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&this->m_streaming_cancel);
    assert(STREAMING_STATUS_STAGE_FIRST == streaming_status);
    assert(!streaming_error);

    if (!streaming_cancel)
    {
        gfx_input_stream_ref input_stream;
        {
            class gfx_input_stream_guard
            {
                gfx_input_stream_ref &m_input_stream;
                void(PT_PTR *m_input_stream_destroy_callback)(gfx_input_stream_ref input_stream);

            public:
                inline gfx_input_stream_guard(
                    gfx_input_stream_ref &input_stream,
                    char const *initial_filename,
                    gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
                    void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
                    : m_input_stream(input_stream),
                      m_input_stream_destroy_callback(input_stream_destroy_callback)
                {
                    m_input_stream = input_stream_init_callback(initial_filename);
                }
                inline ~gfx_input_stream_guard()
                {
                    m_input_stream_destroy_callback(m_input_stream);
                }
            } input_stream_guard(input_stream, initial_filename, input_stream_init_callback, input_stream_destroy_callback);

            struct common_header_t common_header;
            {
                size_t common_data_offset;
                bool res_load_header_from_input_stream = this->load_header_from_input_stream(&common_header, &common_data_offset, input_stream, input_stream_read_callback, input_stream_seek_callback);
                if (PT_UNLIKELY(!res_load_header_from_input_stream))
                {
                    mcrt_atomic_store(&this->m_streaming_error, true);
                    return false;
                }
            }

            specific_header_vk_t specific_header_vk = common_to_specific_header_translate(&common_header);

            // vkGetPhysicalDeviceFormatProperties
            // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior
            {
                struct VkFormatProperties physical_device_format_properties;
                m_gfx_connection->get_physical_device_format_properties(specific_header_vk.format, &physical_device_format_properties);
                if (PT_UNLIKELY(0 == (physical_device_format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)))
                {
                    mcrt_atomic_store(&this->m_streaming_error, true);
                    return false;
                }
            }

            assert(VK_NULL_HANDLE == this->m_image);
            {
                VkImageCreateInfo create_info;
                create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                create_info.pNext = NULL;
                create_info.flags = ((!specific_header_vk.isCubeCompatible) ? 0U : VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
                create_info.imageType = specific_header_vk.imageType;
                create_info.format = specific_header_vk.format;
                create_info.extent = specific_header_vk.extent;
                create_info.mipLevels = specific_header_vk.mipLevels;
                create_info.arrayLayers = specific_header_vk.arrayLayers;
                create_info.samples = VK_SAMPLE_COUNT_1_BIT;
                create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
                create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                create_info.queueFamilyIndexCount = 0U;
                create_info.pQueueFamilyIndices = NULL;
                create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                VkResult res = m_gfx_connection->create_image(&create_info, &this->m_image);
                assert(VK_SUCCESS == res);
            }

            assert(VK_NULL_HANDLE == this->m_gfx_malloc_device_memory);
            {
                VkMemoryRequirements memory_requirements;
                this->m_gfx_connection->get_image_memory_requirements(m_image, &memory_requirements);

                // vkAllocateMemory
                // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior

                this->m_gfx_malloc_device_memory = this->m_gfx_connection->transfer_dst_and_sampled_image_alloc(&memory_requirements, &this->m_gfx_malloc_page_handle, &this->m_gfx_malloc_offset, &this->m_gfx_malloc_size);
                if (PT_UNLIKELY(VK_NULL_HANDLE == this->m_gfx_malloc_device_memory))
                {
                    this->m_gfx_connection->destroy_image(this->m_image);
                    this->m_image = VK_NULL_HANDLE;
                    mcrt_atomic_store(&this->m_streaming_error, true);
                    return false;
                }
            }
            assert(VK_NULL_HANDLE != this->m_gfx_malloc_device_memory);

            {
                VkResult res_bind_image_memory = this->m_gfx_connection->bind_image_memory(this->m_image, this->m_gfx_malloc_device_memory, this->m_gfx_malloc_offset);
                assert(VK_SUCCESS == res_bind_image_memory);
            }
        }

        // pass to the second stage

        this->m_asset_filename = initial_filename;
        this->m_input_stream_init_callback = input_stream_init_callback;
        this->m_input_stream_read_callback = input_stream_read_callback;
        this->m_input_stream_seek_callback = input_stream_seek_callback;
        this->m_input_stream_destroy_callback = input_stream_destroy_callback;

        {
            mcrt_task_ref task = mcrt_task_allocate_root(read_input_stream_task_execute);
            static_assert(sizeof(read_input_stream_task_data) <= sizeof(mcrt_task_user_data_t), "");
            read_input_stream_task_data *task_data = reinterpret_cast<read_input_stream_task_data *>(mcrt_task_get_user_data(task));
            task_data->m_initial_filename = this->m_asset_filename.c_str();
            task_data->m_input_stream_init_callback = this->m_input_stream_init_callback;
            task_data->m_input_stream_read_callback = this->m_input_stream_read_callback;
            task_data->m_input_stream_seek_callback = this->m_input_stream_seek_callback;
            task_data->m_input_stream_destroy_callback = this->m_input_stream_destroy_callback;
            task_data->m_gfx_texture = this;
            // we must cache the streaming_throttling_index to eusure the consistency
            // streaming_throttling_index = streaming_throttling_index;

            // race condition
            // mcrt_task_wait_for_all may happens here and return
            // move streaming_object_list_push to "read_input_stream_task_execute"

            mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_STAGE_SECOND);
            mcrt_task_spawn(task);
        }
    }
    else
    {
        // race condition with the "streaming_done"
        // inevitable since the "transfer_dst_and_sampled_image_alloc" nervertheless races with the "streaming_done"
        this->streaming_cancel();
    }

    return true;
}

void gfx_texture_vk::streaming_task_respawn()
{
    streaming_status_t streaming_status = mcrt_atomic_load(&this->m_streaming_status);
    bool streaming_error = mcrt_atomic_load(&this->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&this->m_streaming_cancel);
    assert(STREAMING_STATUS_STAGE_SECOND == streaming_status);
    assert(!streaming_error);

    if (!streaming_cancel)
    {
        mcrt_task_ref task = mcrt_task_allocate_root(read_input_stream_task_execute);
        static_assert(sizeof(read_input_stream_task_data) <= sizeof(mcrt_task_user_data_t), "");
        read_input_stream_task_data *task_data = reinterpret_cast<read_input_stream_task_data *>(mcrt_task_get_user_data(task));
        task_data->m_initial_filename = this->m_asset_filename.c_str();
        task_data->m_input_stream_init_callback = this->m_input_stream_init_callback;
        task_data->m_input_stream_read_callback = this->m_input_stream_read_callback;
        task_data->m_input_stream_seek_callback = this->m_input_stream_seek_callback;
        task_data->m_input_stream_destroy_callback = this->m_input_stream_destroy_callback;
        task_data->m_gfx_texture = this;

        mcrt_task_spawn(task);
    }
    else
    {
        this->streaming_cancel();
    }
}

mcrt_task_ref gfx_texture_vk::read_input_stream_task_execute(mcrt_task_ref self)
{
    static_assert(sizeof(read_input_stream_task_data) <= sizeof(mcrt_task_user_data_t), "");
    read_input_stream_task_data *task_data = reinterpret_cast<read_input_stream_task_data *>(mcrt_task_get_user_data(self));

    streaming_status_t streaming_status = mcrt_atomic_load(&task_data->m_gfx_texture->m_streaming_status);
    bool streaming_error = mcrt_atomic_load(&task_data->m_gfx_texture->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&task_data->m_gfx_texture->m_streaming_cancel);
    assert(STREAMING_STATUS_STAGE_SECOND == streaming_status);
    assert(!streaming_error);

    // race condition
    // task: current_streaming_throttling_index
    // reduce_streaming_task: ++streaming_throttling_index
    // reduce_streaming_task: mcrt_task_wait_for_all // return immediately
    // task: mcrt_task_increment_ref_count
    task_data->m_gfx_texture->m_gfx_connection->streaming_throttling_index_lock();
    uint32_t streaming_throttling_index = task_data->m_gfx_texture->m_gfx_connection->streaming_throttling_index();
    mcrt_task_increment_ref_count(task_data->m_gfx_texture->m_gfx_connection->streaming_task_root(streaming_throttling_index));
    task_data->m_gfx_texture->m_gfx_connection->streaming_throttling_index_unlock();

    mcrt_task_set_parent(self, task_data->m_gfx_texture->m_gfx_connection->streaming_task_root(streaming_throttling_index));
    {
#ifndef NDEBUG
        class internal_streaming_task_debug_executing_guard
        {
            uint32_t m_streaming_throttling_index;
            class gfx_connection_vk *m_gfx_connection;

        public:
            inline internal_streaming_task_debug_executing_guard(uint32_t streaming_throttling_index, class gfx_connection_vk *gfx_connection)
                : m_streaming_throttling_index(streaming_throttling_index), m_gfx_connection(gfx_connection)
            {
                m_gfx_connection->streaming_task_debug_executing_begin(m_streaming_throttling_index);
            }
            inline ~internal_streaming_task_debug_executing_guard()
            {
                m_gfx_connection->streaming_task_debug_executing_end(m_streaming_throttling_index);
            }
        } instance_internal_streaming_task_debug_executing_guard(streaming_throttling_index, task_data->m_gfx_texture->m_gfx_connection);
#endif

        if (!streaming_cancel)
        {
            gfx_input_stream_ref input_stream;
            {
                class gfx_input_stream_guard
                {
                    gfx_input_stream_ref &m_input_stream;
                    void(PT_PTR *m_input_stream_destroy_callback)(gfx_input_stream_ref input_stream);

                public:
                    inline gfx_input_stream_guard(
                        gfx_input_stream_ref &input_stream,
                        char const *initial_filename,
                        gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
                        void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
                        : m_input_stream(input_stream),
                          m_input_stream_destroy_callback(input_stream_destroy_callback)
                    {
                        m_input_stream = input_stream_init_callback(initial_filename);
                    }
                    inline ~gfx_input_stream_guard()
                    {
                        m_input_stream_destroy_callback(m_input_stream);
                    }
                } input_stream_guard(input_stream, task_data->m_initial_filename, task_data->m_input_stream_init_callback, task_data->m_input_stream_destroy_callback);

                struct common_header_t common_header;
                size_t common_data_offset;
                bool res_load_header_from_input_stream = task_data->m_gfx_texture->load_header_from_input_stream(&common_header, &common_data_offset, input_stream, task_data->m_input_stream_read_callback, task_data->m_input_stream_seek_callback);
                if (!res_load_header_from_input_stream)
                {
                    mcrt_atomic_store(&task_data->m_gfx_texture->m_streaming_error, true);
                    return NULL;
                }

                specific_header_vk_t specific_header_vk = common_to_specific_header_translate(&common_header);

                uint32_t num_subresource = get_format_aspect_count(specific_header_vk.format) * specific_header_vk.arrayLayers * specific_header_vk.mipLevels;

                struct load_memcpy_dest_t *memcpy_dest;
                struct VkBufferImageCopy *cmdcopy_dest;
                {

                    class internal_mem_cmd_copy_dest_guard
                    {
                        struct load_memcpy_dest_t **const m_memcpy_dest;
                        struct VkBufferImageCopy **const m_cmdcopy_dest;

                    public:
                        inline internal_mem_cmd_copy_dest_guard(struct load_memcpy_dest_t **const memcpy_dest, struct VkBufferImageCopy **const cmdcopy_dest, uint32_t num_subresource)
                            : m_memcpy_dest(memcpy_dest), m_cmdcopy_dest(cmdcopy_dest)
                        {
                            (*m_memcpy_dest) = static_cast<struct load_memcpy_dest_t *>(mcrt_aligned_malloc(sizeof(struct load_memcpy_dest_t) * num_subresource, alignof(struct load_memcpy_dest_t)));
                            (*m_cmdcopy_dest) = static_cast<struct VkBufferImageCopy *>(mcrt_aligned_malloc(sizeof(struct VkBufferImageCopy) * num_subresource, alignof(struct VkBufferImageCopy)));
                        }
                        inline ~internal_mem_cmd_copy_dest_guard()
                        {
                            mcrt_free(*m_memcpy_dest);
                            mcrt_free(*m_cmdcopy_dest);
                        }
                    } instance_internal_mem_cmd_copy_dest_guard(&memcpy_dest, &cmdcopy_dest, num_subresource);

                    // get_copyable_footprints
                    {
                        uint64_t transfer_src_buffer_begin = task_data->m_gfx_texture->m_gfx_connection->transfer_src_buffer_begin(streaming_throttling_index);
                        uint64_t transfer_src_buffer_end = uint64_t(-1);
                        uint64_t transfer_src_buffer_size = task_data->m_gfx_texture->m_gfx_connection->transfer_src_buffer_size(streaming_throttling_index);
                        uint64_t base_offset = uint64_t(-1);

                        // allocate memory
                        do
                        {
                            base_offset = mcrt_atomic_load(task_data->m_gfx_texture->m_gfx_connection->transfer_src_buffer_end(streaming_throttling_index));

                            size_t total_size = get_copyable_footprints(&specific_header_vk, task_data->m_gfx_texture->m_gfx_connection->physical_device_limits_optimal_buffer_copy_offset_alignment(), task_data->m_gfx_texture->m_gfx_connection->physical_device_limits_optimal_buffer_copy_row_pitch_alignment(), base_offset, num_subresource, memcpy_dest, cmdcopy_dest);

                            transfer_src_buffer_end = (base_offset + total_size);
                            //assert((transfer_src_buffer_end - transfer_src_buffer_begin) <= transfer_src_buffer_size);

                            // respawn if memory not enough
                            if ((transfer_src_buffer_end - transfer_src_buffer_begin) > transfer_src_buffer_size)
                            {
                                task_data->m_gfx_texture->m_gfx_connection->streaming_task_respawn_list_push(streaming_throttling_index, task_data->m_gfx_texture);
                                return NULL;
                            }

                        } while (base_offset != mcrt_atomic_cas_u64(task_data->m_gfx_texture->m_gfx_connection->transfer_src_buffer_end(streaming_throttling_index), transfer_src_buffer_end, base_offset));
                    }

                    // pass to the third stage
                    {
                        task_data->m_gfx_texture->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_texture);
                    }

                    // load_data_from_input_stream
                    {
                        bool res_load_data_from_input_stream = task_data->m_gfx_texture->load_data_from_input_stream(&common_header, &common_data_offset, task_data->m_gfx_texture->m_gfx_connection->transfer_src_buffer_pointer(), num_subresource, memcpy_dest, calc_subresource, input_stream, task_data->m_input_stream_read_callback, task_data->m_input_stream_seek_callback);
                        if (!res_load_data_from_input_stream)
                        {
                            mcrt_atomic_store(&task_data->m_gfx_texture->m_streaming_error, true);
                            return NULL;
                        }
                    }

                    // copy_buffer_to_image
                    {
                        uint32_t streaming_thread_index = mcrt_task_arena_current_thread_index();
                        VkBuffer transfer_src_buffer = task_data->m_gfx_texture->m_gfx_connection->transfer_src_buffer();
                        VkImageSubresourceRange subresource_range = {VK_IMAGE_ASPECT_COLOR_BIT, 0, specific_header_vk.mipLevels, 0, 1};
                        task_data->m_gfx_texture->m_gfx_connection->copy_buffer_to_image(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, task_data->m_gfx_texture->m_image, &subresource_range, num_subresource, cmdcopy_dest);
                    }

                    mcrt_atomic_store(&task_data->m_gfx_texture->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
                }
            }
        }
        else
        {
            // The slob allocator is serial which is harmful to the parallel performance
            // leave the "steaming_cancel" to the third stage
            // tracker by "slob_lock_busy_count"
#if 0
        task_data->m_gfx_texture->streaming_cancel();
#else
            // pass to the third stage
            {
                task_data->m_gfx_texture->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_texture);
            }

            mcrt_atomic_store(&task_data->m_gfx_texture->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
#endif
        }
    }

    return NULL;
}

void gfx_texture_vk::destroy()
{
    bool streaming_done;
    {
        // make sure this function happens before or after the gfx_streaming_object::streaming_done
        while (0U != mcrt_atomic_xchg_u32(&this->m_spin_lock_streaming_done, 1U))
        {
            mcrt_os_yield();
        }

        streaming_status_t streaming_status = mcrt_atomic_load(&this->m_streaming_status);

        if (STREAMING_STATUS_STAGE_FIRST == streaming_status || STREAMING_STATUS_STAGE_SECOND == streaming_status || STREAMING_STATUS_STAGE_THIRD == streaming_status)
        {
            mcrt_atomic_store(&this->m_streaming_cancel, true);
            streaming_done = false;
        }
        else if (STREAMING_STATUS_DONE == streaming_status)
        {
            streaming_done = true;
        }
        else
        {
            assert(0);
        }

        mcrt_atomic_store(&this->m_spin_lock_streaming_done, 0U);
    }

    if (streaming_done)
    {
        //TODO: the object is used by the rendering system
        //

        //
        // race condition with the "streaming_done"

        if (VK_NULL_HANDLE != this->m_gfx_malloc_device_memory)
        {
            this->m_gfx_connection->transfer_dst_and_sampled_image_free(this->m_gfx_malloc_page_handle, this->m_gfx_malloc_offset, this->m_gfx_malloc_size, this->m_gfx_malloc_device_memory);
        }

        if (VK_NULL_HANDLE != this->m_image)
        {
            this->m_gfx_connection->destroy_image(this->m_image);
        }

        this->~gfx_texture_vk();
        mcrt_free(this);
    }
}

void gfx_texture_vk::streaming_cancel()
{
    bool streaming_cancel = mcrt_atomic_load(&this->m_streaming_cancel);
    assert(streaming_cancel);

    if (VK_NULL_HANDLE != this->m_gfx_malloc_device_memory)
    {
        this->m_gfx_connection->transfer_dst_and_sampled_image_free(this->m_gfx_malloc_page_handle, this->m_gfx_malloc_offset, this->m_gfx_malloc_size, this->m_gfx_malloc_device_memory);
    }

    if (VK_NULL_HANDLE != this->m_image)
    {
        this->m_gfx_connection->destroy_image(this->m_image);
    }

    this->~gfx_texture_vk();
    mcrt_free(this);
}

inline struct gfx_texture_vk::specific_header_vk_t gfx_texture_vk::common_to_specific_header_translate(struct common_header_t const *common_header)
{
    struct specific_header_vk_t specific_header_vk;

    specific_header_vk.isCubeCompatible = common_header->isCubeMap;
    specific_header_vk.imageType = common_to_vulkan_type_translate(common_header->type);
    specific_header_vk.format = common_to_vulkan_format_translate(common_header->format);
    specific_header_vk.extent.width = common_header->width;
    specific_header_vk.extent.height = common_header->height;
    specific_header_vk.extent.depth = common_header->depth;
    specific_header_vk.mipLevels = common_header->mipLevels;
    specific_header_vk.arrayLayers = common_header->arrayLayers;

    return specific_header_vk;
}

inline enum VkImageType gfx_texture_vk::common_to_vulkan_type_translate(enum gfx_texture_common_type_t common_type)
{
    static enum VkImageType const common_to_vulkan_type_map[] = {
        VK_IMAGE_TYPE_1D,
        VK_IMAGE_TYPE_2D,
        VK_IMAGE_TYPE_3D};
    static_assert(gfx_texture_common::PT_GFX_TEXTURE_COMMON_TYPE_RANGE_SIZE == (sizeof(common_to_vulkan_type_map) / sizeof(common_to_vulkan_type_map[0])), "");

    assert(common_type < (sizeof(common_to_vulkan_type_map) / sizeof(common_to_vulkan_type_map[0])));
    return common_to_vulkan_type_map[common_type];
}

inline enum VkFormat gfx_texture_vk::common_to_vulkan_format_translate(enum gfx_texture_common_format_t common_format)
{
    static enum VkFormat const common_to_vulkan_format_map[] = {
        VK_FORMAT_R4G4_UNORM_PACK8,
        VK_FORMAT_R4G4B4A4_UNORM_PACK16,
        VK_FORMAT_B4G4R4A4_UNORM_PACK16,
        VK_FORMAT_R5G6B5_UNORM_PACK16,
        VK_FORMAT_B5G6R5_UNORM_PACK16,
        VK_FORMAT_R5G5B5A1_UNORM_PACK16,
        VK_FORMAT_B5G5R5A1_UNORM_PACK16,
        VK_FORMAT_A1R5G5B5_UNORM_PACK16,
        VK_FORMAT_R8_UNORM,
        VK_FORMAT_R8_SNORM,
        VK_FORMAT_R8_USCALED,
        VK_FORMAT_R8_SSCALED,
        VK_FORMAT_R8_UINT,
        VK_FORMAT_R8_SINT,
        VK_FORMAT_R8_SRGB,
        VK_FORMAT_R8G8_UNORM,
        VK_FORMAT_R8G8_SNORM,
        VK_FORMAT_R8G8_USCALED,
        VK_FORMAT_R8G8_SSCALED,
        VK_FORMAT_R8G8_UINT,
        VK_FORMAT_R8G8_SINT,
        VK_FORMAT_R8G8_SRGB,
        VK_FORMAT_R8G8B8_UNORM,
        VK_FORMAT_R8G8B8_SNORM,
        VK_FORMAT_R8G8B8_USCALED,
        VK_FORMAT_R8G8B8_SSCALED,
        VK_FORMAT_R8G8B8_UINT,
        VK_FORMAT_R8G8B8_SINT,
        VK_FORMAT_R8G8B8_SRGB,
        VK_FORMAT_B8G8R8_UNORM,
        VK_FORMAT_B8G8R8_SNORM,
        VK_FORMAT_B8G8R8_USCALED,
        VK_FORMAT_B8G8R8_SSCALED,
        VK_FORMAT_B8G8R8_UINT,
        VK_FORMAT_B8G8R8_SINT,
        VK_FORMAT_B8G8R8_SRGB,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_R8G8B8A8_SNORM,
        VK_FORMAT_R8G8B8A8_USCALED,
        VK_FORMAT_R8G8B8A8_SSCALED,
        VK_FORMAT_R8G8B8A8_UINT,
        VK_FORMAT_R8G8B8A8_SINT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_B8G8R8A8_UNORM,
        VK_FORMAT_B8G8R8A8_SNORM,
        VK_FORMAT_B8G8R8A8_USCALED,
        VK_FORMAT_B8G8R8A8_SSCALED,
        VK_FORMAT_B8G8R8A8_UINT,
        VK_FORMAT_B8G8R8A8_SINT,
        VK_FORMAT_B8G8R8A8_SRGB,
        VK_FORMAT_A8B8G8R8_UNORM_PACK32,
        VK_FORMAT_A8B8G8R8_SNORM_PACK32,
        VK_FORMAT_A8B8G8R8_USCALED_PACK32,
        VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
        VK_FORMAT_A8B8G8R8_UINT_PACK32,
        VK_FORMAT_A8B8G8R8_SINT_PACK32,
        VK_FORMAT_A8B8G8R8_SRGB_PACK32,
        VK_FORMAT_A2R10G10B10_UNORM_PACK32,
        VK_FORMAT_A2R10G10B10_SNORM_PACK32,
        VK_FORMAT_A2R10G10B10_USCALED_PACK32,
        VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
        VK_FORMAT_A2R10G10B10_UINT_PACK32,
        VK_FORMAT_A2R10G10B10_SINT_PACK32,
        VK_FORMAT_A2B10G10R10_UNORM_PACK32,
        VK_FORMAT_A2B10G10R10_SNORM_PACK32,
        VK_FORMAT_A2B10G10R10_USCALED_PACK32,
        VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
        VK_FORMAT_A2B10G10R10_UINT_PACK32,
        VK_FORMAT_A2B10G10R10_SINT_PACK32,
        VK_FORMAT_R16_UNORM,
        VK_FORMAT_R16_SNORM,
        VK_FORMAT_R16_USCALED,
        VK_FORMAT_R16_SSCALED,
        VK_FORMAT_R16_UINT,
        VK_FORMAT_R16_SINT,
        VK_FORMAT_R16_SFLOAT,
        VK_FORMAT_R16G16_UNORM,
        VK_FORMAT_R16G16_SNORM,
        VK_FORMAT_R16G16_USCALED,
        VK_FORMAT_R16G16_SSCALED,
        VK_FORMAT_R16G16_UINT,
        VK_FORMAT_R16G16_SINT,
        VK_FORMAT_R16G16_SFLOAT,
        VK_FORMAT_R16G16B16_UNORM,
        VK_FORMAT_R16G16B16_SNORM,
        VK_FORMAT_R16G16B16_USCALED,
        VK_FORMAT_R16G16B16_SSCALED,
        VK_FORMAT_R16G16B16_UINT,
        VK_FORMAT_R16G16B16_SINT,
        VK_FORMAT_R16G16B16_SFLOAT,
        VK_FORMAT_R16G16B16A16_UNORM,
        VK_FORMAT_R16G16B16A16_SNORM,
        VK_FORMAT_R16G16B16A16_USCALED,
        VK_FORMAT_R16G16B16A16_SSCALED,
        VK_FORMAT_R16G16B16A16_UINT,
        VK_FORMAT_R16G16B16A16_SINT,
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_FORMAT_R32_UINT,
        VK_FORMAT_R32_SINT,
        VK_FORMAT_R32_SFLOAT,
        VK_FORMAT_R32G32_UINT,
        VK_FORMAT_R32G32_SINT,
        VK_FORMAT_R32G32_SFLOAT,
        VK_FORMAT_R32G32B32_UINT,
        VK_FORMAT_R32G32B32_SINT,
        VK_FORMAT_R32G32B32_SFLOAT,
        VK_FORMAT_R32G32B32A32_UINT,
        VK_FORMAT_R32G32B32A32_SINT,
        VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_FORMAT_R64_UINT,
        VK_FORMAT_R64_SINT,
        VK_FORMAT_R64_SFLOAT,
        VK_FORMAT_R64G64_UINT,
        VK_FORMAT_R64G64_SINT,
        VK_FORMAT_R64G64_SFLOAT,
        VK_FORMAT_R64G64B64_UINT,
        VK_FORMAT_R64G64B64_SINT,
        VK_FORMAT_R64G64B64_SFLOAT,
        VK_FORMAT_R64G64B64A64_UINT,
        VK_FORMAT_R64G64B64A64_SINT,
        VK_FORMAT_R64G64B64A64_SFLOAT,
        VK_FORMAT_B10G11R11_UFLOAT_PACK32,
        VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
        VK_FORMAT_D16_UNORM,
        VK_FORMAT_X8_D24_UNORM_PACK32,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_BC1_RGB_UNORM_BLOCK,
        VK_FORMAT_BC1_RGB_SRGB_BLOCK,
        VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
        VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
        VK_FORMAT_BC2_UNORM_BLOCK,
        VK_FORMAT_BC2_SRGB_BLOCK,
        VK_FORMAT_BC3_UNORM_BLOCK,
        VK_FORMAT_BC3_SRGB_BLOCK,
        VK_FORMAT_BC4_UNORM_BLOCK,
        VK_FORMAT_BC4_SNORM_BLOCK,
        VK_FORMAT_BC5_UNORM_BLOCK,
        VK_FORMAT_BC5_SNORM_BLOCK,
        VK_FORMAT_BC6H_UFLOAT_BLOCK,
        VK_FORMAT_BC6H_SFLOAT_BLOCK,
        VK_FORMAT_BC7_UNORM_BLOCK,
        VK_FORMAT_BC7_SRGB_BLOCK,
        VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
        VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
        VK_FORMAT_EAC_R11_UNORM_BLOCK,
        VK_FORMAT_EAC_R11_SNORM_BLOCK,
        VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
        VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
        VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
        VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
        VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
        VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
        VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
        VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
        VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
        VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
        VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
        VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
        VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
        VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
        VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
        VK_FORMAT_ASTC_12x12_SRGB_BLOCK};
    static_assert(gfx_texture_common::PT_GFX_TEXTURE_COMMON_FORMAT_RANGE_SIZE == (sizeof(common_to_vulkan_format_map) / sizeof(common_to_vulkan_format_map[0])), "");

    assert(common_format < (sizeof(common_to_vulkan_format_map) / sizeof(common_to_vulkan_format_map[0])));
    return common_to_vulkan_format_map[common_format];
}

inline uint32_t gfx_texture_vk::calc_subresource(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mipLevels, uint32_t arrayLayers)
{
    return mipLevel + arrayLayer * mipLevels + aspectIndex * mipLevels * arrayLayers;
}

inline size_t gfx_texture_vk::get_copyable_footprints(
    struct specific_header_vk_t const *specific_header_vk,
    VkDeviceSize physical_device_limits_optimal_buffer_copy_offset_alignment, VkDeviceSize physical_device_limits_optimal_buffer_copy_row_pitch_alignment,
    size_t base_offset,
    size_t num_subresources, struct load_memcpy_dest_t *out_memcpy_dest, VkBufferImageCopy *out_cmdcopy_dest)
{
    // Context::texSubImage2D libANGLE/Context.cpp
    // Texture::setSubImage libANGLE/Texture.cpp
    // TextureVk::setSubImage libANGLE/renderer/vulkan/TextureVk.cpp
    // TextureVk::setSubImageImpl libANGLE/renderer/vulkan/TextureVk.cpp
    // ImageHelper::stageSubresourceUpdate libANGLE/renderer/vulkan/vk_helpers.cpp
    // ImageHelper::CalculateBufferInfo libANGLE/renderer/vulkan/vk_helpers.cpp
    // ImageHelper::stageSubresourceUpdateImpl libANGLE/renderer/vulkan/vk_helpers.cpp

    uint32_t aspectCount = get_format_aspect_count(specific_header_vk->format);

    size_t stagingOffset = base_offset;
    size_t TotalBytes = 0;

    for (uint32_t aspectIndex = 0; aspectIndex < aspectCount; ++aspectIndex)
    {
        for (uint32_t arrayLayer = 0; arrayLayer < specific_header_vk->arrayLayers; ++arrayLayer)
        {
            size_t w = specific_header_vk->extent.width;
            size_t h = specific_header_vk->extent.height;
            size_t d = specific_header_vk->extent.depth;
            for (uint32_t mipLevel = 0; mipLevel < specific_header_vk->mipLevels; ++mipLevel)
            {
                size_t outputRowPitch;
                size_t outputRowSize;
                size_t outputNumRows;
                size_t outputSlicePitch;
                size_t outputNumSlices;

                uint32_t bufferRowLength;
                uint32_t bufferImageHeight;

                size_t texel_block_size;

                if (is_format_compressed(specific_header_vk->format))
                {
                    assert(1 == get_compressed_format_block_depth(specific_header_vk->format));

                    outputRowSize = ((w + get_compressed_format_block_width(specific_header_vk->format) - 1) / get_compressed_format_block_width(specific_header_vk->format)) * get_compressed_format_block_size_in_bytes(specific_header_vk->format);
                    outputNumRows = (h + get_compressed_format_block_height(specific_header_vk->format) - 1) / get_compressed_format_block_height(specific_header_vk->format);
                    outputNumSlices = d;

                    outputRowPitch = mcrt_intrin_round_up(outputRowSize, physical_device_limits_optimal_buffer_copy_row_pitch_alignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    // bufferOffset must be a multiple of 4
                    // bufferRowLength must be 0, or greater than or equal to the width member of imageExtent
                    // bufferImageHeight must be 0, or greater than or equal to the height member of imageExtent
                    // If the calling command’s VkImage parameter is a compressed image, ... , bufferRowLength must be a multiple of the compressed texel block width
                    // If the calling command’s VkImage parameter is a compressed image, ... , bufferImageHeight must be a multiple of the compressed texel block height

                    // 19.4.1. Buffer and Image Addressing
                    // For block-compressed formats, all parameters are still specified in texels rather than compressed texel blocks, but the addressing math operates on whole compressed texel blocks.
                    // Pseudocode for compressed copy addressing is:
                    // ```
                    // rowLength = region->bufferRowLength;
                    // if (rowLength == 0) rowLength = region->imageExtent.width;
                    //
                    // imageHeight = region->bufferImageHeight;
                    // if (imageHeight == 0) imageHeight = region->imageExtent.height;
                    //
                    // compressedTexelBlockSizeInBytes = <compressed texel block size taken from the src/dstImage>;
                    // rowLength /= compressedTexelBlockWidth;
                    // imageHeight /= compressedTexelBlockHeight;
                    //
                    // address of (x,y,z) = region->bufferOffset + (((z * imageHeight) + y) * rowLength + x) * compressedTexelBlockSizeInBytes;
                    // where x,y,z range from (0,0,0) to region->imageExtent.{width/compressedTexelBlockWidth,height/compressedTexelBlockHeight,depth/compressedTexelBlockDepth}.
                    //
                    // ```

                    bufferRowLength = (outputRowPitch / get_compressed_format_block_size_in_bytes(specific_header_vk->format)) * get_compressed_format_block_width(specific_header_vk->format);
                    bufferImageHeight = outputNumRows * get_compressed_format_block_height(specific_header_vk->format);

                    //bufferRowLength = mcrt_intrin_round_up(specific_header_vk->extent.width, get_compressed_format_block_width(specific_header_vk->format));
                    //bufferImageHeight = mcrt_intrin_round_up(specific_header_vk->extent.height, get_compressed_format_block_height(specific_header_vk->format));

                    texel_block_size = get_compressed_format_block_size_in_bytes(specific_header_vk->format);
                }
                else if (is_format_rgba(specific_header_vk->format))
                {

                    outputRowSize = get_rgba_format_pixel_bytes(specific_header_vk->format) * w;
                    outputNumRows = h;
                    outputNumSlices = d;

                    outputRowPitch = mcrt_intrin_round_up(outputRowSize, physical_device_limits_optimal_buffer_copy_row_pitch_alignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    bufferRowLength = outputRowPitch / get_rgba_format_pixel_bytes(specific_header_vk->format);
                    bufferImageHeight = outputNumRows;

                    texel_block_size = get_rgba_format_pixel_bytes(specific_header_vk->format);
                }
                else
                {
                    assert(is_format_depth_stencil(specific_header_vk->format));
                    outputRowSize = get_depth_stencil_format_pixel_bytes(specific_header_vk->format, aspectIndex) * w;
                    outputNumRows = h;
                    outputNumSlices = d;

                    outputRowPitch = mcrt_intrin_round_up(outputRowSize, physical_device_limits_optimal_buffer_copy_row_pitch_alignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    bufferRowLength = outputRowPitch / get_depth_stencil_format_pixel_bytes(specific_header_vk->format, aspectIndex);
                    bufferImageHeight = outputNumRows;

                    texel_block_size = get_depth_stencil_format_pixel_bytes(specific_header_vk->format, aspectIndex);
                }

                size_t stagingOffset_new = mcrt_intrin_round_up(mcrt_intrin_round_up(mcrt_intrin_round_up(mcrt_intrin_round_up(stagingOffset, size_t(4U)), texel_block_size), physical_device_limits_optimal_buffer_copy_offset_alignment), physical_device_limits_optimal_buffer_copy_row_pitch_alignment);
                TotalBytes += (stagingOffset_new - stagingOffset);
                stagingOffset = stagingOffset_new;

                uint32_t DstSubresource = calc_subresource(mipLevel, arrayLayer, aspectIndex, specific_header_vk->mipLevels, specific_header_vk->arrayLayers);
                assert(DstSubresource < num_subresources);

                out_memcpy_dest[DstSubresource].stagingOffset = stagingOffset;
                out_memcpy_dest[DstSubresource].outputRowPitch = outputRowPitch;
                out_memcpy_dest[DstSubresource].outputRowSize = outputRowSize;
                out_memcpy_dest[DstSubresource].outputNumRows = outputNumRows;
                out_memcpy_dest[DstSubresource].outputSlicePitch = outputSlicePitch;
                out_memcpy_dest[DstSubresource].outputNumSlices = outputNumSlices;

                out_cmdcopy_dest[DstSubresource].bufferOffset = stagingOffset;
                out_cmdcopy_dest[DstSubresource].bufferRowLength = bufferRowLength;
                out_cmdcopy_dest[DstSubresource].bufferImageHeight = bufferImageHeight;

                //GetFormatAspectFlags
                out_cmdcopy_dest[DstSubresource].imageSubresource.aspectMask = get_format_aspect_mask(specific_header_vk->format, aspectIndex);
                out_cmdcopy_dest[DstSubresource].imageSubresource.mipLevel = mipLevel;
                out_cmdcopy_dest[DstSubresource].imageSubresource.baseArrayLayer = arrayLayer;
                out_cmdcopy_dest[DstSubresource].imageSubresource.layerCount = 1;
                out_cmdcopy_dest[DstSubresource].imageOffset.x = 0;
                out_cmdcopy_dest[DstSubresource].imageOffset.y = 0;
                out_cmdcopy_dest[DstSubresource].imageOffset.z = 0;
                out_cmdcopy_dest[DstSubresource].imageExtent.width = w;
                out_cmdcopy_dest[DstSubresource].imageExtent.height = h;
                out_cmdcopy_dest[DstSubresource].imageExtent.depth = d;

                size_t surfaceSize = (outputSlicePitch * outputNumSlices);
                stagingOffset += surfaceSize;
                TotalBytes += surfaceSize;
                assert((base_offset + TotalBytes) == stagingOffset);

                w = w >> 1;
                h = h >> 1;
                d = d >> 1;
                if (w == 0)
                {
                    w = 1;
                }
                if (h == 0)
                {
                    h = 1;
                }
                if (d == 0)
                {
                    d = 1;
                }
            }
        }
    }

    return TotalBytes;
}

inline uint32_t gfx_texture_vk::get_format_aspect_count(VkFormat vk_format)
{
    assert(vk_format < (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])));

    struct vulkan_format_info_t vk_format_info = vulkan_format_info_table[vk_format];

    assert(vk_format_info._union_tag == 3 || vk_format_info._union_tag == 1 || vk_format_info._union_tag == 2);
    if (vk_format_info._union_tag == 3 || vk_format_info._union_tag == 1)
    {
        return 1;
    }
    else if (vk_format_info._union_tag == 2)
    {
        if (vk_format_info.depthstencil.depthBytes == 0 || vk_format_info.depthstencil.stencilBytes == 0)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
    else
    {
        return 0;
    }
}

inline uint32_t gfx_texture_vk::get_format_aspect_mask(VkFormat vk_format, uint32_t aspectIndex)
{
    assert(vk_format < (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])));

    vulkan_format_info_t vk_format_info = vulkan_format_info_table[vk_format];

    if (vk_format_info._union_tag == 3 || vk_format_info._union_tag == 1)
    {
        assert(aspectIndex < 1);
        VkImageAspectFlagBits aspectMasks[1] = {VK_IMAGE_ASPECT_COLOR_BIT};
        return aspectMasks[aspectIndex];
    }
    else if (vk_format_info._union_tag == 2)
    {
        if (vk_format_info.depthstencil.stencilBytes == 0)
        {
            assert(aspectIndex < 1);
            VkImageAspectFlagBits aspectMasks[1] = {VK_IMAGE_ASPECT_DEPTH_BIT};
            return aspectMasks[aspectIndex];
        }
        else if (vk_format_info.depthstencil.depthBytes != 0)
        {
            assert(aspectIndex < 2);
            VkImageAspectFlagBits aspectMasks[2] = {VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_STENCIL_BIT};
            return aspectMasks[aspectIndex];
        }
        else
        {
            assert(aspectIndex < 1);
            VkImageAspectFlagBits aspectMasks[1] = {VK_IMAGE_ASPECT_STENCIL_BIT};
            return aspectMasks[aspectIndex];
        }
    }
    else
    {
        return 0x7FFFFFFF;
    }
}

inline bool gfx_texture_vk::is_format_compressed(VkFormat vk_format)
{
    assert(vk_format < (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])));

    vulkan_format_info_t vk_format_info = vulkan_format_info_table[vk_format];

    return (vk_format_info._union_tag == 3);
}

inline bool gfx_texture_vk::is_format_rgba(VkFormat vk_format)
{
    assert(vk_format < (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])));

    vulkan_format_info_t vk_format_info = vulkan_format_info_table[vk_format];

    return (vk_format_info._union_tag == 1);
}

inline bool gfx_texture_vk::is_format_depth_stencil(VkFormat vk_format)
{
    assert(vk_format < (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])));

    vulkan_format_info_t vk_format_info = vulkan_format_info_table[vk_format];

    return (vk_format_info._union_tag == 2);
}

inline uint32_t gfx_texture_vk::get_compressed_format_block_width(VkFormat vk_format)
{
    assert(is_format_compressed(vk_format));

    assert(vk_format < (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])));

    vulkan_format_info_t vk_format_info = vulkan_format_info_table[vk_format];

    return vk_format_info.compressed.compressedBlockWidth;
}

inline uint32_t gfx_texture_vk::get_compressed_format_block_height(VkFormat vk_format)
{
    assert(is_format_compressed(vk_format));

    assert(vk_format < (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])));

    vulkan_format_info_t vk_format_info = vulkan_format_info_table[vk_format];

    return vk_format_info.compressed.compressedBlockHeight;
}

inline uint32_t gfx_texture_vk::get_compressed_format_block_depth(VkFormat vk_format)
{
    assert(is_format_compressed(vk_format));

    assert(vk_format < (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])));

    vulkan_format_info_t vk_format_info = vulkan_format_info_table[vk_format];

    return vk_format_info.compressed.compressedBlockDepth;
}

inline uint32_t gfx_texture_vk::get_compressed_format_block_size_in_bytes(VkFormat vk_format)
{
    assert(is_format_compressed(vk_format));

    assert(vk_format < (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])));

    vulkan_format_info_t vk_format_info = vulkan_format_info_table[vk_format];

    return vk_format_info.compressed.compressedBlockSizeInBytes;
}

inline uint32_t gfx_texture_vk::get_rgba_format_pixel_bytes(VkFormat vk_format)
{
    assert(is_format_rgba(vk_format));

    assert(vk_format < (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])));

    vulkan_format_info_t vk_format_info = vulkan_format_info_table[vk_format];

    return vk_format_info.rgba.pixelBytes;
}

inline uint32_t gfx_texture_vk::get_depth_stencil_format_pixel_bytes(VkFormat vk_format, uint32_t aspectIndex)
{
    assert(is_format_depth_stencil(vk_format));

    assert(vk_format < (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])));

    vulkan_format_info_t vk_format_info = vulkan_format_info_table[vk_format];

    if (vk_format_info.depthstencil.stencilBytes == 0)
    {
        assert(aspectIndex < 1);
        uint32_t pixelBytes[1] = {vk_format_info.depthstencil.depthBytes};
        return pixelBytes[aspectIndex];
    }
    else if (vk_format_info.depthstencil.depthBytes != 0)
    {
        assert(aspectIndex < 2);
        uint32_t pixelBytes[2] = {vk_format_info.depthstencil.depthBytes, vk_format_info.depthstencil.stencilBytes};
        return pixelBytes[aspectIndex];
    }
    else
    {
        assert(aspectIndex < 1);
        uint32_t pixelBytes[1] = {vk_format_info.depthstencil.stencilBytes};
        return pixelBytes[aspectIndex];
    }
}

//--------------------------------------------------------------------------------------

// gFormatInfoTable libANGLE/renderer/Format_table_autogen.cpp
// { FormatID::R8G8B8A8_UNORM, GL_RGBA8, GL_RGBA8, GenerateMip<R8G8B8A8>, NoCopyFunctions, ReadColor<R8G8B8A8, GLfloat>, WriteColor<R8G8B8A8, GLfloat>, GL_UNSIGNED_NORMALIZED, 8, 8, 8, 8, 0, 0, 0, 4, 0 */ {false */ {false */ {false, gl::VertexAttribType::UnsignedByte },

// BuildInternalFormatInfoMap libANGLE/formatutils.cpp
// From ES 3.0.1 spec, table 3.12
//                    | Internal format     |sized| R | G | B | A |S | Format         | Type                             | Component type        | SRGB | Texture supported                                | Filterable     | Texture attachment                               | Renderbuffer                                   | Blend
// AddRGBAFormat(&map, GL_RGBA8,             true,  8,  8,  8,  8, 0, GL_RGBA,         GL_UNSIGNED_BYTE,                  GL_UNSIGNED_NORMALIZED */ {false, RequireESOrExt<3, 0, &Extensions::textureStorage>, AlwaysSupported, RequireESOrExt<3, 0, &Extensions::textureStorage>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>);
// From EXT_texture_compression_bptc
//                          | Internal format                         | W | H |D | BS |CC| SRGB | Texture supported                              | Filterable     | Texture attachment | Renderbuffer  | Blend
// AddCompressedFormat(&map, GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,         4,  4, 1, 128, 4 */ {false, RequireExt<&Extensions::textureCompressionBPTC>, AlwaysSupported, NeverSupported,      NeverSupported, NeverSupported);

// GetLoadFunctionsLoadFunctionsMap libANGLE/renderer/load_functions_table_autogen.cpp
// LoadToNative
// LoadCompressedToNative
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-designator"
struct gfx_texture_vk::vulkan_format_info_t const gfx_texture_vk::vulkan_format_info_table[] = {
    {0},                                       //VK_FORMAT_UNDEFINED
    {1, .rgba = {1}},                          //VK_FORMAT_R4G4_UNORM_PACK8
    {1, .rgba = {2}},                          //VK_FORMAT_R4G4B4A4_UNORM_PACK16
    {1, .rgba = {2}},                          //VK_FORMAT_B4G4R4A4_UNORM_PACK16
    {1, .rgba = {2}},                          //VK_FORMAT_R5G6B5_UNORM_PACK16
    {1, .rgba = {2}},                          //VK_FORMAT_B5G6R5_UNORM_PACK16
    {1, .rgba = {2}},                          //VK_FORMAT_R5G5B5A1_UNORM_PACK16
    {1, .rgba = {2}},                          //VK_FORMAT_B5G5R5A1_UNORM_PACK16
    {1, .rgba = {2}},                          //VK_FORMAT_A1R5G5B5_UNORM_PACK16
    {1, .rgba = {1}},                          //VK_FORMAT_R8_UNORM
    {1, .rgba = {1}},                          //VK_FORMAT_R8_SNORM
    {1, .rgba = {1}},                          //VK_FORMAT_R8_USCALED
    {1, .rgba = {1}},                          //VK_FORMAT_R8_SSCALED
    {1, .rgba = {1}},                          //VK_FORMAT_R8_UINT
    {1, .rgba = {1}},                          //VK_FORMAT_R8_SINT
    {1, .rgba = {1}},                          //VK_FORMAT_R8_SRGB
    {1, .rgba = {2}},                          //VK_FORMAT_R8G8_UNORM
    {1, .rgba = {2}},                          //VK_FORMAT_R8G8_SNORM
    {1, .rgba = {2}},                          //VK_FORMAT_R8G8_USCALED
    {1, .rgba = {2}},                          //VK_FORMAT_R8G8_SSCALED
    {1, .rgba = {2}},                          //VK_FORMAT_R8G8_UINT
    {1, .rgba = {2}},                          //VK_FORMAT_R8G8_SINT
    {1, .rgba = {2}},                          //VK_FORMAT_R8G8_SRGB
    {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_UNORM
    {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_SNORM
    {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_USCALED
    {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_SSCALED
    {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_UINT
    {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_SINT
    {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_SRGB
    {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_UNORM
    {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_SNORM
    {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_USCALED
    {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_SSCALED
    {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_UINT
    {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_SINT
    {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_SRGB
    {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_UNORM
    {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_SNORM
    {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_USCALED
    {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_SSCALED
    {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_UINT
    {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_SINT
    {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_SRGB
    {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_UNORM
    {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_SNORM
    {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_USCALED
    {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_SSCALED
    {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_UINT
    {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_SINT
    {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_SRGB
    {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_UNORM_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_SNORM_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_USCALED_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_SSCALED_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_UINT_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_SINT_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_SRGB_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A2R10G10B10_UNORM_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A2R10G10B10_SNORM_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A2R10G10B10_USCALED_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A2R10G10B10_SSCALED_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A2R10G10B10_UINT_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A2R10G10B10_SINT_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A2B10G10R10_UNORM_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A2B10G10R10_SNORM_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A2B10G10R10_USCALED_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A2B10G10R10_SSCALED_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A2B10G10R10_UINT_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_A2B10G10R10_SINT_PACK32
    {1, .rgba = {2}},                          //VK_FORMAT_R16_UNORM
    {1, .rgba = {2}},                          //VK_FORMAT_R16_SNORM
    {1, .rgba = {2}},                          //VK_FORMAT_R16_USCALED
    {1, .rgba = {2}},                          //VK_FORMAT_R16_SSCALED
    {1, .rgba = {2}},                          //VK_FORMAT_R16_UINT
    {1, .rgba = {2}},                          //VK_FORMAT_R16_SINT
    {1, .rgba = {2}},                          //VK_FORMAT_R16_SFLOAT
    {1, .rgba = {4}},                          //VK_FORMAT_R16G16_UNORM
    {1, .rgba = {4}},                          //VK_FORMAT_R16G16_SNORM
    {1, .rgba = {4}},                          //VK_FORMAT_R16G16_USCALED
    {1, .rgba = {4}},                          //VK_FORMAT_R16G16_SSCALED
    {1, .rgba = {4}},                          //VK_FORMAT_R16G16_UINT
    {1, .rgba = {4}},                          //VK_FORMAT_R16G16_SINT
    {1, .rgba = {4}},                          //VK_FORMAT_R16G16_SFLOAT
    {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_UNORM
    {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_SNORM
    {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_USCALED
    {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_SSCALED
    {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_UINT
    {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_SINT
    {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_SFLOAT
    {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_UNORM
    {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_SNORM
    {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_USCALED
    {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_SSCALED
    {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_UINT
    {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_SINT
    {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_SFLOAT
    {1, .rgba = {4}},                          //VK_FORMAT_R32_UINT
    {1, .rgba = {4}},                          //VK_FORMAT_R32_SINT
    {1, .rgba = {4}},                          //VK_FORMAT_R32_SFLOAT
    {1, .rgba = {8}},                          //VK_FORMAT_R32G32_UINT
    {1, .rgba = {8}},                          //VK_FORMAT_R32G32_SINT
    {1, .rgba = {8}},                          //VK_FORMAT_R32G32_SFLOAT
    {1, .rgba = {12}},                         //VK_FORMAT_R32G32B32_UINT
    {1, .rgba = {12}},                         //VK_FORMAT_R32G32B32_SINT
    {1, .rgba = {12}},                         //VK_FORMAT_R32G32B32_SFLOAT
    {1, .rgba = {16}},                         //VK_FORMAT_R32G32B32A32_UINT
    {1, .rgba = {16}},                         //VK_FORMAT_R32G32B32A32_SINT
    {1, .rgba = {16}},                         //VK_FORMAT_R32G32B32A32_SFLOAT
    {1, .rgba = {8}},                          //VK_FORMAT_R64_UINT
    {1, .rgba = {8}},                          //VK_FORMAT_R64_SINT
    {1, .rgba = {8}},                          //VK_FORMAT_R64_SFLOAT
    {1, .rgba = {16}},                         //VK_FORMAT_R64G64_UINT
    {1, .rgba = {16}},                         //VK_FORMAT_R64G64_SINT
    {1, .rgba = {16}},                         //VK_FORMAT_R64G64_SFLOAT
    {1, .rgba = {24}},                         //VK_FORMAT_R64G64B64_UINT
    {1, .rgba = {24}},                         //VK_FORMAT_R64G64B64_SINT
    {1, .rgba = {24}},                         //VK_FORMAT_R64G64B64_SFLOAT
    {1, .rgba = {32}},                         //VK_FORMAT_R64G64B64A64_UINT
    {1, .rgba = {32}},                         //VK_FORMAT_R64G64B64A64_SINT
    {1, .rgba = {32}},                         //VK_FORMAT_R64G64B64A64_SFLOAT
    {1, .rgba = {4}},                          //VK_FORMAT_B10G11R11_UFLOAT_PACK32
    {1, .rgba = {4}},                          //VK_FORMAT_E5B9G9R9_UFLOAT_PACK32
    {2, .depthstencil = {2, 0}},               //VK_FORMAT_D16_UNORM
    {2, .depthstencil = {4, 0}},               //VK_FORMAT_X8_D24_UNORM_PACK32
    {2, .depthstencil = {4, 0}},               //VK_FORMAT_D32_SFLOAT
    {2, .depthstencil = {0, 1}},               //VK_FORMAT_S8_UINT
    {2, .depthstencil = {2, 2}},               //VK_FORMAT_D16_UNORM_S8_UINT         //data copied to or from the depth aspect of a VK_FORMAT_D16_UNORM or VK_FORMAT_D16_UNORM_S8_UINT format is tightly packed with one VK_FORMAT_D16_UNORM value per texel.
    {2, .depthstencil = {4, 4}},               //VK_FORMAT_D24_UNORM_S8_UINT         //data copied to or from the depth aspect of a VK_FORMAT_X8_D24_UNORM_PACK32 or VK_FORMAT_D24_UNORM_S8_UINT format is packed with one 32-bit word per texel with the D24 value in the LSBs of the word, and undefined values in the eight MSBs.
    {2, .depthstencil = {4, 4}},               //VK_FORMAT_D32_SFLOAT_S8_UINT        //data copied to or from the depth aspect of a VK_FORMAT_D32_SFLOAT or VK_FORMAT_D32_SFLOAT_S8_UINT format is tightly packed with one VK_FORMAT_D32_SFLOAT value per texel.
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_BC1_RGB_UNORM_BLOCK       //GL_COMPRESSED_RGB_S3TC_DXT1_EXT           //R5G6B5_UNORM
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_BC1_RGB_SRGB_BLOCK
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_BC1_RGBA_UNORM_BLOCK      //GL_COMPRESSED_RGBA_S3TC_DXT1_EXT          //R5G6B5A1_UNORM
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_BC1_RGBA_SRGB_BLOCK
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC2_UNORM_BLOCK           //GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE        //R5G6B5A4_UNORM
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC2_SRGB_BLOCK
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC3_UNORM_BLOCK           //GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE        //R5G6B5A8_UNORM
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC3_SRGB_BLOCK
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_BC4_UNORM_BLOCK           //GL_COMPRESSED_RED_RGTC1_EXT               //R8_UNORM
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_BC4_SNORM_BLOCK           //GL_COMPRESSED_SIGNED_RED_RGTC1_EXT        //R8_SNORM
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC5_UNORM_BLOCK           //GL_COMPRESSED_RED_GREEN_RGTC2_EXT         //R8G8_UNORM
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC5_SNORM_BLOCK           //GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT  //R8G8_SNORM
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC6H_UFLOAT_BLOCK         //GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT     //R16G16B16_UFLOAT (HDR)
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC6H_SFLOAT_BLOCK         //GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT       //R16G16B16_SFLOAT (HDR)
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC7_UNORM_BLOCK           //GL_COMPRESSED_RGBA_BPTC_UNORM_EXT         //B7G7R7A8_UNORM
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC7_SRGB_BLOCK            //GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT   //B7G7R7A8_UNORM
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK   //GL_COMPRESSED_RGB8_ETC2
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK    //GL_COMPRESSED_SRGB8_ETC2
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK //GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK  //GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK //GL_COMPRESSED_RGBA8_ETC2_EAC
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK  //GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_EAC_R11_UNORM_BLOCK       //GL_COMPRESSED_R11_EAC
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_EAC_R11_SNORM_BLOCK       //GL_COMPRESSED_SIGNED_R11_EAC
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_EAC_R11G11_UNORM_BLOCK    //GL_COMPRESSED_RG11_EAC
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_EAC_R11G11_SNORM_BLOCK    //GL_COMPRESSED_SIGNED_RG11_EAC
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_ASTC_4x4_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_4x4_KHR             //VK_EXT_astc_decode_mode
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_ASTC_4x4_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR     //VK_EXT_texture_compression_astc_hdr
    {3, .compressed = {5, 4, 1, (128 / 8)}},   //VK_FORMAT_ASTC_5x4_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_5x4_KHR             //
    {3, .compressed = {5, 4, 1, (128 / 8)}},   //VK_FORMAT_ASTC_5x4_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR     //
    {3, .compressed = {5, 5, 1, (128 / 8)}},   //VK_FORMAT_ASTC_5x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_5x5_KHR             //
    {3, .compressed = {5, 5, 1, (128 / 8)}},   //VK_FORMAT_ASTC_5x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR     //
    {3, .compressed = {6, 5, 1, (128 / 8)}},   //VK_FORMAT_ASTC_6x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_6x5_KHR             //
    {3, .compressed = {6, 5, 1, (128 / 8)}},   //VK_FORMAT_ASTC_6x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR     //
    {3, .compressed = {6, 6, 1, (128 / 8)}},   //VK_FORMAT_ASTC_6x6_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_6x6_KHR             //
    {3, .compressed = {6, 6, 1, (128 / 8)}},   //VK_FORMAT_ASTC_6x6_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR     //
    {3, .compressed = {8, 5, 1, (128 / 8)}},   //VK_FORMAT_ASTC_8x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x5_KHR             //
    {3, .compressed = {8, 5, 1, (128 / 8)}},   //VK_FORMAT_ASTC_8x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR     //
    {3, .compressed = {8, 6, 1, (128 / 8)}},   //VK_FORMAT_ASTC_8x6_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x6_KHR             //
    {3, .compressed = {8, 6, 1, (128 / 8)}},   //VK_FORMAT_ASTC_8x6_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR     //
    {3, .compressed = {8, 8, 1, (128 / 8)}},   //VK_FORMAT_ASTC_8x8_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x8_KHR             //
    {3, .compressed = {8, 8, 1, (128 / 8)}},   //VK_FORMAT_ASTC_8x8_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR     //
    {3, .compressed = {10, 5, 1, (128 / 8)}},  //VK_FORMAT_ASTC_10x5_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x5_KHR            //
    {3, .compressed = {10, 5, 1, (128 / 8)}},  //VK_FORMAT_ASTC_10x5_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR    //
    {3, .compressed = {10, 6, 1, (128 / 8)}},  //VK_FORMAT_ASTC_10x6_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x6_KHR            //
    {3, .compressed = {10, 6, 1, (128 / 8)}},  //VK_FORMAT_ASTC_10x6_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR    //
    {3, .compressed = {10, 8, 1, (128 / 8)}},  //VK_FORMAT_ASTC_10x8_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x8_KHR            //
    {3, .compressed = {10, 8, 1, (128 / 8)}},  //VK_FORMAT_ASTC_10x8_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR    //
    {3, .compressed = {10, 10, 1, (128 / 8)}}, //VK_FORMAT_ASTC_10x10_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_10x10_KHR           //
    {3, .compressed = {10, 10, 1, (128 / 8)}}, //VK_FORMAT_ASTC_10x10_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR   //
    {3, .compressed = {12, 10, 1, (128 / 8)}}, //VK_FORMAT_ASTC_12x10_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_12x10_KHR           //
    {3, .compressed = {12, 10, 1, (128 / 8)}}, //VK_FORMAT_ASTC_12x10_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR   //
    {3, .compressed = {12, 12, 1, (128 / 8)}}, //VK_FORMAT_ASTC_12x12_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_12x12_KHR           //
    {3, .compressed = {12, 12, 1, (128 / 8)}}  //VK_FORMAT_ASTC_12x12_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR   //
};
#pragma GCC diagnostic pop
