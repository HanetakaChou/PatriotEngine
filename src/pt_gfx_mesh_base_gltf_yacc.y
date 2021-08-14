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
%token YYTOKEN_BUFFERS
%token YYTOKEN_BUFFERLENGTH
%token YYTOKEN_URI
%token YYTOKEN_TRUE 
%token YYTOKEN_FALSE 
%token YYTOKEN_NULL 
%token YYTOKEN_LEFTBRACE 
%token YYTOKEN_RIGHTBRACE 
%token YYTOKEN_COLON 
%token YYTOKEN_LEFTBRACKET 
%token YYTOKEN_RIGHTBRACKET 
%token YYTOKEN_COMMA
%token <m_token_string> YYTOKEN_STRING
%token <m_token_numberint> YYTOKEN_NUMBER_INT
%token <m_token_numberfloat> YYTOKEN_NUMBER_FLOAT
%token PSEUDO_LEX_ERROR

// Define the nonterminals 
%type gltf_object
%type gltf_members
%type gltf_member
%type asset_object
%type asset_members
%type asset_member
%type scenes_array
%type scene_objects
%type <m_scene_index> scene_object
%type <m_scene_index> scene_properties
%type <m_int_array> scene_property_nodes
%type <m_int_array> scene_property_nodes_elements
%type nodes_array
%type node_objects
%type <m_node_index> node_object
%type <m_node_index> node_properties
%type <m_int_array> node_property_children
%type <m_int_array> node_property_children_elements
%type <m_mat4x4> node_property_matrix
%type <m_token_numberfloat> node_property_matrix_element
%type <m_vec4> node_property_rotation
%type <m_token_numberfloat> node_property_rotation_element
%type <m_vec3> node_property_scale
%type <m_token_numberfloat> node_property_scale_element
%type <m_vec3> node_property_translation
%type <m_token_numberfloat> node_property_translation_element
%type <m_float_array> node_property_weights
%type <m_float_array> node_property_weights_elements
%type buffers_array
%type buffer_objects
%type <m_buffer_index> buffer_object
%type <m_buffer_index> buffer_properties

%type <m_token_string> json_string
%type json_value 
%type json_object
%type json_members
%type json_member
%type json_array
%type json_elements
%type json_element 


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

gltf_member: YYTOKEN_SCENE YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_set_default_scene_index_callback($3, user_defined);
};

gltf_member: YYTOKEN_SCENES YYTOKEN_COLON scenes_array {
	//$$ = $3;
	};

gltf_member: YYTOKEN_NODES YYTOKEN_COLON nodes_array {
	//$$ = $3;
	};

