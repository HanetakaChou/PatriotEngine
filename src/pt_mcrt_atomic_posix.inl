
// https://gcc.gnu.org/onlinedocs/gcc-4.6.4/gcc/Atomic-Builtins.html#Atomic-Builtins

inline int32_t mcrt_atomic_cas_i32(int32_t volatile *dest, int32_t exch, int32_t comp)
{
    return __sync_val_compare_and_swap(dest, comp, exch);
}

inline int64_t mcrt_atomic_cas_i64(int64_t volatile *dest, int64_t exch, int64_t comp)
{
    return __sync_val_compare_and_swap(dest, comp, exch);
}

inline void *mcrt_atomic_cas_ptr(void *volatile *dest, void *exch, void *comp)
{
    return __sync_val_compare_and_swap(dest, comp, exch);
}

inline int32_t mcrt_atomic_xchg_i32(int32_t volatile *dest, int32_t exch)
{
    // https://gcc.gnu.org/onlinedocs/gcc-4.6.4/gcc/Atomic-Builtins.html#Atomic-Builtins
    // Many targets have only minimal support for such locks, and do not support a full exchange operation.
    // In this case, a target may support reduced functionality here by which the only valid value to store is the immediate constant 1.
    // The exact value actually stored in *ptr is implementation defined.

    // return __sync_lock_test_and_set(dest, exch);
    // __sync_lock_release

    int32_t old_dest;
    do
    {
        old_dest = (*dest);
    } while (__sync_val_compare_and_swap(dest, old_dest, exch) != old_dest);
    return old_dest;
}

inline int64_t mcrt_atomic_xchg_i64(int64_t volatile *dest, int64_t exch)
{
    int64_t old_dest;
    do
    {
        old_dest = (*dest);
    } while (__sync_val_compare_and_swap(dest, old_dest, exch) != old_dest);
    return old_dest;
}

inline void *mcrt_atomic_xchg_ptr(void *volatile *dest, void *exch)
{
    void *old_dest;
    do
    {
        old_dest = (*dest);
    } while (__sync_val_compare_and_swap(dest, old_dest, exch) != old_dest);
    return old_dest;
}

inline int32_t mcrt_atomic_add_i32(int32_t volatile *dest, int32_t add)
{
    return __sync_add_and_fetch(dest, add);
}

inline int64_t mcrt_atomic_add_i64(int64_t volatile *dest, int64_t add)
{
    return __sync_add_and_fetch(dest, add);
}

inline int32_t mcrt_atomic_inc_i32(int32_t volatile *dest)
{
    return __sync_add_and_fetch(dest, static_cast<int32_t>(1));
}

inline int64_t mcrt_atomic_inc_i64(int64_t volatile *dest)
{
    return __sync_add_and_fetch(dest, static_cast<int64_t>(1));
}

inline int32_t mcrt_atomic_dec_i32(int32_t volatile *dest)
{
    return __sync_sub_and_fetch(dest, static_cast<int32_t>(1));
}

inline int64_t mcrt_atomic_dec_i64(int64_t volatile *dest)
{
    return __sync_sub_and_fetch(dest, static_cast<int64_t>(1));
}

inline int32_t mcrt_atomic_fetch_add_i32(int32_t volatile *dest, int32_t add)
{
    return __sync_fetch_and_add(dest, add);
}

inline int64_t mcrt_atomic_fetch_add_i64(int64_t volatile *dest, int64_t add)
{
    return __sync_fetch_and_add(dest, add);
}