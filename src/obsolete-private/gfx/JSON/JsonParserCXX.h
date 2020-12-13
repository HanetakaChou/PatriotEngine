#ifndef _PT_XGL_JSON_JSONPARSERCXX_H_
#define _PT_XGL_JSON_JSONPARSERCXX_H_ 1

#include "../PT_XGL_StreamReader.h"
#include "../PT_XGL_JsonParser.h"
#include "JsonLexYacc.h"

#include <memory>
#include <new>
#include <assert.h>

#include "../../../Public/McRT/PT_McRT_CXXAllocator.h"

#include <vector>
#include <string>
#include <unordered_map>

template <typename T>
using PT_McRT_CXX_Vector = std::vector<T, PT_McRT_CXXAllocator<T>>;

using PT_McRT_CXX_String = std::basic_string<char, std::char_traits<char>, PT_McRT_CXXAllocator<char>>;

template <typename Key, typename T, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
using PT_McRT_CXX_Unordered_Map = std::unordered_map<Key, T, Hash, Pred, PT_McRT_CXXAllocator<std::pair<const Key, T>>>;

class JsonParser_Json_Object_Impl : public PT_XGL_Json_Object
{
	~JsonParser_Json_Object_Impl()
	{
		for (auto & _it = m_valueobject.begin(); _it != m_valueobject.end(); ++_it)
		{
			PT_XGL_Json_Value *_member = _it->second;
			_member->Release();
		}
	}

	void Release() override
	{
		this->~JsonParser_Json_Object_Impl();
		PT_McRT_Aligned_Free(this);
	}

    int GetType() override { return PT_XGL_Json_Type_Object; };

    PT_XGL_Json_Object *ToObject() override { return this; }

    PT_XGL_Json_Array *ToArray() override { return NULL; }

    PT_XGL_Json_String *ToString() override { return NULL; }

    PT_XGL_Json_NumberInt *ToNumberInt() override { return NULL; }

    PT_XGL_Json_NumberFloat *ToNumberFloat() override { return NULL; }

    PT_XGL_Json_True *ToTrue() override { return NULL; }

    PT_XGL_Json_False *ToFalse() override { return NULL; }

    PT_XGL_Json_Null *ToNull() override { return NULL; }

    PT_McRT_CXX_Unordered_Map<PT_McRT_CXX_String, PT_XGL_Json_Value *> m_valueobject;

    size_t GetSize() override { return m_valueobject.size(); };

	PT_XGL_Json_Value *GetMember(char const *_cstring) override 
	{ 	
		PT_XGL_Json_Value *_member;

		auto const & _it = m_valueobject.find(_cstring);
		if (_it != m_valueobject.end())
		{
			_member=_it->second;
		}
		else
		{
			_member = NULL;
		}

		return _member;
    };

	inline JsonParser_Json_Object_Impl()
	{

	}

public:
	bool AddMember(PT_McRT_CXX_String const &&_stdstring, PT_XGL_Json_Value *_jsonvalue)
	{
		auto const & _res_ist = m_valueobject.insert(std::pair<PT_McRT_CXX_String const, PT_XGL_Json_Value *>(std::move(_stdstring), _jsonvalue));
		return _res_ist.second;
	}

	static PT_XGL_Json_Object *Create()
	{
		return new(PT_McRT_Aligned_Malloc(sizeof(JsonParser_Json_Object_Impl), alignof(JsonParser_Json_Object_Impl)))JsonParser_Json_Object_Impl();
	}
};

class JsonParser_Json_Array_Impl : public PT_XGL_Json_Array 
{
	~JsonParser_Json_Array_Impl()
	{
		for (auto & _it = m_valuearray.begin(); _it != m_valuearray.end(); ++_it)
		{
			PT_XGL_Json_Value *_element = (*_it);
			_element->Release();
		}
	}

	void Release() override
	{
		this->~JsonParser_Json_Array_Impl();
		PT_McRT_Aligned_Free(this);
	}

	int GetType() override { return PT_XGL_Json_Type_Array; };

	PT_XGL_Json_Object *ToObject() override { return  NULL; }

	PT_XGL_Json_Array *ToArray() override { return this; }

	PT_XGL_Json_String *ToString() override { return NULL; }

	PT_XGL_Json_NumberInt *ToNumberInt() override { return NULL; }

	PT_XGL_Json_NumberFloat *ToNumberFloat() override { return NULL; }

	PT_XGL_Json_True *ToTrue() override { return NULL; }

