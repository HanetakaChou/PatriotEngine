/***
*crtlib.c - CRT DLL initialization and termination routine (Win32, Dosx32)
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This module contains initialization entry point for the CRT DLL
*       in the Win32 environment. It also contains some of the supporting
*       initialization and termination code.
*
*******************************************************************************/

#if defined (CRTDLL)

#include <cruntime.h>
#include <oscalls.h>
#include <dos.h>
#include <internal.h>
#include <malloc.h>
#include <mbctype.h>
#include <mtdll.h>
#include <process.h>
#include <rterr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <awint.h>
#include <tchar.h>
#include <time.h>
#include <io.h>
#include <dbgint.h>
#ifdef _SYSCRT
#include <ntverp.h>
#endif  /* _SYSCRT */
#include <setlocal.h>

/*
 * This header is included for _getdrives(). This function
 * is used to take dependency on msvcrt.dll
 */
#include <direct.h>

/*
 * flag set iff _CRTDLL_INIT was called with DLL_PROCESS_ATTACH
 */
static int proc_attached = 0;

/*
 * command line, environment, and a few other globals
 */
wchar_t *_wcmdln = NULL;           /* points to wide command line */
char *_acmdln = NULL;              /* points to command line */

char *_aenvptr = NULL;      /* points to environment block */
wchar_t *_wenvptr = NULL;   /* points to wide environment block */

extern int _newmode;    /* declared in <internal.h> */

int __error_mode = _OUT_TO_DEFAULT;

int __app_type = _UNKNOWN_APP;

static void __cdecl inherit(void);  /* local function */

/***
*int __[w]getmainargs - get values for args to main()
*
*Purpose:
*       This function invokes the command line parsing and copies the args
*       to main back through the passsed pointers. The reason for doing
*       this here, rather than having _CRTDLL_INIT do the work and exporting
*       the __argc and __argv, is to support the linked-in option to have
*       wildcard characters in filename arguments expanded.
*
*Entry:
*       int *pargc              - pointer to argc
*       _TCHAR ***pargv         - pointer to argv
*       _TCHAR ***penvp         - pointer to envp
*       int dowildcard          - flag (true means expand wildcards in cmd line)
*       _startupinfo * startinfo- other info to be passed to CRT DLL
*
*Exit:
*       Returns 0 on success, negative if _*setargv returns an error. Values
*       for the arguments to main() are copied through the passed pointers.
*
*******************************************************************************/


_CRTIMP int __cdecl __wgetmainargs (
        int *pargc,
        wchar_t ***pargv,
        wchar_t ***penvp,
        int dowildcard,
        _startupinfo * startinfo)
{
        int ret;


        /* set global new mode flag */
        _newmode = startinfo->newmode;

        if ( dowildcard )
            ret = __wsetargv(); /* do wildcard expansion after parsing args */
        else
            ret = _wsetargv();  /* NO wildcard expansion; just parse args */
        if (ret < 0)
#ifdef _SYSCRT
            ExitProcess(-1);      // Failed to parse the cmdline - bail
#else  /* _SYSCRT */
            return ret;
#endif  /* _SYSCRT */

        *pargc = __argc;
        *pargv = __wargv;

        /*
         * if wide environment does not already exist,
         * create it from multibyte environment
         */
        if (!_wenviron)
        {
            _wenvptr=__crtGetEnvironmentStringsW();

            if (_wsetenvp() < 0)
            {
                __mbtow_environ();
            }
        }

        *penvp = _wenviron;

        return ret;
}



_CRTIMP int __cdecl __getmainargs (
        int *pargc,
        char ***pargv,
        char ***penvp,
        int dowildcard
        ,
        _startupinfo * startinfo
        )
{
        int ret;


        /* set global new mode flag */
        _newmode = startinfo->newmode;

        if ( dowildcard )
            ret = __setargv();  /* do wildcard expansion after parsing args */
        else
            ret = _setargv();   /* NO wildcard expansion; just parse args */
        if (ret < 0)
#ifdef _SYSCRT
            ExitProcess(-1);      // Failed to parse the cmdline - bail
#else  /* _SYSCRT */
            return ret;
#endif  /* _SYSCRT */

        *pargc = __argc;
        *pargv = __argv;
        *penvp = _environ;

        return ret;
}

