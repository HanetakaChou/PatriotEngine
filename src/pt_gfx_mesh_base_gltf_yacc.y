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
%token YYTOKEN_BYTELENGTH
%token YYTOKEN_URI
%token YYTOKEN_BUFFERVIEWS
%token YYTOKEN_BUFFER
%token YYTOKEN_BYTEOFFSET
%token YYTOKEN_BYTESTRIDE
%token YYTOKEN_TARGET
%token YYTOKEN_ACCESSORS
%token YYTOKEN_BUFFERVIEW
%token YYTOKEN_COMPONENTTYPE
%token YYTOKEN_NORMALIZED
%token YYTOKEN_COUNT
%token YYTOKEN_TYPE
%token YYTOKEN_MAX
%token YYTOKEN_MIN
%token YYTOKEN_SCALAR 
%token YYTOKEN_VEC2
%token YYTOKEN_VEC3
%token YYTOKEN_VEC4
%token YYTOKEN_MAT2
%token YYTOKEN_MAT3
%token YYTOKEN_MAT4

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
%type asset_properties
%type scenes_array
%type scene_objects
%type <m_scene_index> scene_object
%type <m_scene_index> scene_properties
%type <m_temp_int_array_version> scene_property_nodes
%type <m_temp_int_array_version> scene_property_nodes_elements
%type nodes_array
%type node_objects
%type <m_node_index> node_object
%type <m_node_index> node_properties
%type <m_temp_int_array_version> node_property_children
%type <m_temp_int_array_version> node_property_children_elements
%type <m_mat4x4> node_property_matrix
%type <m_token_numberfloat> node_property_matrix_element
%type <m_vec4> node_property_rotation
%type <m_token_numberfloat> node_property_rotation_element
%type <m_vec3> node_property_scale
%type <m_token_numberfloat> node_property_scale_element
%type <m_vec3> node_property_translation
%type <m_token_numberfloat> node_property_translation_element
%type <m_temp_float_array_version> node_property_weights
%type <m_temp_float_array_version> node_property_weights_elements
%type <m_token_numberfloat> node_property_weights_element
%type buffers_array
%type buffer_objects
%type <m_buffer_index> buffer_object
%type <m_buffer_index> buffer_properties
%type bufferviews_array
%type bufferview_objects
%type <m_bufferview_index> bufferview_object
%type <m_bufferview_index> bufferview_properties
%type accessors_array
%type accessor_objects
%type <m_accessor_index> accessor_object
%type <m_accessor_index> accessor_properties


%type <m_token_string> json_string
%type <m_type_boolean> json_boolean
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

};

gltf_member: YYTOKEN_NODES YYTOKEN_COLON nodes_array {

};

gltf_member: YYTOKEN_BUFFERS YYTOKEN_COLON buffers_array {

};

gltf_member: YYTOKEN_BUFFERVIEWS YYTOKEN_COLON bufferviews_array {

};

gltf_member: YYTOKEN_ACCESSORS YYTOKEN_COLON accessors_array {

};

asset_object : YYTOKEN_LEFTBRACE asset_properties YYTOKEN_RIGHTBRACE { 
	
};	

asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_COPYRIGHT YYTOKEN_COLON json_string { 

};

asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_GENERATOR YYTOKEN_COLON json_string { 

};

asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_VERSION YYTOKEN_COLON json_string { 

};

asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_MINVERSION YYTOKEN_COLON json_string { 

};

asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_EXTRAS YYTOKEN_COLON json_value { 

};

asset_properties: YYTOKEN_COPYRIGHT YYTOKEN_COLON json_string { 

};

asset_properties: YYTOKEN_GENERATOR YYTOKEN_COLON json_string { 

};

asset_properties: YYTOKEN_VERSION YYTOKEN_COLON json_string { 

};

asset_properties: YYTOKEN_MINVERSION YYTOKEN_COLON json_string { 

};

asset_properties: YYTOKEN_EXTRAS YYTOKEN_COLON json_value { 

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
	gltf_yacc_scene_set_nodes_callback($$, $5, user_defined);
	gltf_yacc_temp_int_array_destroy_callback($5, user_defined);
};

scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_NAME YYTOKEN_COLON json_string {
	gltf_yacc_scene_set_name_callback($$, $5.m_data, $5.m_size, user_defined);
};

scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_EXTRAS YYTOKEN_COLON json_value { 
	// ignore
};	

scene_properties: YYTOKEN_NODES YYTOKEN_COLON scene_property_nodes { 
	$$ = gltf_yacc_scene_push_callback(user_defined);
	gltf_yacc_scene_set_nodes_callback($$, $3, user_defined);
	gltf_yacc_temp_int_array_destroy_callback($3, user_defined);
};