gltf_member: YYTOKEN_BUFFERS YYTOKEN_COLON buffers_array {
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

asset_member: YYTOKEN_COPYRIGHT YYTOKEN_COLON json_string { 
	//$$._jsonvalue = $3;
    };

asset_member: YYTOKEN_GENERATOR YYTOKEN_COLON json_string { 
	//$$._jsonvalue = $3;
    };

asset_member: YYTOKEN_VERSION YYTOKEN_COLON json_string { 
	//$$._jsonvalue = $3;
    };

asset_member: YYTOKEN_MINVERSION YYTOKEN_COLON json_string { 
	//$$._jsonvalue = $3;
    };

asset_member: YYTOKEN_EXTRAS YYTOKEN_COLON json_value { 
	//$$._jsonvalue = $3;
    };	

scenes_array: YYTOKEN_LEFTBRACKET scene_objects YYTOKEN_RIGHTBRACKET { 

};

scenes_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET {

};

scene_objects: scene_objects YYTOKEN_COMMA scene_object { 
	assert(-1 == $3 || gltf_yacc_scene_size_callback(user_defined) == ($3 + 1));
};

scene_objects: scene_object {
	assert(-1 == $1 || gltf_yacc_scene_size_callback(user_defined) == ($1 + 1));
};

scene_object: YYTOKEN_LEFTBRACE scene_properties YYTOKEN_RIGHTBRACE {
	$$ = $2;
};

scene_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE { 
	$$ = -1;
};

scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_NODES YYTOKEN_COLON scene_property_nodes {
	gltf_yacc_scene_set_nodes_callback($$, $5.m_data, $5.m_size, user_defined);
};

scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_NAME YYTOKEN_COLON json_string {
	gltf_yacc_scene_set_name_callback($$, $5.m_data, $5.m_size, user_defined);
};

scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_EXTRAS YYTOKEN_COLON json_value { 
	// ignore
};	

scene_properties: YYTOKEN_NODES YYTOKEN_COLON scene_property_nodes { 
	$$ = gltf_yacc_scene_push_callback(user_defined);
	gltf_yacc_scene_set_nodes_callback($$, $3.m_data, $3.m_size, user_defined);
};

scene_properties: YYTOKEN_NAME YYTOKEN_COLON json_string {
	$$ = gltf_yacc_scene_push_callback(user_defined);
	gltf_yacc_scene_set_name_callback($$, $3.m_data, $3.m_size, user_defined);
};

scene_properties: YYTOKEN_EXTRAS YYTOKEN_COLON json_value { 
	// ignore
};

scene_property_nodes: YYTOKEN_LEFTBRACKET scene_property_nodes_elements YYTOKEN_RIGHTBRACKET {
	for (int i = 0; i < $2.m_size; ++i)
	{
		$$.m_data[i] = $2.m_data[i];
	}
	$$.m_size = $2.m_size;
};

scene_property_nodes: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET { 
    $$.m_size = 0;
};

scene_property_nodes_elements: scene_property_nodes_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT {
	$$.m_data[$$.m_size] = $3;
	++$$.m_size;
};

scene_property_nodes_elements : YYTOKEN_NUMBER_INT {
	$$.m_data[0] = $1;
	$$.m_size = 1;
};

nodes_array: YYTOKEN_LEFTBRACKET node_objects YYTOKEN_RIGHTBRACKET {

};

nodes_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET {

};

node_objects: node_objects YYTOKEN_COMMA node_object { 
   	assert(-1 == $3 || gltf_yacc_node_size_callback(user_defined) == ($3 + 1));
};

node_objects: node_object { 
	assert(-1 == $1 || gltf_yacc_node_size_callback(user_defined) == ($1 + 1));
};

node_object: YYTOKEN_LEFTBRACE node_properties YYTOKEN_RIGHTBRACE {
	$$ = $2;
};

node_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE { 
	$$ = -1;
};

node_properties: node_properties YYTOKEN_COMMA YYTOKEN_CAMERA YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_node_set_camera_callback($$, $5, user_defined);
};

node_properties: node_properties YYTOKEN_COMMA YYTOKEN_CHILDREN YYTOKEN_COLON node_property_children {
	gltf_yacc_node_set_children_callback($$, $5.m_data, $5.m_size, user_defined);
};

node_properties: node_properties YYTOKEN_COMMA YYTOKEN_SKIN YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_node_set_skin_callback($$, $5, user_defined);
};

node_properties: node_properties YYTOKEN_COMMA YYTOKEN_MATRIX YYTOKEN_COLON node_property_matrix {
	gltf_yacc_node_set_matrix_callback($$, $5, user_defined);
};

node_properties: node_properties YYTOKEN_COMMA YYTOKEN_MESH YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_node_set_mesh_callback($$, $5, user_defined);
};

node_properties: node_properties YYTOKEN_COMMA YYTOKEN_ROTATION YYTOKEN_COLON node_property_rotation {
	gltf_yacc_node_set_rotation_callback($$, $5, user_defined);
};

node_properties: node_properties YYTOKEN_COMMA YYTOKEN_SCALE YYTOKEN_COLON node_property_scale {
	gltf_yacc_node_set_scale_callback($$, $5, user_defined);
};

