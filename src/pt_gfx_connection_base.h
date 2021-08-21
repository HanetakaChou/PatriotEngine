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

#ifndef _PT_GFX_CONNECTION_COMMON_H_
#define _PT_GFX_CONNECTION_COMMON_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include <pt_mcrt_task.h>
#include <pt_mcrt_rwlock.h>
#include <pt_mcrt_malloc.h>
#include <type_traits>
#include <new>
#include "pt_gfx_frame_object_base.h"

class gfx_connection_proxy
{
    //channel buffer
};

class gfx_connection_base
{

    // proxy -> stub
    // https://docs.microsoft.com/en-us/windows/win32/com/proxy
    // https://docs.microsoft.com/en-us/windows/win32/com/stub

    // https://en.wikipedia.org/wiki/Distributed_object_communication
    // proxy/stub -> skeleton

    //\[Hudson 2006\] Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". ISMM 2006.
    //freeListPush
    //repatriatePublicFreeList

protected:
    // MCRT
    mcrt_task_arena_ref m_task_arena;
    mcrt_task_ref m_task_unused;
    uint32_t m_task_arena_thread_count;

    template <typename T, uint32_t LINEAR_LIST_COUNT>
    struct mpsc_list
    {
        static_assert(std::is_pod<T>::value, "");

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
        mcrt_asset_rwlock_t m_asset_rwlock; // rwlock for the "m_frame_throttling_index" or "m_streaming_throttling_index"
#endif

        uint32_t m_linear_list_count;
        T m_linear_list[LINEAR_LIST_COUNT];

        struct link_list
        {
            struct link_list *m_next;
            T m_value;
        };
        struct link_list *m_link_list_head;

        inline void init();
        inline void produce(T value);
        inline void consume_and_clear(void (*consume_callback)(T value, void *user_defined), void *user_defined);
    };

    // Frame
    enum
    {
        FRAME_THROTTLING_COUNT = 3U
    };
    uint32_t m_frame_throttling_index;
    mcrt_rwlock_t m_rwlock_frame_throttling_index;
    uint32_t m_frame_thread_count;
    mcrt_task_ref m_frame_task_root;

    enum
    {
        NODE_INIT_LIST_COUNT = 32U,
        NODE_DESTROY_LIST_COUNT = 32U,
        FRAME_OBJECT_DESTROY_LIST_COUNT = 32U,
    };
    struct mpsc_list<class gfx_frame_object_base *, FRAME_OBJECT_DESTROY_LIST_COUNT> m_frame_object_destory_list[FRAME_THROTTLING_COUNT];

    // Streaming
    enum
    {
        STREAMING_THROTTLING_COUNT = 3U
    };
    uint32_t m_streaming_throttling_index;
    mcrt_rwlock_t m_rwlock_streaming_throttling_index;
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    mcrt_asset_rwlock_t m_asset_rwlock_streaming_task[STREAMING_THROTTLING_COUNT];
#endif
    uint32_t m_streaming_thread_count;
    mcrt_task_ref m_streaming_task_root[STREAMING_THROTTLING_COUNT];
    mcrt_task_ref m_streaming_task_respawn_root;

    enum
    {
        STREAMING_TASK_RESPAWN_LINEAR_LIST_COUNT = 64U,
        STREAMING_OBJECT_LINEAR_LIST_COUNT = 32U
    };
    struct mpsc_list<mcrt_task_ref, STREAMING_TASK_RESPAWN_LINEAR_LIST_COUNT> m_streaming_task_respawn_list[STREAMING_THROTTLING_COUNT];
    struct mpsc_list<class gfx_streaming_object_base *, STREAMING_OBJECT_LINEAR_LIST_COUNT> m_streaming_object_list[STREAMING_THROTTLING_COUNT];

    // Staging Buffer
    // [RingBuffer](https://docs.microsoft.com/en-us/windows/win32/direct3d12/fence-based-resource-management) related
    uint64_t m_transfer_src_buffer_begin[STREAMING_THROTTLING_COUNT];
    uint64_t m_transfer_src_buffer_end[STREAMING_THROTTLING_COUNT];
    uint64_t m_transfer_src_buffer_size[STREAMING_THROTTLING_COUNT];

public:
    // MCRT
    inline mcrt_task_arena_ref task_arena() { return m_task_arena; }

    // Frame
    void frame_object_destroy_list_push(class gfx_frame_object_base *frame_object);

    // Streaming
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    inline void streaming_task_mark_executing_begin(uint32_t streaming_throttling_index)
    {
        mcrt_asset_rwlock_rdlock(&this->m_asset_rwlock_streaming_task[streaming_throttling_index]);
    }
    inline void streaming_task_mark_executing_end(uint32_t streaming_throttling_index) { mcrt_asset_rwlock_rdunlock(&this->m_asset_rwlock_streaming_task[streaming_throttling_index]); }
#endif
    inline void streaming_throttling_index_lock()
    {
        mcrt_rwlock_rdlock(&this->m_rwlock_streaming_throttling_index);
    }
    inline uint32_t streaming_throttling_index() { return mcrt_atomic_load(&this->m_streaming_throttling_index); }
    inline void streaming_throttling_index_unlock() { mcrt_rwlock_rdunlock(&this->m_rwlock_streaming_throttling_index); }

