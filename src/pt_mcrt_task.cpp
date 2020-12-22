/*
 * Copyright (C) YuqiaoZhang張羽喬(HanetakaYuminaga弓長羽高)
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

class mcrt_task_t;

inline mcrt_task_ref wrap(class mcrt_task_t *t) { return reinterpret_cast<mcrt_task_ref>(t); }

inline class mcrt_task_t *unwrap(mcrt_task_ref t) { return reinterpret_cast<class mcrt_task_t *>(t); }

class mcrt_task_t : public tbb::task
{
    mcrt_task_ref (*m_execute_callback)(mcrt_task_user_data_t *user_data);

    mcrt_task_user_data_t m_user_data;

    tbb::task *execute() override
    {
        mcrt_task_ref t = m_execute_callback(&m_user_data);
        return unwrap(t);
    }

public:
    inline mcrt_task_t(mcrt_task_ref (*execute_callback)(mcrt_task_user_data_t *user_data)) : m_execute_callback(execute_callback) {}

    inline mcrt_task_user_data_t *user_data() { return &m_user_data; }
};

// scheduler.h
// quick_task_size
static_assert(sizeof(mcrt_task_t) <= 192UL, "sizeof(mcrt_task_t) <= quick_task_size");

PT_ATTR_MCRT mcrt_task_ref PT_CALL mcrt_task_allocate_root(mcrt_task_ref (*execute_callback)(mcrt_task_user_data_t *user_data))
{
    mcrt_task_t *t = new (tbb::task::allocate_root()) mcrt_task_t(execute_callback);
    return wrap(t);
}

PT_ATTR_MCRT mcrt_task_ref PT_CALL mcrt_task_allocate_continuation(mcrt_task_ref self, mcrt_task_ref (*execute_callback)(mcrt_task_user_data_t *user_data))
{
    mcrt_task_t *t = new (unwrap(self)->allocate_continuation()) mcrt_task_t(execute_callback);
    return wrap(t);
}

PT_ATTR_MCRT mcrt_task_user_data_t *PT_CALL mcrt_task_user_data(mcrt_task_ref self)
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

PT_ATTR_MCRT int PT_CALL mcrt_task_decrement_ref_count(mcrt_task_ref self)
{
    return unwrap(self)->decrement_ref_count();
}

PT_ATTR_MCRT void PT_CALL mcrt_task_set_parent(mcrt_task_ref self, mcrt_task_ref parent)
{
    return unwrap(self)->set_parent(unwrap(parent));
}

PT_ATTR_MCRT void PT_CALL mcrt_task_spawn(mcrt_task_ref t)
{
    return tbb::task::spawn(*unwrap(t));
}

PT_ATTR_MCRT void PT_CALL mcrt_task_spawn_and_wait_for_all(mcrt_task_ref self, mcrt_task_ref child)
{
    return unwrap(self)->spawn_and_wait_for_all(*unwrap(child));
}