#include "JsonFrontend.h"
#include "JsonScannerParser.h"
#include "../PT_XGL_JsonFrontend.h"

extern "C" void *JsonFrontend_CreateJsonValue_FromString(JsonFrontend *pUserData, char *_valuestring_text, int _valuestring_leng)
{
	PT_XGL_Json_Value *_jasonvalue = new(JsonFrontend_Malloc(pUserData, sizeof(PT_XGL_Json_Value)))PT_XGL_Json_Value(PT_McRT_CXX_String(_valuestring_text, static_cast<size_t>(_valuestring_leng)));
	return _jasonvalue;
}

extern "C" void *JsonFrontend_CreateJsonValue_FromInt(JsonFrontend *pUserData, long _valueint)
{
	PT_XGL_Json_Value *_jasonvalue = new(JsonFrontend_Malloc(pUserData, sizeof(PT_XGL_Json_Value)))PT_XGL_Json_Value(_valueint);
	return _jasonvalue;
}

extern "C" void *JsonFrontend_CreateJsonValue_FromFloat(JsonFrontend *pUserData, float _valuefloat)
{
	PT_XGL_Json_Value *_jasonvalue = new(JsonFrontend_Malloc(pUserData, sizeof(PT_XGL_Json_Value)))PT_XGL_Json_Value(_valuefloat);
	return _jasonvalue;
}

extern "C" void *JsonFrontend_CreateJsonValue_True(JsonFrontend *pUserData)
{
	PT_XGL_Json_Value *_jasonvalue = new(JsonFrontend_Malloc(pUserData, sizeof(PT_XGL_Json_Value)))PT_XGL_Json_Value();
	_jasonvalue->SetTrue();
	return _jasonvalue;
}

extern "C" void *JsonFrontend_CreateJsonValue_False(JsonFrontend *pUserData)
{
	PT_XGL_Json_Value *_jasonvalue = new(JsonFrontend_Malloc(pUserData, sizeof(PT_XGL_Json_Value)))PT_XGL_Json_Value();
	_jasonvalue->SetFalse();
	return _jasonvalue;
}
extern "C" void *JsonFrontend_CreateJsonValue_Null(JsonFrontend *pUserData)
{
	PT_XGL_Json_Value *_jasonvalue = new(JsonFrontend_Malloc(pUserData, sizeof(PT_XGL_Json_Value)))PT_XGL_Json_Value();
	_jasonvalue->SetNull();
	return _jasonvalue;
}

extern "C" void JsonFrontend_DisposeJsonValue(JsonFrontend *pUserData, void *_jsonvaluevoid)
{
	PT_XGL_Json_Value *_jsonvalue = static_cast<PT_XGL_Json_Value *>(_jsonvaluevoid);

	_jsonvalue->~PT_XGL_Json_Value();
	JsonFrontend_Free(pUserData, _jsonvalue);
	return;
}

extern "C" void *JsonFrontend_CreateStdVector_FromJsonValue(JsonFrontend *pUserData, void *_jsonvaluevoid)
{
	PT_XGL_Json_Value *_jsonvalue = static_cast<PT_XGL_Json_Value *>(_jsonvaluevoid);

	PT_McRT_CXX_Vector<PT_XGL_Json_Value> *_stdvector = new(JsonFrontend_Malloc(pUserData, sizeof(PT_McRT_CXX_Vector<PT_XGL_Json_Value>)))PT_McRT_CXX_Vector<PT_XGL_Json_Value>();
	_stdvector->push_back(std::move(*_jsonvalue));
	return _stdvector;
}

extern "C" void JsonFrontend_StdVector_PushJsonValue(JsonFrontend *pUserData, void *_stdvectorvoid, void *_jsonvaluevoid)
{
	PT_XGL_Json_Value *_jsonvalue = static_cast<PT_XGL_Json_Value *>(_jsonvaluevoid);

	PT_McRT_CXX_Vector<PT_XGL_Json_Value> *_stdvector = static_cast<PT_McRT_CXX_Vector<PT_XGL_Json_Value> *>(_stdvectorvoid);
	_stdvector->push_back(std::move(*_jsonvalue));
	return;
}

extern "C" void JsonFrontend_DisposeStdVector(JsonFrontend *pUserData, void *_stdvectorvoid)
{
	PT_McRT_CXX_Vector<PT_XGL_Json_Value> *_stdvector = static_cast<PT_McRT_CXX_Vector<PT_XGL_Json_Value> *>(_stdvectorvoid);
	
	_stdvector->~PT_McRT_CXX_Vector<PT_XGL_Json_Value>();
	JsonFrontend_Free(pUserData, _stdvector);
	return;
}

