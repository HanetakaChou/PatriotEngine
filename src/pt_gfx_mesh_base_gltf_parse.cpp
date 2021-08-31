//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_memcmp.h>
#include <pt_math.h>
#include "pt_gfx_mesh_base_gltf_parse.h"
#include "pt_gfx_mesh_base_gltf_lex_yacc.h"
#include <assert.h>

static_assert(sizeof(math_vec3) == sizeof(float[3]), "");
static_assert(alignof(math_vec3) == alignof(float[3]), "");
static_assert(sizeof(math_vec4) == sizeof(float[4]), "");
static_assert(alignof(math_vec4) == alignof(float[4]), "");
static_assert(sizeof(math_mat4x4) == sizeof(float[16]), "");
static_assert(alignof(math_mat4x4) == alignof(float[16]), "");

inline math_vec3 *unwrap_vec3(float vec3[3]) { return reinterpret_cast<math_vec3 *>(vec3); }
inline math_vec4 *unwrap_vec4(float vec4[4]) { return reinterpret_cast<math_vec4 *>(vec4); }
inline math_mat4x4 *unwrap_mat4x4(float mat4x4[16]) { return reinterpret_cast<math_mat4x4 *>(mat4x4); }

struct gltf_yy_extra_type
{
    intptr_t(PT_PTR *m_input_stream_read_callback)(pt_gfx_input_stream_ref, void *, size_t);
    void *m_error_callback_data;
    void (*m_error_callback)(int line, int column, char const *msg, void *error_callback_data);
    mcrt_string m_temp_string;
    uintptr_t m_temp_string_version;
    mcrt_vector<int> m_temp_int_array;
    uintptr_t m_temp_int_array_version;
    mcrt_vector<float> m_temp_float_array;
    uintptr_t m_temp_float_array_version;
    struct gltf_root *m_gltf_root;
};

// Exported by Lex Yacc
extern "C" int gltf_yylex_init_extra(void *user_defined, void **yyscanner);
extern "C" void gltf_yyset_in(pt_gfx_input_stream_ref gfx_input_stream, void *yyscanner);
extern "C" int gltf_yylex_destroy(void *yyscanner);
extern "C" int gltf_yyparse(void *user_defined, void *yyscanner);

bool gltf_parse_input_stream(struct gltf_root *out_gltf_root, pt_gfx_input_stream_ref gfx_input_stream, intptr_t(PT_PTR *gfx_input_stream_read_callback)(pt_gfx_input_stream_ref, void *, size_t), void *error_callback_data, void (*error_callback)(int line, int column, char const *msg, void *error_callback_data))
{
    struct gltf_yy_extra_type user_defined;
    user_defined.m_input_stream_read_callback = gfx_input_stream_read_callback;
    user_defined.m_error_callback_data = error_callback_data;
    user_defined.m_error_callback = error_callback;
    user_defined.m_temp_string_version = 0;
    user_defined.m_temp_int_array_version = 0;
    user_defined.m_temp_float_array_version = 0;
    user_defined.m_gltf_root = out_gltf_root;

    void *yyscanner;
    int res_yylex_init_extra = gltf_yylex_init_extra(&user_defined, &yyscanner);
    assert(0 == res_yylex_init_extra);

    gltf_yyset_in(gfx_input_stream, yyscanner);

    int res_yyparse = gltf_yyparse(&user_defined, yyscanner);

    int res_yylex_destroy = gltf_yylex_destroy(yyscanner);
    assert(0 == res_yylex_destroy);

    return (res_yyparse == 0) ? true : false;
}

void *gltf_lex_yacc_alloc_callback(size_t size, void *user_defined)
{
    return mcrt_aligned_malloc(size, alignof(uint8_t));
}

void *gltf_lex_yacc_realloc_callback(void *ptr, size_t size, void *user_defined)
{
    return mcrt_aligned_realloc(ptr, size, alignof(uint8_t));
}

void gltf_lex_yacc_free_callback(void *ptr, void *user_defined)
{
    return mcrt_aligned_free(ptr);
}

struct temp_string_version_t *gltf_lex_yacc_temp_string_init_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(0 == user_defined->m_temp_string.length());
    return reinterpret_cast<struct temp_string_version_t *>(user_defined->m_temp_string_version++);
}

