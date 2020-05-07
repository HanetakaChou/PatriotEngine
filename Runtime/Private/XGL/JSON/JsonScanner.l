
%option reentrant
%option prefix="json_ll"
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

WS ([ \r\t](([ \r\t])*))

STRING (\"(\\.|[^\\"\n])*\")

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

TRUE ("true")
FALSE ("false")
JSONNULL ("null")

%%
    /* bugs for yylineno */
    /* ("\r\n") compatible WS \r */
("\n") {
    (yycolumn) = 0;
}

    /* ignore whitespace */
{WS} { 
    
    }

     /* string literal */
{STRING} {
    lvalp->_valuestring->_text = (yytext);
    lvalp->_valuestring->_leng = (yyleng);
    return YYTOKEN_STRING;
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

    /* keywords */
{TRUE} {
    return YYTOKEN_TRUE;
    }

{FALSE} {
    return YYTOKEN_FALSE;
    }

{JSONNULL} {
    return YYTOKEN_JSONNULL;
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

    /* catch-all rule for any other single characters */
(.) { 
        char msg_fatal_error[4096];
        assert(((yyleng) == 1));
        snprintf(msg_fatal_error, 4096, "lex error, unexpected \'%s\'", (yytext));
        YY_FATAL_ERROR(msg_fatal_error);
    }

<<EOF>> {
    json_llpop_buffer_state(yyscanner);
    if(!YY_CURRENT_BUFFER)
    {
        return YYTOKEN_EOF; //yyterminate
    }
    }

%%