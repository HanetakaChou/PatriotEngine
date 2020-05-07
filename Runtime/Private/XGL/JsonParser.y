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
%token NULL "null"
%token LEFTBRACE "{"
%token RIGHTBRACE "}"
%token COLON ":"
%token LEFTBRACKET "["
%token RIGHTBRACKET "]"
%token COMMA ","
%token <_string> STRING
%token <_float> NUMBER
%token PSEUDO_LEX_ERROR "[A lex error has occurred!]"

// Define the nonterminals 
%type <_null> json 
%type <_null> element 
%type <_null> value 
%type <_null> object
%type <_null> members
%type <_null> member
%type <_null> array
%type <_null> elements
%type <_string> string
%type <_float> number

// Define the starting nonterminal
%start json

%%

json: value { $$ = NULL; };

value: object { $$ = NULL; };
value: array { $$ = NULL; };
value: string { $$ = NULL; };
value: number { $$ = NULL; };
value: TRUE { $$ = NULL; };
value: FALSE { $$ = NULL; };
value: NULL { $$ = NULL; };

object: LEFTBRACE members RIGHTBRACE { $$ = NULL; };
object: LEFTBRACE RIGHTBRACE { $$ = NULL; };

members: member COMMA members { $$ = NULL; };
members: member { $$ = NULL; };

member: string ':' value { $$ = NULL; };

array: LEFTBRACKET elements RIGHTBRACKET { $$ = NULL; };
array: LEFTBRACKET RIGHTBRACKET { $$ = NULL; };

elements: element COMMA elements { $$ = NULL; };
elements: element { $$ = NULL; };

element: value { $$ = NULL; };

string: STRING { $$ = NULL; };
number: NUMBER { $$ = NULL; };

%%