#pragma optimize ("", off)
void _CrtEndBoot()
{
        /* do nothing, used to mark the end of the init process */
}
#pragma optimize ("", on)

/***
*BOOL _CRTDLL_INIT(hDllHandle, dwReason, lpreserved) - C DLL initialization.
*
*Purpose:
*       This routine does the C runtime initialization.
*
*Entry:
*
*Exit:
*
*******************************************************************************/

typedef void (__stdcall *NTVERSION_INFO_FCN)(PDWORD, PDWORD, PDWORD);

static
BOOL __cdecl
__CRTDLL_INIT(
        HANDLE  hDllHandle,
        DWORD   dwReason,
        LPVOID  lpreserved
        );

BOOL WINAPI
_CRTDLL_INIT(
        HANDLE  hDllHandle,
        DWORD   dwReason,
        LPVOID  lpreserved
        )
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        /*
         * The /GS security cookie must be initialized before any exception
         * handling targetting the current image is registered.  No function
         * using exception handling can be called in the current image until
         * after __security_init_cookie has been called.
         */
        __security_init_cookie();
    }

    return __CRTDLL_INIT(hDllHandle, dwReason, lpreserved);
}

__declspec(noinline)
BOOL __cdecl
__CRTDLL_INIT(
        HANDLE  hDllHandle,
        DWORD   dwReason,
        LPVOID  lpreserved
        )
{
        if ( dwReason == DLL_PROCESS_ATTACH ) {
            if ( !_heap_init() )    /* initialize heap */
                /*
                 * The heap cannot be initialized, return failure to the
                 * loader.
                 */
                return FALSE;

            if(!_mtinit())          /* initialize multi-thread */
            {
                /*
                 * If the DLL load is going to fail, we must clean up
                 * all resources that have already been allocated.
                 */
                _heap_term();       /* heap is now invalid! */

                return FALSE;       /* fail DLL load on failure */
            }

            if (_ioinit() < 0) {    /* inherit file info */
                /* Clean up already-allocated resources */
                /* free TLS index, call _mtdeletelocks() */
                _mtterm();

                _heap_term();       /* heap is now invalid! */

                return FALSE;       /* fail DLL load on failure */
            }

            _aenvptr = (char *)__crtGetEnvironmentStringsA();

            _acmdln = GetCommandLineA();
            _wcmdln = GetCommandLineW();

#ifdef _MBCS
            /*
             * Initialize multibyte ctype table. Always done since it is
             * needed for processing the environment strings.
             */
            __initmbctable();
#endif  /* _MBCS */

            /*
             * For CRT DLL, since we don't know the type (wide or multibyte)
             * of the program, we create only the multibyte type since that
             * is by far the most likely case. Wide environment will be created
             * on demand as usual.
             */

            if (_setenvp() < 0 ||   /* get environ info */
                _cinit(FALSE) != 0)  /* do C data initialize */
            {
                _ioterm();          /* shut down lowio */
                _mtterm();          /* free TLS index, call _mtdeletelocks() */
                _heap_term();       /* heap is now invalid! */
                return FALSE;       /* fail DLL load on failure */
            }


            /*
             * Increment flag indicating process attach notification
             * has been received.
             */
            proc_attached++;

        }
        else if ( dwReason == DLL_PROCESS_DETACH ) {
            /*
             * if a client process is detaching, make sure minimal
             * runtime termination is performed and clean up our
             * 'locks' (i.e., delete critical sections).
             */
            if ( proc_attached > 0 ) {
                proc_attached--;
                __try {

                    /*
                     * Any basic clean-up done here may also need
                     * to be done below if Process Attach is partly
                     * processed and then a failure is encountered.
                     */

                    if ( _C_Termination_Done == FALSE )
                        _cexit();

                    __crtdll_callstaticterminators();

#ifdef _DEBUG
                    /* Dump all memory leaks */
                    if (_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & _CRTDBG_LEAK_CHECK_DF)
                    {
                        _CrtSetDumpClient(NULL);
                        __freeCrtMemory();
                        _CrtDumpMemoryLeaks();
                    }
#endif  /* _DEBUG */

                    /*
                     * What remains is to clean up the system resources we have
                     * used (handles, critical sections, memory,...,etc.). This
                     * needs to be done if the whole process is NOT terminating.
                     */
                    if ( lpreserved == NULL )
                    {
                        /*
                         * The process is NOT terminating so we must clean up...
                         */
                        _ioterm();

                        /* free TLS index, call _mtdeletelocks() */
                        _mtterm();

                        /* This should be the last thing the C run-time does */
                        _heap_term();   /* heap is now invalid! */
                    }

                }
                __finally {
                    /* we shouldn't really have to care about this, because
                       letting an exception escape from DllMain(DLL_PROCESS_DETACH) should
                       result in process termination. Unfortunately, Windows up to Win7 as of now
                       just silently swallows the exception.

                       I have considered all kinds of tricks, but decided to leave it up to OS
                       folks to fix this.

                       For the time being just remove our FLS callback during phase 2 unwind
                       that would otherwise be left pointing to unmapped address space.
                       */
                     if ( lpreserved == NULL && __flsindex != FLS_OUT_OF_INDEXES )
                         _mtterm();
                }
            }
            else
                /* no prior process attach, just return */
                return FALSE;
        }

        else if ( dwReason == DLL_THREAD_ATTACH )
        {
            _ptiddata ptd;

            /* Initialize FlsGetValue function pointer */
            __set_flsgetvalue();

            if ( (ptd = FLS_GETVALUE(__flsindex)) == NULL)
            {
                if ( ((ptd = _calloc_crt(1, sizeof(struct _tiddata))) != NULL))
                {
                    if (FLS_SETVALUE(__flsindex, (LPVOID)ptd) ) {
                        /*
                        * Initialize of per-thread data
                        */
                        _initptd(ptd, NULL);

                        ptd->_tid = GetCurrentThreadId();
                        ptd->_thandle = (uintptr_t)(-1);
                    } else
                    {
                        _free_crt(ptd);
                        return FALSE;
                    }
                } else
                {
                    return FALSE;
                }
            }
        }
        else if ( dwReason == DLL_THREAD_DETACH )
        {
            _freeptd(NULL);     /* free up per-thread CRT data */
        }

        /* See above declaration of _CrtEndBoot */
        _CrtEndBoot();

        return TRUE;
}

