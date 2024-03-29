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

#include <new>

#include <pt_mcrt_malloc.h>
#include <pt_mcrt_task.h>
#include <pt_mcrt_thread.h>

#define __TBB_NO_IMPLICIT_LINKAGE 1
#define __TBBMALLOC_NO_IMPLICIT_LINKAGE 1
#include <tbb/task.h>
#include <tbb/task_arena.h>

class pt_mcrt_task;

inline mcrt_task_ref wrap(class pt_mcrt_task *task) { return reinterpret_cast<mcrt_task_ref>(task); }

inline class pt_mcrt_task *unwrap(mcrt_task_ref task) { return reinterpret_cast<class pt_mcrt_task *>(task); }

inline mcrt_task_arena_ref wrap(tbb::task_arena *task_arena) { return reinterpret_cast<mcrt_task_arena_ref>(task_arena); }

inline tbb::task_arena *unwrap(mcrt_task_arena_ref task_arena) { return reinterpret_cast<tbb::task_arena *>(task_arena); }

inline mcrt_task_group_context_ref wrap(tbb::task_group_context* task_group_context) { return reinterpret_cast<mcrt_task_group_context_ref>(task_group_context); }

inline tbb::task_group_context* unwrap(mcrt_task_group_context_ref task_group_context) { return reinterpret_cast<tbb::task_group_context*>(task_group_context); }

class pt_mcrt_task : public tbb::task
{
    mcrt_task_ref (*m_execute_callback)(mcrt_task_ref self);

    mcrt_task_user_data_t m_user_data;

    tbb::task *execute() override
    {
        mcrt_task_ref t = m_execute_callback(wrap(this));
        return unwrap(t);
    }

    ~pt_mcrt_task() override
    {

    }

public:
    inline pt_mcrt_task(mcrt_task_ref (*execute_callback)(mcrt_task_ref self)) : m_execute_callback(execute_callback) {}

    inline mcrt_task_user_data_t *user_data() { return &m_user_data; }
};
// #include <../src/tbb/scheduler.h>
// tbb::internal::generic_scheduler::quick_task_size = 192U
static_assert(sizeof(pt_mcrt_task) <= 192U, "");
static_assert(std::is_pod<mcrt_task_user_data_t>::value, "");

PT_ATTR_MCRT mcrt_task_ref PT_CALL mcrt_task_allocate_root(mcrt_task_ref (*execute_callback)(mcrt_task_ref), mcrt_task_group_context_ref task_group_context)
{
    // https://gcc.gnu.org/wiki/VerboseDiagnostics#missing_vtable
    // To fix the linker error be sure you have provided a definition for the first non-inline non-pure virtual function declared in the class
    if (NULL != task_group_context)
    {
        pt_mcrt_task* t = new (tbb::task::allocate_root(*unwrap(task_group_context))) pt_mcrt_task(execute_callback);
        return wrap(t);
    }
    else
    {
        pt_mcrt_task* t = new (tbb::task::allocate_root()) pt_mcrt_task(execute_callback);
        return wrap(t);
    }
}

PT_ATTR_MCRT mcrt_task_ref PT_CALL mcrt_task_allocate_continuation(mcrt_task_ref self, mcrt_task_ref (*execute_callback)(mcrt_task_ref self))
{
    pt_mcrt_task *t = new (unwrap(self)->allocate_continuation()) pt_mcrt_task(execute_callback);
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
    pt_mcrt_task *t = static_cast<pt_mcrt_task *>(unwrap(self)->parent());
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

PT_ATTR_MCRT mcrt_task_group_context_ref PT_CALL mcrt_task_arena_context(mcrt_task_arena_ref task_arena)
{
    tbb::task_group_context* task_arena_context = NULL;

    unwrap(task_arena)->enqueue(
        [&task_arena_context]
        {
            task_arena_context = tbb::task::self().context();
        });

    while (NULL == task_arena_context)
    {
        mcrt_os_yield();
    }

    return wrap(task_arena_context);
}

PT_ATTR_MCRT void PT_CALL mcrt_task_enqueue(mcrt_task_ref task, mcrt_task_arena_ref task_arena)
{
    return tbb::task::enqueue((*unwrap(task)), (*unwrap(task_arena)), tbb::priority_normal);
}

PT_ATTR_MCRT uint32_t PT_CALL mcrt_task_arena_max_concurrency(mcrt_task_arena_ref task_arena)
{
    int max_concurrency = unwrap(task_arena)->max_concurrency();
    return uint32_t(max_concurrency);
}


PT_ATTR_MCRT void PT_CALL mcrt_task_arena_terminate(mcrt_task_arena_ref task_arena)
{
    unwrap(task_arena)->~task_arena();
    mcrt_aligned_free(task_arena);
    return;
}

PT_ATTR_MCRT uint32_t PT_CALL mcrt_this_task_arena_current_thread_index()
{
    int current_thread_index = tbb::this_task_arena::current_thread_index();
    return (tbb::task_arena::not_initialized != current_thread_index) ? uint32_t(current_thread_index) : uint32_t(-1);
}
