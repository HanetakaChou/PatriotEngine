
%option reentrant
%option noinput
%option nounput
%option batch
%option fast
%option nounistd
%option never-interactive
%option noyyalloc
%option noyyfree
%option noyyrealloc
%option yylineno

    /* https://westes.github.io/flex/manual/Start-Conditions.html#Start-Conditions */

    /* https://westes.github.io/flex/manual/Patterns.html#Patterns */

    /* https://www.crockford.com/mckeeman.html */


    /* Ignore whitespace */
WS ([ \r\t](([ \r\t])*))

    /* keywords */
TRUE ("true")
FALSE ("false")
YYTOKEN_NULL ("null")
EXTENSIONS ("extensions")
EXTRAS ("extras")
SCENE ("scene")
SCENES ("scenes")
NODES ("nodes")
NAME ("name")
CAMERA ("camera")
CHILDREN ("children")
SKIN ("skin")
MATRIX ("matrix")
MESH ("mesh")
ROTATION ("rotation")
SCALE ("scale")
TRANSLATION ("translation")
WEIGHTS ("weights")

    /* string literal */
STRING (\"(\\.|[^\\"\n])*\")

    /* Split */
LEFTBRACE ("{")
RIGHTBRACE ("}")
COLON (":")
LEFTBRACKET ("[")
RIGHTBRACKET ("]")
COMMA (",")

INTERGER (("-"?)([0-9])(([0-9])*))
FRACTION ((".")([0-9])(([0-9])*))
EXPONENT ([eE]([-+]?)([0-9])(([0-9])*))
NUMBER_INT {INTERGER}
NUMBER_FLOAT (({INTERGER}{FRACTION})|({INTERGER}{EXPONENT})|({INTERGER}{FRACTION}{EXPONENT}))

%%
    /* bugs for yylineno */
    /* ("\r\n") compatible WS \r */
("\n") {
    (yycolumn) = 0;
}

    /* ignore whitespace */
{WS} { 
    
}

    /* split */
{LEFTBRACE} {
    return YYTOKEN_LEFTBRACE;
}

{RIGHTBRACE} {
    return YYTOKEN_RIGHTBRACE;
}

{COLON} {
    return YYTOKEN_COLON;
}

{LEFTBRACKET} {
    return YYTOKEN_LEFTBRACKET;
}

{RIGHTBRACKET} {
    return YYTOKEN_RIGHTBRACKET;
}

{COMMA} {
    return YYTOKEN_COMMA;
    }

    /* literal values */
{NUMBER_INT} { 
    char *endptr = NULL;
    lvalp->_valueint = strtol((yytext), &endptr, 10);
    assert(endptr <= ((yytext) + (yyleng)));
    return YYTOKEN_NUMBER_INT; 
    }

{NUMBER_FLOAT} { 
    char *endptr = NULL;
    lvalp->_valuefloat = strtof((yytext), &endptr);
    assert(endptr <= ((yytext) + (yyleng)));
    return YYTOKEN_NUMBER_FLOAT; 
}

     /* string literal */
{STRING} {
    /* keywords */
    static char const ASSET[] = "\"asset\"";
    static int const ASSET_LENG = sizeof(ASSET) / sizeof(ASSET[0]) - 1;
    static char const COPYRIGHT[] = "\"copyright\"";
    static int const COPYRIGHT_LENG = sizeof(COPYRIGHT) / sizeof(COPYRIGHT[0]) - 1;
    static char const GENERATOR[] = "\"generator\"";
    static int const GENERATOR_LENG = sizeof(GENERATOR) / sizeof(GENERATOR[0]) - 1;
    static char const VERSION[] = "\"version\"";
    static int const VERSION_LENG = sizeof(VERSION) / sizeof(VERSION[0]) - 1;
    static char const MINVERSION[] = "\"minversion\"";
    static int const MINVERSION_LENG = sizeof(MINVERSION) / sizeof(MINVERSION[0]) - 1;
    if (0 == gltf_lex_memcmp_callback((yytext), ASSET, (((yyleng) < ASSET_LENG) ? (yyleng) : ASSET_LENG), (yyextra)))
    {
        return YYTOKEN_ASSET;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), COPYRIGHT, (((yyleng) < COPYRIGHT_LENG) ? (yyleng) : COPYRIGHT_LENG), (yyextra)))
    {
        return YYTOKEN_COPYRIGHT;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), GENERATOR, (((yyleng) < GENERATOR_LENG) ? (yyleng) : GENERATOR_LENG), (yyextra)))
    {
        return YYTOKEN_GENERATOR;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), VERSION, (((yyleng) < VERSION_LENG) ? (yyleng) : VERSION_LENG), (yyextra)))
    {
        return YYTOKEN_VERSION;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), MINVERSION, (((yyleng) < MINVERSION_LENG) ? (yyleng) : MINVERSION_LENG), (yyextra)))
    {
        return YYTOKEN_MINVERSION;
    }
    else
    {
        assert(((yyleng) + 1) <= YY_TOKEN_STRING_BUFFER_SIZE);
        gltf_lex_memcpy_callback(lvalp->m_token_string, (yytext), (yyleng), (yyextra));
        lvalp->m_token_string[(yyleng)] = '\0';
        return YYTOKEN_STRING;
    }
}

    /* catch-all rule for any other single characters */
(.) { 
        char msg_fatal_error[4096];
        assert(((yyleng) == 1));
        snprintf(msg_fatal_error, 4096, "lex error, unexpected \'%s\'", (yytext));
        YY_FATAL_ERROR(msg_fatal_error);
    }

<<EOF>> {
        yypop_buffer_state(yyscanner);
        if(!YY_CURRENT_BUFFER)
        {
            return YYEOF; //yyterminate
        }
    }

%%