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

#include <stddef.h>
#include <stdlib.h>

#if defined(PT_POSIX)
typedef pthread_mutex_t mcrt_os_mutex;
typedef pthread_cond_t mcrt_os_cond;
#elif defined(PT_WIN32)
typedef CRITICAL_SECTION mcrt_os_mutex;
typedef CONDITION_VARIABLE mcrt_os_cond;
#else
#error Unknown Platform
#endif

inline void mcrt_os_mutex_init (mcrt_os_mutex *mutex);
inline void mcrt_os_mutex_destroy (mcrt_os_mutex *mutex);
inline void mcrt_os_mutex_lock (mcrt_os_mutex *mutex);
inline void mcrt_os_mutex_unlock (mcrt_os_mutex *mutex);

inline void mcrt_os_cond_init(mcrt_os_cond *cond);
inline void mcrt_os_cond_destroy(mcrt_os_cond *cond);
inline void mcrt_os_cond_wait(mcrt_os_cond *cond);
inline int mcrt_os_cond_timedwait(mcrt_os_cond *cond, uint32_t timeout_ms);
inline void mcrt_os_cond_signal(mcrt_os_cond *cond);
inline void mcrt_os_cond_broadcast(mcrt_os_cond *cond);

typedef struct mcrt_os_event
{
    bool m_manual_reset;
    bool m_state_signalled;
} mcrt_os_event;

inline void mcrt_os_event_init(mcrt_os_event *event, bool manual_reset, bool initial_state);

inline void mcrt_os_event_destroy(mcrt_os_event *event);

inline void mcrt_os_event_set(mcrt_os_mutex *mutex, mono_os_cond *condition, mcrt_os_event *event);

inline void mcrt_os_event_reset(mcrt_os_mutex *mutex, mono_os_cond *condition, mcrt_os_event *event);

inline void mcrt_os_event_wait_one(mcrt_os_mutex *mutex, mono_os_cond *condition, mcrt_os_event *event);

inline int mcrt_os_event_timedwait_one(mcrt_os_mutex *mutex, mono_os_cond *condition, mcrt_os_event *event, uint32_t timeout_ms);

inline int mcrt_os_event_wait_multiple(mcrt_os_mutex *mutex, mono_os_cond *condition, mcrt_os_event **events, size_t nevents, bool waitall);

inline int mcrt_os_event_timedwait_multiple(mcrt_os_mutex *mutex, mono_os_cond *condition, mcrt_os_event **events, size_t nevents, bool waitall, uint32_t timeout_ms);

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

#ifndef NDEBUG
    bool m_is_valid;
#endif

    inline void initialize();

public:
    // Constructs a new uninitialized event.
    inline mcrt_event();

    // Initializes this event to be a host-aware manual reset event with the
    // given initial state.
    inline void create_manual_event(bool initial_state);

    // Initializes this event to be a host-aware auto-resetting event with the
    // given initial state.
    inline void create_auto_event(bool initial_state);

    // Closes the event. Attempting to use the event past calling CloseEvent
    // is a logic error.
    inline void close_event();

    // Waits for this event to be signalled.
    inline void wait();

    // Waits for a specified period of time in milliseconds for this event
    // to be signalled.
    // Returns:
    //   true if this event was signalled and woke up this thread, false if the
    //   timeout interval expired without this event being signalled.
    inline bool wait_time(uint32_t timeout_milliseconds);

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
};

#include "PTSThread.h"

inline bool mcrt_thread_create(PTSThreadEntry *, void *pThreadParam, PTSThread *pThreadOut);

inline void mcrt_thread_setname(PTSThread *pThread, char const *name);

#endif