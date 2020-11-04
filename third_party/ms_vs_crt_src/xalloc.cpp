// MINITHREADS
#include <stdexcept>
#include <windows.h>

namespace stdext {
    namespace threads {
        _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtx_new(void *& _Ptr) {
            _Ptr = new CRITICAL_SECTION;

            InitializeCriticalSection(static_cast<CRITICAL_SECTION *>(_Ptr));
        }

        _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtx_delete(void * _Ptr) {
             DeleteCriticalSection(static_cast<CRITICAL_SECTION *>(_Ptr));

             delete static_cast<CRITICAL_SECTION *>(_Ptr);
        }

        _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtx_lock(void * _Ptr) {
            EnterCriticalSection(static_cast<CRITICAL_SECTION *>(_Ptr));
        }

        _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtx_unlock(void * _Ptr) {
            LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(_Ptr));
        }
    } // namespace threads
} // namespace stdext

/*
 * Copyright (c) 1995-2009 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.20:0009 */
