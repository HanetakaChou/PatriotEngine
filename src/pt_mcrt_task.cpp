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
    mcrt_task (*m_execute_callback)(void *user_data);
    void *m_user_data;

    tbb::task *execute() override
    {
        mcrt_task t = m_execute_callback(m_user_data);
        return unwrap(t);
    }

public:
    inline pt_mcrt_task(mcrt_task (*execute_callback)(void *user_data), void *user_data) : m_execute_callback(execute_callback), m_user_data(user_data) {}
};

PT_MCRT_ATTR mcrt_task PT_CALL allocate_root(mcrt_task (*execute_callback)(void *user_data), void *user_data)
{
    tbb::task *t = new (tbb::task::allocate_root()) pt_mcrt_task(execute_callback, user_data);
    return wrap(t);
}