/*
 * Earlier the below exports were only for X86, but with invent of /clr:pure,
 * they need to be defined for all the platform.
 */

/*
 * Functions to access user-visible, per-process variables
 */

/*
 * Macro to construct the name of the access function from the variable
 * name.
 */
#define AFNAME(var) __p_ ## var

/*
 * Macro to construct the access function's return value from the variable
 * name.
 */
#define AFRET(var)  &var

/*
 ***
 ***  Template
 ***

_CRTIMP __cdecl
AFNAME() (void)
{
        return AFRET();
}

 ***
 ***
 ***
 */

#ifdef _DEBUG

_CRTIMP long *
AFNAME(_crtAssertBusy) (void)
{
        return AFRET(_crtAssertBusy);
}

_CRTIMP long *
AFNAME(_crtBreakAlloc) (void)
{
        return AFRET(_crtBreakAlloc);
}

_CRTIMP int *
AFNAME(_crtDbgFlag) (void)
{
        return AFRET(_crtDbgFlag);
}

#endif  /* _DEBUG */

_CRTIMP char ** __cdecl
AFNAME(_acmdln) (void)
{
        return AFRET(_acmdln);
}

_CRTIMP wchar_t ** __cdecl
AFNAME(_wcmdln) (void)
{
        return AFRET(_wcmdln);
}

