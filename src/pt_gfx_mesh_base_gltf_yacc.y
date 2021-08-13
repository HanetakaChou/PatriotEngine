%define api.pure full

%define api.value.type {union YYSTYPE}

// Tell Bison to track locations for improved error messages
%locations

%define parse.error verbose

%parse-param {void *user_defined} {void *yyscanner}
%lex-param {void *user_defined} {void *yyscanner}

%code{
    int yylex(union YYSTYPE *lvalp, YYLTYPE *llocp, void *user_defined, void *yyscanner);
    void yyerror(YYLTYPE *llocp, void *user_defined, void *yyscanner, const char *msg);
}


// Define the terminal symbols.
%token YYTOKEN_ASSET
%token YYTOKEN_COPYRIGHT
%token YYTOKEN_GENERATOR
%token YYTOKEN_VERSION
%token YYTOKEN_MINVERSION
%token YYTOKEN_EXTENSIONS
%token YYTOKEN_EXTRAS
%token YYTOKEN_SCENE
%token YYTOKEN_SCENES
%token YYTOKEN_NODES
%token YYTOKEN_NAME
%token YYTOKEN_CAMERA
%token YYTOKEN_CHILDREN
%token YYTOKEN_SKIN
%token YYTOKEN_MATRIX
%token YYTOKEN_MESH
%token YYTOKEN_ROTATION
%token YYTOKEN_SCALE
%token YYTOKEN_TRANSLATION
%token YYTOKEN_WEIGHTS
%token YYTOKEN_TRUE 
%token YYTOKEN_FALSE 
%token YYTOKEN_NULL 
%token YYTOKEN_LEFTBRACE 
%token YYTOKEN_RIGHTBRACE 
%token YYTOKEN_COLON 
%token YYTOKEN_LEFTBRACKET 
%token YYTOKEN_RIGHTBRACKET 
%token YYTOKEN_COMMA
%token <_stdstring> YYTOKEN_STRING
%token <_valueint> YYTOKEN_NUMBER_INT
%token <_valuefloat> YYTOKEN_NUMBER_FLOAT
%token PSEUDO_LEX_ERROR

// Define the nonterminals 
%type <m_json_object> gltf_object
%type <m_json_object> gltf_members
%type <m_json_object> gltf_member
%type <m_json_object> asset_object
%type <m_json_object> asset_members
%type <m_json_object> asset_member
%type <m_json_object> scenes_array
%type <m_json_object> scene_objects
%type <m_json_object> scene_object
%type <m_json_object> scene_members
%type <m_json_object> scene_member
%type <m_json_object> scene_nodes
%type <m_json_object> scene_root_node_indices
%type <m_json_object> nodes_array
%type <m_json_object> node_objects
%type <m_json_object> node_object
%type <m_json_object> node_members
%type <m_json_object> node_member
%type <m_json_object> string_value

%type <m_json_value> json_value 
%type <m_json_value> json_object
%type <m_json_value> json_members
%type <m_json_member> json_member
%type <m_json_value> json_array
%type <m_json_value> json_elements
%type <m_json_value> json_element 


// Define the starting nonterminal
%start gltf_object

%%
gltf_object : YYTOKEN_LEFTBRACE gltf_members YYTOKEN_RIGHTBRACE { 
	//$$ = $2;
	};

gltf_members: gltf_members YYTOKEN_COMMA gltf_member { 
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, $1, $3._stdstring, $3._jsonvalue); 
	//JsonParser_Std_String_Dispose(pUserData, $3._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = $1;
    };

gltf_members: gltf_member { 
    //void *_jsonobject = JsonParser_Json_Object_Create(pUserData);
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, _jsonobject, $1._stdstring, $1._jsonvalue);
	//JsonParser_Std_String_Dispose(pUserData, $1._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = _jsonobject;
    };

gltf_member: YYTOKEN_ASSET YYTOKEN_COLON asset_object {
	//$$ = $3;
	};

gltf_member: YYTOKEN_SCENES YYTOKEN_COLON scenes_array {
	//$$ = $3;
	};

gltf_member: YYTOKEN_NODES YYTOKEN_COLON nodes_array {
	//$$ = $3;
	};

asset_object : YYTOKEN_LEFTBRACE asset_members YYTOKEN_RIGHTBRACE { 
	//$$ = $2;
	};	

asset_members: asset_members YYTOKEN_COMMA asset_member { 
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, $1, $3._stdstring, $3._jsonvalue); 
	//JsonParser_Std_String_Dispose(pUserData, $3._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = $1;
    };

asset_members: asset_member { 
    //void *_jsonobject = JsonParser_Json_Object_Create(pUserData);
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, _jsonobject, $1._stdstring, $1._jsonvalue);
	//JsonParser_Std_String_Dispose(pUserData, $1._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = _jsonobject;
    };

asset_member: YYTOKEN_COPYRIGHT YYTOKEN_COLON string_value { 
	//$$._jsonvalue = $3;
    };

asset_member: YYTOKEN_GENERATOR YYTOKEN_COLON string_value { 
	//$$._jsonvalue = $3;
    };

asset_member: YYTOKEN_VERSION YYTOKEN_COLON string_value { 
	//$$._jsonvalue = $3;
    };

asset_member: YYTOKEN_MINVERSION YYTOKEN_COLON string_value { 
	//$$._jsonvalue = $3;
    };

