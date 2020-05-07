#ifndef _PT_XGL_JSON_JSONFRONTEND_H_
#define _PT_XGL_JSON_JSONFRONTEND_H_ 1

#include <stddef.h>
#include <stdlib.h>

#include "../PT_XGL_JsonFrontend.h"

class JsonFrontend
{
	PT_XGL_StreamReaderRef (*m_pFn_StreamReader_Create)(char const *pFileName);
	ptrdiff_t (*m_pFn_StreamReader_read)(PT_XGL_StreamReaderRef pStreamReaderRef, void *buf, size_t count);
	void (*m_pFn_StreamReader_Dispose)(PT_XGL_StreamReaderRef pStreamReaderRef);
	
	//PTS_CPP_Vector<PTS_CPP_String> m_streamreader_filename_stack;
public:
	JsonFrontend(
		PT_XGL_StreamReaderRef (*pFn_StreamReader_Create)(char const *pFileName), //For process #include
		ptrdiff_t (*pFn_StreamReader_read)(PT_XGL_StreamReaderRef pStreamReaderRef, void *buf, size_t count),
		void (*pFn_StreamReader_Dispose)(PT_XGL_StreamReaderRef pStreamReaderRef));

	void Run(char const *pInitialFileName);

	ptrdiff_t Callback_StreamReader_Read(PT_XGL_StreamReaderRef pStreamReaderRef, void *buf, size_t count);
	int Callback_Wrap();
	void Callback_Error(int line, int column, char const *s);
	void *Callback_Malloc(size_t size);
	void *Callback_Realloc(void *ptr, size_t size);
	void Callback_Free(void *ptr);
};

#endif