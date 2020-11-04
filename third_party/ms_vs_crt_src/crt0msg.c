/***
*crt0msg.c - startup error messages
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Prints out banner for runtime error messages.
*
*******************************************************************************/

#include <cruntime.h>
#include <internal.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rterr.h>
#include <cmsgs.h>
#include <awint.h>
#include <windows.h>
#include <dbgint.h>

/* struct used to lookup and access runtime error messages */

struct rterrmsgs {
        int rterrno;        /* error number */
        const wchar_t *rterrtxt;     /* text of error message */
};

/* runtime error messages */

static const struct rterrmsgs rterrs[] = {

        /* 2 */
        { _RT_FLOAT, _RT_FLOAT_TXT },

        /* 8 */
        { _RT_SPACEARG, _RT_SPACEARG_TXT },

        /* 9 */
        { _RT_SPACEENV, _RT_SPACEENV_TXT },

        /* 10 */
        { _RT_ABORT, _RT_ABORT_TXT },

        /* 16 */
        { _RT_THREAD, _RT_THREAD_TXT },

        /* 17 */
        { _RT_LOCK, _RT_LOCK_TXT },

        /* 18 */
        { _RT_HEAP, _RT_HEAP_TXT },

        /* 19 */
        { _RT_OPENCON, _RT_OPENCON_TXT },

        /* 22 */
        /* { _RT_NONCONT, _RT_NONCONT_TXT }, */

        /* 23 */
        /* { _RT_INVALDISP, _RT_INVALDISP_TXT }, */

        /* 24 */
        { _RT_ONEXIT, _RT_ONEXIT_TXT },

        /* 25 */
        { _RT_PUREVIRT, _RT_PUREVIRT_TXT },

        /* 26 */
        { _RT_STDIOINIT, _RT_STDIOINIT_TXT },

        /* 27 */
        { _RT_LOWIOINIT, _RT_LOWIOINIT_TXT },

        /* 28 */
        { _RT_HEAPINIT, _RT_HEAPINIT_TXT },

        ///* 29 */
        //{ _RT_BADCLRVERSION, _RT_BADCLRVERSION_TXT },

        /* 30 */
        { _RT_CRT_NOTINIT, _RT_CRT_NOTINIT_TXT },

        /* 31 */
        { _RT_CRT_INIT_CONFLICT, _RT_CRT_INIT_CONFLICT_TXT},

        /* 32 */
        { _RT_LOCALE, _RT_LOCALE_TXT},

        /* 33 */
        { _RT_CRT_INIT_MANAGED_CONFLICT, _RT_CRT_INIT_MANAGED_CONFLICT_TXT},

        ///* 35 - not for _NMSG_WRITE, text passed directly to FatalAppExit */
        //{ _RT_COOKIE_INIT, _RT_COOKIE_INIT_TXT},

        /* 120 */
        { _RT_DOMAIN, _RT_DOMAIN_TXT },

        /* 121 */
        { _RT_SING, _RT_SING_TXT },

        /* 122 */
        { _RT_TLOSS, _RT_TLOSS_TXT },

        /* 252 */
        { _RT_CRNL, _RT_CRNL_TXT },

        /* 255 */
        { _RT_BANNER, _RT_BANNER_TXT }

};

/***
*_FF_MSGBANNER - writes out first part of run-time error messages
*
*Purpose:
*       This routine writes "\r\nrun-time error " to standard error.
*
*Entry:
*       No arguments.
*
*Exit:
*       Nothing returned.
*
*Exceptions:
*       None handled.
*
*******************************************************************************/

void __cdecl _FF_MSGBANNER (
        void
        )
{

        if ( (_set_error_mode(_REPORT_ERRMODE) == _OUT_TO_STDERR) ||
             ((_set_error_mode(_REPORT_ERRMODE) == _OUT_TO_DEFAULT) &&
              (__app_type == _CONSOLE_APP)) )
        {
            _NMSG_WRITE(_RT_CRNL);  /* new line to begin error message */
            _NMSG_WRITE(_RT_BANNER); /* run-time error message banner */
        }
}

/***
*_GET_RTERRMSG(message) - returns ptr to error text for given runtime error
*
*Purpose:
*       This routine returns the message associated with rterrnum
*
*Entry:
*       int rterrnum - runtime error number
*
*Exit:
*       no return value
*
*Exceptions:
*       none
*
*******************************************************************************/

