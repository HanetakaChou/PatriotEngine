#ifndef _PT_XGL_JSONPARSER_H_
#define _PT_XGL_JSONPARSER_H_ 1

#ifndef __cplusplus
void *JsonFrontend_CreateJsonValue_FromString(void *pUserData, char *_valuestring_text, int _valuestring_leng);
void *JsonFrontend_CreateJsonValue_FromInt(void *pUserData, long _valueint);
void *JsonFrontend_CreateJsonValue_FromFloat(void *pUserData, float _valuefloat);
void *JsonFrontend_CreateJsonValue_True(void *pUserData);
void *JsonFrontend_CreateJsonValue_False(void *pUserData);
void *JsonFrontend_CreateJsonValue_Null(void *pUserData);
void JsonFrontend_DisposeJsonValue(void *pUserData, void *_jsonvalue);
void *JsonFrontend_CreateStdVector_FromJsonValue(void *pUserData, void *_jsonvalue);
void JsonFrontend_StdVector_PushJsonValue(void *pUserData, void *_stdvector, void *_jsonvalue);
void *JsonFrontend_DisposeStdVector(void *pUserData, void *_stdvector);
void *JsonFrontend_CreateJsonArray_FromStdVector(void *pUserData, void *_stdvector);
void *JsonFrontend_CreateJsonArray_FromEmpty(void *pUserData);
void *JsonFrontend_CreateStdPair_FromStringAndJsonValue(void *pUserData, char *_valuestring_text, int _valuestring_leng, void *_jsonvalue);
void JsonFrontend_DisposeStdPair(void *pUserData, void *_stdpair);
void *JsonFrontend_CreateStdUnorderedMap_FromStdPair(void *pUserData, void *_stdpair);
void JsonFrontend_StdUnorderedMap_InsertStdPair(void *pUserData, void *_stdunorderedmap, void *_stdpair);
void *JsonFrontend_DisposeStdUnorderedMap(void *pUserData, void *_stdunorderedmap);
void *JsonFrontend_CreateJsonObject_FromStdUnorderedMap(void *pUserData, void *_stdunorderedmap);
void *JsonFrontend_CreateJsonObject_FromEmpty(void *pUserData);
void *JsonFrontend_Malloc(void *pUserData, size_t size);
void *JsonFrontend_Realloc(void *pUserData, void *ptr, size_t size);
void JsonFrontend_Free(void *pUserData, void *ptr);
#else
extern "C" {
	void *JsonFrontend_Malloc(JsonFrontend *pUserData, size_t size);
	void *JsonFrontend_Realloc(JsonFrontend *pUserData, void *ptr, size_t size);
	void JsonFrontend_Free(JsonFrontend *pUserData, void *ptr);
}
#endif

#ifndef __cplusplus
union YYSTYPE {
    struct
    {
        char *_text;
        int _leng;
    } _valuestring;
    long _valueint;
    float _valuefloat;
    void *_jsonvalue;
    void *_stdvector;
    void *_stdpair;
    void *_stdunorderedmap;
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