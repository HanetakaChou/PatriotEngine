/***
*stat64.c - get file status
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _stat64() - get file status
*
*******************************************************************************/

#include <cruntime.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <msdos.h>
#include <oscalls.h>
#include <string.h>
#include <internal.h>
#include <stdlib.h>
#include <direct.h>
#include <mbstring.h>
#include <tchar.h>
#include <io.h>
#include <share.h>
#include <fcntl.h>

#ifndef _USE_INT64
#define _USE_INT64 1
#endif  /* _USE_INT64 */

#define ISSLASH(a)  ((a) == _T('\\') || (a) == _T('/'))


/*
 * Number of 100 nanosecond units from 1/1/1601 to 1/1/1970
 */
#define EPOCH_BIAS  116444736000000000i64


#ifdef _UNICODE
#define __tdtoxmode __wdtoxmode
#else  /* _UNICODE */
#define __tdtoxmode __dtoxmode
#endif  /* _UNICODE */


/*
 * Local routine which returns true if the argument is a UNC name
 * specifying the root name of a share, such as '\\server\share\'.
 */

static int IsRootUNCName(const _TSCHAR *path);
static _TSCHAR * _tfullpath_helper(_TSCHAR * ,const _TSCHAR *,size_t , _TSCHAR **);

extern unsigned short __cdecl __tdtoxmode(int, const _TSCHAR *);


/***
*int _stat64(name, buf) - get file status info
*
*Purpose:
*       _stat64 obtains information about the file and stores it in the
*       structure pointed to by buf.
*
*       Note: Unlike _stat, _stat64 uses the UTC time values returned in
*       WIN32_FIND_DATA struct. This means the time values will always be
*       correct on NTFS, but may be wrong on FAT file systems for file times
*       whose DST state is different from the current DST state (this an NT
*       bug).
*
*Entry:
*       _TSCHAR *name -    pathname of given file
*       struct _stat *buffer - pointer to buffer to store info in
*
*Exit:
*       fills in structure pointed to by buffer
*       returns 0 if successful
*       returns -1 and sets errno if unsuccessful
*
*Exceptions:
*
*******************************************************************************/

#if _USE_INT64
 #define _STAT_FUNC _tstat64
 #define _STAT_STRUCT _stat64
 #define _FSTAT_FUNC _fstat64
#else  /* _USE_INT64 */
 #define _STAT_FUNC _tstat64i32
 #define _STAT_STRUCT _stat64i32
 #define _FSTAT_FUNC _fstat64i32
#endif  /* _USE_INT64 */


