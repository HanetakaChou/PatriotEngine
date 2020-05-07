#include "JsonFrontend.h"
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <iostream>
#include <new>
#include <assert.h>

void PT_XGL_JsonFrontend_Run(char const *pInitialFileName, struct PT_XGL_StreamReader_Callbacks const *pStreamReader)
{
	
}

extern "C" int json_lllex_init_extra(JsonFrontend *user_defined, struct llscan_t **scanner);
extern "C" void json_llset_in(PT_XGL_StreamReaderRef user_inputstream, struct llscan_t *scanner);
extern "C" int json_lllex_destroy(struct llscan_t *scanner);
extern "C" int json_yyparse(JsonFrontend *pUserData, struct llscan_t *pScanner);

JsonFrontend::JsonFrontend(
	PT_XGL_StreamReaderRef (*pFn_StreamReader_Create)(char const *pFileName),
	ptrdiff_t (*pFn_StreamReader_read)(PT_XGL_StreamReaderRef pStreamReaderRef, void *buf, size_t count),
	void (*pFn_StreamReader_Dispose)(PT_XGL_StreamReaderRef pStreamReaderRef))
	: m_pFn_StreamReader_Create(pFn_StreamReader_Create),
	  m_pFn_StreamReader_read(pFn_StreamReader_read),
	  m_pFn_StreamReader_Dispose(pFn_StreamReader_Dispose)
{
}

void JsonFrontend::Run(char const *pInitialFileName)
{
	struct llscan_t *scanner;
	json_lllex_init_extra(this, &scanner);

	PT_XGL_StreamReaderRef _InitialStreamReader = m_pFn_StreamReader_Create(pInitialFileName);
	m_streamreader_filename_stack.push_back(pInitialFileName);

	json_llset_in(_InitialStreamReader, scanner);

	json_yyparse(this, scanner);

	m_pFn_InputStreamDispose(_InitialStreamReader);

	json_lllex_destroy(scanner);
}

ptrdiff_t JsonFrontend::Callback_StreamReader_Read(PT_XGL_StreamReaderRef _pStreamReaderRef, void *buf, size_t count)
{
	return m_pFn_StreamReader_read(_pStreamReaderRef, buf, count);
}

int JsonFrontend::Callback_Wrap()
{
	m_streamreader_filename_stack.pop_back();
	return ((m_streamreader_filename_stack.empty()) ? 1 : 0);
}

std::string *JsonFrontend::Callback_CreateString(char const *s)
{
	return new (Callback_Malloc(sizeof(std::string))) std::string(s);
}

std::string *JsonFrontend::Callback_StringAppend(std::string *l, std::string *r)
{
	std::string &ret = ((*l).append((*r)));
#ifndef NDEBUG
	(*r).clear();
#endif
	return &ret;
}

void JsonFrontend::Callback_Error(int line, int column, char const *s)
{
	char msg_yy_error[4096];
	snprintf(msg_yy_error, 4096, "%s(%d,%d): %s", m_streamreader_filename_stack.back().c_str(), line, column, s);
	std::cout << msg_yy_error << std::endl;
}

void *JsonFrontend::Callback_Malloc(size_t size)
{
	return malloc(size);
}

void *JsonFrontend::Callback_Realloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}

void JsonFrontend::Callback_Free(void *ptr)
{
	return free(ptr);
}
