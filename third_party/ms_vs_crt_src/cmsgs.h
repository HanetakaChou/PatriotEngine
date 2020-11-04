/***
*cmsgs.h - runtime errors
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       The file defines, in one place, all error message strings used within
*       the C run-time library.
*
*       [Internal]
*
****/

#pragma once

#ifndef _INC_CMSGS
#define _INC_CMSGS

#ifndef _CRTBLD
/*
 * This is an internal C runtime header file. It is used when building
 * the C runtimes only. It is not to be used as a public header file.
 */
#error ERROR: Use of C runtime library internal header file.
#endif  /* _CRTBLD */

/*
 * runtime error and termination messages
 */

#define EOL L"\r\n"

#define _RT_STACK_TXT      L"R6000" EOL L"- stack overflow" EOL

#define _RT_FLOAT_TXT      L"R6002" EOL L"- floating point support not loaded" EOL

#define _RT_INTDIV_TXT     L"R6003" EOL L"- integer divide by 0" EOL

#define _RT_SPACEARG_TXT   L"R6008" EOL L"- not enough space for arguments" EOL

#define _RT_SPACEENV_TXT   L"R6009" EOL L"- not enough space for environment" EOL

#define _RT_ABORT_TXT      L"R6010" EOL L"- abort() has been called" EOL

#define _RT_THREAD_TXT     L"R6016" EOL L"- not enough space for thread data" EOL

#define _RT_LOCK_TXT       L"R6017" EOL L"- unexpected multithread lock error" EOL

#define _RT_HEAP_TXT       L"R6018" EOL L"- unexpected heap error" EOL

#define _RT_OPENCON_TXT    L"R6019" EOL L"- unable to open console device" EOL

#define _RT_NONCONT_TXT    L"R6022" EOL L"- non-continuable exception" EOL

#define _RT_INVALDISP_TXT  L"R6023" EOL L"- invalid exception disposition" EOL

/*
 * _RT_ONEXIT_TXT is specific to Win32 and Dosx32 platforms
 */
#define _RT_ONEXIT_TXT     L"R6024" EOL L"- not enough space for _onexit/atexit table" EOL

#define _RT_PUREVIRT_TXT   L"R6025" EOL L"- pure virtual function call" EOL

#define _RT_STDIOINIT_TXT  L"R6026" EOL L"- not enough space for stdio initialization" EOL

#define _RT_LOWIOINIT_TXT  L"R6027" EOL L"- not enough space for lowio initialization" EOL

#define _RT_HEAPINIT_TXT   L"R6028" EOL L"- unable to initialize heap" EOL

#define _RT_CRT_NOTINIT_TXT L"R6030" EOL L"- CRT not initialized" EOL

#define _RT_CRT_INIT_CONFLICT_TXT L"R6031" EOL L"- Attempt to initialize the CRT more than once.\n" \
    L"This indicates a bug in your application." EOL

#define _RT_LOCALE_TXT L"R6032" EOL L"- not enough space for locale information" EOL

#define _RT_CRT_INIT_MANAGED_CONFLICT_TXT L"R6033" EOL L"- Attempt to use MSIL code from this assembly during native code initialization\n" \
    L"This indicates a bug in your application. It is most likely the result of calling an MSIL-compiled (/clr) function from a native constructor or from DllMain." EOL

/*
 * _RT_COOKIE_INIT_TXT is used directly as the argument to FatalAppExitW, since it is
 * used in __security_init_cookie when __crtMessageBox may not be available yet.
 */
#define _RT_COOKIE_INIT_TXT L"Microsoft Visual C++ Runtime Library, Error R6035 - " \
                            L"A module in this application is initializing the module's " \
                            L"global security cookie while a function relying on that " \
                            L"security cookie is active.  Call __security_init_cookie earlier."

/*
 * _RT_DOMAIN_TXT, _RT_SING_TXT and _RT_TLOSS_TXT are used by the floating
 * point library.
 */
#define _RT_DOMAIN_TXT     L"DOMAIN error" EOL

#define _RT_SING_TXT       L"SING error" EOL

#define _RT_TLOSS_TXT      L"TLOSS error" EOL


#define _RT_CRNL_TXT       EOL

#define _RT_BANNER_TXT     L"runtime error "


#endif  /* _INC_CMSGS */
