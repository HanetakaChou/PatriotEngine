#ifndef PT_MCRT_THREAD_H
#define PT_MCRT_THREAD_H 1

#include "../PTCommon.h"

#ifdef PTWIN32
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(PTPOSIX)
#include <pthread.h>
#else
#error 未知的平台
#endif

class mcrt_event
{
#if defined(PTPOSIX)
    pthread_mutex_t m_mutex;
    pthread_cond_t m_condition;
#elif defined(PTWIN32)
    CRITICAL_SECTION m_mutex;
    CONDITION_VARIABLE m_condition;
#else
#error 未知的平台
#endif
    bool m_manual_reset;
    bool m_state;
    bool m_is_valid;

public:
    // Constructs a new uninitialized event.
    inline mcrt_event();

    // Initializes this event to be a host-aware manual reset event with the
    // given initial state.
    // Returns:
    //   true if the initialization succeeded, false if it did not
    inline bool create_manual_event(bool initial_state);

    // Initializes this event to be a host-aware auto-resetting event with the
    // given initial state.
    // Returns:
    //   true if the initialization succeeded, false if it did not
    inline bool create_auto_event(bool initial_state);

    // Closes the event. Attempting to use the event past calling CloseEvent
    // is a logic error.
    inline void close_event();

    // Waits for some period of time for this event to be signalled. The
    // period of time may be infinite (if the timeout argument is INFINITE) or
    // it may be a specified period of time, in milliseconds.
    // Returns:
    //   One of three values, depending on how why this thread was awoken:
    //      WAIT_OBJECT_0 - This event was signalled and woke up this thread.
    //      WAIT_TIMEOUT  - The timeout interval expired without this event being signalled.
    //      WAIT_FAILED   - The wait failed.
    inline bool wait();

    inline bool timedwait(uint32_t timeout_milliseconds);

    // "Sets" the event, indicating that a particular event has occured. May
    // wake up other threads waiting on this event. Depending on whether or
    // not this event is an auto-reset event, the state of the event may
    // or may not be automatically reset after Set is called.
    inline void set();

    // Resets the event, resetting it back to a non-signalled state. Auto-reset
    // events automatically reset once the event is set, while manual-reset
    // events do not reset until Reset is called. It is a no-op to call Reset
    // on an auto-reset event.
    inline void reset();

private:
    inline bool initialize();
};

#include "PTSThread.h"

inline void mcrt_thread_setname(PTSThread *pThread, char const *name);

#endif