int __cdecl _STAT_FUNC (
    REG1 const _TSCHAR *name,
    REG2 struct _STAT_STRUCT *buf
    )
{
    _TSCHAR *  path;
    _TSCHAR    pathbuf[ _MAX_PATH ];
    int drive;          /* A: = 1, B: = 2, etc. */
    HANDLE findhandle;
    WIN32_FIND_DATA findbuf;
    int retval = 0;

    _VALIDATE_CLEAR_OSSERR_RETURN( (name != NULL), EINVAL, -1);
    _VALIDATE_CLEAR_OSSERR_RETURN( (buf != NULL), EINVAL, -1);

    /* Don't allow wildcards to be interpreted by system */

#ifdef _UNICODE
    if (wcspbrk(name, L"?*"))
#else  /* _UNICODE */
    if (_mbspbrk(name, "?*"))
#endif  /* _UNICODE */
    {
        errno = ENOENT;
        _doserrno = E_nofile;
        return(-1);
    }

    /* Try to get disk from name.  If none, get current disk.  */

    if (name[1] == _T(':'))
    {
        if ( *name && !name[2] )
        {
            errno = ENOENT;             /* return an error if name is   */
            _doserrno = E_nofile;       /* just drive letter then colon */
            return( -1 );
        }
        drive = _totlower(*name) - _T('a') + 1;
    }
    else
    {
        drive = _getdrive();
    }

    /* Call Find Match File */
    findhandle = FindFirstFileEx(name, FindExInfoStandard, &findbuf, FindExSearchNameMatch, NULL, 0);
    if ( findhandle == INVALID_HANDLE_VALUE )
    {
        _TSCHAR * pBuf = NULL;

#ifdef _UNICODE
        if ( !( wcspbrk(name, L"./\\") &&
#else  /* _UNICODE */
        if ( !( _mbspbrk(name, "./\\") &&
#endif  /* _UNICODE */
             (path = _tfullpath_helper( pathbuf, name, _MAX_PATH, &pBuf )) &&
             /* root dir. ('C:\') or UNC root dir. ('\\server\share\') */
             ((_tcslen( path ) == 3) || IsRootUNCName(path)) &&
             (GetDriveType( path ) > 1) ) )
        {
            if(pBuf)
            {
                free(pBuf);
            }

            errno = ENOENT;
            _doserrno = E_nofile;
            return( -1 );
        }

        if(pBuf)
        {
            free(pBuf);
        }

        /*
         * Root directories (such as C:\ or \\server\share\ are fabricated.
         */

        findbuf.dwFileAttributes = A_D;
        findbuf.nFileSizeHigh = 0;
        findbuf.nFileSizeLow = 0;
        findbuf.cFileName[0] = _T('\0');

        buf->st_mtime = __loctotime64_t(1980,1,1,0,0,0, -1);
        buf->st_atime = buf->st_mtime;
        buf->st_ctime = buf->st_mtime;
    }
    else if ( (findbuf.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
              (findbuf.dwReserved0 == IO_REPARSE_TAG_SYMLINK) )
    {
        /* if the file is a symbolic link, then use fstat to fill the info in the _stat struct */
        int fd = -1;
        errno_t e;

        e = _tsopen_s(&fd, name, _O_RDONLY, _SH_DENYNO, 0 /* ignored */);
        if (e != 0 || fd == -1)
        {
            errno = ENOENT;
            _doserrno = E_nofile;
            return -1;
        }

        retval = _FSTAT_FUNC(fd, buf);
        _close(fd);

        return retval;
    }
    else
    {
        SYSTEMTIME SystemTime;
        FILETIME LocalFTime;

        if ( findbuf.ftLastWriteTime.dwLowDateTime ||
             findbuf.ftLastWriteTime.dwHighDateTime )
        {
            if ( !FileTimeToLocalFileTime( &findbuf.ftLastWriteTime,
                                           &LocalFTime )            ||
                 !FileTimeToSystemTime( &LocalFTime, &SystemTime ) )
            {
                _dosmaperr( GetLastError() );
                FindClose( findhandle );
                return( -1 );
            }

            buf->st_mtime = __loctotime64_t( SystemTime.wYear,
                                             SystemTime.wMonth,
                                             SystemTime.wDay,
                                             SystemTime.wHour,
                                             SystemTime.wMinute,
                                             SystemTime.wSecond,
                                             -1 );
        }
        else
        {
            buf->st_mtime = 0;
        }

        if ( findbuf.ftLastAccessTime.dwLowDateTime ||
             findbuf.ftLastAccessTime.dwHighDateTime )
        {
            if ( !FileTimeToLocalFileTime( &findbuf.ftLastAccessTime,
                                           &LocalFTime )                ||
                 !FileTimeToSystemTime( &LocalFTime, &SystemTime ) )
            {
                _dosmaperr( GetLastError() );
                FindClose( findhandle );
                return( -1 );
            }

            buf->st_atime = __loctotime64_t( SystemTime.wYear,
                                           SystemTime.wMonth,
                                           SystemTime.wDay,
                                           SystemTime.wHour,
                                           SystemTime.wMinute,
                                           SystemTime.wSecond,
                                           -1 );
        }
        else
        {
            buf->st_atime = buf->st_mtime;
        }

        if ( findbuf.ftCreationTime.dwLowDateTime ||
             findbuf.ftCreationTime.dwHighDateTime )
        {
            if ( !FileTimeToLocalFileTime( &findbuf.ftCreationTime,
                                           &LocalFTime )                ||
                 !FileTimeToSystemTime( &LocalFTime, &SystemTime ) )
            {
                _dosmaperr( GetLastError() );
                FindClose( findhandle );
                return( -1 );
            }

            buf->st_ctime = __loctotime64_t( SystemTime.wYear,
                                           SystemTime.wMonth,
                                           SystemTime.wDay,
                                           SystemTime.wHour,
                                           SystemTime.wMinute,
                                           SystemTime.wSecond,
                                           -1 );
        }
        else
        {
            buf->st_ctime = buf->st_mtime;
        }

        FindClose(findhandle);
    }

    /* Fill in buf */

    buf->st_mode = __tdtoxmode(findbuf.dwFileAttributes, name);
    buf->st_nlink = 1;
#if _USE_INT64
    buf->st_size = ((__int64)(findbuf.nFileSizeHigh)) * (0x100000000i64) +
                    (__int64)(findbuf.nFileSizeLow);
#else  /* _USE_INT64 */
    buf->st_size = findbuf.nFileSizeLow;
    if (findbuf.nFileSizeHigh != 0)
    {
        buf->st_size = 0;
        retval = -1;
    }
#endif  /* _USE_INT64 */

    /* now set the common fields */

    buf->st_uid = buf->st_gid = buf->st_ino = 0;

    buf->st_rdev = buf->st_dev = (_dev_t)(drive - 1); /* A=0, B=1, etc. */

    return retval;
}


/*
 * IsRootUNCName - returns TRUE if the argument is a UNC name specifying
 *      a root share.  That is, if it is of the form \\server\share\.
 *      This routine will also return true if the argument is of the
 *      form \\server\share (no trailing slash) but Win32 currently
 *      does not like that form.
 *
 *      Forward slashes ('/') may be used instead of backslashes ('\').
 */

static int IsRootUNCName(const _TSCHAR *path)
{
    /*
     * If a root UNC name, path will start with 2 (but not 3) slashes
     */

    if ( ( _tcslen ( path ) >= 5 ) /* minimum string is "//x/y" */
         && ISSLASH(path[0]) && ISSLASH(path[1])
         && !ISSLASH(path[2]))
    {
        const _TSCHAR * p = path + 2 ;

        /*
         * find the slash between the server name and share name
         */
        while ( * ++ p )
        {
            if ( ISSLASH(*p) )
            {
                break;
            }
        }

        if ( *p && p[1] )
        {
            /*
             * is there a further slash?
             */
            while ( * ++ p )
            {
                if ( ISSLASH(*p) )
                {
                    break;
                }
            }

            /*
             * just final slash (or no final slash)
             */
            if ( !*p || !p[1])
            {
                return 1;
            }
        }
    }

    return 0 ;
}

static _TSCHAR * __cdecl _tfullpath_helper(_TSCHAR * buf,const _TSCHAR *path,size_t sz, _TSCHAR ** pBuf)
{
    _TSCHAR * ret;
    errno_t save_errno = errno;

    errno = 0;
    ret = _tfullpath(buf, path, sz);
    if (ret)
    {
        errno = save_errno;
        return ret;
    }

    if (errno != ERANGE)
    {
        return NULL;
    }
    errno = save_errno;

    *pBuf = _tfullpath(NULL, path, 0);

    return *pBuf;
}
