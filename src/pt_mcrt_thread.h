#ifndef PT_MCRT_THREAD_H
#define PT_MCRT_THREAD_H 1

#include "pt_common.h"

#include <stddef.h>
#include <stdlib.h>

#if defined(PT_POSIX)
#include <pthread.h>
typedef pthread_mutex_t mcrt_os_mutex;
typedef pthread_cond_t mcrt_os_cond;
#elif defined(PT_WIN32)
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
typedef CRITICAL_SECTION mcrt_os_mutex;
typedef CONDITION_VARIABLE mcrt_os_cond;
#else
#error Unknown Platform
#endif

//inline bool mcrt_thread_create(PTSThreadEntry *, void *pThreadParam, PTSThread *pThreadOut);

//inline void mcrt_thread_setname(PTSThread *pThread, char const *name);

inline void mcrt_os_mutex_init(mcrt_os_mutex *mutex);
inline void mcrt_os_mutex_destroy(mcrt_os_mutex *mutex);
inline void mcrt_os_mutex_lock(mcrt_os_mutex *mutex);
inline int mcrt_os_mutex_trylock(mcrt_os_mutex *mutex);
inline void mcrt_os_mutex_unlock(mcrt_os_mutex *mutex);

inline void mcrt_os_cond_init(mcrt_os_cond *cond);
inline void mcrt_os_cond_destroy(mcrt_os_cond *cond);
inline int mcrt_os_cond_wait(mcrt_os_cond *cond, mcrt_os_mutex *mutex);
inline int mcrt_os_cond_timedwait(mcrt_os_cond *cond, mcrt_os_mutex *mutex, uint32_t timeout_ms);
inline void mcrt_os_cond_signal(mcrt_os_cond *cond);
inline void mcrt_os_cond_broadcast(mcrt_os_cond *cond);

typedef struct mcrt_os_event
{
#ifndef NDEBUG
private:
#endif
    bool mcrtp_manual_reset;
    bool mcrtp_state_signalled;
#ifndef NDEBUG
    mcrt_os_cond *mcrtp_condition;
    mcrt_os_mutex *mcrtp_mutex;

public:
    friend void mcrt_os_event_init(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event, bool manual_reset, bool initial_state);
    friend void mcrt_os_event_destroy(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event);
    friend void mcrt_os_event_set(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event);
    friend void mcrt_os_event_reset(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event);
    friend int mcrt_os_event_wait_one(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event);
    friend int mcrt_os_event_timedwait_one(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event, uint32_t timeout_ms);
    friend int mcrt_os_event_wait_multiple(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event **events, size_t nevents, bool waitall);
    friend int mcrt_os_event_timedwait_multiple(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event **events, size_t nevents, bool waitall, uint32_t timeout_ms);
#endif
} mcrt_os_event;

inline void mcrt_os_event_init(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event, bool manual_reset, bool initial_state);

inline void mcrt_os_event_destroy(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event);

inline void mcrt_os_event_set(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event);

inline void mcrt_os_event_reset(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event);

inline int mcrt_os_event_wait_one(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event);

inline int mcrt_os_event_timedwait_one(mcrt_os_cond *condition, mcrt_os_mutex *mutex, mcrt_os_event *event, uint32_t timeout_ms);

// Returns:
//  If waitall is true:
//       0 - The return value indicates that the state of all specified events is signaled.
//      -1 - The function has failed.
//  If waitall is false:
//       0 to (nevents-1) - The return value indicates the events array index of the event that satisfied the wait.
//                          If more than one event became signaled during the call, this is the array index of
//                          the signaled event with the smallest index value of all the signaled events.
//      -1                - The function has failed.
inline int mcrt_os_event_wait_multiple(mcrt_os_mutex *mutex, mcrt_os_cond *condition, mcrt_os_event **events, size_t nevents, bool waitall);

// Returns:
//  If waitall is true:
//       0 - The return value indicates that the state of all specified events is signaled.
//      -1 - The time-out interval elapsed and the conditions specified by the waitall parameter are not satisfied or the function has failed.
//  If waitall is false:
//       0 to (nevents-1) - The return value indicates the events array index of the event that satisfied the wait.
//                          If more than one event became signaled during the call, this is the array index of
//                          the signaled event with the smallest index value of all the signaled events.
//      -1                - The time-out interval elapsed and the conditions specified by the waitall parameter are not satisfied or the function has failed.
inline int mcrt_os_event_timedwait_multiple(mcrt_os_mutex *mutex, mcrt_os_cond *condition, mcrt_os_event **events, size_t nevents, bool waitall, uint32_t timeout_ms);

#include "pt_mcrt_thread.inl"

#if defined PT_POSIX
#include "pt_mcrt_thread_posix.inl"
#elif defined PT_WIN32
#include "pt_mcrt_thread_win32.inl"
#else
#error 未知的平台
#endif

#endif