node_properties: node_properties YYTOKEN_COMMA YYTOKEN_TRANSLATION YYTOKEN_COLON node_property_translation {
	gltf_yacc_node_set_translation_callback($$, $5, user_defined);
};

node_properties: node_properties YYTOKEN_COMMA YYTOKEN_WEIGHTS YYTOKEN_COLON node_property_weights {
	gltf_yacc_node_set_weights_callback($$, $5.m_data, $5.m_size, user_defined);
};

node_properties: node_properties YYTOKEN_COMMA YYTOKEN_NAME YYTOKEN_COLON json_string {
	gltf_yacc_node_set_name_callback($$, $5.m_data, $5.m_size, user_defined);
};

node_properties: YYTOKEN_CAMERA YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_camera_callback($$, $3, user_defined);
};

node_properties: YYTOKEN_CHILDREN YYTOKEN_COLON node_property_children {
	$$ = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_children_callback($$, $3.m_data, $3.m_size, user_defined);
};

node_properties: YYTOKEN_SKIN YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_skin_callback($$, $3, user_defined);
};

node_properties: YYTOKEN_MATRIX YYTOKEN_COLON node_property_matrix {
	$$ = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_matrix_callback($$, $3, user_defined);
};

node_properties: YYTOKEN_MESH YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_mesh_callback($$, $3, user_defined);
};

node_properties: YYTOKEN_ROTATION YYTOKEN_COLON node_property_rotation {
	$$ = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_rotation_callback($$, $3, user_defined);
};

node_properties: YYTOKEN_SCALE YYTOKEN_COLON node_property_scale {
	$$ = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_scale_callback($$, $3, user_defined);
};

node_properties: YYTOKEN_TRANSLATION YYTOKEN_COLON node_property_translation {
	$$ = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_translation_callback($$, $3, user_defined);
};

node_properties: YYTOKEN_WEIGHTS YYTOKEN_COLON node_property_weights {
	$$ = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_weights_callback($$, $3.m_data, $3.m_size, user_defined);
};

node_properties: YYTOKEN_NAME YYTOKEN_COLON json_string {
	$$ = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_name_callback($$, $3.m_data, $3.m_size, user_defined);
};

node_property_children: YYTOKEN_LEFTBRACKET node_property_children_elements YYTOKEN_RIGHTBRACKET {
	for (int i = 0; i < $2.m_size; ++i)
	{
		$$.m_data[i] = $2.m_data[i];
	}
	$$.m_size = $2.m_size;
};

node_property_children: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET { 
    $$.m_size = 0;
};

node_property_children_elements: node_property_children_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT {
	$$.m_data[$$.m_size] = $3;
	++$$.m_size;
};

node_property_children_elements : YYTOKEN_NUMBER_INT {
	$$.m_data[0] = $1;
	$$.m_size = 1;
};

node_property_matrix: YYTOKEN_LEFTBRACKET node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_RIGHTBRACKET {
	$$[0] = $2;
	$$[1] = $4;
	$$[2] = $6;
	$$[3] = $8;
	$$[4] = $10;
	$$[5] = $12;
	$$[6] = $14;
	$$[7] = $16;
	$$[8] = $18;
	$$[9] = $20;
	$$[10] = $22;
	$$[11] = $24;
	$$[12] = $26;
	$$[13] = $28;
	$$[14] = $30;
	$$[15] = $32;
}

node_property_matrix_element: YYTOKEN_NUMBER_FLOAT {
	$$ = $1;
}

node_property_matrix_element: YYTOKEN_NUMBER_INT {
	$$ = $1;
}

node_property_rotation: YYTOKEN_LEFTBRACKET node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_RIGHTBRACKET {
	$$[0] = $2;
	$$[1] = $4;
	$$[2] = $6;
	$$[3] = $8;
}

node_property_rotation_element: YYTOKEN_NUMBER_FLOAT {
	$$ = $1;
}

node_property_rotation_element: YYTOKEN_NUMBER_INT {
	$$ = $1;
}

