#ifndef _PT_XGL_JSONPARSER_H_
#define _PT_XGL_JSONPARSER_H_ 1

#include "PT_XGL_StreamReader.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

enum
{
    PT_XGL_Json_Type_Object,
    PT_XGL_Json_Type_Array,
    PT_XGL_Json_Type_String,
    PT_XGL_Json_Type_NumberInt,
    PT_XGL_Json_Type_NumberFloat,
    PT_XGL_Json_Type_True,
    PT_XGL_Json_Type_False,
    PT_XGL_Json_Type_Null
};

struct PT_XGL_Json_Value;
struct PT_XGL_Json_Object;
struct PT_XGL_Json_Array;
struct PT_XGL_Json_String;
struct PT_XGL_Json_NumberInt;
struct PT_XGL_Json_NumberFloat;
struct PT_XGL_Json_True;
struct PT_XGL_Json_False;
struct PT_XGL_Json_Null;

struct PT_XGL_Json_Value
{
    virtual void Release() = 0;
    virtual int GetType() = 0;
    virtual PT_XGL_Json_Object *ToObject() = 0;
    virtual PT_XGL_Json_Array *ToArray() = 0;
    virtual PT_XGL_Json_String *ToString() = 0;
    virtual PT_XGL_Json_NumberInt *ToNumberInt() = 0;
    virtual PT_XGL_Json_NumberFloat *ToNumberFloat() = 0;
    virtual PT_XGL_Json_True *ToTrue() = 0;
    virtual PT_XGL_Json_False *ToFalse() = 0;
    virtual PT_XGL_Json_Null *ToNull() = 0;
};

struct PT_XGL_Json_Object : PT_XGL_Json_Value
{
    virtual size_t GetSize() = 0;
    virtual PT_XGL_Json_Value *GetMember(char const *) = 0;
};

struct PT_XGL_Json_Array : PT_XGL_Json_Value
{
    virtual size_t GetSize() = 0;
    virtual PT_XGL_Json_Value *GetElement(size_t index) = 0;
};

struct PT_XGL_Json_String : PT_XGL_Json_Value
{
    virtual char const *GetString() = 0;
};

struct PT_XGL_Json_NumberInt : PT_XGL_Json_Value
{
    virtual long GetInt() = 0;
};

struct PT_XGL_Json_NumberFloat : PT_XGL_Json_Value
{
    virtual float GetFloat() = 0;
};

struct PT_XGL_Json_True : PT_XGL_Json_Value
{
};

struct PT_XGL_Json_False : PT_XGL_Json_Value
{
};

struct PT_XGL_Json_Null : PT_XGL_Json_Value
{
};

PT_XGL_Json_Value *PT_XGL_JsonParser_Parse(PT_XGL_StreamReaderRef _streamreader, PT_XGL_PFN_StreamReader_read _pfn_streamreader_read, char *error_out, size_t error_maxsize);

#endif