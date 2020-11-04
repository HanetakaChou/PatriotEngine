/***
*cgetws.c - buffered keyboard input
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _cgetws() - read a string directly from console
*
*******************************************************************************/
#include <cruntime.h>
#include <oscalls.h>
#include <mtdll.h>
#include <conio.h>
#include <stdlib.h>
#include <internal.h>
#include <internal_securecrt.h>

#define BUF_MAX_LEN 64

extern intptr_t _coninpfh;

/*
 * Use of the following buffer variables is primarily for syncronizing with
 * _cgets_s. _cget_s fills the MBCS buffer and if the user passes in single
 * character buffer and the unicode character is not converted to single byte
 * MBC, then _cget_s should buffer that character so that next call to
 * _cgetws_s can return the same character.
 */
wchar_t __console_wchar_buffer;
int __console_wchar_buffer_used = 0;

/***
*errno_t _cgetws_s(string, sizeInBytes, pSizeRead) - read string from console
*
*Purpose:
*       Reads a string from the console via ReadConsole on a cooked console
*       handle.
*
*Entry:
*       wchar_t *string - place to store read string
*       sizeInBytes  - max length to read
*       pSizeRead - sets the max read in this
*
*Exit:
*       returns error code or 0 if no error occurs
*
*Exceptions:
*
*******************************************************************************/

errno_t __cdecl _cgetws_s (
        wchar_t *string,
        size_t sizeInWords,
        size_t * pSizeRead
        )
{
        ULONG oldstate;
        ULONG num_read;
        errno_t err = 0;

        _VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE((string != NULL), EINVAL);
        _VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE((sizeInWords > 0), EINVAL);
        _RESET_STRING(string, sizeInWords);

        _VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE((pSizeRead != NULL), EINVAL);

        _mlock(_CONIO_LOCK);            /* lock the console */
        __TRY

            /*
             * We need to decrement sizeInWords because ReadConsole reads as
             * many characters as the parameter passed, doesn't null terminate
             */

            --sizeInWords;
            *pSizeRead = 0;

            /*
             * If the __console_whcar_buffer_used is set, then first fill the
             * buffered character and then proceed.
             */
            if (__console_wchar_buffer_used != 0 && sizeInWords > 0)
            {
                *string++ = __console_wchar_buffer;
                __console_wchar_buffer = 0;
                --sizeInWords;
                (*pSizeRead)++;
                if (__console_wchar_buffer == L'\0')
                    sizeInWords = 0;
            }

            /* if the user only asked for one character, we have now filled their request
            */
            if (sizeInWords != 0)
            {
                /*
                * _coninpfh, the handle to the console input, is created the first
                * time that either _getch() or _cgets() or _kbhit() is called.
                */

                if ( _coninpfh == -2 )
                    __initconin();

                if ( _coninpfh != -1 ) {
                    GetConsoleMode( (HANDLE)_coninpfh, &oldstate );
                    SetConsoleMode( (HANDLE)_coninpfh, ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT );

                    // First try usual way just as _cgets

                    if (ReadConsoleW((HANDLE) _coninpfh,
                                     (LPVOID) string,
                                     (DWORD) sizeInWords,
                                     &num_read,
                                     NULL))
                    {
                        /* set length of string and null terminate it */

                        if (num_read >= 2 && string[num_read - 2] == L'\r') {
                            (*pSizeRead) += num_read - 2;
                            string[num_read - 2] = L'\0';
                        } else if ( (num_read == sizeInWords) &&
                                                (string[num_read - 1] == L'\r') ) {
                            /* special case 1 - \r\n straddles the boundary */
                            (*pSizeRead) += num_read -1;
                            string[num_read - 1] = L'\0';
                        } else if ( (num_read == 1) && (string[0] == L'\n') ) {
                            /* special case 2 - read a single '\n'*/
                            string[0] = L'\0';
                            (*pSizeRead) += 0;
                        } else {
                            (*pSizeRead) += num_read;
                            string[num_read] = L'\0';
                        }
                    }

                    SetConsoleMode( (HANDLE)_coninpfh, oldstate );
                } else {
                    _dosmaperr(GetLastError());
                    err = errno;
                }
            }

        __FINALLY
            _munlock(_CONIO_LOCK);          /* unlock the console */
        __END_TRY_FINALLY

        if (err != 0)
        {
            errno = err;
        }
        return err;
}

/***
*wchar_t *_cgetws(string) - read string from console
*
*Purpose:
*       Reads a string from the console via ReadConsoleW on a cooked console
*       handle.  string[0] must contain the maximum length of the
*       string.  Returns pointer to str[2].
*
*       NOTE: _cgetsw() does NOT check the pushback character buffer (i.e.,
*       _chbuf).  Thus, _cgetws() will not return any character that is
*       pushed back by the _ungetwch() call.
*Entry:
*       char *string - place to store read string, str[0] = max length.
*
*Exit:
*       returns pointer to str[2], where the string starts.
*       returns NULL if error occurs
*
*Exceptions:
*
*******************************************************************************/

wchar_t * __cdecl _cgetws (
        wchar_t *string)
{
        size_t sizeInWords;
        size_t sizeRead;
        errno_t err;

        _VALIDATE_CLEAR_OSSERR_RETURN((string != NULL), EINVAL, NULL);

        sizeInWords = (size_t)string[0];

        err = _cgetws_s(string + 2, sizeInWords, &sizeRead);

        if(err != 0)
        {
            return NULL;
        }

       string[1] = (wchar_t)sizeRead;

       return string + 2;
}
