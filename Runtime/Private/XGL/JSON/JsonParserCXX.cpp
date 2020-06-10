#define _CRT_SECURE_NO_WARNINGS 1

#include "JsonParserCXX.h"

#include <sstream>
using PT_McRT_CXX_StringStream = std::basic_stringstream<char, std::char_traits<char>, PT_McRT_CXXAllocator<char>>;

class JsonParser
{
public:
	PT_XGL_PFN_StreamReader_read m_pfn_streamreader_read;
	PT_XGL_Json_Value *m_root_value;
	PT_McRT_CXX_StringStream m_msg_error;
};

extern "C" void *JsonParser_Json_Object_Create(class JsonParser *pUserData)
{
	return JsonParser_Json_Object_Impl::Create();
}

extern "C" bool JsonParser_Json_Object_AddMember(class JsonParser *pUserData, void *_jsonobjectvoid, void *_stdstringvoid, void *_jsonvaluevoid)
{
	JsonParser_Json_Object_Impl *_jsonobject = static_cast<JsonParser_Json_Object_Impl *>(_jsonobjectvoid);
	PT_McRT_CXX_String *_stdstring = static_cast<PT_McRT_CXX_String *>(_stdstringvoid);
	PT_XGL_Json_Value *_jsonvalue = static_cast<PT_XGL_Json_Value *>(_jsonvaluevoid);

	assert(static_cast<PT_XGL_Json_Object *>(_jsonobject)->ToObject() != NULL);
	return _jsonobject->AddMember(_stdstring->c_str(), _jsonvalue);
}

extern "C" void *JsonParser_Json_Array_Create(class JsonParser *pUserData)
{
	return JsonParser_Json_Array_Impl::Create();
}

extern "C" void JsonParser_Json_Array_AddElement(class JsonParser *pUserData, void *_jsonarrayvoid, void *_jsonvaluevoid)
{
	JsonParser_Json_Array_Impl *_jsonarray = static_cast<JsonParser_Json_Array_Impl *>(_jsonarrayvoid);
	PT_XGL_Json_Value *_jsonvalue = static_cast<PT_XGL_Json_Value *>(_jsonvaluevoid);

	assert(static_cast<PT_XGL_Json_Array*>(_jsonarray)->ToArray() != NULL);
	_jsonarray->AddElement(_jsonvalue);
}

extern "C" void *JsonParser_Json_String_Create(class JsonParser *pUserData, void *_stdstringvoid)
{
	PT_McRT_CXX_String *_stdstring = static_cast<PT_McRT_CXX_String *>(_stdstringvoid);

	return JsonParser_Json_String_Impl::Create(std::move(*_stdstring));
}

extern "C" void *JsonParser_Json_NumberInt_Create(class JsonParser *pUserData, long _valueint)
{
	return JsonParser_Json_NumberInt_Impl::Create(_valueint);
}

extern "C" void *JsonParser_Json_NumberFloat_Create(class JsonParser *pUserData, float _valuefloat)
{
	return JsonParser_Json_NumberFloat_Impl::Create(_valuefloat);
}

extern "C" void *JsonParser_Json_True_Create(class JsonParser *pUserData)
{
	return JsonParser_Json_True_Impl::Create();
}

extern "C" void *JsonParser_Json_False_Create(class JsonParser *pUserData)
{
	return JsonParser_Json_False_Impl::Create();
}

extern "C" void *JsonParser_Json_Null_Create(class JsonParser *pUserData)
{
	return JsonParser_Json_Null_Impl::Create();
}

extern "C" void JsonParser_Json_Root_Value(class JsonParser *pUserData, void *_jsonvaluevoid)
{
	PT_XGL_Json_Value *_jsonvalue = static_cast<PT_XGL_Json_Value *>(_jsonvaluevoid);

	pUserData->m_root_value = _jsonvalue;
	return;
}

extern "C" void *JsonParser_Std_String_Create(class JsonParser *pUserData, char *_valuestring_text, int _valuestring_leng)
{
	return new(PT_McRT_Aligned_Malloc(sizeof(PT_McRT_CXX_String), alignof(PT_McRT_CXX_String)))PT_McRT_CXX_String(_valuestring_text, _valuestring_leng);
}

extern "C" void JsonParser_Std_String_Dispose(class JsonParser *pUserData, void *_stdstringvoid)
{
	PT_McRT_CXX_String *_stdstring = static_cast<PT_McRT_CXX_String *>(_stdstringvoid);

	assert(_stdstring->empty());
	_stdstring->~PT_McRT_CXX_String();
	PT_McRT_Aligned_Free(_stdstring);
	return;
}

extern "C" int json_lllex_init_extra(class JsonParser *user_defined, struct llscan_t **scanner);
extern "C" void json_llset_in(PT_XGL_StreamReaderRef user_inputstream, struct llscan_t *scanner);
extern "C" int json_lllex_destroy(struct llscan_t *scanner);
extern "C" int json_yyparse(class JsonParser *pUserData, struct llscan_t *pScanner);

PT_XGL_Json_Value *PT_XGL_JsonParser_Parse(PT_XGL_StreamReaderRef _streamreader, PT_XGL_PFN_StreamReader_read _pfn_streamreader_read, char *error_out, size_t error_maxsize)
{
	JsonParser _jsonparser;

	struct llscan_t *scanner;
	json_lllex_init_extra(&_jsonparser, &scanner);

	PT_XGL_StreamReaderRef _InitialStreamReader = _streamreader;
	json_llset_in(_InitialStreamReader, scanner);

	int _res_parse = json_yyparse(&_jsonparser, scanner);

	json_lllex_destroy(scanner);

	if ((error_out != NULL) && (!_jsonparser.m_msg_error.str().empty()))
	{
		strncpy(error_out, _jsonparser.m_msg_error.str().c_str(), error_maxsize);
	}

	return (_res_parse == 0) ? _jsonparser.m_root_value : NULL;
}

extern "C" ptrdiff_t JsonParser_Callback_StreamReader_Read(class JsonParser *pUserData, PT_XGL_StreamReaderRef _pUserStream, void *buf, size_t count)
{
	return pUserData->m_pfn_streamreader_read(_pUserStream, buf, count);
}

extern "C" int JsonParser_Callback_Wrap(class JsonParser *pUserData)
{
	return 1;
}

extern "C" void JsonParser_Error(class JsonParser *pUserData, int line, int column, char const *s)
{
	char msg_yy_error[4096];
	snprintf(msg_yy_error, 4096, "(%d,%d): %s",  line, column, s);
	pUserData->m_msg_error << msg_yy_error << std::endl;
}

extern "C" void *JsonParser_Malloc(class JsonParser *pUserData, size_t size)
{
	return PT_McRT_Malloc(size);
}

extern "C" void *JsonParser_Realloc(class JsonParser *pUserData, void *ptr, size_t size)
{

	return PT_McRT_Realloc(ptr, size);
}

extern "C" void JsonParser_Free(class JsonParser *pUserData, void *ptr)
{
	return PT_McRT_Free(ptr);
}