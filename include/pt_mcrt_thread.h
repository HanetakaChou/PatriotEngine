//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef _PT_MCRT_THREAD_H_
#define _PT_MCRT_THREAD_H_ 1

#include "pt_common.h"
#include "pt_mcrt_common.h"

#include <stddef.h>
#include <stdint.h>

#if defined(PT_POSIX)
#include <pthread.h>
typedef pthread_t mcrt_native_thread_id;
typedef pthread_key_t mcrt_native_tls_key;
typedef pthread_mutex_t mcrt_mutex_t;
typedef pthread_cond_t mcrt_cond_t;
#elif defined(PT_WIN32)
//https://docs.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers#faster-builds-with-smaller-header-files
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOCOMM
#include <Windows.h>
typedef HANDLE mcrt_native_thread_id;
typedef DWORD mcrt_native_tls_key;
typedef CRITICAL_SECTION mcrt_mutex_t;
typedef CONDITION_VARIABLE mcrt_cond_t;
#else
#error Unknown Platform
#endif

#if defined(PT_POSIX)
inline bool mcrt_native_thread_create(mcrt_native_thread_id *tid, void *(*func)(void *), void *arg);
#elif defined(PT_WIN32)
inline bool mcrt_native_thread_create(mcrt_native_thread_id *tid, unsigned(__stdcall *func)(void *), void *arg);
#else
#error Unknown Platform
#endif
inline void mcrt_native_thread_set_name(mcrt_native_thread_id tid, char const *name);
inline bool mcrt_native_thread_join(mcrt_native_thread_id tid);

inline mcrt_native_thread_id mcrt_native_thread_id_get(void);

inline bool mcrt_native_thread_id_equal(mcrt_native_thread_id tid_left, mcrt_native_thread_id tid_right);

#if defined(PT_POSIX)
inline bool mcrt_native_tls_alloc(mcrt_native_tls_key *key, void(*destructor)(void *));
#elif defined(PT_WIN32)
inline bool mcrt_native_tls_alloc(mcrt_native_tls_key *key, void(NTAPI *destructor)(void *));
#else
#error Unknown Platform
#endif
inline void mcrt_native_tls_free(mcrt_native_tls_key key);
inline bool mcrt_native_tls_set_value(mcrt_native_tls_key key, void *value);
inline void *mcrt_native_tls_get_value(mcrt_native_tls_key key);

inline void mcrt_os_yield();

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

typedef struct mcrt_event_t
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
    friend void mcrt_os_event_init(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event, bool manual_reset, bool initial_state);
    friend void mcrt_os_event_destroy(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event);
    friend void mcrt_os_event_set(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event);
    friend void mcrt_os_event_reset(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event);
    friend int mcrt_os_event_wait_one(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event);
    friend int mcrt_os_event_timedwait_one(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event, uint32_t timeout_ms);
    friend int mcrt_os_event_wait_multiple(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t **events, int nevents, bool waitall);
    friend int mcrt_os_event_timedwait_multiple(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t **events, int nevents, bool waitall, uint32_t timeout_ms);
#endif
} mcrt_event_t;

inline void mcrt_os_event_init(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event, bool manual_reset, bool initial_state);
inline void mcrt_os_event_destroy(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event);
inline void mcrt_os_event_set(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event);
inline void mcrt_os_event_reset(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event);
inline int mcrt_os_event_wait_one(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event);
inline int mcrt_os_event_timedwait_one(mcrt_cond_t *condition, mcrt_mutex_t *mutex, mcrt_event_t *event, uint32_t timeout_ms);
// Returns:
//  If waitall is true:
//       0 - The return value indicates that the state of all specified events is signaled.
//      -1 - The function has failed.
//  If waitall is false:
//       0 to (nevents-1) - The return value indicates the events array index of the event that satisfied the wait.
//                          If more than one event became signaled during the call, this is the array index of
//                          the signaled event with the smallest index value of all the signaled events.
//      -1                - The function has failed.
inline int mcrt_os_event_wait_multiple(mcrt_mutex_t *mutex, mcrt_cond_t *condition, mcrt_event_t **events, int nevents, bool waitall);
// Returns:
//  If waitall is true:
//       0 - The return value indicates that the state of all specified events is signaled.
//      -1 - The time-out interval elapsed and the conditions specified by the waitall parameter are not satisfied or the function has failed.
//  If waitall is false:
//       0 to (nevents-1) - The return value indicates the events array index of the event that satisfied the wait.
//                          If more than one event became signaled during the call, this is the array index of
//                          the signaled event with the smallest index value of all the signaled events.
//      -1                - The time-out interval elapsed and the conditions specified by the waitall parameter are not satisfied or the function has failed.
inline int mcrt_os_event_timedwait_multiple(mcrt_mutex_t *mutex, mcrt_cond_t *condition, mcrt_event_t **events, int nevents, bool waitall, uint32_t timeout_ms);

#if defined PT_POSIX
#include "pt_mcrt_thread_posix.inl"
#elif defined PT_WIN32
#include "pt_mcrt_thread_win32.inl"
#else
#error Unknown Platform
#endif

#include "pt_mcrt_thread.inl"

#endif