
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
    static char const BUFFERS[] = "\"buffers\"";
    static int const BUFFERS_LENG = sizeof(BUFFERS) / sizeof(BUFFERS[0]) - 1;
    static char const BYTELENGTH[] = "\"byteLength\"";
    static int const BYTELENGTH_LENG = sizeof(BYTELENGTH) / sizeof(BYTELENGTH[0]) - 1;
    static char const URI[] = "\"uri\"";
    static int const URI_LENG = sizeof(URI) / sizeof(URI[0]) - 1;
    static char const BUFFERVIEWS[] = "\"bufferViews\"";
    static int const BUFFERVIEWS_LENG = sizeof(BUFFERVIEWS) / sizeof(BUFFERVIEWS[0]) - 1;
    static char const BUFFER[] = "\"buffer\"";
    static int const BUFFER_LENG = sizeof(BUFFER) / sizeof(BUFFER[0]) - 1;
    static char const BYTEOFFSET[] = "\"byteOffset\"";
    static int const BYTEOFFSET_LENG = sizeof(BYTEOFFSET) / sizeof(BYTEOFFSET[0]) - 1;
    static char const BYTESTRIDE[] = "\"byteStride\"";
    static int const BYTESTRIDE_LENG = sizeof(BYTESTRIDE) / sizeof(BYTESTRIDE[0]) - 1;
    static char const TARGET[] = "\"target\"";
    static int const TARGET_LENG = sizeof(TARGET) / sizeof(TARGET[0]) - 1;
    static char const ACCESSORS[] = "\"accessors\"";
    static int const ACCESSORS_LENG = sizeof(ACCESSORS) / sizeof(ACCESSORS[0]) - 1;
    static char const BUFFERVIEW[] = "\"bufferView\"";
    static int const BUFFERVIEW_LENG = sizeof(BUFFERVIEW) / sizeof(BUFFERVIEW[0]) - 1;
    static char const COMPONENTTYPE[] = "\"componentType\"";
    static int const COMPONENTTYPE_LENG = sizeof(COMPONENTTYPE) / sizeof(COMPONENTTYPE[0]) - 1;
    static char const NORMALIZED[] = "\"normalized\"";
    static int const NORMALIZED_LENG = sizeof(NORMALIZED) / sizeof(NORMALIZED[0]) - 1;
    static char const COUNT[] = "\"count\"";
    static int const COUNT_LENG = sizeof(COUNT) / sizeof(COUNT[0]) - 1;
    static char const TYPE[] = "\"type\"";
    static int const TYPE_LENG = sizeof(TYPE) / sizeof(TYPE[0]) - 1;
    static char const MAX[] = "\"max\"";
    static int const MAX_LENG = sizeof(MAX) / sizeof(MAX[0]) - 1;
    static char const MIN[] = "\"min\"";
    static int const MIN_LENG = sizeof(MIN) / sizeof(MIN[0]) - 1;
    static char const TRUE[] = "\"true\"";
    static int const TRUE_LENG = sizeof(TRUE) / sizeof(TRUE[0]) - 1;
    static char const FALSE[] = "\"false\"";
    static int const FALSE_LENG = sizeof(FALSE) / sizeof(FALSE[0]) - 1;
    static char const TOKEN_NULL[] = "\"null\"";
    static int const TOKEN_NULL_LENG = sizeof(TOKEN_NULL) / sizeof(TOKEN_NULL[0]) - 1;

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
    else if (0 == gltf_lex_memcmp_callback((yytext), BUFFERS, (((yyleng) < BUFFERS_LENG) ? (yyleng) : BUFFERS_LENG), (yyextra)))
    {
        return YYTOKEN_BUFFERS;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), BYTELENGTH, (((yyleng) < BYTELENGTH_LENG) ? (yyleng) : BYTELENGTH_LENG), (yyextra)))
    {
        return YYTOKEN_BYTELENGTH;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), URI, (((yyleng) < URI_LENG) ? (yyleng) : URI_LENG), (yyextra)))
    {
        return YYTOKEN_URI;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), BUFFERVIEWS, (((yyleng) < BUFFERVIEWS_LENG) ? (yyleng) : BUFFERVIEWS_LENG), (yyextra)))
    {
        return YYTOKEN_BUFFERVIEWS;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), BUFFER, (((yyleng) < BUFFER_LENG) ? (yyleng) : BUFFER_LENG), (yyextra)))
    {
        return YYTOKEN_BUFFER;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), BYTEOFFSET, (((yyleng) < BYTEOFFSET_LENG) ? (yyleng) : BYTEOFFSET_LENG), (yyextra)))
    {
        return YYTOKEN_BYTEOFFSET;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), BYTESTRIDE, (((yyleng) < BYTESTRIDE_LENG) ? (yyleng) : BYTESTRIDE_LENG), (yyextra)))
    {
        return YYTOKEN_BYTESTRIDE;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), TARGET, (((yyleng) < TARGET_LENG) ? (yyleng) : TARGET_LENG), (yyextra)))
    {
        return YYTOKEN_TARGET;
    }
        else if (0 == gltf_lex_memcmp_callback((yytext), ACCESSORS, (((yyleng) < ACCESSORS_LENG) ? (yyleng) : ACCESSORS_LENG), (yyextra)))
    {
        return YYTOKEN_ACCESSORS;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), BUFFERVIEW, (((yyleng) < BUFFERVIEW_LENG) ? (yyleng) : BUFFERVIEW_LENG), (yyextra)))
    {
        return YYTOKEN_BUFFERVIEW;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), COMPONENTTYPE, (((yyleng) < COMPONENTTYPE_LENG) ? (yyleng) : COMPONENTTYPE_LENG), (yyextra)))
    {
        return YYTOKEN_COMPONENTTYPE;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), NORMALIZED, (((yyleng) < NORMALIZED_LENG) ? (yyleng) : NORMALIZED_LENG), (yyextra)))
    {
        return YYTOKEN_NORMALIZED;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), COUNT, (((yyleng) < COUNT_LENG) ? (yyleng) : COUNT_LENG), (yyextra)))
    {
        return YYTOKEN_COUNT;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), TYPE, (((yyleng) < TYPE_LENG) ? (yyleng) : TYPE_LENG), (yyextra)))
    {
        return YYTOKEN_TYPE;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), MAX, (((yyleng) < MAX_LENG) ? (yyleng) : MAX_LENG), (yyextra)))
    {
        return YYTOKEN_MAX;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), MIN, (((yyleng) < MIN_LENG) ? (yyleng) : MIN_LENG), (yyextra)))
    {
        return YYTOKEN_MIN;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), TRUE, (((yyleng) < TRUE_LENG) ? (yyleng) : TRUE_LENG), (yyextra)))
    {
        return YYTOKEN_TRUE;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), FALSE, (((yyleng) < FALSE_LENG) ? (yyleng) : FALSE_LENG), (yyextra)))
    {
        return YYTOKEN_FALSE;
    }
    else if (0 == gltf_lex_memcmp_callback((yytext), TOKEN_NULL, (((yyleng) < TOKEN_NULL_LENG) ? (yyleng) : TOKEN_NULL_LENG), (yyextra)))
    {
        return YYTOKEN_NULL;
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