void gltf_lex_yacc_temp_string_set_callback(struct temp_string_version_t *temp_string_version, char const *text, int length, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert((reinterpret_cast<uintptr_t>(temp_string_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_string_version));
    user_defined->m_temp_string.assign(text, length);
}

void gltf_lex_yacc_temp_string_destroy_callback(struct temp_string_version_t *temp_string_version, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert((reinterpret_cast<uintptr_t>(temp_string_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_string_version));
    assert(0 != user_defined->m_temp_string.size());
    user_defined->m_temp_string.clear();
}

ptrdiff_t gltf_lex_input_callback(void *input_stream_void, void *buf, size_t size, void *user_defined_void)
{
    pt_gfx_input_stream_ref gfx_input_stream = static_cast<pt_gfx_input_stream_ref>(input_stream_void);
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    return user_defined->m_input_stream_read_callback(gfx_input_stream, buf, size);
}

void gltf_lex_fatal_error_callback(int line, int column, char const *msg, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    user_defined->m_error_callback(line, column, msg, user_defined->m_error_callback_data);
    return;
}

int gltf_lex_memcmp_callback(void const *ptr1, void const *ptr2, size_t num, void *user_defined)
{
    return mcrt_memcmp(ptr1, ptr2, num);
}

void gltf_yacc_error_callback(int line, int column, char const *msg, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    user_defined->m_error_callback(line, column, msg, user_defined->m_error_callback_data);
    return;
}

struct temp_int_array_version_t *gltf_yacc_temp_int_array_init_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(0 == user_defined->m_temp_int_array.size());
    return reinterpret_cast<struct temp_int_array_version_t *>(user_defined->m_temp_int_array_version++);
}

void gltf_yacc_temp_int_array_push_callback(struct temp_int_array_version_t *temp_int_array_version, int int_element, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert((reinterpret_cast<uintptr_t>(temp_int_array_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_int_array_version));
    user_defined->m_temp_int_array.push_back(int_element);
}

void gltf_yacc_temp_int_array_destroy_callback(struct temp_int_array_version_t *temp_int_array_version, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert((reinterpret_cast<uintptr_t>(temp_int_array_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_int_array_version));
    assert(0 != user_defined->m_temp_int_array.size());
    user_defined->m_temp_int_array.clear();
}

struct temp_float_array_version_t *gltf_yacc_temp_float_array_init_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(0 == user_defined->m_temp_float_array.size());
    return reinterpret_cast<struct temp_float_array_version_t *>(user_defined->m_temp_float_array_version++);
}

void gltf_yacc_temp_float_array_push_callback(struct temp_float_array_version_t *temp_float_array_version, float float_element, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert((reinterpret_cast<uintptr_t>(temp_float_array_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_float_array_version));
    user_defined->m_temp_float_array.push_back(float_element);
}

void gltf_yacc_temp_float_array_destroy_callback(struct temp_float_array_version_t *temp_float_array_version, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert((reinterpret_cast<uintptr_t>(temp_float_array_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_float_array_version));
    assert(0 != user_defined->m_temp_float_array.size());
    user_defined->m_temp_float_array.clear();
}

void gltf_yacc_gltf_set_scene_index_callback(int scene_index, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    user_defined->m_gltf_root->m_scene_index = scene_index;
}

int gltf_yacc_scene_push_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int scene_index = user_defined->m_gltf_root->m_scenes.size();
    user_defined->m_gltf_root->m_scenes.emplace_back();
    return scene_index;
}

void gltf_yacc_scene_set_nodes_callback(int scene_index, struct temp_int_array_version_t *temp_int_array_version, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(scene_index < user_defined->m_gltf_root->m_scenes.size());
    assert(0 == user_defined->m_gltf_root->m_scenes[scene_index].m_nodes.size());
    assert((reinterpret_cast<uintptr_t>(temp_int_array_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_int_array_version));
    assert(0 != user_defined->m_temp_int_array.size());
    user_defined->m_gltf_root->m_scenes[scene_index].m_nodes.swap(user_defined->m_temp_int_array);
#ifndef NDEBUG
    user_defined->m_temp_int_array.assign({-1, -1, -1, -1, -1, -1, -1});
#endif
}

void gltf_yacc_scene_set_name_callback(int scene_index, struct temp_string_version_t *temp_string_version, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(scene_index < user_defined->m_gltf_root->m_scenes.size());
    assert(0 == user_defined->m_gltf_root->m_scenes[scene_index].m_name.length());
    assert((reinterpret_cast<uintptr_t>(temp_string_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_string_version));
    assert(0 != user_defined->m_temp_string.length());
    user_defined->m_gltf_root->m_scenes[scene_index].m_name.swap(user_defined->m_temp_string);
#ifndef NDEBUG
    user_defined->m_temp_string.assign("debug_string");
#endif
}

int gltf_yacc_scene_size_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int scene_index = user_defined->m_gltf_root->m_scenes.size();
    return scene_index;
}

int gltf_yacc_node_push_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int node_index = user_defined->m_gltf_root->m_nodes.size();
    user_defined->m_gltf_root->m_nodes.emplace_back();
    return node_index;
}

void gltf_yacc_node_set_camera_callback(int node_index, int camera_index, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(-1 == user_defined->m_gltf_root->m_nodes[node_index].m_camera);
    user_defined->m_gltf_root->m_nodes[node_index].m_camera = camera_index;
}

void gltf_yacc_node_set_children_callback(int node_index, struct temp_int_array_version_t *temp_int_array_version, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(0 == user_defined->m_gltf_root->m_nodes[node_index].m_children.size());
    assert((reinterpret_cast<uintptr_t>(temp_int_array_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_int_array_version));
    assert(0 != user_defined->m_temp_int_array.size());
    user_defined->m_gltf_root->m_nodes[node_index].m_children.swap(user_defined->m_temp_int_array);
#ifndef NDEBUG
    user_defined->m_temp_int_array.assign({-1, -1, -1, -1, -1, -1, -1});
#endif
}

void gltf_yacc_node_set_skin_callback(int node_index, int skin_index, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(-1 == user_defined->m_gltf_root->m_nodes[node_index].m_skin);
    user_defined->m_gltf_root->m_nodes[node_index].m_skin = skin_index;
}

void gltf_yacc_node_set_matrix_callback(int node_index, float mat4x4[16], void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[0]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[1]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[2]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[3]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[4]);
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[5]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[6]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[7]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[8]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[9]);
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[10]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[11]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[12]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[13]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[14]);
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[15]);
    math_store_mat4x4(unwrap_mat4x4(user_defined->m_gltf_root->m_nodes[node_index].m_matrix), math_load_mat4x4(unwrap_mat4x4(mat4x4)));
}

