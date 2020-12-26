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

#include <assert.h>

inline void mcrt_os_event_init(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event, bool manual_reset, bool initial_state)
{
    assert(condition != NULL);
    assert(mutex != NULL);
    assert(event != NULL);

    event->mcrtp_manual_reset = manual_reset;
    event->mcrtp_state_signalled = initial_state;
#ifndef NDEBUG
    event->mcrtp_condition = condition;
    event->mcrtp_mutex = mutex;
#endif
}

inline void mcrt_os_event_destroy(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event)
{
    assert(condition != NULL);
    assert(mutex != NULL);
    assert(event != NULL);

#ifndef NDEBUG
    event->mcrtp_condition = NULL;
    event->mcrtp_mutex = NULL;
#endif
}

inline void mcrt_os_event_set(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event)
{
    assert(condition != NULL);
    assert(mutex != NULL);
    assert(event != NULL);

    assert(event->mcrtp_condition == condition);
    assert(event->mcrtp_mutex == mutex);

    mcrt_os_mutex_lock(mutex);

    event->mcrtp_state_signalled = true;

    //mcrt_os_cond_signal(condition);

    mcrt_os_mutex_unlock(mutex);

    mcrt_os_cond_broadcast(condition);
}

inline void mcrt_os_event_reset(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event)
{
    assert(condition != NULL);
    assert(mutex != NULL);
    assert(event != NULL);

    assert(event->mcrtp_condition == condition);
    assert(event->mcrtp_mutex == mutex);

    mcrt_os_mutex_lock(mutex);

    event->mcrtp_state_signalled = false;

    mcrt_os_mutex_unlock(mutex);
}

inline int mcrt_os_event_wait_one(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event)
{
    assert(condition != NULL);
    assert(mutex != NULL);
    assert(event != NULL);

    assert(event->mcrtp_condition == condition);
    assert(event->mcrtp_mutex == mutex);

    mcrt_os_mutex_lock(mutex);

    int res;
    bool signalled;
    do
    {
        // We shouldn't wait if the event has been signalled
        if (!event->mcrtp_state_signalled)
        {
            res = mcrt_os_cond_wait(condition, mutex);
            signalled = false;
        }
        else
        {
            res = 0;
            signalled = true;
        }

    } while ((res == 0) && (!signalled));

    if ((res == 0) && signalled && (!event->mcrtp_manual_reset))
    {
        event->mcrtp_state_signalled = false;
    }

    mcrt_os_mutex_unlock(mutex);

    return ((res == 0) ? 0 : -1);
}

inline int mcrt_os_event_timedwait_one(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event, uint32_t timeout_ms)
{
    assert(condition != NULL);
    assert(mutex != NULL);
    assert(event != NULL);

    assert(event->mcrtp_condition == condition);
    assert(event->mcrtp_mutex == mutex);

    mcrt_os_mutex_lock(mutex);

    int res;
    bool signalled;
    do
    {
        // We shouldn't wait if the event has been signalled
        if (!event->mcrtp_state_signalled)
        {
            res = mcrt_os_cond_timedwait(condition, mutex, timeout_ms);
            signalled = false;
        }
        else
        {
            res = 0;
            signalled = true;
        }

    } while ((res == 0) && (!signalled));

    if ((res == 0) && signalled && (!event->mcrtp_manual_reset))
    {
        event->mcrtp_state_signalled = false;
    }

    mcrt_os_mutex_unlock(mutex);

    return ((res == 0) ? 0 : -1);
}