	PT_XGL_Json_False *ToFalse() override { return NULL; }

	PT_XGL_Json_Null *ToNull() override { return NULL; }

	PT_McRT_CXX_Vector<PT_XGL_Json_Value *> m_valuearray;

	size_t GetSize() override { return m_valuearray.size(); };

	PT_XGL_Json_Value *GetElement(size_t index) override { return m_valuearray[index]; }

	inline JsonParser_Json_Array_Impl()
	{

	}

public:
	void AddElement(PT_XGL_Json_Value *_jsonvalue) { return m_valuearray.push_back(_jsonvalue); }

	static PT_XGL_Json_Array *Create()
	{
		return new(PT_McRT_Aligned_Malloc(sizeof(JsonParser_Json_Array_Impl), alignof(JsonParser_Json_Array_Impl)))JsonParser_Json_Array_Impl();
	}
};


class JsonParser_Json_String_Impl :public PT_XGL_Json_String
{
	void Release() override
	{
		this->~JsonParser_Json_String_Impl();
		PT_McRT_Aligned_Free(this);
	}

	int GetType() override { return PT_XGL_Json_Type_String; };

	PT_XGL_Json_Object *ToObject() override { return  NULL; }

	PT_XGL_Json_Array *ToArray() override { return NULL; }

	PT_XGL_Json_String *ToString() override { return this; }

	PT_XGL_Json_NumberInt *ToNumberInt() override { return NULL; }

	PT_XGL_Json_NumberFloat *ToNumberFloat() override { return NULL; }

	PT_XGL_Json_True *ToTrue() override { return NULL; }

	PT_XGL_Json_False *ToFalse() override { return NULL; }

	PT_XGL_Json_Null *ToNull() override { return NULL; }

	PT_McRT_CXX_String m_valuestring;

	char const *GetString() override { return m_valuestring.c_str(); }

	inline JsonParser_Json_String_Impl(PT_McRT_CXX_String const &&_valuestring):m_valuestring(_valuestring)
	{

	}

public:
	static PT_XGL_Json_String *Create(PT_McRT_CXX_String const &&_valuestring)
	{
		return new(PT_McRT_Aligned_Malloc(sizeof(JsonParser_Json_String_Impl), alignof(JsonParser_Json_String_Impl)))JsonParser_Json_String_Impl(std::move(_valuestring));
	}
};

class JsonParser_Json_NumberInt_Impl : public  PT_XGL_Json_NumberInt
{
	void Release() override
	{
		this->~JsonParser_Json_NumberInt_Impl();
		PT_McRT_Aligned_Free(this);
	}

	int GetType() override { return PT_XGL_Json_Type_NumberInt; };

	PT_XGL_Json_Object *ToObject() override { return  NULL; }

	PT_XGL_Json_Array *ToArray() override { return NULL; }

	PT_XGL_Json_String *ToString() override { return NULL; }

	PT_XGL_Json_NumberInt *ToNumberInt() override { return this; }

	PT_XGL_Json_NumberFloat *ToNumberFloat() override { return NULL; }

	PT_XGL_Json_True *ToTrue() override { return NULL; }

	PT_XGL_Json_False *ToFalse() override { return NULL; }

	PT_XGL_Json_Null *ToNull() override { return NULL; }

	long m_valueint;

	long GetInt() override {return m_valueint;};

	JsonParser_Json_NumberInt_Impl(long _valueint) :m_valueint(_valueint)
	{

	}

public:
	static PT_XGL_Json_NumberInt *Create(long _valueint)
	{
		return new(PT_McRT_Aligned_Malloc(sizeof(JsonParser_Json_NumberInt_Impl), alignof(JsonParser_Json_NumberInt_Impl)))JsonParser_Json_NumberInt_Impl(_valueint);
	}
};

class JsonParser_Json_NumberFloat_Impl : public  PT_XGL_Json_NumberFloat
{
	void Release() override
	{
		this->~JsonParser_Json_NumberFloat_Impl();
		PT_McRT_Aligned_Free(this);
	}

	int GetType() override { return PT_XGL_Json_Type_NumberFloat; };

	PT_XGL_Json_Object *ToObject() override { return  NULL; }

	PT_XGL_Json_Array *ToArray() override { return NULL; }

	PT_XGL_Json_String *ToString() override { return NULL; }

	PT_XGL_Json_NumberInt *ToNumberInt() override { return NULL; }

