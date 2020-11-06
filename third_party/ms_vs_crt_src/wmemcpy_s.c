/***
*wmemcpy_s.c - contains memcpy_s routine
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       wmemcpy_s() copies a source memory buffer to a destination buffer in units of wchar_ts.
*       Overlapping buffers are not treated specially, so propagation may occur.
*
*******************************************************************************/

#include <cruntime.h>
#include <wchar.h>
#include <internal.h>

/***
*wmemcpy_s - Copy source buffer to destination buffer
*
*Purpose:
*       wmemcpy_s() copies a source memory buffer to a destination memory buffer.
*       This routine does NOT recognize overlapping buffers, and thus can lead
*       to propagation.
*
*       For cases where propagation must be avoided, wmemmove_s() must be used.
*
*Entry:
*       wchar_t *dst = pointer to destination buffer
*       size_t sizeInWchar_ts = size in wchar_ts of the destination buffer
*       const wchar_t *src = pointer to source buffer
*       size_t count = number of wchar_ts to copy
*
*Exit:
*       Returns 0 if everything is ok, else return the error code.
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*       On error, the error code is returned and the destination buffer is zeroed.
*
*******************************************************************************/

errno_t __cdecl wmemcpy_s(
    wchar_t * dst,
    size_t sizeInWchar_ts,
    const wchar_t * src,
    size_t count
)
{
    if (count == 0)
    {
        /* nothing to do */
        return 0;
    }

    /* validation section */
    _VALIDATE_RETURN_ERRCODE(dst != NULL, EINVAL);
    if (src == NULL || sizeInWchar_ts < count)
    {
        /* zeroes the destination buffer */
        wmemset(dst, 0, sizeInWchar_ts);

        _VALIDATE_RETURN_ERRCODE(src != NULL, EINVAL);
        _VALIDATE_RETURN_ERRCODE(sizeInWchar_ts >= count, ERANGE);
        /* useless, but prefast is confused */
        return EINVAL;
    }

    wmemcpy(dst, src, count);
    return 0;
}