node_property_scale: YYTOKEN_LEFTBRACKET node_property_scale_element YYTOKEN_COMMA node_property_scale_element YYTOKEN_COMMA node_property_scale_element YYTOKEN_RIGHTBRACKET {
	$$[0] = $2;
	$$[1] = $4;
	$$[2] = $6;
}

node_property_scale_element: YYTOKEN_NUMBER_FLOAT {
	$$ = $1;
}

node_property_scale_element: YYTOKEN_NUMBER_INT {
	$$ = $1;
}

node_property_translation: YYTOKEN_LEFTBRACKET node_property_translation_element YYTOKEN_COMMA node_property_translation_element YYTOKEN_COMMA node_property_translation_element YYTOKEN_RIGHTBRACKET {
	$$[0] = $2;
	$$[1] = $4;
	$$[2] = $6;
}

node_property_translation_element: YYTOKEN_NUMBER_FLOAT {
	$$ = $1;
}

node_property_translation_element: YYTOKEN_NUMBER_INT {
	$$ = $1;
}

node_property_weights: YYTOKEN_LEFTBRACKET node_property_weights_elements YYTOKEN_RIGHTBRACKET {
	for (int i = 0; i < $2.m_size; ++i)
	{
		$$.m_data[i] = $2.m_data[i];
	}
	$$.m_size = $2.m_size;
};

node_property_weights: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET { 
    $$.m_size = 0;
};

node_property_weights_elements: node_property_weights_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT {
	$$.m_data[$$.m_size] = $3;
	++$$.m_size;
};

node_property_weights_elements : YYTOKEN_NUMBER_INT {
	$$.m_data[0] = $1;
	$$.m_size = 1;
};

buffers_array: YYTOKEN_LEFTBRACKET buffer_objects YYTOKEN_RIGHTBRACKET {

};

buffers_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET {

};

buffer_objects: buffer_objects YYTOKEN_COMMA buffer_object { 
   	assert(-1 == $3 || gltf_yacc_buffer_size_callback(user_defined) == ($3 + 1));
};

buffer_objects: buffer_object { 
	assert(-1 == $1 || gltf_yacc_buffer_size_callback(user_defined) == ($1 + 1));
};

buffer_object: YYTOKEN_LEFTBRACE buffer_properties YYTOKEN_RIGHTBRACE {
	$$ = $2;
};

buffer_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE { 
	$$ = -1;
};

buffer_properties: buffer_properties YYTOKEN_COMMA YYTOKEN_BUFFERLENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_buffer_set_bufferlength_callback($$, $5, user_defined);
};

buffer_properties: buffer_properties YYTOKEN_COMMA YYTOKEN_URI YYTOKEN_COLON json_string {
	gltf_yacc_buffer_set_url_callback($$, $5.m_data, $5.m_size, user_defined);
};


buffer_properties: YYTOKEN_BUFFERLENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_buffer_push_callback(user_defined);
	gltf_yacc_buffer_set_bufferlength_callback($$, $3, user_defined);
};

buffer_properties: YYTOKEN_URI YYTOKEN_COLON json_string {
	$$ = gltf_yacc_buffer_push_callback(user_defined);
	gltf_yacc_buffer_set_url_callback($$, $3.m_data, $3.m_size, user_defined);
};


json_string: YYTOKEN_STRING {

};

json_string: YYTOKEN_ASSET {

};

json_string: YYTOKEN_COPYRIGHT {

};

json_string: YYTOKEN_GENERATOR {

};

json_string: YYTOKEN_VERSION {

};

json_string: YYTOKEN_MINVERSION {

};

json_string: YYTOKEN_EXTENSIONS {

};

json_string: YYTOKEN_EXTRAS {

};

json_string: YYTOKEN_SCENES {

};

json_string: YYTOKEN_NODES {

};

json_string: YYTOKEN_NAME {

};

json_value: json_object { 
};

json_value: json_array { 
};

json_value: json_string { 
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