	PT_XGL_Json_NumberFloat *ToNumberFloat() override { return this; }

	PT_XGL_Json_True *ToTrue() override { return NULL; }

	PT_XGL_Json_False *ToFalse() override { return NULL; }

	PT_XGL_Json_Null *ToNull() override { return NULL; }

	float m_valuefloat;

	float GetFloat() override { return m_valuefloat; };

	JsonParser_Json_NumberFloat_Impl(float _valuefloat) :m_valuefloat(_valuefloat)
	{

	}

public:
	static PT_XGL_Json_NumberFloat *Create(float _valuefloat)
	{
		return new(PT_McRT_Aligned_Malloc(sizeof(JsonParser_Json_NumberFloat_Impl), alignof(JsonParser_Json_NumberFloat_Impl)))JsonParser_Json_NumberFloat_Impl(_valuefloat);
	}
};

class JsonParser_Json_True_Impl : public  PT_XGL_Json_True
{
	void Release() override
	{
		this->~JsonParser_Json_True_Impl();
		PT_McRT_Aligned_Free(this);
	}

	int GetType() override { return PT_XGL_Json_Type_True; };

	PT_XGL_Json_Object *ToObject() override { return  NULL; }

	PT_XGL_Json_Array *ToArray() override { return NULL; }

	PT_XGL_Json_String *ToString() override { return NULL; }

	PT_XGL_Json_NumberInt *ToNumberInt() override { return NULL; }

	PT_XGL_Json_NumberFloat *ToNumberFloat() override { return  NULL; }

	PT_XGL_Json_True *ToTrue() override { return this; }

	PT_XGL_Json_False *ToFalse() override { return NULL; }

	PT_XGL_Json_Null *ToNull() override { return NULL; }

	JsonParser_Json_True_Impl()
	{

	}

public:
	static PT_XGL_Json_True *Create()
	{
		return new(PT_McRT_Aligned_Malloc(sizeof(JsonParser_Json_True_Impl), alignof(JsonParser_Json_True_Impl)))JsonParser_Json_True_Impl();
	}
};

class JsonParser_Json_False_Impl : public  PT_XGL_Json_False
{
	void Release() override
	{
		this->~JsonParser_Json_False_Impl();
		PT_McRT_Aligned_Free(this);
	}

	int GetType() override { return PT_XGL_Json_Type_False; };

	PT_XGL_Json_Object *ToObject() override { return  NULL; }

	PT_XGL_Json_Array *ToArray() override { return NULL; }

	PT_XGL_Json_String *ToString() override { return NULL; }

	PT_XGL_Json_NumberInt *ToNumberInt() override { return NULL; }

	PT_XGL_Json_NumberFloat *ToNumberFloat() override { return  NULL; }

	PT_XGL_Json_True *ToTrue() override { return NULL; }

	PT_XGL_Json_False *ToFalse() override { return this; }

	PT_XGL_Json_Null *ToNull() override { return NULL; }

	JsonParser_Json_False_Impl()
	{

	}

public:
	static PT_XGL_Json_False *Create()
	{
		return new(PT_McRT_Aligned_Malloc(sizeof(JsonParser_Json_False_Impl), alignof(JsonParser_Json_False_Impl)))JsonParser_Json_False_Impl();
	}
};

class JsonParser_Json_Null_Impl : public  PT_XGL_Json_Null
{
	void Release() override
	{
		this->~JsonParser_Json_Null_Impl();
		PT_McRT_Aligned_Free(this);
	}

	int GetType() override { return PT_XGL_Json_Type_Null; };

	PT_XGL_Json_Object *ToObject() override { return  NULL; }

	PT_XGL_Json_Array *ToArray() override { return NULL; }

	PT_XGL_Json_String *ToString() override { return NULL; }

	PT_XGL_Json_NumberInt *ToNumberInt() override { return NULL; }

	PT_XGL_Json_NumberFloat *ToNumberFloat() override { return  NULL; }

	PT_XGL_Json_True *ToTrue() override { return NULL; }

	PT_XGL_Json_False *ToFalse() override { return NULL; }

	PT_XGL_Json_Null *ToNull() override { return this; }

	JsonParser_Json_Null_Impl()
	{

	}

public:
	static PT_XGL_Json_Null *Create()
	{
		return new(PT_McRT_Aligned_Malloc(sizeof(JsonParser_Json_Null_Impl), alignof(JsonParser_Json_Null_Impl)))JsonParser_Json_Null_Impl();
	}
};

#endif