/*
 * Copyright (C) YuqiaoZhang
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

#include "pt_mcrt_task.h"

#include <tbb/task.h>

inline mcrt_task wrap(tbb::task *t) { return reinterpret_cast<mcrt_task>(t); }

inline tbb::task *unwrap(mcrt_task t) { return reinterpret_cast<tbb::task *>(t); }

class pt_mcrt_task : public tbb::task
{
    mcrt_task (*m_execute_callback)(char user_data[]);

public:
    char m_user_data[128];

private:
    tbb::task *execute() override
    {
        mcrt_task t = m_execute_callback(m_user_data);
        return unwrap(t);
    }

public:
    inline pt_mcrt_task(mcrt_task (*execute_callback)(char user_data[])) : m_execute_callback(execute_callback) {}
};

// scheduler.h
// quick_task_size
static_assert(sizeof(pt_mcrt_task) <= 192UL, "sizeof(pt_mcrt_task) <= quick_task_size");

PT_MCRT_ATTR mcrt_task PT_CALL mcrt_task_allocate_root(mcrt_task (*execute_callback)(char user_data[128]), void (*init_callback)(char user_data[128]))
{
    pt_mcrt_task *t = new (tbb::task::allocate_root()) pt_mcrt_task(execute_callback);
    init_callback(t->m_user_data);
    return wrap(t);
}

PT_MCRT_ATTR mcrt_task PT_CALL mcrt_task_allocate_continuation(mcrt_task self, mcrt_task (*execute_callback)(char user_data[128]), void (*init_callback)(char user_data[128]))
{
    pt_mcrt_task *t = new (unwrap(self)->allocate_continuation()) pt_mcrt_task(execute_callback);
    init_callback(t->m_user_data);
    return wrap(t);
}

PT_MCRT_ATTR void PT_CALL mcrt_task_recycle_as_child_of(mcrt_task self, mcrt_task successor)
{
    return unwrap(self)->recycle_as_child_of(*unwrap(successor));
}