asset_member: YYTOKEN_EXTRAS YYTOKEN_COLON json_value { 
	//$$._jsonvalue = $3;
    };	

scenes_array: YYTOKEN_LEFTBRACKET scene_objects YYTOKEN_RIGHTBRACKET { 
    //$$ = $2;
    };

scenes_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET { 
    //$$ = JsonParser_Json_Array_Create(pUserData); 
    };

scene_objects: scene_objects YYTOKEN_COMMA scene_object { 
    //JsonParser_Json_Array_AddElement(pUserData, $1, $3);
    //$$ = $1;
    };

scene_objects: scene_object { 
    //void *_jsonarray = JsonParser_Json_Array_Create(pUserData);
	//JsonParser_Json_Array_AddElement(pUserData, _jsonarray, $1); 
	//$$ = _jsonarray;
    };

scene_object: scene_members YYTOKEN_COMMA scene_member { 
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, $1, $3._stdstring, $3._jsonvalue); 
	//JsonParser_Std_String_Dispose(pUserData, $3._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = $1;
    };

scene_members: scene_member { 
    //void *_jsonobject = JsonParser_Json_Object_Create(pUserData);
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, _jsonobject, $1._stdstring, $1._jsonvalue);
	//JsonParser_Std_String_Dispose(pUserData, $1._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = _jsonobject;
    };

scene_member: YYTOKEN_NODES YYTOKEN_COLON scene_nodes { 
	//$$._jsonvalue = $3;
    };

scene_nodes: YYTOKEN_LEFTBRACKET scene_root_node_indices YYTOKEN_RIGHTBRACKET { 
    //$$ = $2;
    };

scene_nodes: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET { 
    //$$ = JsonParser_Json_Array_Create(pUserData); 
    };

scene_root_node_indices: scene_root_node_indices YYTOKEN_COMMA YYTOKEN_NUMBER_INT { 
    //JsonParser_Json_Array_AddElement(pUserData, $1, $3);
    //$$ = $1;
    };

scene_root_node_indices: YYTOKEN_NUMBER_INT { 
    //void *_jsonarray = JsonParser_Json_Array_Create(pUserData);
	//JsonParser_Json_Array_AddElement(pUserData, _jsonarray, $1); 
	//$$ = _jsonarray;
    };

scene_member: YYTOKEN_NAME YYTOKEN_COLON string_value { 
	//$$._jsonvalue = $3;
    };

scene_member: YYTOKEN_EXTRAS YYTOKEN_COLON json_value { 
	//$$._jsonvalue = $3;
    };	

nodes_array: YYTOKEN_LEFTBRACKET node_objects YYTOKEN_RIGHTBRACKET { 
    //$$ = $2;
    };

nodes_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET { 
    //$$ = JsonParser_Json_Array_Create(pUserData); 
    };

node_objects: node_objects YYTOKEN_COMMA node_object { 
    //JsonParser_Json_Array_AddElement(pUserData, $1, $3);
    //$$ = $1;
    };

node_objects: node_object { 
    //void *_jsonarray = JsonParser_Json_Array_Create(pUserData);
	//JsonParser_Json_Array_AddElement(pUserData, _jsonarray, $1); 
	//$$ = _jsonarray;
    };

node_object: node_members YYTOKEN_COMMA node_member { 
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, $1, $3._stdstring, $3._jsonvalue); 
	//JsonParser_Std_String_Dispose(pUserData, $3._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = $1;
    };

node_members: node_member { 
    //void *_jsonobject = JsonParser_Json_Object_Create(pUserData);
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, _jsonobject, $1._stdstring, $1._jsonvalue);
	//JsonParser_Std_String_Dispose(pUserData, $1._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = _jsonobject;
    };

node_member: YYTOKEN_CAMERA YYTOKEN_COLON YYTOKEN_NUMBER_INT { 
    };

string_value: YYTOKEN_STRING {

};

string_value: YYTOKEN_ASSET {

};

string_value: YYTOKEN_COPYRIGHT {

};

string_value: YYTOKEN_GENERATOR {

};

string_value: YYTOKEN_VERSION {

};

string_value: YYTOKEN_MINVERSION {

};

string_value: YYTOKEN_EXTENSIONS {

};

string_value: YYTOKEN_EXTRAS {

};

string_value: YYTOKEN_SCENES {

};

string_value: YYTOKEN_NODES {

};

string_value: YYTOKEN_NAME {

};

json_value: json_object { 
};

json_value: json_array { 
};

json_value: YYTOKEN_STRING { 
};

json_value: YYTOKEN_NUMBER_INT { 
};

json_value: YYTOKEN_NUMBER_FLOAT { 
};

json_value: YYTOKEN_TRUE { 
};

json_value: YYTOKEN_FALSE { 
};

json_value: YYTOKEN_NULL { 
};

json_object: YYTOKEN_LEFTBRACE json_members YYTOKEN_RIGHTBRACE { 
};

json_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE { 
};

json_members: json_members YYTOKEN_COMMA json_member { 
};

json_members: json_member { 
};

json_member: YYTOKEN_STRING YYTOKEN_COLON json_value { 
};

json_array: YYTOKEN_LEFTBRACKET json_elements YYTOKEN_RIGHTBRACKET { 
};

json_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET { 
};

json_elements: json_elements YYTOKEN_COMMA json_element { 
};

json_elements: json_element { 
};

json_element: json_value {
	
};

%%