void gltf_yacc_node_set_mesh_callback(int node_index, int mesh_index, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(-1 == user_defined->m_gltf_root->m_nodes[node_index].m_mesh);
    user_defined->m_gltf_root->m_nodes[node_index].m_mesh = mesh_index;
}

void gltf_yacc_node_set_rotation_callback(int node_index, float vec4[4], void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_rotation[0]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_rotation[1]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_rotation[2]);
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_rotation[3]);
    math_store_vec4(unwrap_vec4(user_defined->m_gltf_root->m_nodes[node_index].m_rotation), math_load_vec4(unwrap_vec4(vec4)));
}

void gltf_yacc_node_set_scale_callback(int node_index, float vec3[3], void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_scale[0]);
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_scale[1]);
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_scale[2]);
    math_store_vec3(unwrap_vec3(user_defined->m_gltf_root->m_nodes[node_index].m_scale), math_load_vec3(unwrap_vec3(vec3)));
}

void gltf_yacc_node_set_translation_callback(int node_index, float vec3[3], void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_translation[0]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_translation[1]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_translation[2]);
    math_store_vec3(unwrap_vec3(user_defined->m_gltf_root->m_nodes[node_index].m_translation), math_load_vec3(unwrap_vec3(vec3)));
}

void gltf_yacc_node_set_weights_callback(int node_index, struct temp_float_array_version_t *temp_float_array_version, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(0 == user_defined->m_gltf_root->m_nodes[node_index].m_weights.size());
    assert((reinterpret_cast<uintptr_t>(temp_float_array_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_float_array_version));
    assert(0 != user_defined->m_temp_float_array.size());
    user_defined->m_gltf_root->m_nodes[node_index].m_weights.swap(user_defined->m_temp_float_array);
#ifndef NDEBUG
    user_defined->m_temp_float_array.assign({-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f});
#endif
}

void gltf_yacc_node_set_name_callback(int node_index, struct temp_string_version_t *temp_string_version, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_scenes.size());
    assert(0 == user_defined->m_gltf_root->m_nodes[node_index].m_name.length());
    assert((reinterpret_cast<uintptr_t>(temp_string_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_string_version));
    assert(0 != user_defined->m_temp_string.length());
    user_defined->m_gltf_root->m_nodes[node_index].m_name.swap(user_defined->m_temp_string);
#ifndef NDEBUG
    user_defined->m_temp_string.assign("debug_string");
#endif
}

int gltf_yacc_node_size_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int node_index = user_defined->m_gltf_root->m_nodes.size();
    return node_index;
}