    inline mcrt_task_ref streaming_task_root(uint32_t streaming_throttling_index) { return m_streaming_task_root[streaming_throttling_index]; }
    inline mcrt_task_ref streaming_task_respawn_root() { return m_streaming_task_respawn_root; }

    inline void streaming_object_list_push(uint32_t streaming_throttling_index, class gfx_streaming_object_base *streaming_object) { return this->m_streaming_object_list[streaming_throttling_index].produce(streaming_object); }
    inline void streaming_task_respawn_list_push(uint32_t streaming_throttling_index, mcrt_task_ref streaming_task) { return this->m_streaming_task_respawn_list[streaming_throttling_index].produce(streaming_task); }

    inline uint64_t transfer_src_buffer_begin(uint32_t streaming_throttling_index) { return m_transfer_src_buffer_begin[streaming_throttling_index]; }
    inline uint64_t *transfer_src_buffer_end(uint32_t streaming_throttling_index) { return &m_transfer_src_buffer_end[streaming_throttling_index]; }
    inline uint64_t transfer_src_buffer_size(uint32_t streaming_throttling_index) { return m_transfer_src_buffer_size[streaming_throttling_index]; }

    // API
    virtual void destroy() = 0;
    virtual class gfx_node_base *create_node() = 0;
    virtual class gfx_mesh_base *create_mesh() = 0;
    virtual class gfx_texture_base *create_texture() = 0;
    virtual void on_wsi_resized(float width, float height) = 0;
    virtual void on_wsi_redraw_needed_acquire() = 0; //frame throttling
    virtual void on_wsi_redraw_needed_release() = 0;
};

template <typename T, uint32_t LINEAR_LIST_COUNT>
inline void gfx_connection_base::mpsc_list<T, LINEAR_LIST_COUNT>::init()
{
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    mcrt_asset_rwlock_init(&this->m_asset_rwlock);
#endif
    this->m_linear_list_count = 0U;
    this->m_link_list_head = NULL;
}

template <typename T, uint32_t LINEAR_LIST_COUNT>
inline void gfx_connection_base::mpsc_list<T, LINEAR_LIST_COUNT>::produce(T value)
{
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    mcrt_asset_rwlock_rdlock(&this->m_asset_rwlock);
#endif

    uint32_t linear_list_index = mcrt_atomic_inc_u32(&this->m_linear_list_count) - 1U;
    if (linear_list_index < LINEAR_LIST_COUNT)
    {
        this->m_linear_list[linear_list_index] = value;
    }
    else
    {
        mcrt_atomic_dec_u32(&this->m_linear_list_count);

        // performance issue

        // Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 2 Public Free List / freeListPush + repatriatePublicFreeList
        // "This is ABA safe without concern for versioning because the concurrent data structure is a single consumer (the owning thread) and multiple producers."

        // ABA safe since the fence ensures that there is no consumer
        struct link_list *link_list_node = new (mcrt_aligned_malloc(sizeof(struct link_list), alignof(struct link_list))) link_list{};
        link_list_node->m_value = value;

        struct link_list *link_list_head;
        do
        {
            link_list_head = mcrt_atomic_load(&this->m_link_list_head);
            link_list_node->m_next = link_list_head;
        } while (link_list_head != mcrt_atomic_cas_ptr(&this->m_link_list_head, link_list_node, link_list_head));
    }

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    mcrt_asset_rwlock_rdunlock(&this->m_asset_rwlock);
#endif
}

template <typename T, uint32_t LINEAR_LIST_COUNT>
inline void gfx_connection_base::mpsc_list<T, LINEAR_LIST_COUNT>::consume_and_clear(void (*consume_callback)(T value, void *user_defined), void *user_defined)
{
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    mcrt_asset_rwlock_wrlock(&this->m_asset_rwlock);
#endif

    // list
    for (uint32_t linear_list_index = 0U; linear_list_index < this->m_linear_list_count; ++linear_list_index)
    {
        consume_callback(this->m_linear_list[linear_list_index], user_defined);
    }
    this->m_linear_list_count = 0U;

    // link list
    struct link_list *link_list_node = this->m_link_list_head;
    while (link_list_node != NULL)
    {
        consume_callback(link_list_node->m_value, user_defined);
        struct link_list *link_list_next = link_list_node->m_next;
        mcrt_aligned_free(link_list_node);

        link_list_node = link_list_next;
    }
    this->m_link_list_head = NULL;

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    mcrt_asset_rwlock_wrunlock(&this->m_asset_rwlock);
#endif
}

class gfx_connection_base *gfx_connection_common_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window);

#endif
