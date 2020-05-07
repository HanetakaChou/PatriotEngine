#ifndef _PT_XGL_STREAMREADERCALLBACKS_H_
#define _PT_XGL_STREAMREADERCALLBACKS_H_ 1

#include <stddef.h>
#include <stdint.h>

typedef struct PT_XGL_OpaqueStreamReader *PT_XGL_StreamReaderRef;

enum
{
    PT_XGL_STREAMREADER_SEEK_SET = 0,
    PT_XGL_STREAMREADER_SEEK_CUR = 1,
    PT_XGL_STREAMREADER_SEEK_END = 2
};

struct PT_XGL_StreamReaderCallbacks
{
    PT_XGL_StreamReaderRef (*pFn_StreamReader_Create)(char const *pFileName);
    ptrdiff_t (*pFn_StreamReader_read)(PT_XGL_StreamReaderRef pStreamReaderRef, void *buf, size_t count);
    int64_t (*pFn_StreamReader_seek)(PT_XGL_StreamReaderRef pStreamReaderRef, int64_t offset, int whence);
    void (*pFn_StreamReader_Dispose)(PT_XGL_StreamReaderRef pStreamReaderRef);
};

#endif