int gltf_yacc_buffer_push_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int buffer_index = user_defined->m_gltf_root->m_buffers.size();
    user_defined->m_gltf_root->m_buffers.emplace_back();
    return buffer_index;
}

void gltf_yacc_buffer_set_bytelength_callback(int buffer_index, int byteLength, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(buffer_index < user_defined->m_gltf_root->m_buffers.size());
    assert(-1 == user_defined->m_gltf_root->m_buffers[buffer_index].m_byteLength);
    user_defined->m_gltf_root->m_buffers[buffer_index].m_byteLength = byteLength;
}

void gltf_yacc_buffer_set_url_callback(int buffer_index, struct temp_string_version_t *temp_string_version, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(buffer_index < user_defined->m_gltf_root->m_buffers.size());
    assert(0 == user_defined->m_gltf_root->m_buffers[buffer_index].m_url.length());
    assert((reinterpret_cast<uintptr_t>(temp_string_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_string_version));
    assert(0 != user_defined->m_temp_string.length());
    user_defined->m_gltf_root->m_buffers[buffer_index].m_url.swap(user_defined->m_temp_string);
#ifndef NDEBUG
    user_defined->m_temp_string.assign("debug_string");
#endif
}

int gltf_yacc_buffer_size_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int buffer_index = user_defined->m_gltf_root->m_buffers.size();
    return buffer_index;
}

int gltf_yacc_bufferview_push_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int bufferview_index = user_defined->m_gltf_root->m_bufferviews.size();
    user_defined->m_gltf_root->m_bufferviews.emplace_back();
    return bufferview_index;
}

void gltf_yacc_bufferview_set_buffer_callback(int bufferview_index, int buffer_index, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(bufferview_index < user_defined->m_gltf_root->m_bufferviews.size());
    assert(-1 == user_defined->m_gltf_root->m_bufferviews[bufferview_index].m_buffer);
    user_defined->m_gltf_root->m_bufferviews[bufferview_index].m_buffer = buffer_index;
}

void gltf_yacc_bufferview_set_byteoffset_callback(int bufferview_index, int byteoffset, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(bufferview_index < user_defined->m_gltf_root->m_bufferviews.size());
    assert(0 == user_defined->m_gltf_root->m_bufferviews[bufferview_index].m_byteOffset);
    user_defined->m_gltf_root->m_bufferviews[bufferview_index].m_byteOffset = byteoffset;
}

void gltf_yacc_bufferview_set_bytelength_callback(int bufferview_index, int bytelength, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(bufferview_index < user_defined->m_gltf_root->m_bufferviews.size());
    assert(-1 == user_defined->m_gltf_root->m_bufferviews[bufferview_index].m_byteLength);
    user_defined->m_gltf_root->m_bufferviews[bufferview_index].m_byteLength = bytelength;
}

void gltf_yacc_bufferview_set_bytestride_callback(int bufferview_index, int bytestride, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(bufferview_index < user_defined->m_gltf_root->m_bufferviews.size());
    assert(-1 == user_defined->m_gltf_root->m_bufferviews[bufferview_index].m_byteStride);
    user_defined->m_gltf_root->m_bufferviews[bufferview_index].m_byteStride = bytestride;
}

void gltf_yacc_bufferview_set_target_callback(int bufferview_index, int target, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(bufferview_index < user_defined->m_gltf_root->m_bufferviews.size());
    assert(-1 == user_defined->m_gltf_root->m_bufferviews[bufferview_index].m_target);
    user_defined->m_gltf_root->m_bufferviews[bufferview_index].m_target = static_cast<enum gltf_target>(target);
}

int gltf_yacc_bufferview_size_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int bufferview_index = user_defined->m_gltf_root->m_bufferviews.size();
    return bufferview_index;
}

int gltf_yacc_accessor_push_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int accessor_index = user_defined->m_gltf_root->m_accessors.size();
    user_defined->m_gltf_root->m_accessors.emplace_back();
    return accessor_index;
}

