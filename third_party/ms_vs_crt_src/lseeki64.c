/***
*lseeki64.c - change file position
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _lseeki64() - move the file pointer
*
*******************************************************************************/

#include <cruntime.h>
#include <oscalls.h>
#include <mtdll.h>
#include <io.h>
#include <internal.h>
#include <stdlib.h>
#include <errno.h>
#include <msdos.h>
#include <stdio.h>

/*
 * Convenient union for accessing the upper and lower 32-bits of a 64-bit
 * integer.
 */
typedef union doubleint {
        __int64 bigint;
        struct {
            unsigned long lowerhalf;
            long upperhalf;
        } twoints;
} DINT;


/***
*__int64 _lseeki64( fh, pos, mthd ) - move the file pointer
*
*Purpose:
*       Moves the file pointer associated with fh to a new position. The new
*       position is pos bytes (pos may be negative) away from the origin
*       specified by mthd.
*
*       If mthd == SEEK_SET, the origin in the beginning of file
*       If mthd == SEEK_CUR, the origin is the current file pointer position
*       If mthd == SEEK_END, the origin is the end of the file
*
*       Multi-thread:
*       _lseeki64()    = locks/unlocks the file
*       _lseeki64_nolock() = does NOT lock/unlock the file (it is assumed that
*                        the caller has the aquired the file lock, if needed).
*
*Entry:
*       int     fh   - file handle to move file pointer on
*       __int64 pos  - position to move to, relative to origin
*       int     mthd - specifies the origin pos is relative to (see above)
*
*Exit:
*       returns the offset, in bytes, of the new position from the beginning
*       of the file.
*       returns -1i64 (and sets errno) if fails.
*       Note that seeking beyond the end of the file is not an error.
*       (although seeking before the beginning is.)
*
*Exceptions:
*
*******************************************************************************/

__int64 __cdecl _lseeki64 (
        int fh,
        __int64 pos,
        int mthd
        )
{
        __int64 r = -1i64;

        /* validate fh */
        _CHECK_FH_CLEAR_OSSERR_RETURN( fh, EBADF, -1i64 );
        _VALIDATE_CLEAR_OSSERR_RETURN((fh >= 0 && (unsigned)fh < (unsigned)_nhandle), EBADF, -1i64);
        _VALIDATE_CLEAR_OSSERR_RETURN((_osfile(fh) & FOPEN), EBADF, -1i64);

        _lock_fh(fh);                   /* lock file handle */
        __try {
                /* make sure the file is open (after locking) */
                if ( _osfile(fh) & FOPEN )
                        r = _lseeki64_nolock( fh, pos, mthd );  /* seek */
                else {
                        errno = EBADF;
                        _doserrno = 0;  /* not OS error */
                        r =  -1i64;
                        _ASSERTE(("Invalid file descriptor. File possibly closed by a different thread",0));
                }
        }
        __finally {
                _unlock_fh(fh);         /* unlock file handle */
        }

        return( r );
}


/***
*__int64 _lseeki64_nolock( fh, pos, mthd ) - move the file pointer
*
*Purpose:
*       Non-locking version of _lseeki64 for internal use only.
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

__int64 __cdecl _lseeki64_nolock (
        int fh,
        __int64 pos,
        int mthd
        )
{
        DINT newpos;                    /* new file position */
        unsigned long err;          /* error code from API call */
        HANDLE osHandle;        /* o.s. handle value */

        newpos.bigint = pos;

        /* tell OS to seek */

#if SEEK_SET != FILE_BEGIN || SEEK_CUR != FILE_CURRENT || SEEK_END != FILE_END
    #error Xenix and Win32 seek constants not compatible
#endif  /* SEEK_SET != FILE_BEGIN || SEEK_CUR != FILE_CURRENT || SEEK_END != FILE_END */

        if ((osHandle = (HANDLE)_get_osfhandle(fh)) == (HANDLE)-1)
        {
            errno = EBADF;
            _ASSERTE(("Invalid file descriptor. File possibly closed by a different thread",0));
            return( -1i64 );
        }

        if ( ((newpos.twoints.lowerhalf =
               SetFilePointer( osHandle,
                               newpos.twoints.lowerhalf,
                               &(newpos.twoints.upperhalf),
                               mthd )) == -1L) &&
             ((err = GetLastError()) != NO_ERROR) )
        {
                _dosmaperr( err );
                return( -1i64 );
        }

        _osfile(fh) &= ~FEOFLAG;        /* clear the ctrl-z flag on the file */
        return( newpos.bigint );        /* return */
}