_CRTIMP int * __cdecl
AFNAME(__argc) (void)
{
        return AFRET(__argc);
}

_CRTIMP char *** __cdecl
AFNAME(__argv) (void)
{
        return AFRET(__argv);
}

_CRTIMP wchar_t *** __cdecl
AFNAME(__wargv) (void)
{
        return AFRET(__wargv);
}

_CRTIMP int * __cdecl
AFNAME(_commode) (void)
{
        return AFRET(_commode);
}

_CRTIMP int * __cdecl
AFNAME(_daylight) (void)
{
_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
        return AFRET(_daylight);
_END_SECURE_CRT_DEPRECATION_DISABLE
}

_CRTIMP long * __cdecl
AFNAME(_dstbias) (void)
{
_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
        return AFRET(_dstbias);
_END_SECURE_CRT_DEPRECATION_DISABLE
}

_CRTIMP char *** __cdecl
AFNAME(_environ) (void)
{
        return AFRET(_environ);
}

_CRTIMP wchar_t *** __cdecl
AFNAME(_wenviron) (void)
{
        return AFRET(_wenviron);
}

_CRTIMP int * __cdecl
AFNAME(_fmode) (void)
{
_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
        return AFRET(_fmode);
_END_SECURE_CRT_DEPRECATION_DISABLE
}

_CRTIMP char *** __cdecl
AFNAME(__initenv) (void)
{
        return AFRET(__initenv);
}

_CRTIMP wchar_t *** __cdecl
AFNAME(__winitenv) (void)
{
        return AFRET(__winitenv);
}

_CRTIMP FILE *
AFNAME(_iob) (void)
{
        return &_iob[0];
}

_CRTIMP unsigned char * __cdecl
AFNAME(_mbctype) (void)
{
        return &_mbctype[0];
}

_CRTIMP unsigned char * __cdecl
AFNAME(_mbcasemap) (void)
{
        return &_mbcasemap[0];
}

_CRTIMP int * __cdecl
AFNAME(__mb_cur_max) (void)
{
        _ptiddata ptd = _getptd();
        pthreadlocinfo ptloci = ptd->ptlocinfo;

        __UPDATE_LOCALE(ptd, ptloci);
        return AFRET(__MB_CUR_MAX(ptloci));
}

_CRTIMP const unsigned short ** __cdecl
AFNAME(_pctype) (void)
{
        _ptiddata ptd = _getptd();
        pthreadlocinfo ptloci = ptd->ptlocinfo;

        __UPDATE_LOCALE(ptd, ptloci);
        return AFRET(ptloci->pctype);
}

_CRTIMP const unsigned short ** __cdecl
AFNAME(_pwctype) (void)
{
        return AFRET(_pwctype);
}

_CRTIMP char **  __cdecl
AFNAME(_pgmptr) (void)
{
_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
        return AFRET(_pgmptr);
_END_SECURE_CRT_DEPRECATION_DISABLE
}

_CRTIMP wchar_t ** __cdecl
AFNAME(_wpgmptr) (void)
{
_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
        return AFRET(_wpgmptr);
_END_SECURE_CRT_DEPRECATION_DISABLE
}

_CRTIMP long * __cdecl
AFNAME(_timezone) (void)
{
_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
        return AFRET(_timezone);
_END_SECURE_CRT_DEPRECATION_DISABLE
}

_CRTIMP char ** __cdecl
AFNAME(_tzname) (void)
{
_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
        return &_tzname[0];
_END_SECURE_CRT_DEPRECATION_DISABLE
}

#endif  /* defined (CRTDLL) */
