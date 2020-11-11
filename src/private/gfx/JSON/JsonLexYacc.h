#ifndef _PT_XGL_JSON_LEXYACC_H_
#define _PT_XGL_JSON_LEXYACC_H_ 1

#ifndef __cplusplus
#include <stdbool.h>
void *JsonParser_Malloc(void *pUserData, size_t size);
void *JsonParser_Realloc(void *pUserData, void *ptr, size_t size);
void JsonParser_Free(void *pUserData, void *ptr);

ptrdiff_t JsonParser_Callback_StreamReader_Read(void *pUserData, void *pUserStream, void *buf, size_t count);
int JsonParser_Callback_Wrap(void *pUserData);

void JsonParser_Error(void *pUserData, int line, int column, char const *s);

void *JsonParser_Std_String_Create(void *pUserData, char *_valuestring_text, int _valuestring_leng);
void JsonParser_Std_String_Dispose(void *pUserData, void *_stdstring);

void *JsonParser_Json_Object_Create(void *pUserData);
bool JsonParser_Json_Object_AddMember(void *pUserData, void *_jsonobject, void *_stdstring, void *_jsonvalue);
void *JsonParser_Json_Array_Create(void *pUserData);
void JsonParser_Json_Array_AddElement(void *pUserData, void *_jsonarray, void *_jsonvalue);
void *JsonParser_Json_String_Create(void *pUserData, void *_stdstring);
void *JsonParser_Json_NumberInt_Create(void *pUserData, long _valueint);
void *JsonParser_Json_NumberFloat_Create(void *pUserData, float _valuefloat);
void *JsonParser_Json_True_Create(void *pUserData);
void *JsonParser_Json_False_Create(void *pUserData);
void *JsonParser_Json_Null_Create(void *pUserData);

void JsonParser_Json_Root_Value(void *pUserData, void *_jsonvalue);
#else
#include <stddef.h>
#include <stdlib.h>
extern "C"
{
    void *JsonParser_Malloc(class JsonParser *pUserData, size_t size);
    void *JsonParser_Realloc(class JsonParser *pUserData, void *ptr, size_t size);
    void JsonParser_Free(class JsonParser *pUserData, void *ptr);
	
	ptrdiff_t JsonParser_Callback_StreamReader_Read(class JsonParser *pUserData, PT_XGL_StreamReaderRef pUserStream, void *buf, size_t count);
	int JsonParser_Callback_Wrap(class JsonParser *pUserData);
	
	void JsonParser_Error(class JsonParser *pUserData, int line, int column, char const *s);
	
	void *JsonParser_Std_String_Create(class JsonParser *pUserData, char *_valuestring_text, int _valuestring_leng);
	void JsonParser_Std_String_Dispose(class JsonParser *pUserData, void *_stdstring);
	
	void *JsonParser_Json_Object_Create(class JsonParser *pUserData);
	bool JsonParser_Json_Object_AddMember(class JsonParser *pUserData, void *_jsonobject, void *_stdstring, void *_jsonvalue);
	void *JsonParser_Json_Array_Create(class JsonParser *pUserData);
	void JsonParser_Json_Array_AddElement(class JsonParser *pUserData, void *_jsonarray, void *_jsonvalue);
	void *JsonParser_Json_String_Create(class JsonParser *pUserData, void *_stdstring);
	void *JsonParser_Json_NumberInt_Create(class JsonParser *pUserData, long _valueint);
	void *JsonParser_Json_NumberFloat_Create(class JsonParser *pUserData, float _valuefloat);
	void *JsonParser_Json_True_Create(class JsonParser *pUserData);
	void *JsonParser_Json_False_Create(class JsonParser *pUserData);
	void *JsonParser_Json_Null_Create(class JsonParser *pUserData);

	void JsonParser_Json_Root_Value(class JsonParser *pUserData, void *_jsonvalue);
}
#endif

#ifndef __cplusplus
union YYSTYPE {
    void *_stdstring;

    long _valueint;

    float _valuefloat;

	void *_jsonvalue;

	struct {
		void *_stdstring;
		void *_jsonvalue;
	} _jsonmember;
};

extern int YYTOKEN_STRING;
extern int YYTOKEN_LEFTBRACE;
extern int YYTOKEN_RIGHTBRACE;
extern int YYTOKEN_COLON;
extern int YYTOKEN_LEFTBRACKET;
extern int YYTOKEN_RIGHTBRACKET;
extern int YYTOKEN_COMMA;
extern int YYTOKEN_NUMBER_INT;
extern int YYTOKEN_NUMBER_FLOAT;
extern int YYTOKEN_TRUE;
extern int YYTOKEN_FALSE;
extern int YYTOKEN_JSONNULL;
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    extern int YYTOKEN_PSEUDO_LEX_ERROR;
    extern int YYTOKEN_EOF;
#ifdef __cplusplus
}
#endif

#endif