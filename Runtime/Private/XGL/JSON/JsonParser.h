#ifndef _PT_XGL_JSONPARSER_H_
#define _PT_XGL_JSONPARSER_H_ 1

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#ifndef __cplusplus
union YYSTYPE {
    char _string[256];
    float _float;
    void *_null;
};

extern int YYTOKEN_STRING;
extern int YYTOKEN_LEFTBRACE;
extern int YYTOKEN_RIGHTBRACE;
extern int YYTOKEN_COLON;
extern int YYTOKEN_LEFTBRACKET;
extern int YYTOKEN_RIGHTBRACKET;
extern int YYTOKEN_COMMA;
extern int YYTOKEN_NUMBER;
extern int YYTOKEN_TRUE;
extern int YYTOKEN_FALSE;
extern int YYTOKEN_NULL;
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