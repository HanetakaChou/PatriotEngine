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

// [Intel® Threading Building Blocks / Developer Guide / General Acyclic Graphs of Tasks](https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-guide/the-task-scheduler/general-acyclic-graphs-of-tasks.html)

#include <stddef.h>
#include <pt_mcrt_task.h>
#include <assert.h>
#include <stdio.h>

// 1 -> 1 -> 1 -> 1 -> 1
// |    |    |    |    |
// 1 -> 2 -> 3 -> 4 -> 5
// |    |    |    |    |
// 1 -> 3 -> 6 -> 10-> 15
// |    |    |    |    |
// 1 -> 4 -> 10-> 20-> 35

class dag_task
{
    int m_i;
    int m_j;
    int m_value_top;
    int m_value_left;
    mcrt_task m_successor_bottom;
    mcrt_task m_successor_right;

    static int const value_uninit = 0xdeadbeef; //0xbaddcafe

    static dag_task *unwrap(mcrt_task_user_data_t *user_data)
    {
        return reinterpret_cast<dag_task *>(user_data);
    }

    static void init(mcrt_task_user_data_t *user_data, int i, int j, mcrt_task successor_bottom, mcrt_task successor_right)
    {
        dag_task *self = unwrap(user_data);

        self->m_i = i;
        self->m_j = j;
        self->m_value_top = value_uninit;
        self->m_value_left = value_uninit;
        self->m_successor_bottom = successor_bottom;
        self->m_successor_right = successor_right;
    }

    static mcrt_task execute(mcrt_task_user_data_t *user_data)
    {
        dag_task *self = unwrap(user_data);

        // execute
        if (self->m_successor_bottom != NULL || self->m_successor_right != NULL)
        {
            // init
            if (self->m_i != 0 && self->m_j != 0)
            {
                assert(value_uninit != self->m_value_top);
                assert(value_uninit != self->m_value_left);
            }
            else if (self->m_i == 0 && self->m_j != 0)
            {
                assert(value_uninit == self->m_value_top);
                assert(value_uninit != self->m_value_left);
                self->m_value_top = 0;
            }
            else if (self->m_i != 0 && self->m_j == 0)
            {
                assert(value_uninit != self->m_value_top);
                assert(value_uninit == self->m_value_left);
                self->m_value_left = 0;
            }
            else
            {
                assert(value_uninit == self->m_value_top);
                assert(value_uninit == self->m_value_left);
                self->m_value_top = 1;
                self->m_value_left = 0;
            }

            if (NULL != self->m_successor_bottom)
            {
                dag_task *successor_bottom = unwrap(mcrt_task_user_data(self->m_successor_bottom));

                assert(value_uninit == successor_bottom->m_value_top);
                successor_bottom->m_value_top = self->m_value_left + self->m_value_top;
            }
            if (NULL != self->m_successor_right)
            {
                dag_task *successor_right = unwrap(mcrt_task_user_data(self->m_successor_right));

                assert(value_uninit == successor_right->m_value_left);
                successor_right->m_value_left = self->m_value_left + self->m_value_top;
            }
        }
        else
        {
            //we reach the end point
            printf("%i + %i = %i\n", self->m_value_top, self->m_value_left, self->m_value_top + self->m_value_left);
        }

        // successor
        mcrt_task bypass_slot = NULL;
        if (NULL != self->m_successor_bottom)
        {
            tally_completion_of_predecessor(self->m_successor_bottom, &bypass_slot);
        }
        if (NULL != self->m_successor_right)
        {
            tally_completion_of_predecessor(self->m_successor_right, &bypass_slot);
        }
        return bypass_slot;
    }

    static void tally_completion_of_predecessor(mcrt_task s, mcrt_task *out_bypass_slot)
    {
        // tally_completion_of_predecessor
        // SchedulerTraits::has_slow_atomic
        // bypass_slot

        if (1 == mcrt_task_ref_count(s))
        {
            mcrt_task_set_ref_count(s, 0);
        }
        else
        {
            int new_ref_count = mcrt_task_decrement_ref_count(s);

            if (new_ref_count > 0)
            {
                return;
            }
        }

        assert(0 == mcrt_task_ref_count(s));

        if ((*out_bypass_slot) == NULL)
        {
            (*out_bypass_slot) = s;
        }
        else
        {
            mcrt_task_spawn(s);
        }
    }

public:
    static mcrt_task allocate_root(int i, int j, mcrt_task successor_bottom, mcrt_task successor_right)
    {
        mcrt_task t = mcrt_task_allocate_root(execute);
        init(mcrt_task_user_data(t), i, j, successor_bottom, successor_right);
        return t;
    }
};
static_assert(sizeof(dag_task) <= sizeof(mcrt_task_user_data_t), "sizeof(dag_task) <= sizeof(mcrt_task_user_data_t)");

class dummy_task
{
    static mcrt_task execute(mcrt_task_user_data_t *user_data)
    {
        assert(false); //Must never get here
        return NULL;
    }

public:
    static mcrt_task allocate_root()
    {
        mcrt_task t = mcrt_task_allocate_root(execute);
        return t;
    }
};
static_assert(sizeof(dummy_task) <= sizeof(mcrt_task_user_data_t), "sizeof(dummy_task) <= sizeof(mcrt_task_user_data_t)");

int main(int argc, char **argv)
{
    int const M = 4;
    int const N = 5;
    mcrt_task x[M][N];

    for (int i = (M - 1); i >= 0; --i)
    {
        for (int j = (N - 1); j >= 0; --j)
        {
            mcrt_task successor_bottom = ((i + 1) < M) ? x[i + 1][j] : NULL;
            mcrt_task successor_right = ((j + 1) < N) ? x[i][j + 1] : NULL;
            x[i][j] = dag_task::allocate_root(i, j, successor_bottom, successor_right);
            int predecessor_top = (i > 0) ? 1 : 0;
            int predecessor_left = (j > 0) ? 1 : 0;
            mcrt_task_set_ref_count(x[i][j], predecessor_top + predecessor_left);
        }
    }

    mcrt_task real_root = dummy_task::allocate_root();
    mcrt_task_set_parent(x[M - 1][N - 1], real_root);
    mcrt_task_set_ref_count(real_root, 2);
    mcrt_task_spawn_and_wait_for_all(real_root, x[0][0]);
    assert(0 == mcrt_task_ref_count(real_root));

    // we can reuse the dummy task for next wait  
    mcrt_task_destory(real_root);

    return 0;
}