#include <assert.h>

inline void mcrt_os_event_init(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event, bool manual_reset, bool initial_state)
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

inline void mcrt_os_event_destroy(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event)
{
    assert(condition != NULL);
    assert(mutex != NULL);
    assert(event != NULL);

#ifndef NDEBUG
    event->mcrtp_condition = NULL;
    event->mcrtp_mutex = NULL;
#endif
}

inline void mcrt_os_event_set(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event)
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

inline void mcrt_os_event_reset(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event)
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

inline int mcrt_os_event_wait_one(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event)
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
        res = mcrt_os_cond_wait(condition, mutex);

        signalled = event->mcrtp_state_signalled;

    } while ((res == 0) && (!signalled));

    if ((res == 0) && signalled && (event->mcrtp_manual_reset))
    {
        event->mcrtp_state_signalled = false;
    }

    mcrt_os_mutex_unlock(mutex);

    return ((res == 0) ? 0 : -1);
}

inline int mcrt_os_event_timedwait_one(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event, uint32_t timeout_ms)
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
        res = mcrt_os_cond_timedwait(condition, mutex, timeout_ms);

        signalled = event->mcrtp_state_signalled;

    } while ((res == 0) && (!signalled));

    if ((res == 0) && signalled && (event->mcrtp_manual_reset))
    {
        event->mcrtp_state_signalled = false;
    }

    mcrt_os_mutex_unlock(mutex);

    return ((res == 0) ? 0 : -1);
}

inline int mcrt_os_event_wait_multiple(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event **events, size_t nevents, bool waitall)
{
    assert(condition != NULL);
    assert(mutex != NULL);
    assert(events != NULL);
    assert(nevents > 0);

#ifndef NDEBUG
    for (size_t i = 0; i < nevents; ++i)
    {
        assert(events[i]->mcrtp_condition == condition);
        assert(events[i]->mcrtp_mutex == mutex);
    }
#endif

    if (waitall)
    {
        mcrt_os_mutex_lock(mutex);

        int res;
        int lowest; //==-1
        bool signalled;
        do
        {
            res = mcrt_os_cond_wait(condition, mutex);

            bool signalled_and = true;
            for (size_t i = 0; i < nevents; ++i)
            {
                if (!events[i]->mcrtp_state_signalled)
                {
                    signalled_and = false;
                    lowest = i;
                    break;
                }
            }

            signalled = signalled_and;

        } while ((res == 0) && (!signalled));

        if ((res == 0) && signalled)
        {
            for (size_t i = 0; i < nevents; ++i)
            {
                if (events[i]->mcrtp_manual_reset)
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
        int lowest; //==-1
        bool signalled;
        do
        {
            res = mcrt_os_cond_wait(condition, mutex);

            bool signalled_or = false;
            for (size_t i = 0; i < nevents; ++i)
            {
                if (events[i]->mcrtp_state_signalled)
                {
                    signalled_or = true;
                    lowest = i;
                    break;
                }
            }

            signalled = signalled_or;

        } while ((res == 0) && (!signalled));

        if ((res == 0) && signalled)
        {
            assert(lowest >= 0);
            assert(lowest < nevents);
            if (events[lowest]->mcrtp_manual_reset)
            {
                events[lowest]->mcrtp_state_signalled = false;
            }
        }

        mcrt_os_mutex_unlock(mutex);

        return ((res == 0) ? lowest : -1);
    }
}

inline int mcrt_os_event_timedwait_multiple(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event **events, size_t nevents, bool waitall, uint32_t timeout_ms)
{
    assert(condition != NULL);
    assert(mutex != NULL);
    assert(events != NULL);
    assert(nevents > 0);

#ifndef NDEBUG
    for (size_t i = 0; i < nevents; ++i)
    {
        assert(events[i]->mcrtp_condition == condition);
        assert(events[i]->mcrtp_mutex == mutex);
    }
#endif

    if (waitall)
    {
        mcrt_os_mutex_lock(mutex);

        int res;
        int lowest; //==-1
        bool signalled;
        do
        {
            res = mcrt_os_cond_timedwait(condition, mutex, timeout_ms);

            bool signalled_and = true;
            for (size_t i = 0; i < nevents; ++i)
            {
                if (!events[i]->mcrtp_state_signalled)
                {
                    signalled_and = false;
                    lowest = i;
                    break;
                }
            }

            signalled = signalled_and;

        } while ((res == 0) && (!signalled));

        if ((res == 0) && signalled)
        {
            for (size_t i = 0; i < nevents; ++i)
            {
                if (events[i]->mcrtp_manual_reset)
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
        int lowest; //==-1
        bool signalled;
        do
        {
            res = mcrt_os_cond_timedwait(condition, mutex, timeout_ms);

            bool signalled_or = false;
            for (size_t i = 0; i < nevents; ++i)
            {
                if (events[i]->mcrtp_state_signalled)
                {
                    signalled_or = true;
                    lowest = i;
                    break;
                }
            }

            signalled = signalled_or;

        } while ((res == 0) && (!signalled));

        if ((res == 0) && signalled)
        {
            assert(lowest >= 0);
            assert(lowest < nevents);
            if (events[lowest]->mcrtp_manual_reset)
            {
                events[lowest]->mcrtp_state_signalled = false;
            }
        }

        mcrt_os_mutex_unlock(mutex);

        return ((res == 0) ? lowest : -1);
    }
}
