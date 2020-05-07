#ifndef _PT_XGL_JSONFRONTEND_H_
#define _PT_XGL_JSONFRONTEND_H_ 1

#include "PT_XGL_StreamReader.h"

enum
{
	PT_XGL_Json_Type_Number_Invalid,
    PT_XGL_Json_Type_Object,
    PT_XGL_Json_Type_Array,
    PT_XGL_Json_Type_String,
    PT_XGL_Json_Type_Number_Int,
    PT_XGL_Json_Type_Number_Float,
    PT_XGL_Json_Type_True,
    PT_XGL_Json_Type_False,
    PT_XGL_Json_Type_Null
};

#include <memory>
#include <new>
#include <assert.h>

#include "../../Public/McRT/PT_McRT_CXXAllocator.h"

#include <vector>
#include <string>
#include <unordered_map>

template <typename T>
using PT_McRT_CXX_Vector = std::vector<T, PT_McRT_CXXAllocator<T>>;

using PT_McRT_CXX_String = std::basic_string<char, std::char_traits<char>, PT_McRT_CXXAllocator<char>>;

template <typename Key, typename T, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
using PT_McRT_CXX_Unordered_Map = std::unordered_map<Key, T, Hash, Pred, PT_McRT_CXXAllocator<std::pair<const Key, T>>>;

struct PT_XGL_Json_Value
{
private:
	int type;

	union Value_T
	{
	private:
		PT_McRT_CXX_Unordered_Map<PT_McRT_CXX_String, PT_XGL_Json_Value> valueobject;

		PT_McRT_CXX_Vector<PT_XGL_Json_Value> valuearray;

		PT_McRT_CXX_String valuestring;

		long valueint;

		float valuefloat;

	public:
		inline Value_T(PT_McRT_CXX_Unordered_Map<PT_McRT_CXX_String, PT_XGL_Json_Value> &&_value) :valueobject(_value)
		{

		}

		inline Value_T(PT_McRT_CXX_Vector<PT_XGL_Json_Value> &&_value) : valuearray(_value)
		{

		}

		inline Value_T(PT_McRT_CXX_String &&_value) : valuestring(_value)
		{

		}

		inline Value_T(long _value) : valueint(_value)
		{

		}

		inline Value_T(float _value) : valuefloat(_value)
		{

		}

		inline Value_T()
		{

		}

		~Value_T()
		{

		}
	}value;

public:
	inline PT_XGL_Json_Value(PT_McRT_CXX_Unordered_Map<PT_McRT_CXX_String, PT_XGL_Json_Value> &&_value) :type(PT_XGL_Json_Type_Object), value(std::move(_value))
	{

	}

	inline PT_XGL_Json_Value(PT_McRT_CXX_Vector<PT_XGL_Json_Value> &&_value) : type(PT_XGL_Json_Type_Array), value(std::move(_value))
	{

	}

	inline PT_XGL_Json_Value(PT_McRT_CXX_String &&_value) : type(PT_XGL_Json_Type_String), value(std::move(_value))
	{

	}

	inline PT_XGL_Json_Value(long _value) : type(PT_XGL_Json_Type_Number_Int), value(_value)
	{

	}

	inline PT_XGL_Json_Value(float _value) : type(PT_XGL_Json_Type_Number_Float), value(_value)
	{

	}

	inline PT_XGL_Json_Value() : type(PT_XGL_Json_Type_Number_Invalid), value()
	{

	}

	inline void SetTrue()
	{
		assert(type == PT_XGL_Json_Type_Number_Invalid);
		type = PT_XGL_Json_Type_True;
	}

	inline void SetFalse()
	{
		assert(type == PT_XGL_Json_Type_Number_Invalid);
		type = PT_XGL_Json_Type_False;
	}

	inline void SetNull()
	{
		assert(type == PT_XGL_Json_Type_Number_Invalid);
		type = PT_XGL_Json_Type_Null;
	}
};

void PT_XGL_JsonFrontend_Run(
    PT_XGL_StreamReaderRef _streamreader, PT_XGL_PFN_StreamReader_read _pfn_streamreader_read);

#endif