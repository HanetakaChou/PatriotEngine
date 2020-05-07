%define api.pure full

%define api.value.type {union YYSTYPE}

// Tell Bison to track locations for improved error messages
%locations

%define parse.error verbose

%parse-param {void *pUserData} {void *pScanner}
%lex-param {void *pUserData} {void *pScanner}

%code{
    int yylex(union YYSTYPE *lvalp, YYLTYPE *llocp, void *pUserData, void *pScanner);
    void yyerror(YYLTYPE *llocp, void *pUserData, void *pScanner, const char *s);
}


// Define the terminal symbols.
%token TRUE "true"
%token FALSE "false"
%token JSONNULL "null"
%token LEFTBRACE "{"
%token RIGHTBRACE "}"
%token COLON ":"
%token LEFTBRACKET "["
%token RIGHTBRACKET "]"
%token COMMA ","
%token <_valuestring> STRING
%token <_valueint> NUMBER_INT
%token <_valuefloat> NUMBER_FLOAT
%token PSEUDO_LEX_ERROR "[A lex error has occurred!]"

// Define the nonterminals 
%type <_jsonvalue> json 
%type <_jsonvalue> value 
%type <_jsonvalue> object
%type <_stdunorderedmap> members
%type <_stdpair> member
%type <_jsonvalue> array
%type <_stdvector> elements
%type <_jsonvalue> element 

// Define the starting nonterminal
%start json

%%

json: value { $$ = $1; };

value: object { $$ = $1; };
value: array { $$ = $1; };
value: STRING { $$ = JsonFrontend_CreateJsonValue_FromString(pUserData, $1._text, $1._leng); };
value: NUMBER_INT { $$ = JsonFrontend_CreateJsonValue_FromInt(pUserData, $1); };
value: NUMBER_FLOAT { $$ = JsonFrontend_CreateJsonValue_FromFloat(pUserData, $1); };
value: TRUE { $$ = JsonFrontend_CreateJsonValue_True(pUserData); };
value: FALSE { $$ = JsonFrontend_CreateJsonValue_False(pUserData); };
value: JSONNULL { $$ = JsonFrontend_CreateJsonValue_Null(pUserData); };

object: LEFTBRACE members RIGHTBRACE { 
    $$ = JsonFrontend_CreateJsonObject_FromStdUnorderedMap(pUserData, $2);
    JsonFrontend_DisposeStdUnorderedMap(pUserData, $2);
    };
object: LEFTBRACE RIGHTBRACE { 
    $$ = JsonFrontend_CreateJsonObject_FromEmpty(pUserData); 
    };

members: member COMMA members { 
    JsonFrontend_StdUnorderedMap_InsertStdPair(pUserData, $1, $3);
    JsonFrontend_DisposeStdPair(pUserData, $3);
    };
members: member { 
    $$ = JsonFrontend_CreateStdUnorderedMap_FromStdPair(pUserData, $1);
    JsonFrontend_DisposeStdPair(pUserData, $1);
    };

member: STRING ':' value { 
    $$ = JsonFrontend_CreateStdPair_FromStringAndJsonValue(pUserData, $1._text, $1._leng, $3); 
    JsonFrontend_DisposeJsonValue(pUserData, $3);
    };

array: LEFTBRACKET elements RIGHTBRACKET { 
    $$ = JsonFrontend_CreateJsonArray_FromStdVector(pUserData, $2); 
    JsonFrontend_DisposeStdVector(pUserData, $2);
    };
array: LEFTBRACKET RIGHTBRACKET { 
    $$ = JsonFrontend_CreateJsonArray_FromEmpty(pUserData); 
    };

elements: elements COMMA element { 
    JsonFrontend_StdVector_PushJsonValue(pUserData, $1, $3);
    JsonFrontend_DisposeJsonValue(pUserData, $3);
    $$ = $1;
    };
elements: element { 
    $$ = JsonFrontend_CreateStdVector_FromJsonValue(pUserData, $1); 
    JsonFrontend_DisposeJsonValue(pUserData, $1);
    };

element: value { $$ = $1; };

%%
