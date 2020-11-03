#ifndef PT_MCRT_THREAD_H
#define PT_MCRT_THREAD_H 1

#include "pt_common.h"

#include <stddef.h>
#include <stdlib.h>

#if defined(PT_POSIX)
#include <pthread.h>
typedef pthread_t mcrt_native_thread_id;
typedef pthread_key_t mcrt_native_tls_key;
typedef pthread_mutex_t mcrt_mutex_t;
typedef pthread_cond_t mcrt_cond_t;
#elif defined(PT_WIN32)
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
typedef HANDLE mcrt_native_thread_id;
typedef DWORD mcrt_native_tls_key;
typedef CRITICAL_SECTION mcrt_mutex_t;
typedef CONDITION_VARIABLE mcrt_cond_t;
#else
#error Unknown Platform
#endif

//inline bool mcrt_thread_create(PTSThreadEntry *, void *pThreadParam, PTSThread *pThreadOut);

//inline void mcrt_thread_setname(PTSThread *pThread, char const *name);
inline bool mcrt_native_thread_create(mcrt_native_thread_id *tid, void *(*func)(void *), void *arg);
inline void mcrt_native_thread_set_name(mcrt_native_thread_id tid, char const *name);

inline bool mcrt_native_tls_alloc(mcrt_native_thread_id *key, void(*destructor)(void *));
inline void mcrt_native_tls_free(mcrt_native_thread_id key);
inline bool mcrt_native_tls_set_value(mcrt_native_thread_id key, void *value);
inline void *mcrt_native_tls_get_value(mcrt_native_thread_id key);

inline void mcrt_os_mutex_init(mcrt_mutex_t *mutex);
inline void mcrt_os_mutex_destroy(mcrt_mutex_t *mutex);
inline void mcrt_os_mutex_lock(mcrt_mutex_t *mutex);
inline int mcrt_os_mutex_trylock(mcrt_mutex_t *mutex);
inline void mcrt_os_mutex_unlock(mcrt_mutex_t *mutex);

inline void mcrt_os_cond_init(mcrt_cond_t *cond);
inline void mcrt_os_cond_destroy(mcrt_cond_t *cond);
inline int mcrt_os_cond_wait(mcrt_cond_t *cond, mcrt_mutex_t *mutex);
inline int mcrt_os_cond_timedwait(mcrt_cond_t *cond, mcrt_mutex_t *mutex, uint32_t timeout_ms);
inline void mcrt_os_cond_signal(mcrt_cond_t *cond);
inline void mcrt_os_cond_broadcast(mcrt_cond_t *cond);

typedef struct mcrt_os_event
{
#ifndef NDEBUG
private:
#endif
    bool mcrtp_manual_reset;
    bool mcrtp_state_signalled;
#ifndef NDEBUG
    mcrt_cond_t *mcrtp_condition;
    mcrt_mutex_t *mcrtp_mutex;

public:
    friend void mcrt_os_event_init(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event *event, bool manual_reset, bool initial_state);
    friend void mcrt_os_event_destroy(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event *event);
    friend void mcrt_os_event_set(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event *event);
    friend void mcrt_os_event_reset(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event *event);
    friend int mcrt_os_event_wait_one(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event *event);
    friend int mcrt_os_event_timedwait_one(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event *event, uint32_t timeout_ms);
    friend int mcrt_os_event_wait_multiple(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event **events, size_t nevents, bool waitall);
    friend int mcrt_os_event_timedwait_multiple(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event **events, size_t nevents, bool waitall, uint32_t timeout_ms);
#endif
} mcrt_os_event;

inline void mcrt_os_event_init(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event *event, bool manual_reset, bool initial_state);

inline void mcrt_os_event_destroy(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event *event);

inline void mcrt_os_event_set(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event *event);

inline void mcrt_os_event_reset(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event *event);

inline int mcrt_os_event_wait_one(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event *event);

inline int mcrt_os_event_timedwait_one(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_os_event *event, uint32_t timeout_ms);

// Returns:
//  If waitall is true:
//       0 - The return value indicates that the state of all specified events is signaled.
//      -1 - The function has failed.
//  If waitall is false:
//       0 to (nevents-1) - The return value indicates the events array index of the event that satisfied the wait.
//                          If more than one event became signaled during the call, this is the array index of
//                          the signaled event with the smallest index value of all the signaled events.
//      -1                - The function has failed.
inline int mcrt_os_event_wait_multiple(mcrt_mutex_t *mutex, mcrt_cond_t *condition, mcrt_os_event **events, size_t nevents, bool waitall);

// Returns:
//  If waitall is true:
//       0 - The return value indicates that the state of all specified events is signaled.
//      -1 - The time-out interval elapsed and the conditions specified by the waitall parameter are not satisfied or the function has failed.
//  If waitall is false:
//       0 to (nevents-1) - The return value indicates the events array index of the event that satisfied the wait.
//                          If more than one event became signaled during the call, this is the array index of
//                          the signaled event with the smallest index value of all the signaled events.
//      -1                - The time-out interval elapsed and the conditions specified by the waitall parameter are not satisfied or the function has failed.
inline int mcrt_os_event_timedwait_multiple(mcrt_mutex_t *mutex, mcrt_cond_t *condition, mcrt_os_event **events, size_t nevents, bool waitall, uint32_t timeout_ms);

#include "pt_mcrt_thread.inl"

#if defined PT_POSIX
#include "pt_mcrt_thread_posix.inl"
#elif defined PT_WIN32
#include "pt_mcrt_thread_win32.inl"
#else
#error 未知的平台
#endif

#endif