inline int mcrt_os_event_wait_multiple(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t **events, int nevents, bool waitall)
{
    assert(condition != NULL);
    assert(mutex != NULL);
    assert(events != NULL);
    assert(nevents > 0);

#ifndef NDEBUG
    for (int i = 0; i < nevents; ++i)
    {
        assert(events[i]->mcrtp_condition == condition);
        assert(events[i]->mcrtp_mutex == mutex);
    }
#endif

    if (waitall)
    {
        mcrt_os_mutex_lock(mutex);

        int res;
        int lowest = -1;
        bool signalled;
        do
        {
            bool signalled_and = true;
            for (int i = 0; i < nevents; ++i)
            {
                if (!events[i]->mcrtp_state_signalled)
                {
                    signalled_and = false;
                    lowest = i;
                    break;
                }
            }

            // We shouldn't wait if the event has been signalled
            if (!signalled_and)
            {
                res = mcrt_os_cond_wait(condition, mutex);
                signalled = false;
            }
            else
            {
                res = 0;
                signalled = true;
            }

        } while ((res == 0) && (!signalled));

        if ((res == 0) && signalled)
        {
            for (int i = 0; i < nevents; ++i)
            {
                if (!events[i]->mcrtp_manual_reset)
                {
                    events[i]->mcrtp_state_signalled = false;
                }
            }
        }

        mcrt_os_mutex_unlock(mutex);

        return ((res == 0) ? 0 : -1);
    }
    else
    {
        mcrt_os_mutex_lock(mutex);

        int res;
        int lowest = -1;
        bool signalled;
        do
        {
            bool signalled_or = false;
            for (int i = 0; i < nevents; ++i)
            {
                if (events[i]->mcrtp_state_signalled)
                {
                    signalled_or = true;
                    lowest = i;
                    break;
                }
            }

            // We shouldn't wait if the event has been signalled
            if (!signalled_or)
            {
                res = mcrt_os_cond_wait(condition, mutex);
                signalled = false;
            }
            else
            {
                res = 0;
                signalled = true;
            }

        } while ((res == 0) && (!signalled));

        if ((res == 0) && signalled)
        {
            assert(lowest >= 0);
            assert(lowest < nevents);
            if (!events[lowest]->mcrtp_manual_reset)
            {
                events[lowest]->mcrtp_state_signalled = false;
            }
        }

        mcrt_os_mutex_unlock(mutex);

        return ((res == 0) ? lowest : -1);
    }
}

inline int mcrt_os_event_timedwait_multiple(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t **events, int nevents, bool waitall, uint32_t timeout_ms)
{
    assert(condition != NULL);
    assert(mutex != NULL);
    assert(events != NULL);
    assert(nevents > 0);

#ifndef NDEBUG
    for (int i = 0; i < nevents; ++i)
    {
        assert(events[i]->mcrtp_condition == condition);
        assert(events[i]->mcrtp_mutex == mutex);
    }
#endif

    if (waitall)
    {
        mcrt_os_mutex_lock(mutex);

        int res;
        int lowest = -1;
        bool signalled;
        do
        {
            bool signalled_and = true;
            for (int i = 0; i < nevents; ++i)
            {
                if (!events[i]->mcrtp_state_signalled)
                {
                    signalled_and = false;
                    lowest = i;
                    break;
                }
            }

            // We shouldn't wait if the event has been signalled
            if (!signalled_and)
            {
                res = mcrt_os_cond_timedwait(condition, mutex, timeout_ms);
                signalled = false;
            }
            else
            {
                res = 0;
                signalled = true;
            }

        } while ((res == 0) && (!signalled));

        if ((res == 0) && signalled)
        {
            for (int i = 0; i < nevents; ++i)
            {
                if (!events[i]->mcrtp_manual_reset)
                {
                    events[i]->mcrtp_state_signalled = false;
                }
            }
        }

        mcrt_os_mutex_unlock(mutex);

        return ((res == 0) ? 0 : -1);
    }
    else
    {
        mcrt_os_mutex_lock(mutex);

        int res;
        int lowest = -1;
        bool signalled;
        do
        {
            bool signalled_or = false;
            for (int i = 0; i < nevents; ++i)
            {
                if (events[i]->mcrtp_state_signalled)
                {
                    signalled_or = true;
                    lowest = i;
                    break;
                }
            }

            // We shouldn't wait if the event has been signalled
            if (!signalled_or)
            {
                res = mcrt_os_cond_timedwait(condition, mutex, timeout_ms);
                signalled = false;
            }
            else
            {
                res = 0;
                signalled = true;
            }

        } while ((res == 0) && (!signalled));

        if ((res == 0) && signalled)
        {
            assert(lowest >= 0);
            assert(static_cast<size_t>(lowest) < nevents);
            if (!events[lowest]->mcrtp_manual_reset)
            {
                events[lowest]->mcrtp_state_signalled = false;
            }
        }

        mcrt_os_mutex_unlock(mutex);

        return ((res == 0) ? lowest : -1);
    }
}
