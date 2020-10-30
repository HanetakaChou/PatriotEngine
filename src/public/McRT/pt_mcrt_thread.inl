
inline void mcrt_os_event_init(mcrt_os_event *event, bool manual_reset, bool initial_state)
{
    event->mcrtp_manual_reset = manual_reset;
    event->mcrtp_state_signalled = initial_state;
}

inline void mcrt_os_event_destroy(mcrt_os_event *event)
{
}

inline void mcrt_os_event_set(mcrt_os_mutex *mutex, mono_os_cond *condition, mcrt_os_event *event)
{
    mcrt_os_mutex_lock(mutex);

    event->mcrtp_state_signalled = true;

    //mcrt_os_cond_signal(condition);

    mcrt_os_mutex_unlock(mutex);

    mcrt_os_cond_broadcast(condition);
}

inline void mcrt_os_event_reset(mcrt_os_mutex *mutex, mono_os_cond *condition, mcrt_os_event *event)
{
    mcrt_os_mutex_lock(mutex);

    event->mcrtp_state_signalled = false;

    mcrt_os_mutex_unlock(mutex);
}

inline void mcrt_os_event_wait_one(mcrt_os_mutex *mutex, mono_os_cond *condition, mcrt_os_event *event)
{
}

inline int mcrt_os_event_timedwait_one(mcrt_os_mutex *mutex, mono_os_cond *condition, mcrt_os_event *event, uint32_t timeout_ms)
{
}

inline int mcrt_os_event_wait_multiple(mcrt_os_mutex *mutex, mono_os_cond *condition, mcrt_os_event **events, size_t nevents, bool waitall)
{
    if (waitall)
    {
        mcrt_os_mutex_lock(mutex);

        for (;;)
        {
            mcrt_os_cond_wait(condition);

            bool all_signalled = true;
            for (size_t i = 0; i < nevents; ++i)
            {
                if (!events[i]->mcrtp_state_signalled)
                {
                    all_signalled = false;
                    break;
                }
            }

            if (all_signalled)
            {
                for (size_t i = 0; i < nevents; ++i)
                {
                    if (events[i]->mcrtp_manual_reset)
                    {
                        events[i]->mcrtp_state_signalled = false;
                    }
                }

                break;
            }
        }

        mcrt_os_mutex_unlock(mutex);
    }
    else
    {
    }

    do
    {
    } while ((st2 == 0) && (!m_state));

    if ((st2 == 0) && !m_manual_reset)
    {
        // Clear the state for auto-reset events so that only one waiter gets released
        m_state = false;
    }

    int st3 =
}

inline int mcrt_os_event_timedwait_multiple(mcrt_os_mutex *mutex, mono_os_cond *condition, mcrt_os_event **events, size_t nevents, bool waitall, uint32_t timeout_ms);
