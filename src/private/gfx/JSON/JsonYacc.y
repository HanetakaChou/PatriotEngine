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
%token <_stdstring> STRING
%token <_valueint> NUMBER_INT
%token <_valuefloat> NUMBER_FLOAT
%token PSEUDO_LEX_ERROR "[A lex error has occurred!]"

// Define the nonterminals 
%type <_jsonvalue> json 
%type <_jsonvalue> value 
%type <_jsonvalue> object
%type <_jsonvalue> members
%type <_jsonmember> member
%type <_jsonvalue> array
%type <_jsonvalue> elements
%type <_jsonvalue> element 

// Define the starting nonterminal
%start json

%%

json: value { $$ = $1; JsonParser_Json_Root_Value(pUserData, $$); };

value: object { 
	$$ = $1; 
	};
value: array { 
	$$ = $1; 
	};
value: STRING { 
	$$ = JsonParser_Json_String_Create(pUserData, $1); 
	JsonParser_Std_String_Dispose(pUserData, $1); 
	};
value: NUMBER_INT { 
	$$ = JsonParser_Json_NumberInt_Create(pUserData, $1); 
	};
value: NUMBER_FLOAT { 
	$$ = JsonParser_Json_NumberFloat_Create(pUserData, $1); 
	};
value: TRUE { 
	$$ = JsonParser_Json_True_Create(pUserData); 
	};
value: FALSE { 
	$$ = JsonParser_Json_False_Create(pUserData); 
	};
value: JSONNULL { 
	$$ = JsonParser_Json_Null_Create(pUserData); 
	};

object: LEFTBRACE members RIGHTBRACE { 
	$$ = $2;
    };
object: LEFTBRACE RIGHTBRACE { 
    $$ = JsonParser_Json_Object_Create(pUserData);
    };

members: members COMMA member { 
	bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, $1, $3._stdstring, $3._jsonvalue); 
	JsonParser_Std_String_Dispose(pUserData, $3._stdstring);
	if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	$$ = $1;
    };
members: member { 
    void *_jsonobject = JsonParser_Json_Object_Create(pUserData);
	bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, _jsonobject, $1._stdstring, $1._jsonvalue);
	JsonParser_Std_String_Dispose(pUserData, $1._stdstring);
	if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	$$ = _jsonobject;
    };

member: STRING ':' value { 
	$$._stdstring = $1;
	$$._jsonvalue = $3;
    };

array: LEFTBRACKET elements RIGHTBRACKET { 
    $$ = $2;
    };
array: LEFTBRACKET RIGHTBRACKET { 
    $$ = JsonParser_Json_Array_Create(pUserData); 
    };

elements: elements COMMA element { 
    JsonParser_Json_Array_AddElement(pUserData, $1, $3);
    $$ = $1;
    };
elements: element { 
    void *_jsonarray = JsonParser_Json_Array_Create(pUserData);
	JsonParser_Json_Array_AddElement(pUserData, _jsonarray, $1); 
	$$ = _jsonarray;
    };

element: value { $$ = $1; };

%%
