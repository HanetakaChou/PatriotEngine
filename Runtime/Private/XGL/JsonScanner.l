
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

NEWLINE (("\n")|("\r\n"))
WS ([ \r\t](([ \r\t])*))
STRING (\"(\\.|[^\\"\n])*\")
LEFTBRACE ("{")
RIGHTBRACE ("}")
COLON (":")
LEFTBRACKET ("[")
RIGHTBRACKET ("]")
COMMA (",")
NUMBER ((("-"?)([0-9])(([0-9])*))(((".")([0-9])(([0-9])*))?)(([eE]([-+]?)([0-9])(([0-9])*))?))
TRUE ("true")
FALSE ("false")
NULL ("null")

%%
    /* Bugs for yylineno */
{NEWLINE} {
    (yycolumn) = 0;
}

    /* Ignore whitespace */
{WS} { 
    
    }

     /* string literal */
{STRING} {
    return YYTOKEN_STRING_LITERAL;
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
{NUMBER} { 
    char *endptr = NULL;
    lvalp->_INTEGER_LITERAL = strtol(yytext, &endptr, 10);
    assert(endptr <= ((yytext) + (yyleng)));
    return YYTOKEN_INTEGER_LITERAL; 
    }

{TRUE} {
    return YYTOKEN_TRUE;
    }

{FALSE} {
    return YYTOKEN_FALSE;
    }

{NULL} {
    return YYTOKEN_NULL;
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