extern "C" void *JsonFrontend_CreateJsonArray_FromStdVector(JsonFrontend *pUserData, void *_stdvectorvoid)
{
	PT_McRT_CXX_Vector<PT_XGL_Json_Value> *_stdvector = static_cast<PT_McRT_CXX_Vector<PT_XGL_Json_Value> *>(_stdvectorvoid);

	PT_XGL_Json_Value *_jsonvalue = new(JsonFrontend_Malloc(pUserData, sizeof(PT_XGL_Json_Value)))PT_XGL_Json_Value(std::move(*_stdvector));
	return _jsonvalue;
}

extern "C" void *JsonFrontend_CreateJsonArray_FromEmpty(JsonFrontend *pUserData)
{
	PT_XGL_Json_Value *_jsonvalue = new(JsonFrontend_Malloc(pUserData, sizeof(PT_XGL_Json_Value)))PT_XGL_Json_Value(PT_McRT_CXX_Vector<PT_XGL_Json_Value>());
	return _jsonvalue;
}
extern "C" void *JsonFrontend_CreateStdPair_FromStringAndJsonValue(JsonFrontend *pUserData, char *_valuestring_text, int _valuestring_leng, void *_jsonvaluevoid)
{
	PT_XGL_Json_Value *_jsonvalue = static_cast<PT_XGL_Json_Value *>(_jsonvaluevoid);

	PT_McRT_CXX_String _first(_valuestring_text, static_cast<size_t>(_valuestring_leng));
	std::pair<PT_McRT_CXX_String const, PT_XGL_Json_Value> *_stdpair = new(JsonFrontend_Malloc(pUserData, sizeof(std::pair<PT_McRT_CXX_String const, PT_XGL_Json_Value>)))
		std::pair<PT_McRT_CXX_String const, PT_XGL_Json_Value>();
	return _stdpair;
}

extern "C" void JsonFrontend_DisposeStdPair(JsonFrontend *pUserData, void *_stdpairvoid)
{
	std::pair<PT_McRT_CXX_String, PT_XGL_Json_Value> *_stdpair = static_cast<std::pair<PT_McRT_CXX_String, PT_XGL_Json_Value> *>(_stdpairvoid);
	
	_stdpair->~pair<PT_McRT_CXX_String, PT_XGL_Json_Value>();
	JsonFrontend_Free(pUserData, _stdpair);
	return;
}

extern "C" void *JsonFrontend_CreateStdUnorderedMap_FromStdPair(JsonFrontend *pUserData, void *_stdpairvoid)
{
	std::pair<PT_McRT_CXX_String const, PT_XGL_Json_Value> *_stdpair = static_cast<std::pair<PT_McRT_CXX_String const, PT_XGL_Json_Value> *>(_stdpairvoid);

	PT_McRT_CXX_Unordered_Map<PT_McRT_CXX_String, PT_XGL_Json_Value> *_stdunorderedmap = new(JsonFrontend_Malloc(pUserData, sizeof(PT_McRT_CXX_Unordered_Map<PT_McRT_CXX_String, PT_XGL_Json_Value>)))PT_McRT_CXX_Unordered_Map<PT_McRT_CXX_String, PT_XGL_Json_Value>();
	_stdunorderedmap->insert(*_stdpair);
	return _stdunorderedmap;
}

extern "C" void JsonFrontend_StdUnorderedMap_InsertStdPair(JsonFrontend *pUserData, void *_stdunorderedmap, void *_stdpair)
{
}

extern "C" void *JsonFrontend_DisposeStdUnorderedMap(JsonFrontend *pUserData, void *_stdunorderedmap)
{
}

extern "C" void *JsonFrontend_CreateJsonObject_FromStdUnorderedMap(JsonFrontend *pUserData, void *_stdunorderedmap)
{
}

extern "C" void *JsonFrontend_CreateJsonObject_FromEmpty(JsonFrontend *pUserData)
{
}

extern "C" void *JsonFrontend_Malloc(JsonFrontend *pUserData, size_t size)
{

}

extern "C" void *JsonFrontend_Realloc(JsonFrontend *pUserData, void *ptr, size_t size)
{

}

extern "C" void JsonFrontend_Free(JsonFrontend *pUserData, void *ptr)
{

}