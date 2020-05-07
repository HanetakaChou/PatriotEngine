#ifndef _PT_XGL_STREAMREADER_H_
#define _PT_XGL_STREAMREADER_H_ 1

#include <stddef.h>
#include <stdint.h>

typedef struct PT_XGL_OpaqueStreamReader *PT_XGL_StreamReaderRef;

enum
{
    PT_XGL_STREAMREADER_SEEK_SET = 0,
    PT_XGL_STREAMREADER_SEEK_CUR = 1,
    PT_XGL_STREAMREADER_SEEK_END = 2
};

typedef PT_XGL_StreamReaderRef (*PT_XGL_PFN_StreamReader_Create)(char const *pFileName);
typedef ptrdiff_t (*PT_XGL_PFN_StreamReader_read)(PT_XGL_StreamReaderRef _streamreader, void *buf, size_t count);
typedef int64_t (*PT_XGL_PFN_StreamReader_seek)(PT_XGL_StreamReaderRef _streamreader, int64_t offset, int whence);
typedef void (*PT_XGL_PFN_StreamReader_Dispose)(PT_XGL_StreamReaderRef _streamreader);

#endif