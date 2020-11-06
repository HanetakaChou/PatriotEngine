#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
//#include <intrin.h>

inline int32_t mcrt_atomic_cas_i32(int32_t volatile *dest, int32_t exch, int32_t comp)
{
    return InterlockedCompareExchange(reinterpret_cast<LONG volatile *>(dest), static_cast<LONG>(exch), static_cast<LONG>(comp));
}

inline int64_t mcrt_atomic_cas_i64(int64_t volatile *dest, int64_t exch, int64_t comp)
{
    return InterlockedCompareExchange64(reinterpret_cast<LONGLONG volatile *>(dest), static_cast<LONGLONG>(exch), static_cast<LONGLONG>(comp));
}

inline void *mcrt_atomic_cas_ptr(void *volatile *dest, void *exch, void *comp)
{
    return InterlockedCompareExchangePointer(dest, exch, comp);
}

inline int32_t mcrt_atomic_xchg_i32(int32_t volatile *dest, int32_t exch)
{
    return InterlockedExchange(reinterpret_cast<LONG volatile *>(dest), static_cast<LONG>(exch));
}

inline int64_t mcrt_atomic_xchg_i64(int64_t volatile *dest, int64_t exch)
{
    return InterlockedExchange64(reinterpret_cast<LONGLONG volatile *>(dest), static_cast<LONGLONG>(exch));
}

inline void *mcrt_atomic_xchg_ptr(void *volatile *dest, void *exch)
{
    return InterlockedExchange64(dest, exch);
}

inline int32_t mcrt_atomic_add_i32(int32_t volatile *dest, int32_t add)
{
    return InterlockedAdd(reinterpret_cast<LONG volatile *>(dest), static_cast<LONGLONG>(add));
}

inline int64_t mcrt_atomic_add_i64(int64_t volatile *dest, int64_t add)
{
    return InterlockedAdd64(reinterpret_cast<LONGLONG volatile *>(dest), static_cast<LONGLONG>(add));
}

inline int32_t mcrt_atomic_inc_i32(int32_t volatile *dest)
{
    return InterlockedIncrement(reinterpret_cast<LONG volatile *>(dest));
}

inline int64_t mcrt_atomic_inc_i64(int64_t volatile *dest)
{
    return InterlockedIncrement64(reinterpret_cast<LONGLONG volatile *>(dest));
}

inline int32_t mcrt_atomic_dec_i32(int32_t volatile *dest)
{
    return InterlockedDecrement(reinterpret_cast<LONG volatile *>(dest));
}

inline int64_t mcrt_atomic_dec_i64(int64_t volatile *dest)
{
    return InterlockedDecrement64(reinterpret_cast<LONGLONG volatile *>(dest));
}

inline int32_t mcrt_atomic_fetch_add_i32(int32_t volatile *dest, int32_t add)
{
    return InterlockedExchangeAdd(reinterpret_cast<LONG volatile *>(dest), static_cast<LONGLONG>(add));
}

inline int64_t mcrt_atomic_fetch_add_i64(int64_t volatile *dest, int64_t add)
{
    return InterlockedExchangeAdd64(reinterpret_cast<LONGLONG volatile *>(dest), static_cast<LONGLONG>(add));
}