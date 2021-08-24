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
#include <stdint.h>
#include <pt_mcrt_intrin.h>
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_log.h>
#include "pt_gfx_material_base.h"
#include <assert.h>

bool gfx_material_base::init_with_texture(class gfx_connection_base *gfx_connection, uint32_t material_model, uint32_t texture_count, class gfx_texture_base **gfx_textures)
{
    PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&this->m_streaming_status);
    PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&this->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&this->m_streaming_cancel);
    assert(STREAMING_STATUS_STAGE_FIRST == streaming_status);
    assert(!streaming_error);

    if (!streaming_cancel)
    {
        this->m_texture_count = texture_count;
        for (uint32_t texture_index = 0U; texture_index < texture_count; ++texture_index)
        {
            gfx_textures[texture_index]->addref();
            this->m_gfx_textures[texture_index] = gfx_textures[texture_index];
        }

        // pass to the second stage
        {
            mcrt_task_ref task = mcrt_task_allocate_root(material_streaming_stage_second_task_execute);
            static_assert(sizeof(struct material_streaming_stage_second_task_data_t) <= sizeof(mcrt_task_user_data_t), "");
            struct material_streaming_stage_second_task_data_t *task_data = reinterpret_cast<struct material_streaming_stage_second_task_data_t *>(mcrt_task_get_user_data(task));
            task_data->m_gfx_streaming_object = this;
            task_data->m_gfx_connection = gfx_connection;

            mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_STAGE_SECOND);
            // different master task doesn't share the task_arena
            // we need to share the same the task arena to make sure the "tbb::this_task_arena::current_thread_id" unique
            //mcrt_task_spawn(task);
            mcrt_task_enqueue(task, gfx_connection->task_arena());
        }
    }
    else
    {
        // race condition with the "streaming_done"
        // inevitable since the "transfer_dst_and_sampled_image_alloc" nervertheless races with the "streaming_done"
        this->streaming_destroy_callback(gfx_connection);
    }

    return true;
}

mcrt_task_ref gfx_material_base::material_streaming_stage_second_task_execute(mcrt_task_ref self)
{
    static_assert(sizeof(struct material_streaming_stage_second_task_data_t) <= sizeof(mcrt_task_user_data_t), "");
    struct material_streaming_stage_second_task_data_t *task_data = reinterpret_cast<struct material_streaming_stage_second_task_data_t *>(mcrt_task_get_user_data(self));

    // different master task doesn't share the task_arena
    // we need to share the same the task arena to make sure the "tbb::this_task_arena::current_thread_id" unique
    assert(NULL != mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()));
    assert(mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()) == mcrt_this_task_arena_internal_arena());

    PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&task_data->m_gfx_streaming_object->m_streaming_status);
    PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&task_data->m_gfx_streaming_object->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&task_data->m_gfx_streaming_object->m_streaming_cancel);
    assert(STREAMING_STATUS_STAGE_SECOND == streaming_status);
    assert(!streaming_error);

    // race condition
    // task: load streaming_throttling_index
    // reduce_streaming_task: store ++streaming_throttling_index
    // reduce_streaming_task: mcrt_task_wait_for_all // return immediately
    // task: mcrt_task_increment_ref_count
    task_data->m_gfx_connection->streaming_throttling_index_lock();
    uint32_t streaming_throttling_index = task_data->m_gfx_connection->streaming_throttling_index();
    mcrt_task_increment_ref_count(task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));
    task_data->m_gfx_connection->streaming_throttling_index_unlock();

    task_data->m_gfx_connection->streaming_task_mark_execute_begin(streaming_throttling_index);

    mcrt_task_set_parent(self, task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));

    if (!streaming_cancel)
    {

        for (uint32_t texture_index = 0U; texture_index < task_data->m_gfx_streaming_object->m_texture_count; ++texture_index)
        {
            if (task_data->m_gfx_streaming_object->m_gfx_textures[texture_index]->is_streaming_error())
            {
                mcrt_atomic_store(&task_data->m_gfx_streaming_object->m_streaming_error, true);
                mcrt_atomic_store(&task_data->m_gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
                task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_streaming_object);
                task_data->m_gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
                return NULL;
            }

            // respawn if texture not done
            if (!task_data->m_gfx_streaming_object->m_gfx_textures[texture_index]->is_streaming_done())
            {
                // recycle to prevent free_task
                mcrt_task_recycle_as_child_of(self, task_data->m_gfx_connection->streaming_task_respawn_root());

                task_data->m_gfx_connection->streaming_task_respawn_list_push(streaming_throttling_index, self);

                task_data->m_gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
                // recycle needs manually tally_completion_of_predecessor
                // the "mcrt_task_decrement_ref_count" must be called after all works(include the C++ destructors) are done
                mcrt_task_decrement_ref_count(task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));
                return NULL;
            }
        }

        // pass to the third stage
        mcrt_atomic_store(&task_data->m_gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
        task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_streaming_object);
    }
    else
    {
        // The slob allocator is serial which is harmful to the parallel performance
        // leave the "steaming_cancel" to the third stage
        // tracker by "slob_lock_busy_count"
#if 0
            task_data->m_streaming_object->streaming_destroy_callback();
#else
        // pass to the third stage
        mcrt_atomic_store(&task_data->m_gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
        task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_streaming_object);
#endif
    }

    task_data->m_gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
    return NULL;
}

void gfx_material_base::destroy(class gfx_connection_base *gfx_connection)
{
    this->release(gfx_connection);
}

void gfx_material_base::addref()
{
    PT_MAYBE_UNUSED uint32_t ref_count = mcrt_atomic_inc_u32(&this->m_ref_count);
    // can't set_mesh after destory
    assert(1U < ref_count);
}

void gfx_material_base::release(class gfx_connection_base *gfx_connection)
{
    if (0U == mcrt_atomic_dec_u32(&this->m_ref_count))
    {
        bool streaming_done;
        this->streaming_destroy_request(&streaming_done);

        if (streaming_done)
        {
            // the object is used by the rendering system
            this->frame_destroy_request(gfx_connection);
        }
    }
}

// API
inline gfx_connection_ref wrap(class gfx_connection_base *gfx_connection) { return reinterpret_cast<gfx_connection_ref>(gfx_connection); }
inline class gfx_connection_base *unwrap(gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_base *>(gfx_connection); }

inline gfx_material_ref wrap(class gfx_material_base *gfx_material) { return reinterpret_cast<gfx_material_ref>(gfx_material); }
inline class gfx_material_base *unwrap(gfx_material_ref gfx_material) { return reinterpret_cast<class gfx_material_base *>(gfx_material); }

inline gfx_texture_ref *wrap(class gfx_texture_base **textures) { return reinterpret_cast<gfx_texture_ref *>(textures); }
inline class gfx_texture_base **unwrap(gfx_texture_ref *textures) { return reinterpret_cast<class gfx_texture_base **>(textures); }

PT_ATTR_GFX bool PT_CALL gfx_material_init_with_texture(gfx_connection_ref gfx_connection, gfx_material_ref gfx_material, uint32_t material_model, uint32_t texture_count, gfx_texture_ref *gfx_textures)
{
    return unwrap(gfx_material)->init_with_texture(unwrap(gfx_connection), material_model, texture_count, unwrap(gfx_textures));
}

PT_ATTR_GFX void PT_CALL gfx_material_destroy(gfx_connection_ref gfx_connection, gfx_material_ref gfx_material)
{
    return unwrap(gfx_material)->destroy(unwrap(gfx_connection));
}