const wchar_t * __cdecl _GET_RTERRMSG (
        int rterrnum
        )
{
        int i;

        for ( i = 0 ; i < _countof(rterrs); i++ )
            if ( rterrnum == rterrs[i].rterrno )
                return rterrs[i].rterrtxt;

        return NULL;
}

/***
*_NMSG_WRITE(message) - write a given message to handle 2 (stderr)
*
*Purpose:
*       This routine writes the message associated with rterrnum
*       to stderr.
*
*Entry:
*       int rterrnum - runtime error number
*
*Exit:
*       no return value
*
*Exceptions:
*       none
*
*******************************************************************************/

void __cdecl _NMSG_WRITE (
        int rterrnum
        )
{
        const wchar_t * const error_text = _GET_RTERRMSG(rterrnum);

        if (error_text)
        {
            int msgshown = 0;
#ifdef _DEBUG
            /*
             * Report error.
             *
             * If _CRT_ERROR has _CRTDBG_REPORT_WNDW on, and user chooses
             * "Retry", call the debugger.
             *
             * Otherwise, continue execution.
             *
             */

            if (rterrnum != _RT_CRNL && rterrnum != _RT_BANNER && rterrnum != _RT_CRT_NOTINIT)
            {
                switch (_CrtDbgReportW(_CRT_ERROR, NULL, 0, NULL, error_text))
                {
                case 1: _CrtDbgBreak(); msgshown = 1; break;
                case 0: msgshown = 1; break;
                }
            }
#endif  /* _DEBUG */
            if (!msgshown)
            {
                if ( (_set_error_mode(_REPORT_ERRMODE) == _OUT_TO_STDERR) ||
                     ((_set_error_mode(_REPORT_ERRMODE) == _OUT_TO_DEFAULT) &&
                      (__app_type == _CONSOLE_APP)) )
                {
                    HANDLE hStdErr = GetStdHandle(STD_ERROR_HANDLE);
                    if (hStdErr && hStdErr != INVALID_HANDLE_VALUE)
                    {
                        /*
                         * This purposely truncates wchar_t to char.
                         * error_text doesn't contain the application's name.
                         * Currently, error_text contains only ASCII,
                         * so this truncation preserves VC9's behavior.
                         * If error_text is ever localized, this will have to be changed.
                         *
                         */

                        char buffer[500];
                        int i;
                        DWORD bytes_written;

                        for (i = 0; i < _countof(buffer); ++i)
                        {
                            buffer[i] = (char) error_text[i];

                            if (error_text[i] == L'\0')
                            {
                                break;
                            }
                        }

                        buffer[_countof(buffer) - 1] = '\0';

                        WriteFile( hStdErr,
                                      buffer,
                                      (unsigned long)strlen(buffer),
                                      &bytes_written,
                                      NULL );
                    }
                }
                else if (rterrnum != _RT_CRNL)
                {
                    #define MSGTEXTPREFIX L"Runtime Error!\n\nProgram: "
                    static wchar_t outmsg[sizeof(MSGTEXTPREFIX) / sizeof(wchar_t) + _MAX_PATH + 2 + 500];
                        // runtime error msg + progname + 2 newline + runtime error text.
                    wchar_t * progname = &outmsg[sizeof(MSGTEXTPREFIX) / sizeof(wchar_t) - 1];
                    size_t progname_size = _countof(outmsg) - (progname - outmsg);
                    wchar_t * pch = progname;

                    _ERRCHECK(wcscpy_s(outmsg, _countof(outmsg), MSGTEXTPREFIX));

                    progname[MAX_PATH] = L'\0';
                    if (!GetModuleFileNameW(NULL, progname, MAX_PATH))
                        _ERRCHECK(wcscpy_s(progname, progname_size, L"<program name unknown>"));

                    #define MAXLINELEN 60
                    if (wcslen(pch) + 1 > MAXLINELEN)
                    {
                        pch += wcslen(progname) + 1 - MAXLINELEN;
                        _ERRCHECK(wcsncpy_s(pch, progname_size - (pch - progname), L"...", 3));
                    }

                    _ERRCHECK(wcscat_s(outmsg, _countof(outmsg), L"\n\n"));
                    _ERRCHECK(wcscat_s(outmsg, _countof(outmsg), error_text));

                    __crtMessageBoxW(outmsg,
                            L"Microsoft Visual C++ Runtime Library",
                            MB_OK|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);
                }
            }
        }
}
