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

#include <pt_mcrt_task.h>

#include <tbb/task.h>

#include "pt_mcrt_scalable_allocator_tbb_scheduler.h"

#include <tbb/task_arena.h>

#include <pt_mcrt_malloc.h>

#include <new>

class mcrt_task_t;

inline mcrt_task_ref wrap(class mcrt_task_t *task) { return reinterpret_cast<mcrt_task_ref>(task); }

inline class mcrt_task_t *unwrap(mcrt_task_ref task) { return reinterpret_cast<class mcrt_task_t *>(task); }

inline mcrt_task_arena_ref wrap(tbb::task_arena *task_arena) { return reinterpret_cast<mcrt_task_arena_ref>(task_arena); }

inline tbb::task_arena *unwrap(mcrt_task_arena_ref task_arena) { return reinterpret_cast<tbb::task_arena *>(task_arena); }

class mcrt_task_t : public tbb::task
{
    mcrt_task_ref (*m_execute_callback)(mcrt_task_ref self);

    mcrt_task_user_data_t m_user_data;

    tbb::task *execute() override
    {
        mcrt_task_ref t = m_execute_callback(wrap(this));
        return unwrap(t);
    }

public:
    inline mcrt_task_t(mcrt_task_ref (*execute_callback)(mcrt_task_ref self)) : m_execute_callback(execute_callback) {}

    inline mcrt_task_user_data_t *user_data() { return &m_user_data; }
};

static_assert(sizeof(mcrt_task_t) <= mcrt::internal::generic_scheduler::quick_task_size, "");

PT_ATTR_MCRT mcrt_task_ref PT_CALL mcrt_task_allocate_root(mcrt_task_ref (*execute_callback)(mcrt_task_ref self))
{
    mcrt_task_t *t = new (tbb::task::allocate_root()) mcrt_task_t(execute_callback);
    return wrap(t);
}

PT_ATTR_MCRT mcrt_task_ref PT_CALL mcrt_task_allocate_continuation(mcrt_task_ref self, mcrt_task_ref (*execute_callback)(mcrt_task_ref self))
{
    mcrt_task_t *t = new (unwrap(self)->allocate_continuation()) mcrt_task_t(execute_callback);
    return wrap(t);
}

PT_ATTR_MCRT mcrt_task_user_data_t *PT_CALL mcrt_task_get_user_data(mcrt_task_ref self)
{
    return unwrap(self)->user_data();
}

PT_ATTR_MCRT void PT_CALL mcrt_task_destory(mcrt_task_ref victim)
{
    return tbb::task::destroy(*unwrap(victim));
}

PT_ATTR_MCRT void PT_CALL mcrt_task_recycle_as_child_of(mcrt_task_ref self, mcrt_task_ref successor)
{
    return unwrap(self)->recycle_as_child_of(*unwrap(successor));
}

PT_ATTR_MCRT int PT_CALL mcrt_task_ref_count(mcrt_task_ref self)
{
    return unwrap(self)->ref_count();
}

PT_ATTR_MCRT void PT_CALL mcrt_task_set_ref_count(mcrt_task_ref self, int count)
{
    return unwrap(self)->set_ref_count(count);
}

PT_ATTR_MCRT void PT_CALL mcrt_task_increment_ref_count(mcrt_task_ref self)
{
    return unwrap(self)->increment_ref_count();
}

PT_ATTR_MCRT int PT_CALL mcrt_task_decrement_ref_count(mcrt_task_ref self)
{
    return unwrap(self)->decrement_ref_count();
}

PT_ATTR_MCRT mcrt_task_ref PT_CALL mcrt_task_parent(mcrt_task_ref self)
{
    mcrt_task_t *t = static_cast<mcrt_task_t *>(unwrap(self)->parent());
    return wrap(t);
}

PT_ATTR_MCRT void PT_CALL mcrt_task_set_parent(mcrt_task_ref self, mcrt_task_ref parent)
{
    return unwrap(self)->set_parent(unwrap(parent));
}

PT_ATTR_MCRT void PT_CALL mcrt_task_spawn(mcrt_task_ref t)
{
    return tbb::task::spawn(*unwrap(t));
}

PT_ATTR_MCRT void PT_CALL mcrt_task_wait_for_all(mcrt_task_ref self)
{
    return unwrap(self)->wait_for_all();
}

PT_ATTR_MCRT void PT_CALL mcrt_task_spawn_and_wait_for_all(mcrt_task_ref self, mcrt_task_ref child)
{
    return unwrap(self)->spawn_and_wait_for_all(*unwrap(child));
}

PT_ATTR_MCRT mcrt_task_arena_ref PT_CALL mcrt_task_arena_attach()
{
    tbb::task_arena *task_arena = new (mcrt_aligned_malloc(sizeof(tbb::task_arena), alignof(tbb::task_arena))) tbb::task_arena(tbb::task_arena::attach{});
    return wrap(task_arena);
}

PT_ATTR_MCRT bool PT_CALL mcrt_task_arena_is_active(mcrt_task_arena_ref task_arena)
{
    return unwrap(task_arena)->is_active();
}

PT_ATTR_MCRT void PT_CALL mcrt_task_enqueue(mcrt_task_ref task, mcrt_task_arena_ref task_arena)
{
    return tbb::task::enqueue((*unwrap(task)), (*unwrap(task_arena)), tbb::priority_normal);
}

PT_ATTR_MCRT void PT_CALL mcrt_task_arena_terminate(mcrt_task_arena_ref task_arena)
{
    unwrap(task_arena)->~task_arena();
    mcrt_free(task_arena);
    return;
}

PT_ATTR_MCRT uint32_t PT_CALL mcrt_this_task_arena_current_thread_index()
{
    int current_thread_index = tbb::this_task_arena::current_thread_index();
    return (tbb::task_arena::not_initialized != current_thread_index) ? uint32_t(current_thread_index) : uint32_t(-1);
}

PT_ATTR_MCRT uint32_t PT_CALL mcrt_this_task_arena_max_concurrency()
{
    int max_concurrency = tbb::this_task_arena::max_concurrency();
    return uint32_t(max_concurrency);
}