void gltf_yacc_accessor_set_bufferview_callback(int accessor_index, int bufferview_index, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(accessor_index < user_defined->m_gltf_root->m_accessors.size());
    assert(-1 == user_defined->m_gltf_root->m_accessors[accessor_index].m_bufferview);
    user_defined->m_gltf_root->m_accessors[accessor_index].m_bufferview = bufferview_index;
}

void gltf_yacc_accessor_set_byteoffset_callback(int accessor_index, int byteoffset, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(accessor_index < user_defined->m_gltf_root->m_accessors.size());
    assert(0 == user_defined->m_gltf_root->m_accessors[accessor_index].m_byteoffset);
    user_defined->m_gltf_root->m_accessors[accessor_index].m_byteoffset = byteoffset;
}

void gltf_yacc_accessor_set_componenttype_callback(int accessor_index, int componenttype, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(accessor_index < user_defined->m_gltf_root->m_accessors.size());
    assert(GLTF_COMPONENT_TYPE_UNKNOWN == user_defined->m_gltf_root->m_accessors[accessor_index].m_componenttype);
    user_defined->m_gltf_root->m_accessors[accessor_index].m_componenttype = static_cast<enum gltf_component_type>(componenttype);
}

void gltf_yacc_accessor_set_normalized_callback(int accessor_index, bool normalized, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(accessor_index < user_defined->m_gltf_root->m_accessors.size());
    assert(false == user_defined->m_gltf_root->m_accessors[accessor_index].m_normalized);
    user_defined->m_gltf_root->m_accessors[accessor_index].m_normalized = normalized;
}

void gltf_yacc_accessor_set_count_callback(int accessor_index, int count, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(accessor_index < user_defined->m_gltf_root->m_accessors.size());
    assert(-1 == user_defined->m_gltf_root->m_accessors[accessor_index].m_count);
    user_defined->m_gltf_root->m_accessors[accessor_index].m_count = count;
}

void gltf_yacc_accessor_set_type_callback(int accessor_index, int type, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(accessor_index < user_defined->m_gltf_root->m_accessors.size());
    assert(GLTF_TYPE_UNKNOWN == user_defined->m_gltf_root->m_accessors[accessor_index].m_type);
    user_defined->m_gltf_root->m_accessors[accessor_index].m_type = static_cast<enum gltf_type>(type);
}

void gltf_yacc_accessor_set_max_callback(int accessor_index, float max[16], void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(accessor_index < user_defined->m_gltf_root->m_accessors.size());
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[0]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[1]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[2]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[3]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[4]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[5]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[6]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[7]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[8]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[9]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[10]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[11]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[12]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[13]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[14]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_max[15]);
    math_store_mat4x4(unwrap_mat4x4(user_defined->m_gltf_root->m_accessors[accessor_index].m_max), math_load_mat4x4(unwrap_mat4x4(max)));
}

void gltf_yacc_accessor_set_min_callback(int accessor_index, float min[16], void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(accessor_index < user_defined->m_gltf_root->m_accessors.size());
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[0]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[1]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[2]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[3]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[4]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[5]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[6]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[7]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[8]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[9]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[10]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[11]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[12]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[13]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[14]);
    assert(0.0f == user_defined->m_gltf_root->m_accessors[accessor_index].m_min[15]);
    math_store_mat4x4(unwrap_mat4x4(user_defined->m_gltf_root->m_accessors[accessor_index].m_min), math_load_mat4x4(unwrap_mat4x4(min)));
}

void gltf_yacc_accessor_set_name_callback(int accessor_index, struct temp_string_version_t *temp_string_version, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(accessor_index < user_defined->m_gltf_root->m_accessors.size());
    assert(0 == user_defined->m_gltf_root->m_accessors[accessor_index].m_name.length());
    assert((reinterpret_cast<uintptr_t>(temp_string_version) + 1) == reinterpret_cast<uintptr_t>(user_defined->m_temp_string_version));
    assert(0 != user_defined->m_temp_string.length());
    user_defined->m_gltf_root->m_accessors[accessor_index].m_name.swap(user_defined->m_temp_string);
#ifndef NDEBUG
    user_defined->m_temp_string.assign("debug_string");
#endif
}

int gltf_yacc_accessor_size_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int accessor_index = user_defined->m_gltf_root->m_accessors.size();
    return accessor_index;
}