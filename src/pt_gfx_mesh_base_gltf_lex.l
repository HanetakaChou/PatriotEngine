
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
    long numberlong = strtol((yytext), &endptr, 10);
    assert(endptr <= ((yytext) + (yyleng)));
    lvalp->m_token_numberint = numberlong;
    return YYTOKEN_NUMBER_INT;
}

{NUMBER_FLOAT} { 
    char *endptr = NULL;
    lvalp->m_token_numberfloat = strtof((yytext), &endptr);
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
    static char const SCENE[] = "\"scene\"";
    static int const SCENE_LENG = sizeof(SCENE) / sizeof(SCENE[0]) - 1;
    static char const SCENES[] = "\"scenes\"";
    static int const SCENES_LENG = sizeof(SCENES) / sizeof(SCENES[0]) - 1;
    static char const NODES[] = "\"nodes\"";
    static int const NODES_LENG = sizeof(NODES) / sizeof(NODES[0]) - 1;
    static char const NAME[] = "\"name\"";
    static int const NAME_LENG = sizeof(NAME) / sizeof(NAME[0]) - 1;
    static char const CAMERA[] = "\"camera\"";
    static int const CAMERA_LENG = sizeof(CAMERA) / sizeof(CAMERA[0]) - 1;
    static char const CHILDREN[] = "\"children\"";
    static int const CHILDREN_LENG = sizeof(CHILDREN) / sizeof(CHILDREN[0]) - 1;
    static char const SKIN[] = "\"skin\"";
    static int const SKIN_LENG = sizeof(SKIN) / sizeof(SKIN[0]) - 1;
    static char const MATRIX[] = "\"matrix\"";
    static int const MATRIX_LENG = sizeof(MATRIX) / sizeof(MATRIX[0]) - 1;
    static char const MESH[] = "\"mesh\"";
    static int const MESH_LENG = sizeof(MESH) / sizeof(MESH[0]) - 1;
    static char const ROTATION[] = "\"rotation\"";
    static int const ROTATION_LENG = sizeof(ROTATION) / sizeof(ROTATION[0]) - 1;
    static char const SCALE[] = "\"scale\"";
    static int const SCALE_LENG = sizeof(SCALE) / sizeof(SCALE[0]) - 1;
    static char const TRANSLATION[] = "\"translation\"";
    static int const TRANSLATION_LENG = sizeof(TRANSLATION) / sizeof(TRANSLATION[0]) - 1;
    static char const WEIGHTS[] = "\"weights\"";
    static int const WEIGHTS_LENG = sizeof(WEIGHTS) / sizeof(WEIGHTS[0]) - 1;
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
    else if (0 == gltf_lex_memcmp_callback((yytext), SCENE, (((yyleng) < SCENE_LENG) ? (yyleng) : SCENE_LENG), (yyextra)))
    {
        return YYTOKEN_SCENE;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), SCENES, (((yyleng) < SCENES_LENG) ? (yyleng) : SCENES_LENG), (yyextra)))
    {
        return YYTOKEN_SCENES;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), NODES, (((yyleng) < NODES_LENG) ? (yyleng) : NODES_LENG), (yyextra)))
    {
        return YYTOKEN_NODES;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), NAME, (((yyleng) < NAME_LENG) ? (yyleng) : NAME_LENG), (yyextra)))
    {
        return YYTOKEN_NAME;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), CAMERA, (((yyleng) < CAMERA_LENG) ? (yyleng) : CAMERA_LENG), (yyextra)))
    {
        return YYTOKEN_CAMERA;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), CHILDREN, (((yyleng) < CHILDREN_LENG) ? (yyleng) : CHILDREN_LENG), (yyextra)))
    {
        return YYTOKEN_CHILDREN;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), SKIN, (((yyleng) < SKIN_LENG) ? (yyleng) : SKIN_LENG), (yyextra)))
    {
        return YYTOKEN_SKIN;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), MATRIX, (((yyleng) < MATRIX_LENG) ? (yyleng) : MATRIX_LENG), (yyextra)))
    {
        return YYTOKEN_MATRIX;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), MESH, (((yyleng) < MESH_LENG) ? (yyleng) : MESH_LENG), (yyextra)))
    {
        return YYTOKEN_MESH;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), ROTATION, (((yyleng) < ROTATION_LENG) ? (yyleng) : ROTATION_LENG), (yyextra)))
    {
        return YYTOKEN_ROTATION;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), SCALE, (((yyleng) < SCALE_LENG) ? (yyleng) : SCALE_LENG), (yyextra)))
    {
        return YYTOKEN_SCALE;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), TRANSLATION, (((yyleng) < TRANSLATION_LENG) ? (yyleng) : TRANSLATION_LENG), (yyextra)))
    {
        return YYTOKEN_TRANSLATION;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), WEIGHTS, (((yyleng) < WEIGHTS_LENG) ? (yyleng) : WEIGHTS_LENG), (yyextra)))
    {
        return YYTOKEN_WEIGHTS;
    }
    else
    {
        assert(((yyleng) + 1) <= YY_TOKEN_STRING_MAX_SIZE);
        gltf_lex_memcpy_callback(lvalp->m_token_string.m_data, (yytext), (yyleng), (yyextra));
        lvalp->m_token_string.m_data[(yyleng)] = '\0';
        lvalp->m_token_string.m_size = (yyleng);
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