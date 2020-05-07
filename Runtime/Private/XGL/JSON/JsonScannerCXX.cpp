#include "JsonFrontend.h"
#include <exception>
extern "C" JsonFrontend *json_llget_extra(struct llscan_t *yyscanner);

extern "C" int json_ll_stream_read(JsonFrontend *pUserData, PT_XGL_StreamReaderRef _pStreamReaderRef, void *buf, size_t size)
{
	ptrdiff_t _res = pUserData->Callback_StreamReader_Read(_InputStreamRef, buf, size);
	return static_cast<int>(_res);
}

//https://westes.github.io/flex/manual/Generated-Scanner.html#Generated-Scanner
extern "C" int json_llwrap(struct llscan_t *scanner)
{
	return json_llget_extra(scanner)->Callback_Wrap();
}

#if defined(_MSC_VER)
static inline __declspec(noreturn) void json_ll_fatal_error_cxx(int line, int column, char const *msg, JsonFrontend *pUserData);
extern "C" void json_ll_fatal_error(int line, int column, char const *msg, JsonFrontend *pUserData)
{
	return json_ll_fatal_error_cxx(line, column, msg, pUserData);
}
static inline __declspec(noreturn) void json_ll_fatal_error_cxx(int line, int column, char const *msg, JsonFrontend *pUserData)
#elif defined(__GNUC__)
extern "C" __attribute__((__noreturn__)) void json_ll_fatal_error(int line, int column, char const *msg, JsonFrontend *pUserData)
#else
#error Unknown Compiler //未知的编译器
#endif
{
	pUserData->Callback_Error(line, column, msg);

	//RaiseException
	throw std::exception();
}

extern "C" void *json_llalloc(size_t size, struct llscan_t *scanner)
{
	return json_llget_extra(scanner)->Callback_Malloc(size);
}

extern "C" void *json_llrealloc(void *ptr, size_t size, struct llscan_t *scanner)
{
	return json_llget_extra(scanner)->Callback_Realloc(ptr, size);
}

extern "C" void json_llfree(void *ptr, struct llscan_t *scanner)
{
	return json_llget_extra(scanner)->Callback_Free(ptr);
}