scene_properties: YYTOKEN_NAME YYTOKEN_COLON json_string {
	$$ = gltf_yacc_scene_push_callback(user_defined);
	gltf_yacc_scene_set_name_callback($$, $3.m_data, $3.m_size, user_defined);
};

scene_properties: YYTOKEN_EXTRAS YYTOKEN_COLON json_value { 

};

scene_property_nodes: YYTOKEN_LEFTBRACKET scene_property_nodes_elements YYTOKEN_RIGHTBRACKET {
	$$ = $2;
};

scene_property_nodes: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET { 
	$$ = gltf_yacc_temp_int_array_init_callback(user_defined);
};

scene_property_nodes_elements: scene_property_nodes_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT {
	gltf_yacc_temp_int_array_push_callback($$, $3, user_defined);
};

scene_property_nodes_elements : YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_temp_int_array_init_callback(user_defined);
	gltf_yacc_temp_int_array_push_callback($$, $1, user_defined);
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
	gltf_yacc_node_set_children_callback($$, $5, user_defined);
	gltf_yacc_temp_int_array_destroy_callback($5, user_defined);
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
	gltf_yacc_node_set_weights_callback($$, $5, user_defined);
	gltf_yacc_temp_float_array_destroy_callback($5, user_defined);
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
	gltf_yacc_node_set_children_callback($$, $3, user_defined);
	gltf_yacc_temp_int_array_destroy_callback($3, user_defined);
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
	gltf_yacc_node_set_weights_callback($$, $3, user_defined);
	gltf_yacc_temp_float_array_destroy_callback($3, user_defined);
};

node_properties: YYTOKEN_NAME YYTOKEN_COLON json_string {
	$$ = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_name_callback($$, $3.m_data, $3.m_size, user_defined);
};

node_property_children: YYTOKEN_LEFTBRACKET node_property_children_elements YYTOKEN_RIGHTBRACKET {
	$$ = $2;
};

node_property_children: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET { 
    $$ = gltf_yacc_temp_int_array_init_callback(user_defined);
};

node_property_children_elements: node_property_children_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT {
	gltf_yacc_temp_int_array_push_callback($$, $3, user_defined);
};

node_property_children_elements : YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_temp_int_array_init_callback(user_defined);
	gltf_yacc_temp_int_array_push_callback($$, $1, user_defined);
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
	$$ = $2;
};

node_property_weights: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET { 
    $$ = gltf_yacc_temp_float_array_init_callback(user_defined);
};

node_property_weights_elements: node_property_weights_elements YYTOKEN_COMMA node_property_weights_element {
	gltf_yacc_temp_float_array_push_callback($$, $3, user_defined);
};

node_property_weights_elements : node_property_weights_element {
	$$ = gltf_yacc_temp_float_array_init_callback(user_defined);
	gltf_yacc_temp_float_array_push_callback($$, $1, user_defined);
};


node_property_weights_element: YYTOKEN_NUMBER_FLOAT {
	$$ = $1;
}

node_property_weights_element: YYTOKEN_NUMBER_INT {
	$$ = $1;
}


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

buffer_properties: buffer_properties YYTOKEN_COMMA YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_buffer_set_bytelength_callback($$, $5, user_defined);
};

buffer_properties: buffer_properties YYTOKEN_COMMA YYTOKEN_URI YYTOKEN_COLON json_string {
	gltf_yacc_buffer_set_url_callback($$, $5.m_data, $5.m_size, user_defined);
};


buffer_properties: YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_buffer_push_callback(user_defined);
	gltf_yacc_buffer_set_bytelength_callback($$, $3, user_defined);
};

buffer_properties: YYTOKEN_URI YYTOKEN_COLON json_string {
	$$ = gltf_yacc_buffer_push_callback(user_defined);
	gltf_yacc_buffer_set_url_callback($$, $3.m_data, $3.m_size, user_defined);
};

bufferviews_array: YYTOKEN_LEFTBRACKET bufferview_objects YYTOKEN_RIGHTBRACKET {

};

bufferviews_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET {

};

bufferview_objects: bufferview_objects YYTOKEN_COMMA bufferview_object { 
   	assert(-1 == $3 || gltf_yacc_bufferview_size_callback(user_defined) == ($3 + 1));
};

bufferview_objects: bufferview_object { 
	assert(-1 == $1 || gltf_yacc_bufferview_size_callback(user_defined) == ($1 + 1));
};

bufferview_object: YYTOKEN_LEFTBRACE bufferview_properties YYTOKEN_RIGHTBRACE {
	$$ = $2;
};

bufferview_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE { 
	$$ = -1;
};

bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BUFFER YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_bufferview_set_buffer_callback($$, $5, user_defined);
};

bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_bufferview_set_byteoffset_callback($$, $5, user_defined);
};

bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_bufferview_set_bytelength_callback($$, $5, user_defined);
};

bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BYTESTRIDE YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_bufferview_set_bytestride_callback($$, $5, user_defined);
};

bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_TARGET YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_bufferview_set_target_callback($$, $5, user_defined);
};

bufferview_properties: YYTOKEN_BUFFER YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_buffer_callback($$, $3, user_defined);
};

bufferview_properties: YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_byteoffset_callback($$, $3, user_defined);
};

bufferview_properties: YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_bytelength_callback($$, $3, user_defined);
};

bufferview_properties: YYTOKEN_BYTESTRIDE YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_bytestride_callback($$, $3, user_defined);
};

bufferview_properties: YYTOKEN_TARGET YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_target_callback($$, $3, user_defined);
};

accessors_array: YYTOKEN_LEFTBRACKET accessor_objects YYTOKEN_RIGHTBRACKET {

};

accessors_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET {

};

accessor_objects: accessor_objects YYTOKEN_COMMA accessor_object { 
   	assert(-1 == $3 || gltf_yacc_accessor_size_callback(user_defined) == ($3 + 1));
};

accessor_objects: accessor_object { 
	assert(-1 == $1 || gltf_yacc_accessor_size_callback(user_defined) == ($1 + 1));
};

accessor_object: YYTOKEN_LEFTBRACE accessor_properties YYTOKEN_RIGHTBRACE {
	$$ = $2;
};

accessor_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE { 
	$$ = -1;
};

accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_BUFFERVIEW YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_accessor_set_bufferview_callback($$, $5, user_defined);
};

accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_accessor_set_byteoffset_callback($$, $5, user_defined);
};

accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COMPONENTTYPE YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_accessor_set_componenttype_callback($$, $5, user_defined);
};

accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_NORMALIZED YYTOKEN_COLON json_boolean {
	gltf_yacc_accessor_set_normalized_callback($$, $5, user_defined);
};

accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	gltf_yacc_accessor_set_count_callback($$, $5, user_defined);
};

accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_SCALAR {
	gltf_yacc_accessor_set_type_callback($$, 1, user_defined);
};

accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_VEC2 {
	gltf_yacc_accessor_set_type_callback($$, 2, user_defined);
};

accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_VEC3 {
	gltf_yacc_accessor_set_type_callback($$, 3, user_defined);
};

accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_VEC4 {
	gltf_yacc_accessor_set_type_callback($$, 4, user_defined);
};

accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_MAT2 {
	gltf_yacc_accessor_set_type_callback($$, 5, user_defined);
};

accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_MAT3 {
	gltf_yacc_accessor_set_type_callback($$, 9, user_defined);
};

accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_MAT4 {
	gltf_yacc_accessor_set_type_callback($$, 16, user_defined);
};

accessor_properties: YYTOKEN_BUFFERVIEW YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_bufferview_callback($$, $3, user_defined);
};

accessor_properties: YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_byteoffset_callback($$, $3, user_defined);
};

accessor_properties: YYTOKEN_COMPONENTTYPE YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_componenttype_callback($$, $3, user_defined);
};

accessor_properties: YYTOKEN_COMPONENTTYPE YYTOKEN_COLON json_boolean {
	$$ = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_normalized_callback($$, $3, user_defined);
};

accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_NUMBER_INT {
	$$ = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_count_callback($$, $3, user_defined);
};

accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_SCALAR {
	$$ = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback($$, 1, user_defined);
};

accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_VEC2 {
	$$ = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback($$, 2, user_defined);
};

accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_VEC3 {
	$$ = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback($$, 3, user_defined);
};

accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_VEC4 {
	$$ = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback($$, 4, user_defined);
};

accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_MAT2 {
	$$ = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback($$, 5, user_defined);
};

accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_MAT3 {
	$$ = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback($$, 9, user_defined);
};

accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_MAT4 {
	$$ = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback($$, 16, user_defined);
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

json_string: YYTOKEN_CAMERA {

};

json_string: YYTOKEN_CHILDREN {

};

json_string: YYTOKEN_SKIN {

};

json_string: YYTOKEN_MATRIX {

};

json_string: YYTOKEN_MESH {

};

json_string: YYTOKEN_ROTATION {

};

json_string: YYTOKEN_SCALE {

};

json_string: YYTOKEN_TRANSLATION {

};

json_string: YYTOKEN_WEIGHTS {

};

json_string: YYTOKEN_BUFFERS {

};

json_string: YYTOKEN_BYTELENGTH {

};

json_string: YYTOKEN_URI {

};

json_boolean: YYTOKEN_TRUE {
	$$ = true;
};

json_boolean: YYTOKEN_FALSE {
	$$ = false;
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

json_value: json_boolean { 

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
